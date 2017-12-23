package GlobalEvent;
use SourceEvent;
@ISA=qw(SourceEvent);

sub new
{
    my ($class,$name,$node) = @_;
    my $result = new SourceEvent($name,$node);
    bless $result,$class;
    return $result;
}

sub GetPublicHeaders
{
    my ($this) = @_;
    my $result;
    my $ename = $this->{NAME};

    $result .= "  void Unicast${ename}(const std::string &i_PlayerName) const;\n";
    $result .= "  void Varicast${ename}(const NameBoolean &i_Variator) const;\n";
    $result .= "  void Broadcast${ename}() const;\n";

    my $namesuffix = "";
    my $arglist = "";

    my $i;
    for ($i = 0 ; $i < @{$this->{ITERS}} ; ++$i)
    {
	my $iter = $this->{ITERS}->[$i];
	if ($i != 0)
	{
	    $namesuffix .= "And";
	    $arglist .= ",";
	}
	$namesuffix .= "By" . $iter->{NAME};
	$arglist .= "const " . $iter->{TYPE} . "& " . $iter->{NAME};

	$result .= "  void UnicastUpdate${ename}${namesuffix}(const std::string &i_PlayerName,\n        ${arglist}) const;\n";
	$result .= "  void VaricastUpdate${ename}${namesuffix}(const NameBoolean &i_Variator,\n        ${arglist}) const;\n";
	$result .= "  void BroadcastUpdate${ename}${namesuffix}(\n        ${arglist}) const;\n";
    }

    $result .= "\n";

    return $result;
}

sub GetPublicCodeBody
{
    my ($this,$oo) = @_;
    my $result;
    my $ename = $this->{NAME};
    my $cname = $oo . "GameInfo::";

    # iters have fields TYPE NAME START STOP

    undef $namesuffix;
    undef $actarglist;
    undef $arglist;
    undef @argvec;
    undef @actargvec;
    undef @namesuffixvec;

    my $i;
    for ($i = 0 ; $i < @{$this->{ITERS}} ; ++$i)
    {
	$iter = $this->{ITERS}->[$i];
	if ($i > 0)
	{
	    $namesuffix .= "And";
	    $actarglist .= ",";
	    $arglist .= ",";
	}
	$namesuffix .= "By" . $iter->{NAME};
	$actarglist .= $iter->{NAME};
	$arglist .= "const " . $iter->{TYPE} . "& " . $iter->{NAME};
	
	$argvec[$i] = $arglist;
	$actargvec[$i] = $actarglist;
	$namesuffixvec[$i] = $namesuffix;

    }

	
    for ($i = -1 ; $i < @{$this->{ITERS}} ; ++$i)
    {
	my $iter = undef;
	my $fname;
	my $argliststring;
	my $islast = ($i == @{$this->{ITERS}} - 1);

	my $cargs;
	my $args;

	if ($i == -1)
	{
	    $fname = ${ename};
	    $cargs = "";
	    $args = "";
	}
	else
	{
	    $fname = "Update" . ${ename} . $namesuffixvec[$i];
	    $cargs = ((length($argvec[$i]) > 0) ? "," . "\n      "  . $argvec[$i] : "");
	    $args = "\n      " .  $argvec[$i];
	}

	my $nextiter = $this->{ITERS}->[$i+1];
	my $iterstring = "for( " . $nextiter->{TYPE} . " " . $nextiter->{NAME} . " = " . $nextiter->{START} . " ; " .
	    $nextiter->{NAME} . " != " . $nextiter->{STOP} . " ; " . "++" . $nextiter->{NAME} . " ) {";
	my $aargs = $actargvec[$i];
	my $caargs = (length($aargs) > 0) ? "," . $aargs : "";

	my $nextaargs = $actargvec[$i+1];
	my $nextcaargs = (length($nextaargs) > 0) ? "," . $nextaargs : "";


	$result .= "void ${cname}Unicast${fname}(const std::string &i_PlayerName${cargs}) const\n";
	$result .= "{\n";
	if ($islast)
	{
	    $result .= "  m_OutputPort.UniCast(i_PlayerName,Make${ename}Message($aargs));\n";
	}
	else
	{
	    $result .= "  " . $iterstring . "\n";
	    $result .= "    UnicastUpdate${ename}" . $namesuffixvec[$i+1] . "(i_PlayerName${nextcaargs});\n";
	    $result .= "  }\n";
	}

	$result .= "}\n";
	$result .= "void ${cname}Varicast${fname}(const NameBoolean &i_Variator${cargs}) const\n";
	$result .= "{\n";
	if ($islast)
	{
	    $result .= "  m_OutputPort.VariCast(i_Variator,Make${ename}Message($aargs));\n";
	}	    
	else
	{
	    $result .= "  " . $iterstring . "\n";
	    $result .= "    VaricastUpdate${ename}" . $namesuffixvec[$i+1] . "(i_Variator${nextcaargs});\n";
	    $result .= "  }\n";
	}
	$result .= "}\n";
	$result .= "void ${cname}Broadcast${fname}(${args}) const\n";
	$result .= "{\n";
	if ($islast)
	{
	    $result .= "  m_OutputPort.BroadCast(Make${ename}Message($aargs));\n";
	}	    
	else
	{
	    $result .= "  " . $iterstring . "\n";
	    $result .= "    BroadcastUpdate${ename}" . $namesuffixvec[$i+1] . "(${nextaargs});\n";
	    $result .= "  }\n";
	}
	$result .= "}\n";
    }

    $result .= "\n";

    return $result;



}

sub OldWay
{
    $result .= "void ${cname}UnicastUpdate${ename}(const std::string &i_PlayerName".$chdrs.") const\n";
    $result .= "{\n";
    $result .= "  m_OutputPort.UniCast(i_PlayerName,Make${ename}Message(${args}));\n";
    $result .= "}\n";
	
    $result .= "void ${cname}VaricastUpdate${ename}(const NameBoolean &i_Variator".$chdrs.") const\n";
    $result .= "{\n";
    $result .= "  m_OutputPort.VariCast(i_Variator,Make${ename}Message(${args}));\n";
    $result .= "}\n";

    $result .= "void ${cname}BroadcastUpdate${ename}(".$hdrs.") const\n";
    $result .= "{\n";
    $result .= "  m_OutputPort.BroadCast(Make${ename}Message(${args}));\n";
    $result .= "}\n";

    $result .= "void ${cname}Unicast${ename}(const std::string &i_PlayerName) const\n";
    $result .= "{\n";
    $result .= "  ".$this->GetIterOpen()."\n";
    $result .= "  UnicastUpdate${ename}(i_PlayerName${cargs});\n";
    $result .= "  ".$this->GetIterClose()."\n";
    $result .= "}\n";
	
    $result .= "void ${cname}Varicast${ename}(const NameBoolean &i_Variator) const\n";
    $result .= "{\n";
    $result .= "  ".$this->GetIterOpen()."\n";
    $result .= "  VaricastUpdate${ename}(i_Variator${cargs});\n";
    $result .= "  ".$this->GetIterClose()."\n";
    $result .= "}\n";

    $result .= "void ${cname}Broadcast${ename}() const\n";
    $result .= "{\n";
    $result .= "  ".$this->GetIterOpen()."\n";
    $result .= "  BroadcastUpdate${ename}(${args});\n";
    $result .= "  ".$this->GetIterClose()."\n";
    $result .= "}\n";
    $result .= "\n";
    
    return $result;
}
    


1;
