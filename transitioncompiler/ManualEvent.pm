package ManualEvent;
use NonSourceEvent;

@ISA=qw(NonSourceEvent);

sub new
{
    my ($class,$name,$node) = @_;
    my $result = new NonSourceEvent($name,$node);
    bless $result,$class;
    return $result;
}

sub GetPublicHeaders
{
    my ($this) = @_;
    my $result;
    my $hdrs = $this->GetMessageArgList();
    my $chdrs = ((length($hdrs) > 0) ? "," : "") . $hdrs;
    my $ename = $this->{NAME};

    $result .= "  void Unicast${ename}(const std::string &i_PlayerName".$chdrs.") const;\n";
    $result .= "  void Varicast${ename}(const NameBoolean &i_Variator".$chdrs.") const;\n";
    $result .= "  void Broadcast${ename}(".$hdrs.") const;\n";
    $result .= "\n";

    return $result;
}

sub GetPublicCodeBody
{
    my ($this,$oo) = @_;
    my $result;
    my $ename = $this->{NAME};
    my $hdrs = $this->GetMessageArgList();
    my $chdrs = ((length($hdrs) > 0) ? "," : "") . $hdrs;
    my $args = $this->GetActArgs();
    my $cname = $oo . "GameInfo::";

    $result .= "void ${cname}Unicast${ename}(const std::string &i_PlayerName".$chdrs.") const\n";
    $result .= "{\n";
    if ($ename eq "ERROR")
    {
	$result .= "  ++m_errorcount;\n";
    }
    $result .= "  m_OutputPort.UniCast(i_PlayerName,Make${ename}Message(${args}));\n";
    $result .= "}\n";
	
    $result .= "void ${cname}Varicast${ename}(const NameBoolean &i_Variator".$chdrs.") const\n";
    $result .= "{\n";
    $result .= "  m_OutputPort.VariCast(i_Variator,Make${ename}Message(${args}));\n";
    $result .= "}\n";

    $result .= "void ${cname}Broadcast${ename}(".$hdrs.") const\n";
    $result .= "{\n";
    $result .= "  m_OutputPort.BroadCast(Make${ename}Message(${args}));\n";
    $result .= "}\n";
    $result .= "\n";
    
    return $result;
}
    


1;
