
package Transition;
use XML::DOM;
use NodeUtilities;

sub new
{
    my ($class,$document,$node) = @_;
    my $this = {};
    bless $this,$class;

    $this->{DOCUMENT} = $document;
    $this->{TRANSITIONS} = [];
    $this->{AUTO} = "false";
    $this->{ALLOWED} = "true";
    $this->{BODY} = "return true;";

    if (!$node)
    {
	return undef;
    }

    if ($node->getNodeName ne "transition")
    {
	return undef;
    }
    
    my $name = $node->getAttribute("name");
    if (!$name)
    {
	print "Transition has no name attribute!\n";
	return undef;
    }

    $this->{NAME} = $name;

    my $children = GetChildren($node);
    if (!$children)
    {
	print "Transition $name has no children!\n";
	return undef;
    }

    for (my $i = 0 ; $i < $children->getLength ; ++$i)
    {
	if (!$this->HandleChildNode($children->item($i)))
	{
	    return undef;
	}
    }
    return $this;
}

sub HandleChildNode
{
    my ($this,$node) = @_;
    
    if ($node->getNodeName eq "#text" && $node->getNodeValue =~ /^\s+$/) { return 1; }
    if ($node->getNodeName eq "cyclic") { return $this->HandleCyclic($node); }
    if ($node->getNodeName eq "transit") { return $this->HandleTransit($node); }
    if ($node->getNodeName eq "action") { return $this->HandleAction($node); }
    if ($node->getNodeName eq "allowed") { return $this->HandleAllowed($node); }
    if ($node->getNodeName eq "auto") { return $this->HandleAuto($node); }
    if ($node->getNodeName eq "body") { return $this->HandleBody($node); }

    print "Unknown Child Node ",$node->getNodeName," of transition node.\n";
    return undef;
}

sub HandleCyclic
{
    my ($this,$node) = @_;

    my $state = $node->getAttribute("state");

    if (!$state)
    {
	print "Cyclic has no state attribute.\n";
	return undef;
    }
    if (!$this->{DOCUMENT}->HasState($state))
    {
	print "Cyclic has unknown state $state\n";
	return undef;
    }
    
    return $this->AddTransition($state,$state);
}

sub HandleTransit
{
    my ($this,$node) = @_;

    my $from = $node->getAttribute("from");
    my $to = $node->getAttribute("to");

    if (!$from)
    {
	print "Transit has no from attribute.\n";
	return undef;
    }

    if (!$to)
    {
	print "Transit has no to attribute.\n";
	return undef;
    }

    if (!$this->{DOCUMENT}->HasState($from))
    {
	print "Transit has unknown from state $from\n";
	return undef;
    }

    if (!$this->{DOCUMENT}->HasState($to))
    {
	print "Transit has unknown to state $to\n";
	return undef;
    }
    
    return $this->AddTransition($from,$to);
}


	
sub AddTransition
{
    my ($this,$from,$to) = @_;

    my $trans = {};
    $trans->{FROM} = $from;
    $trans->{TO} = $to;

    if ($from eq "TerminalState")
    {
	print "Transition ",$this->{NAME}," is attempting to leave TerminalState!\n";
	return undef;
    }

    if (exists $this->{FROMSTATES}->{$from})
    {
	print "Transition ",$this->{NAME}," is multiply instanciated on source node $from\n";
	return undef;
    }

    $this->{FROMSTATES}->{$from} = 1;

    push @{$this->{TRANSITIONS}},$trans;
    return 1;
}
    
sub GetName
{
    my ($this) = @_;
    return $this->{NAME};
}

sub HandleAllowed
{
    my ($this,$node) = @_;
    my $text = GetNodeText($node);
    if (!$text)
    {
	return undef;
    }
    $this->{ALLOWED} = $text;
    return 1;
}

sub HandleAuto
{
    my ($this,$node) = @_;
    
    my $text = GetNodeText($node);
    if (!$text)
    {
	return undef;
    }
    $this->{AUTO} = $text;
    return 1;
}

