package PrivateEvent;
use SourceEvent;
use XML::DOM;
use NodeUtilities;

@ISA=qw(SourceEvent);

sub new
{
    my ($class,$name,$node) = @_;
    my $result = new SourceEvent($name,$node);
    bless $result,$class;

    $result->{TONAME} = $node->getAttribute("toname");
    if (!$result->{TONAME})
    {
	print "Private Event ${name} needs toname attribute.\n";
	return undef;
    }

    return $result;
}

sub GetPublicHeaders
{
    my ($this) = @_;
    my $ename = $this->{NAME};
    my $hdrs = $this->GetMessageArgList();
    my $result;

    $result .= "  void SingleCast${ename}(${hdrs}) const;\n";
    $result .= "  void AllCast${ename}() const;\n";
    $result .= "\n";
    
    return $result;
}


sub GetPublicCodeBody
{
    my ($this,$oo) = @_;
    my $ename = $this->{NAME};
    my $hdrs = $this->GetMessageArgList();
    my $cname = $oo . "GameInfo::";
    my $toname=$this->{TONAME};
    my $args = $this->GetActArgs();
    my $cargs = ((length($args) > 0) ? "," : "") . $args;

    my $result;

    $result .= "void ${cname}SingleCast${ename}(${hdrs}) const\n";
    $result .= "{\n";
    $result .= "  m_OutputPort.UniCast(${toname},Make${ename}Message(${args}));\n";
    $result .= "}\n";

    $result .= "void ${cname}AllCast${ename}() const\n";
    $result .= "{\n";
    $result .= "  ".$this->GetIterOpen()."\n";
    $result .= "  SingleCast${ename}(${args});\n";
    $result .= "  ".$this->GetIterClose()."\n";
    $result .= "}\n";
    $result .= "\n";

    return $result;
}


1;
