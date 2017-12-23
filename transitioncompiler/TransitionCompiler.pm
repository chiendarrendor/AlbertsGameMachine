
package TransitionCompiler;
use XML::DOM;
use NodeUtilities;
use Transition;
use EventFactory;

sub new
{
    my ($class,$basenode) = @_;
    my $this = {};
    bless $this,$class;

    $this->{STATE}->{"InitialState"} = "Initial Game State";
    $this->{STATE}->{"TerminalState"} = "End Of Game State";
    $this->{AUTOLIMIT} = "10";

    $this->{EVENTS}->{NEWSTATE} = new ManualEvent("NEWSTATE");
    $this->{EVENTS}->{NEWSTATE}->AddVar("i_EventName","std::string");
    $this->{EVENTS}->{NEWSTATE}->AddVar("i_EventDesc","std::string");
    
    $this->{EVENTS}->{LEGALACTION} = new ManualEvent("LEGALACTION");
    $this->{EVENTS}->{LEGALACTION}->AddVar("i_ActionName","std::string");

    $this->{EVENTS}->{ERROR} = new ManualEvent("ERROR");
    $this->{EVENTS}->{ERROR}->AddVar("i_ErrorText","std::string");

    $this->{EVENTS}->{MESSAGE} = new ManualEvent("MESSAGE");
    $this->{EVENTS}->{MESSAGE}->AddVar("i_MessageText","std::string");

    $this->{EVENTS}->{RESET} = new GlobalEvent("RESET");
    $this->{EVENTS}->{RESET}->AddVar("i_GameName","std::string","m_ServerGameInfo.GetName()");

    if (!$basenode)
    {
	$this->{STATUS} = "EMPTY";
	return $this;
    }

    if ($basenode->getNodeName ne "game")
    {
	print "File Base Node is not 'game'\n";
	$this->{STATUS} = "BAD";
	return $this;
    }

    my $name = $basenode->getAttribute("name");

    if (!$name)
    {
	print "game node has no name attribute\n";
	$this->{STATUS} = "BAD";
	return $this;
    }

    $this->{NAME} = $name;

    my $autolimit = $basenode->getAttribute("autorecursiondepth");
    if ($autolimit)
    {
	$this->{AUTOLIMIT} = $autolimit;
    }

    my $children = GetChildren($basenode);
    if (!$children)
    {
	$this->{STATUS} = "EMPTY";
	return $this;
    }
    
    for (my $i = 0 ; $i < $children->getLength ; ++$i)
    {
	if (!$this->HandleBaseChild($children->item($i)))
	{
	    $this->{STATUS} = "BAD";
	    return $this;
	}
    }

    $this->{STATUS} = "PARSED";
    return $this;
}

sub HandleBaseChild
{
    my ($this,$node) = @_;
    
    if ($node->getNodeName eq "#text" && $node->getNodeValue =~ /^\s+$/) { return 1; }
    if ($node->getNodeName eq "#comment") { return 1; }
    if ($node->getNodeName eq "state") { return $this->HandleStateNode($node); }
    if ($node->getNodeName eq "transition") { return $this->HandleTransitionNode($node); }
    if ($node->getNodeName eq "event") { return $this->HandleEventNode($node); }
    if ($node->getNodeName eq "refresh") { return $this->HandleRefreshNode($node); }
    if ($node->getNodeName eq "preface") { return $this->HandlePrefaceNode($node); }

    print "Unknown Base Child Node ",$node->getNodeName,"\n";
    return undef;
}


sub HandlePrefaceNode
{
    my ($this,$node) = @_;
    my $text = GetNodeText($node);
    if (!$text)
    {
	print "Preface node parse failed.\n";
	return undef;
    }
    $this->{PREFACE} = $text;
    return 1;
}


sub HandleRefreshNode
{
    my ($this,$node) = @_;
    my $text = GetNodeText($node);
    if (!$text)
    {
	print "Refresh node parse failed.\n";
	return undef;
    }
    $this->{REFRESH} = $text;
    return 1;
}

sub HandleEventNode
{
    my ($this,$node) = @_;
    my $event = EventFactory::MakeEvent($node);
    if (!$event) { return undef; }
    $this->{EVENTS}->{$event->{NAME}} = $event;
    return 1;
}


sub HandleStateNode
{
    my ($this,$node) = @_;
    
    my $statename = $node->getAttribute("name");
    my $statedesc = $node->getAttribute("desc");

    if (!$statename || !$statedesc)
    {
	print "state node has no name or desc.\n";
	return undef;
    }
    
    if ($this->HasState($statename))
    {
	print "State Name $statename duplicated!";
	return undef;
    }

    $this->{STATE}->{$statename} = $statedesc;

    if (GetChildren($node))
    {
	print "state node shouldn't have children!\n";
	return undef;
    }
    return 1;
}

sub HandleTransitionNode
{
    my ($this,$node) = @_;
    
    my $transition = new Transition($this,$node);
    
    if (!$transition)
    {
	return undef;
    }

    if (exists $this->{TRANSITIONS}->{$transition->GetName})
    {
	print "Duplicate Transition with name ",$transition->GetName,"\n";
	return undef;
    }

    $this->{TRANSITIONS}->{$transition->GetName} = $transition;
    return 1;
}

sub HasState
{
    my ($this,$name) = @_;
    return exists $this->{STATE}->{$name};
}

1;

