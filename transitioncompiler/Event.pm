
package Event;
use XML::DOM;
use NodeUtilities;

sub new
{
    my ($class,$name,$usesource,$node) = @_;

    my $this = {};
    $this->{NAME} = $name;
    $this->{VARS} = [];
    $this->{ITERS} = [];
    $this->{USESOURCE} = $usesource;
    bless $this,$class;

    if (!$node) { return $this; }

    my $children = GetChildren($node);
    if (!$children) { return $this; }

    for (my $i = 0 ; $i < $children->getLength ; ++$i)
    {
	my $child = $children->item($i);

	if    ($child->getNodeName eq "#text" && $child->getNodeValue =~ /^\s+$/) { next; }
	elsif ($child->getNodeName eq "iterator") 
	{ 
	    my $itnode = $this->HandleIterator($child);
	    if (!$itnode) { return undef; }
	}
	elsif ($child->getNodeName eq "var") 
	{ 
	    my $varnode = $this->HandleVar($child);
	    if (!$varnode) { return undef; }
	}
	else
	{
	    print "Unknown Node Name ",$child->getNodeName," in event $name\n";
	    return undef;
	}
    }
    return $this;
}

sub AddIterator
{
    my ($this,$name,$type,$start,$stop) = @_;
    my $usesource = $this->{USESOURCE};

    if (!$usesource)
    {
	print "Iterators make no sense in non global/private event ",$this->{NAME},"!\n";
	return undef;
    }
    my $iternode = {};
    $iternode->{NAME} = $name;
    $iternode->{TYPE} = $type;
    $iternode->{START} = $start;
    $iternode->{STOP} = $stop;
    
    if (!$iternode->{NAME})
    {
	print "missing name for iterator.\n";
	return undef;
    }

    if (!$iternode->{TYPE})
    {
	print "missing type for iterator ",$iternode->{NAME}," of event ",$this->{NAME},"\n";
	return undef;
    }

    if (length($iternode->{START}) == 0)
    {
	print "missing start for iterator ",$iternode->{NAME}," of event ",$this->{NAME},"\n";
	return undef;
    }
	
    if (length($iternode->{STOP}) == 0)
    {
	print "missing end for iterator ",$iternode->{NAME}," of event ",$this->{NAME},"\n";
	return undef;
    }

    push @{$this->{ITERS}},$iternode;
    return 1;
}


sub HandleIterator
{
    my ($this,$node) = @_;
    my $name = $node->getAttribute("name");
    my $type = $node->getAttribute("type");
    my $start= $node->getAttribute("start");
    my $stop = $node->getAttribute("stop");

    if (GetChildren($node))
    {
	print "Iterator Node should not have children!\n";
	return undef;
    }
    
    return $this->AddIterator($name,$type,$start,$stop);
}

sub AddVar
{
    my ($this,$name,$type,$source) = @_;
    my $usesource = $this->{USESOURCE};
    my $var = {};

    $var->{NAME} = $name;
    $var->{TYPE} = $type;
    $var->{SOURCE} = $source;

    if (!$var->{NAME})
    {
	print "Var Node has no name!\n";
	return undef;
    }
    if (!$var->{TYPE})
    {
	print "Var Node ",$var->{NAME}," has no type!\n";
	return undef;
    }

    if ($usesource && length($var->{SOURCE}) == 0)
    {
	print "global/private var ",$var->{NAME}," must have source.\n";
	return undef;
    }

    if (!$usesource && length($var->{SOURCE}) != 0)
    {
	print "manual var ",$var->{NAME}," may not have source.\n";
	return undef;
    }
    
    push @{$this->{VARS}} , $var;
    return 1;
}

	
sub HandleVar
{
    my ($this,$node) = @_;
    my $name = $node->getAttribute("name");
    my $type = $node->getAttribute("type");
    my $source = $node->getAttribute("source");
    if (GetChildren($node))
    {
	print "Var Node $name should not have children!\n";
	return undef;
    }

    return $this->AddVar($name,$type,$source);
}


sub GetPrivateHeaders
{
    my ($this) = @_;
    return "  std::string Make",$this->{NAME},"Message(",$this->GetMessageArgList(),") const;\n";
}

sub GetPrivateCodeBody
{
    my ($this,$oo) = @_;
    my $result;


    $result .= "std::string ${oo}GameInfo::Make".$this->{NAME}."Message(".$this->GetMessageArgList().") const\n";
    $result .= "{\n";
    $result .= "  std::ostringstream oss;\n";
    $result .= "  oss << \"".$this->{NAME}."\"";

    $result .= $this->GetMessageStream();

    $result .= "  ;\n";
    $result .= "  return oss.str();\n";
    $result .= "}\n";
    $result .= "\n";

    return $result;
}




1;
