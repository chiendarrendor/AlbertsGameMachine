package NodeUtilities;
use XML::DOM;
use Exporter 'import';
@EXPORT = qw(GetChildren GetNodeText RecursivePrintXML);

# returns the children object from the node,
# or undef if it doesn't exist or is empty.
sub GetChildren
{
    my ($node) = @_;

    my $children = $node->getChildNodes;
    if (!$children || $children->getLength == 0)
    {
	return undef;
    }
    return $children;
}

# returns the text from all #text subnodes of this
# node, or undef if the subnodes are not all #text or 
# there are no subnodes.

sub GetNodeText
{
    my ($node) = @_;
    my $result = "";

    my $children = GetChildren($node);
    if (!$children)
    {
	return undef;
    }
    
    for (my $i = 0 ; $i < $children->getLength ; ++$i)
    {
	my $subnode = $children->item($i);
	if ($subnode->getNodeName ne "#text")
	{
	    return undef;
	}
	$result .= $subnode->getNodeValue;
    }
    return $result;
}

sub GetAttrVarValue
{
    my ($attrs,$index) = @_;

    my $attrnode = $attrs->item($index);
    if ($attrnode == undef)
    {
	return undef;
    }
    my $var = $attrnode->getNodeName;
    
    my $subnode = $attrnode->getFirstChild;

    my $val = $subnode->getNodeValue;

    return ($var,$val);
}

sub RecursivePrintXML
{
    my ($node,$indent) = @_;

#    if ($node->getNodeName eq "#text" &&
#	$node->getNodeValue =~ /^\s+$/)
#    {
#	return;
#    }
    if (!$node)
    {
	print $indent,"Empty Node!\n";
	return;
    }

    print $indent,"Name: ",$node->getNodeName;
    print "(",$node->getNodeTypeName,")\n";

    if ($node->getNodeValue)
    {
	print $indent,"  Value: ",$node->getNodeValue,"\n";
    }

    my $attrs = $node->getAttributes;
    if ($attrs && $attrs->getLength != 0)
    {
	print $indent,"  Attributes:\n";

	for (my $i = 0 ; $i < $attrs->getLength ; $i++)
	{
	    @varval = GetAttrVarValue($attrs,$i);
	    print $indent,"    ",$varval[0]," = ",$varval[1],"\n";
	}
    }
    
    my $children = $node->getChildNodes;
    if ($children && $children->getLength != 0)
    {
	print $indent,"  Children:\n";
	for (my $j = 0 ; $j < $children->getLength ; ++$j)
	{
	    RecursivePrintXML($children->item($j),$indent . "    ");
	}
    }
}



1;
