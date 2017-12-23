use XML::DOM;

sub LoadXML
{
    my ($fname) = @_;

    my $parser = new XML::DOM::Parser;
    my $doc = $parser->parsefile($fname);

    return $doc;
}


1;

