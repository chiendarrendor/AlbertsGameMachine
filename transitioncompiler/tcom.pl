use XMLReader;
use TransitionCompiler;
use FileWriter;

if (@ARGV < 1)
{
    print "Need file on Command line.";
    exit(1);
}

my $doc = LoadXML($ARGV[0]);
my $data = new TransitionCompiler($doc->getDocumentElement);

if ($data->{STATUS} ne "PARSED")
{
    print "Parse Failed: ", $data->{STATUS},"\n";
    exit(1);
}


WriteFiles($data);

exit(0);