sub HandleBody
{
    my ($this,$node) = @_;
    my $text = GetNodeText($node);
    if (!$text)
    {
	return undef;
    }
    $this->{BODY} = $text;
    return 1;
}

sub HandleAction
{
    my ($this,$node) = @_;

    my $children = GetChildren($node);
    if (!$children)
    {
	print "Action Node has no children!\n";
	return undef;
    }

    if (exists $this->{ACTIONS})
    {
	print "Transition ",$this->{NAME}," has multiple action blocks!\n";
	return undef;
    }

    $this->{ACTIONS} = [];

    for (my $i = 0 ; $i < $children->getLength ; ++$i)
    {
	my $child = $children->item($i);
	
	if ($child->getNodeName eq "#text" && $child->getNodeValue =~ /^\s+$/) { next; }
	if ($child->getNodeName eq "var")
	{
	    if (!$this->HandleVar($child))
	    {
		return undef;
	    }
	}
	else
	{
	    print "Transition ",$this->{NAME}, " has unknown item in <action>.\n";
	    return undef;
	}
    }
    return 1;
}

sub HandleVar
{
    my ($this,$node) = @_;

    my $name = $node->getAttribute("name");
    my $type = $node->getAttribute("type");
    my $trans = $node->getAttribute("trans");
    my $min  = $node->getAttribute("min");
    my $max  = $node->getAttribute("max");
    my $list = $node->getAttribute("list");

    if (!$name)
    {
	print "Nameless var in <action> of transition ",$this->{NAME},"\n";
	return undef;
    }

    if ($trans && !$type)
    {
	print "action var $name has type-transfer '$trans', but no type!\n";
	return undef;
    }

    if (!$type)
    {
	$type = "int";
    }

    if (!$trans)
    {
	$trans = "boost::lexical_cast<$type>";
    }

    if ((length($min)>0 || length($max)>0) && $list)
    {
	print "action var $name has min/max and list\n";
	return undef;
    }

    my $result = {};
    $result->{NAME} = $name;

    $result->{TYPE} = ($type) ? $type : "int";
    $result->{TRANS} = $trans;

    if (length($min)>0)  { $result->{MIN}  = $min;  }
    if (length($max)>0)  { $result->{MAX}  = $max;  }
    if ($list) { $result->{LIST} = $list; }

    push @{$this->{ACTIONS}},$result;
    return 1;
}

	


sub Print
{
    my ($this) = @_;
    my $indent = "    ";
    print $indent,"Name: ",$this->{NAME},"\n";
    print $indent,"Transitions: \n";
    for (my $i = 0 ; $i < @{$this->{TRANSITIONS}} ; ++$i)
    {
	print $indent,"  ",$this->{TRANSITIONS}->[$i]->{FROM}," -> ",$this->{TRANSITIONS}->[$i]->{TO},"\n";
    }
    print "${indent}Allowed:\n$indent----------\n",$this->{ALLOWED},"\n$indent---------\n";
    print "${indent}Auto:\n$indent----------\n",$this->{AUTO},"\n$indent---------\n";
    print "${indent}Body:\n$indent----------\n",$this->{BODY},"\n$indent---------\n";

    for (my $i = 0 ; $i < @{$this->{ACTIONS}} ; ++$i)
    {
	my $name = $this->{ACTIONS}->[$i]->{NAME};
	my $type = $this->{ACTIONS}->[$i]->{TYPE};
	my $trans = $this->{ACTIONS}->[$i]->{TRANS};
	my $min = $this->{ACTIONS}->[$i]->{MIN};
	my $max = $this->{ACTIONS}->[$i]->{MAX};
	my $list = $this->{ACTIONS}->[$i]->{LIST};

	print $indent,"$type $name($trans(<input>));";
	if ($min || $max)
	{
	    print "// between ($min) and ($max)\n";
	}
	elsif ($list)
	{
	    print "// restricted to items in ($list)\n";
	}
	else
	{
	    print "\n";
	}
    }
}



1;
