package FileWriter;

use Exporter 'import';
@EXPORT = qw(WriteFiles);

sub FromStateValidator
{
    my ($trans,$gamename) = @_;
    my $result;

    $result .= "  if(";
    my $i;
    for ($i = 0 ; $i < @{$trans->{TRANSITIONS}} ; ++$i)
    {
	if ($i != 0)
	{
	    $result .= "     && ";
	}
	$result .= "i_StateName != ${gamename}GameInfo::" . $trans->{TRANSITIONS}->[$i]->{FROM} . "\n";
    }
    $result .= "  )\n";
    $result .= "  {\n";
    $result .= "    std::ostringstream oss;\n";
    $result .= "    oss << \"Unknown state \" << i_StateName << \" in transition \" << \"" . 
	$trans->{NAME} . "\";\n";
    $result .= "    throw std::out_of_range(oss.str());\n";
    $result .= "  }\n";
    $result .= "\n";

    return $result;
}

    



sub WriteFiles
{
    my ($tcom) = @_;
    my $name = $tcom->{NAME};
    my $autolimit = $tcom->{AUTOLIMIT};

    # we will open two files. the GameInfo .hpp, and
    # the DLL.cpp
    my $fname = $name . "DLL.cpp";
    open(DLLCPP,">$fname") || die("Can't open file $fname for write.\n");

    $fname = $name . "GameInfo.hpp";
    open(GIHPP,">$fname") || die("Can'd open file $fname for write.\n");

    my $hname= $name . "GAMEINFOHPP";
    print GIHPP "#ifndef $hname\n";
    print GIHPP "#define $hname\n";

    print GIHPP "#include \"GameInfo.hpp\"\n";
    print GIHPP "#include \"${name}Set.hpp\"\n";
    print GIHPP "#include \"${name}State.hpp\"\n";
    print GIHPP "#include \"StateMachine.hpp\"\n";
    print GIHPP "\n";
    print GIHPP "class NameBoolean;\n";
    print GIHPP "\n";
    

    print DLLCPP "#include <sstream>\n";
    print DLLCPP "#include <set>\n";
    print DLLCPP "#include <stdexcept>\n";
    print DLLCPP "#include <boost/lexical_cast.hpp>\n";
    print DLLCPP "#include \"StringUtilities.hpp\"\n";
    print DLLCPP "#include \"GameBoxDLL.hpp\"\n";
    print DLLCPP "#include \"${name}GameInfo.hpp\"\n";
    print DLLCPP "#include \"DLLGame.hpp\"\n";
    print DLLCPP "\n";
    print DLLCPP $tcom->{PREFACE};
    print DLLCPP "\n";

    
    foreach my $evname (keys %{$tcom->{EVENTS}})
    {
	print DLLCPP $tcom->{EVENTS}->{$evname}->GetPublicCodeBody($name);
	print DLLCPP $tcom->{EVENTS}->{$evname}->GetPrivateCodeBody($name);
    }

    print DLLCPP "\n";

    print GIHPP "class ${name}GameInfo : public GameInfo<${name}Set,${name}State>\n";
    print GIHPP "{\n";
    print GIHPP "public:\n";

    print GIHPP "  ${name}GameInfo(const ${name}Set &i_Set,\n";
    print GIHPP "                  const ServerGameInfo &i_ginfo,\n";
    print GIHPP "                  OutputPort $i_OutputPort &i_Port);\n";

    print DLLCPP "${name}GameInfo::${name}GameInfo(const ${name}Set &i_Set,\n";
    print DLLCPP "                                 const ServerGameInfo &i_ginfo,\n";
    print DLLCPP "                                 OutputPort $i_OutputPort &i_Port) :\n";
    print DLLCPP "  GameInfo<${name}Set,${name}State>(i_Set,i_ginfo,i_Port) {}\n";
    print DLLCPP "\n";

    print GIHPP "private:\n";
    foreach $state (keys %{$tcom->{STATE}})
    {
	print GIHPP "  static const StateName $state;\n";
    }

    print GIHPP "  mutable int m_errorcount;\n";
    print GIHPP "public:\n";
    print GIHPP "  void ResetERRORCount();\n";
    print GIHPP "  int GetERRORCount() const;\n";
    
    print DLLCPP "void ${name}GameInfo::ResetERRORCount() { m_errorcount = 0; }\n";
    print DLLCPP "int ${name}GameInfo::GetERRORCount() const { return m_errorcount; }\n";


    foreach $transkey (keys %{$tcom->{TRANSITIONS}})
    {
	my $trans = $tcom->{TRANSITIONS}->{$transkey};
	my $tname = $trans->{NAME};

	print GIHPP "  bool ${tname}_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const;\n";

	print DLLCPP "bool ${name}GameInfo::${tname}_IsLegal(const std::string &i_PlayerName,const StateName &i_StateName) const\n";
	print DLLCPP "{\n";
	print DLLCPP FromStateValidator($trans,$name);
	print DLLCPP "  return ",$trans->{ALLOWED},";\n";
        print DLLCPP "}\n";
	print DLLCPP "\n";

	print GIHPP "  bool ${tname}_IsAuto(const StateName &i_StateName) const;\n";

	print DLLCPP "bool ${name}GameInfo::${tname}_IsAuto(const StateName &i_StateName) const\n";
	print DLLCPP "{\n";
	print DLLCPP FromStateValidator($trans,$name);
	print DLLCPP "  return ",$trans->{AUTO},";\n";
        print DLLCPP "}\n";
	print DLLCPP "\n";

	print GIHPP "  bool ${tname}_ExecuteAction(const std::string &i_PlayerName,\n";
	print GIHPP "                              const ActionParser &i_ap,\n";
	print GIHPP "                              const StateName &i_StateName);\n";

	print DLLCPP "bool ${name}GameInfo::${tname}_ExecuteAction(const std::string &i_PlayerName,\n";
	print DLLCPP "                                             const ActionParser &i_ap,\n";
	print DLLCPP "                                             const StateName &i_StateName)\n";
	print DLLCPP "{\n";
	print DLLCPP FromStateValidator($trans,$name);
	
	my $actionsize = (exists $trans->{ACTIONS}) ? scalar @{$trans->{ACTIONS}} : 0;

	print DLLCPP "  if (i_ap.GetNumArguments() != ",$actionsize,")\n";
	print DLLCPP "  {\n";
        print DLLCPP "    UnicastERROR(i_PlayerName,\"Bad number of arguments to ${tname}\");\n";
	print DLLCPP "    return false;\n";
        print DLLCPP "  }\n";
	print DLLCPP "\n";

	for ($i = 0 ; $i < $actionsize ; ++$i)
	{
	    my $action = $trans->{ACTIONS}->[$i];
	    my $atype = $action->{TYPE};
	    my $aname = $action->{NAME};
	    
	    print DLLCPP "  $atype $aname(",$action->{TRANS},"(i_ap[${i}]));\n";
	    print DLLCPP "\n";
	    if (exists $action->{MIN})
	    {
		print DLLCPP "  if ($aname < ",$action->{MIN},")\n";
		print DLLCPP "  {\n";
		print DLLCPP "    UnicastERROR(i_PlayerName,\"Action $tname variable $aname is below minimum\");\n";
		print DLLCPP "    return false;\n";
		print DLLCPP "  }\n";
		print DLLCPP "\n";
	    }
	    if (exists $action->{MAX})
	    {
		print DLLCPP "  if ($aname > ",$action->{MAX},")\n";
		print DLLCPP "  {\n";
		print DLLCPP "    UnicastERROR(i_PlayerName,\"Action $tname variable $aname is above maximum\");\n";
		print DLLCPP "    return false;\n";
		print DLLCPP "  }\n";
		print DLLCPP "\n";
	    }
	    if (exists $action->{LIST})
	    {
		print DLLCPP "  std::set<$atype> validlist(",$action->{LIST},");\n";
		print DLLCPP "  std::set<$atype>::iterator findit = validlist.find($aname);\n";
		print DLLCPP "  if (findit == validlist.end())\n";
		print DLLCPP "  {\n";
		print DLLCPP "    UnicastERROR(i_PlayerName,\"Action $tname variable $aname has illegal value\");\n";
		print DLLCPP "    return false;\n";
		print DLLCPP "  }\n";
		print DLLCPP "\n";
	    }



	}

	print DLLCPP "  ",$trans->{BODY},";\n";
        print DLLCPP "}\n";
	print DLLCPP "\n";
    }

    print DLLCPP "\n";

    print GIHPP "  void SendFullState(const std::string &i_PlayerName,const StateName &i_StateName) const;\n";
    print GIHPP "\n";
    print GIHPP "\n";

    foreach my $evname (keys %{$tcom->{EVENTS}})
    {
	print GIHPP $tcom->{EVENTS}->{$evname}->GetPublicHeaders();
    }

    print GIHPP "private:\n";

    print GIHPP "  template<class T_ArgType> std::string UnCommaStringify(const T_ArgType &i_arg) const\n";
    print GIHPP "  {\n";
    print GIHPP "    std::ostringstream oss;\n";
    print GIHPP "    oss << i_arg;\n";
    print GIHPP "    return UnComma(oss.str());\n";
    print GIHPP "  }\n";
    print GIHPP "\n";

    foreach my $evname (keys %{$tcom->{EVENTS}})
    {
	print GIHPP $tcom->{EVENTS}->{$evname}->GetPrivateHeaders();
    }

    print GIHPP "};\n";

    print DLLCPP "void ${name}GameInfo::SendFullState(const std::string &i_PlayerName,const StateName &i_StateName) const\n";
    print DLLCPP "{\n";
    print DLLCPP $tcom->{REFRESH},"\n";
    print DLLCPP "}\n";
    print DLLCPP "\n";
    
    foreach $state (keys %{$tcom->{STATE}})
    {
	print DLLCPP "const StateName ${name}GameInfo::$state = \"$state\";\n";
    }



    print DLLCPP "typedef DLLGame<${name}Set,${name}GameInfo> ${name}Game;\n";
    print DLLCPP "\n";

    foreach $transkey (keys %{$tcom->{TRANSITIONS}})
    {
	my $trans = $tcom->{TRANSITIONS}->{$transkey};
	my $tname = $trans->{NAME};

	print GIHPP "class ${tname}_Transition : public Transition<${name}GameInfo>\n";
	print GIHPP "{\n";
	print GIHPP "public:\n";
	print GIHPP "  virtual bool IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const ${name}GameInfo &i_gi) const;\n";
	
	print DLLCPP "bool ${tname}_Transition::IsLegal(const std::string &i_PlayerName,const StateName &i_StateName,const ${name}GameInfo &i_gi) const\n";
	print DLLCPP "{\n";
        print DLLCPP "  return i_gi.${tname}_IsLegal(i_PlayerName,i_StateName);\n";
	print DLLCPP "}\n";
        print DLLCPP "\n";

	print GIHPP "  virtual bool IsAuto(const StateName &i_StateName,const ${name}GameInfo &i_gi) const;\n";

	print DLLCPP "bool ${tname}_Transition::IsAuto(const StateName &i_StateName,const ${name}GameInfo &i_gi) const\n";
	print DLLCPP "{\n";
        print DLLCPP "  return i_gi.${tname}_IsAuto(i_StateName);\n";
	print DLLCPP "}\n";
        print DLLCPP "\n";

	print GIHPP "  virtual bool ExecuteAction(const StateName &i_StateName,${name}GameInfo &i_gi,\n";
        print GIHPP "                             const std::string &i_PlayerName,\n";
        print GIHPP "                             const ActionParser &i_ap) const;\n";


	print DLLCPP "bool ${tname}_Transition::ExecuteAction(const StateName &i_StateName,${name}GameInfo &i_gi,\n";
        print DLLCPP "                           const std::string &i_PlayerName,\n";
        print DLLCPP "                           const ActionParser &i_ap) const\n";
	print DLLCPP "{\n";
        print DLLCPP "  return i_gi.${tname}_ExecuteAction(i_PlayerName,i_ap,i_StateName);\n";
	print DLLCPP "}\n";
	print DLLCPP "\n";

	print GIHPP "  virtual std::string GetName() const;\n";

	print DLLCPP "std::string ${tname}_Transition::GetName() const\n";
	print DLLCPP "{\n";
	print DLLCPP "  return \"${tname}\";\n";
        print DLLCPP "}\n";

	print GIHPP "};\n";

        print DLLCPP "\n";
    }

    print GIHPP "\n";
    print GIHPP "class ${name}StateMachine : public StateMachine<${name}GameInfo>\n";
    print GIHPP "{\n";
    print GIHPP "public:\n";
    print GIHPP "  ${name}StateMachine();\n";

    print DLLCPP "${name}StateMachine::${name}StateMachine()\n";
    print DLLCPP "{\n";

    foreach $state (keys %{$tcom->{STATE}})
    {
	my $sdesc = $tcom->{STATE}->{$state};
	print DLLCPP "  StateType *p${state} = new StateType(\"$state\",\"$sdesc\");\n";
	print DLLCPP "  InsertState(p${state});\n";
    }

    foreach $transkey (keys %{$tcom->{TRANSITIONS}})
    {
	my $trans = $tcom->{TRANSITIONS}->{$transkey};
	my $tname = $trans->{NAME};
	print DLLCPP "  TransitionType *p${tname}_Transition = new ${tname}_Transition;\n";
	print DLLCPP "  InsertTransition(p${tname}_Transition);\n";
    }
    print DLLCPP "\n";
    foreach $transkey (keys %{$tcom->{TRANSITIONS}})
    {
	my $trans = $tcom->{TRANSITIONS}->{$transkey};
	my $tname = $trans->{NAME};
	for ($i = 0 ; $i < @{$trans->{TRANSITIONS}} ; ++$i)
	{
	    my $fromstate = $trans->{TRANSITIONS}->[$i]->{FROM};
	    my $tostate = $trans->{TRANSITIONS}->[$i]->{TO};
	    
	    print DLLCPP "  p${fromstate}->InsertTransition(*p${tname}_Transition,*p${tostate});\n";
	}
    }    

    print DLLCPP "}\n";

    print GIHPP "};\n";
    print GIHPP "\n#endif\n\n";

    print DLLCPP "\n";
    print DLLCPP "${name}Set *pGameSet;\n";
    print DLLCPP "${name}StateMachine *pStateMachine;\n";
    print DLLCPP "\n";
    print DLLCPP "bool Initialize(const std::string &i_DataDir)\n";
    print DLLCPP "{\n";
    print DLLCPP "  pStateMachine = new ${name}StateMachine;\n";
    print DLLCPP "  pGameSet = new ${name}Set(i_DataDir);\n";
    print DLLCPP "  if (!pGameSet || !pGameSet->IsValid())\n";
    print DLLCPP "  {\n";
    print DLLCPP "    return false;\n";
    print DLLCPP "  }\n";
    print DLLCPP "  return true;\n";
    print DLLCPP "}\n";
    print DLLCPP "\n";
    print DLLCPP "Game *CreateGame(const ServerGameInfo &i_rServerGameInfo,OutputPort &i_rOutputPort)\n";
    print DLLCPP "{\n";
    print DLLCPP "  ${name}Game *pNewGame = new ${name}Game(i_rServerGameInfo,i_rOutputPort,\n";
    print DLLCPP "                                   *pGameSet,*pStateMachine,$autolimit);\n";
    print DLLCPP "  return pNewGame;\n";
    print DLLCPP "}\n";

    close DLLCPP;
}

1;
