package EventFactory;
use XML::DOM;
use NodeUtilities;
use GlobalEvent;
use PrivateEvent;
use ManualEvent;

sub MakeEvent
{
    my ($node) = @_;
    if ($node->getNodeName ne "event")
    {
	print "Event node is not an event!?\n";
	return undef;
    }

    my $name = $node->getAttribute("name");
    if (!$name)
    {
	print "Event node is nameless.\n";
	return undef;
    }
    
    my $type = $node->getAttribute("type");
    if ($type eq "global") { return new GlobalEvent($name,$node); }
    if ($type eq "private") { return new PrivateEvent($name,$node); }
    if (!$type) { return new ManualEvent($name,$node); }
    
    print "Unknown Event Type $type\n";
    return undef;
}

    
1;
