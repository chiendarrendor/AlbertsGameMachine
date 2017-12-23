use XMLReader;
use NodeUtilities;


if (@ARGV < 1)
{
    print "Need file on Command line.";
    exit(1);
}

my $doc = LoadXML($ARGV[0]);


RecursivePrintXML($doc->getDocumentElement,"");




