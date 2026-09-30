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
# A server <transition> counts as player-triggerable unless it has a direct
# <auto> child -- the established authoring convention in both real games:
# every auto-fired transition pairs an <auto> block with <allowed>false</allowed>,
# and is never meant to be reachable via a client action at all.
#
# Only var *names* (as a set) are compared -- not order, not type. Order
# stopped being load-bearing once the wire went JSON-keyed (see Phase 2), and
# the client's <var> declarations don't carry an explicit type attribute at
# all (a var's type is implicit in whichever widget tag is nested inside it),
# so type-checking isn't attempted here -- see .claude/TODO.md's long-term
# "unify the two action schemas" item for that harder problem.

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

sub CompareVars
{
    my ($name,$servervars,$clientvars,$problems) = @_;

    my %sset = map { $_ => 1 } @$servervars;
    my %cset = map { $_ => 1 } @$clientvars;

    my @missingonclient = grep { !exists $cset{$_} } @$servervars;
    my @extraonclient   = grep { !exists $sset{$_} } @$clientvars;

    if (@missingonclient || @extraonclient)
    {
	push @$problems,
	    "Action '$name' var mismatch: server has [@$servervars], client has [@$clientvars].";
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

	next if DirectChildren($trans,"auto"); # automatic-only transition -- no client action expected.

	my @actionnodes = DirectChildren($trans,"action");
	my @varnames;
	if (@actionnodes)
	{
	    for my $var (DirectChildren($actionnodes[0],"var"))
	    {
		my $varname = $var->getAttribute("name");
		push @varnames, $varname if $varname;
	    }
	}
	$result{$name} = \@varnames;
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

	my @varnames;
	for my $var (DirectChildren($action,"var"))
	{
	    my $varname = $var->getAttribute("name");
	    push @varnames, $varname if $varname;
	}
	$result{$name} = \@varnames;
    }
    return %result;
}

1;
