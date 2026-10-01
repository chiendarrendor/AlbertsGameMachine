# Cross-validates a game's server transition/action schema (<Game>Server.xml)
# against its client action schema (<Game>Client.xml, already spliced flat by
# the caller's cpp -P step) -- nothing else in the build enforces that these
# two independently-authored documents describe the same set of actions and
# arguments, despite the game depending on them matching exactly. See
# .claude/server_game_interface_spec.md's "Action namespacing" section for
# the wire design this is protecting.
#
# Usage: perl -I<transitioncompiler dir> validate_action_schema.pl <ServerXML> <ClientXML>
# Exits nonzero (and prints every mismatch found) if the two disagree.
#
# A server <transition> counts as player-triggerable unless its <allowed> is
# literally "false" -- confirmed against the real engine (stateengine/
# StateWalker.hpp): <auto> and direct player invocation are NOT mutually
# exclusive. Execute() (a player firing a named action) only ever checks
# IsLegal() (the <allowed> condition); it never looks at IsAuto() at all --
# that's only consulted by the separate ExecuteAuto() pass that runs after
# every transition. So a transition can legitimately be both auto-fired under
# one condition and player-triggered under a different one (MerchantOfVenus's
# ENDMOVE: <auto>AutoStop()</auto> for a forced stop, <allowed>...&&
# ManualStop()</allowed> for a voluntary one -- initially missed here by
# wrongly keying this off "<auto> present" instead of "<allowed> is false",
# which almost shipped a wrong deletion of ENDMOVE's client action; see
# .claude/TODO.md). A transition with no <allowed> at all defaults to "true"
# per Transition.pm::HandleVar -- NOT auto-only, even if it also has <auto>.
#
# Compares the actual wire contract, not the client's internal widget-wiring
# label: a client <var>'s wire key is its paramname attribute if present,
# else its name (the two are allowed to differ -- see the client-side Phase 2
# plan's finding on why the client's name can't safely be renamed to match
# the server). Likewise paramtype (bool/int/string, defaulting to "string")
# is checked against the server's declared type. Only the *set* of (paramname,
# paramtype) pairs per action is compared, not order -- order stopped being
# load-bearing once the wire went JSON-keyed (Phase 2).

use strict;
use warnings;
use XMLReader;
use NodeUtilities;

if (@ARGV != 2)
{
    print "Usage: validate_action_schema.pl <ServerXML> <ClientXML>\n";
    exit(1);
}

my ($serverxml,$clientxml) = @ARGV;

my $serverdoc = LoadXML($serverxml);
my $clientdoc = LoadXML($clientxml);

my %serveractions = ParseServerActions($serverdoc);
my %clientactions = ParseClientActions($clientdoc);

my @problems;

for my $name (sort keys %serveractions)
{
    if (!exists $clientactions{$name})
    {
	push @problems, "Server transition '$name' is player-triggerable, but has no matching <action> in the client XML.";
	next;
    }
    CompareVars($name,$serveractions{$name},$clientactions{$name},\@problems);
}

for my $name (sort keys %clientactions)
{
    if (!exists $serveractions{$name})
    {
	push @problems, "Client action '$name' has no matching player-triggerable transition in the server XML.";
    }
}

if (@problems)
{
    print "Action schema mismatch between $serverxml and $clientxml:\n";
    for my $p (@problems)
    {
	print "  - $p\n";
    }
    exit(1);
}

print "Action schema OK: ", scalar(keys %serveractions), " actions match between $serverxml and $clientxml.\n";
exit(0);

sub ServerTypeToParamType
{
    my ($t) = @_;
    return "bool" if $t eq "bool";
    return "int" if $t eq "int" || $t eq "size_t";
    return "string" if $t eq "std::string";
    return "unknown($t)";
}

sub CompareVars
{
    my ($name,$servervars,$clientvars,$problems) = @_;

    my %sbyname = map { $_->[0] => ServerTypeToParamType($_->[1]) } @$servervars;
    my %cbyname = map { $_->[0] => $_->[1] } @$clientvars;

    my @servernames = sort keys %sbyname;
    my @clientnames = sort keys %cbyname;

    my @missingonclient = grep { !exists $cbyname{$_} } @servernames;
    my @extraonclient   = grep { !exists $sbyname{$_} } @clientnames;

    if (@missingonclient || @extraonclient)
    {
	push @$problems,
	    "Action '$name' param-name mismatch: server has [@servernames], client has [@clientnames].";
	return;
    }

    for my $pname (@servernames)
    {
	if ($sbyname{$pname} ne $cbyname{$pname})
	{
	    push @$problems,
		"Action '$name' param '$pname' type mismatch: server declares '$sbyname{$pname}', client paramtype is '$cbyname{$pname}'.";
	}
    }
}

sub DirectChildren
{
    my ($node,$tagname) = @_;
    my @result;

    my $children = GetChildren($node);
    return @result if !$children;

    for (my $i = 0 ; $i < $children->getLength ; ++$i)
    {
	my $child = $children->item($i);
	if ($child->getNodeName eq $tagname)
	{
	    push @result, $child;
	}
    }
    return @result;
}

sub IsAutoOnly
{
    my ($trans) = @_;
    my @allowednodes = DirectChildren($trans,"allowed");
    return 0 if !@allowednodes; # no <allowed> at all -> defaults to "true" -> not auto-only.
    my $text = GetNodeText($allowednodes[0]);
    return 0 if !defined $text;
    $text =~ s/^\s+|\s+$//g;
    return $text eq "false";
}

sub ParseServerActions
{
    my ($doc) = @_;
    my %result;

    my $transitions = $doc->getElementsByTagName("transition");
    for (my $i = 0 ; $i < $transitions->getLength ; ++$i)
    {
	my $trans = $transitions->item($i);
	my $name = $trans->getAttribute("name");
	next if !$name;

	next if IsAutoOnly($trans); # <allowed>false</allowed> -- no client action expected.

	my @actionnodes = DirectChildren($trans,"action");
	my @vars;
	if (@actionnodes)
	{
	    for my $var (DirectChildren($actionnodes[0],"var"))
	    {
		my $varname = $var->getAttribute("name");
		next if !$varname;
		my $vartype = $var->getAttribute("type");
		$vartype = "int" if !$vartype; # matches Transition.pm::HandleVar's own default.
		push @vars, [$varname,$vartype];
	    }
	}
	$result{$name} = \@vars;
    }
    return %result;
}

sub ParseClientActions
{
    my ($doc) = @_;
    my %result;

    my $actions = $doc->getElementsByTagName("action");
    for (my $i = 0 ; $i < $actions->getLength ; ++$i)
    {
	my $action = $actions->item($i);
	my $name = $action->getAttribute("name");
	next if !$name;

	my @vars;
	for my $var (DirectChildren($action,"var"))
	{
	    my $varname = $var->getAttribute("name");
	    next if !$varname;
	    my $paramname = $var->getAttribute("paramname");
	    $paramname = $varname if !$paramname;
	    my $paramtype = $var->getAttribute("paramtype");
	    $paramtype = "string" if !$paramtype;
	    push @vars, [$paramname,$paramtype];
	}
	$result{$name} = \@vars;
    }
    return %result;
}

1;
