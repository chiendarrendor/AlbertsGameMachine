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

    # we will open two files. the GameInfo .hpp, and
    # the DLL.cpp
    my $fname = $name . "DLL.cpp";
    open(DLLCPP,">$fname") || die("Can't open file $fname for write.\n");

    $fname = $name . "GameInfo.hpp";
    open(GIHPP,">$fname") || die("Can'd open file $fname for write.\n");

    my $hname= $name . "GAMEINFOHPP";
    print GIHPP "#ifndef $hname\n";
    print GIHPP "#define $hname\n";

    # Must come before GameInfo.hpp: this file's own UnCommaStringify()
    # template method calls the non-template UnComma(), and GCC's two-phase
    # template lookup needs that declaration visible when the template is
    # first *parsed*, not just when it's instantiated -- whether that's true
    # depends on this file's own include order, not the includer's.
    print GIHPP "#include \"StringUtilities.hpp\"\n";
    print GIHPP "#include \"GameInfo.hpp\"\n";
    print GIHPP "#include \"${name}Set.hpp\"\n";
    print GIHPP "#include \"${name}State.hpp\"\n";
    print GIHPP "#include \"StateMachine.hpp\"\n";
    print GIHPP "#include <boost/json.hpp>\n";
    print GIHPP "\n";
    

    print DLLCPP "#include <sstream>\n";
    print DLLCPP "#include <set>\n";
    print DLLCPP "#include <stdexcept>\n";
    print DLLCPP "#include <boost/lexical_cast.hpp>\n";
    print DLLCPP "#include <boost/json.hpp>\n";
    print DLLCPP "#include \"JsonParamExtraction.hpp\"\n";
    print DLLCPP "#include \"GameActionError.hpp\"\n";
    print DLLCPP "#include \"StringUtilities.hpp\"\n";
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

    print GIHPP "public:\n";


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

	print GIHPP "  void ${tname}_ExecuteAction(const std::string &i_PlayerName,\n";
	print GIHPP "                              const ActionParser &i_ap,\n";
	print GIHPP "                              const StateName &i_StateName);\n";

	print DLLCPP "void ${name}GameInfo::${tname}_ExecuteAction(const std::string &i_PlayerName,\n";
	print DLLCPP "                                             const ActionParser &i_ap,\n";
	print DLLCPP "                                             const StateName &i_StateName)\n";
	print DLLCPP "{\n";
	print DLLCPP FromStateValidator($trans,$name);

	my $actionsize = (exists $trans->{ACTIONS}) ? scalar @{$trans->{ACTIONS}} : 0;

	print DLLCPP "  if (i_ap.GetParams().size() != ",$actionsize,")\n";
	print DLLCPP "  {\n";
        print DLLCPP "    throw GameActionError(\"Bad number of arguments to ${tname}\");\n";
        print DLLCPP "  }\n";
	print DLLCPP "\n";

	for ($i = 0 ; $i < $actionsize ; ++$i)
	{
	    my $action = $trans->{ACTIONS}->[$i];
	    my $atype = $action->{TYPE};
	    my $aname = $action->{NAME};

	    print DLLCPP "  $atype $aname;\n";
	    print DLLCPP "  try\n";
	    print DLLCPP "  {\n";
	    print DLLCPP "    $aname = ExtractJsonParam<$atype>(i_ap.GetParams(),\"$aname\");\n";
	    print DLLCPP "  }\n";
	    print DLLCPP "  catch (const std::exception &)\n";
	    print DLLCPP "  {\n";
	    print DLLCPP "    throw GameActionError(\"Malformed arguments to ${tname}\");\n";
	    print DLLCPP "  }\n";
	    print DLLCPP "\n";
	    if (exists $action->{MIN})
	    {
		print DLLCPP "  if ($aname < ",$action->{MIN},")\n";
		print DLLCPP "  {\n";
		print DLLCPP "    throw GameActionError(\"Action $tname variable $aname is below minimum\");\n";
		print DLLCPP "  }\n";
		print DLLCPP "\n";
	    }
	    if (exists $action->{MAX})
	    {
		print DLLCPP "  if ($aname > ",$action->{MAX},")\n";
		print DLLCPP "  {\n";
		print DLLCPP "    throw GameActionError(\"Action $tname variable $aname is above maximum\");\n";
		print DLLCPP "  }\n";
		print DLLCPP "\n";
	    }
	    if (exists $action->{LIST})
	    {
		print DLLCPP "  std::set<$atype> validlist(",$action->{LIST},");\n";
		print DLLCPP "  std::set<$atype>::iterator findit = validlist.find($aname);\n";
		print DLLCPP "  if (findit == validlist.end())\n";
		print DLLCPP "  {\n";
		print DLLCPP "    throw GameActionError(\"Action $tname variable $aname has illegal value\");\n";
		print DLLCPP "  }\n";
		print DLLCPP "\n";
	    }



	}

	print DLLCPP "  ",$trans->{BODY},";\n";
        print DLLCPP "}\n";
	print DLLCPP "\n";
    }

    print DLLCPP "\n";

    print GIHPP "  boost::json::object GetClientState(const std::string &i_PlayerName,const StateName &i_StateName) const;\n";
    print GIHPP "\n";

    # <sequence> methods claim their own corner of the generated namespace --
    # "<Name>Count"/"<Name>Body", no underscore -- distinct in both direction
    # and spelling from every other generated name pattern here (the
    # per-transition "<Name>_IsLegal"/"<Name>_IsAuto"/"<Name>_ExecuteAction"/
    # "<Name>_Transition" family all use a leading underscore before a
    # different word; the one fixed, ungenerated name is "GetClientState").
    # A sequence named e.g. "ClientState" would generate "ClientStateCount"/
    # "ClientStateBody", textually distinct from "GetClientState" itself; a
    # sequence reusing a <state> name (data members, not functions, with no
    # suffix at all) is likewise never ambiguous. See docs/Sequences.txt.
    foreach my $seqname (keys %{$tcom->{SEQUENCES}})
    {
	print GIHPP "  int ${seqname}Count(const std::string &i_PlayerName) const;\n";
	print GIHPP "  boost::json::array ${seqname}Body(const std::string &i_PlayerName,size_t i_start,size_t i_end) const;\n";
    }

    # The glue layer DLLGame.hpp's generic code actually calls: it has no
    # way to write "m_GameInfo.SpendsCount(...)" literally (a game's
    # sequence names don't exist at the generic-template level, only here
    # in generated code), so it needs one fixed-name entry point per
    # operation that dispatches on a runtime string instead -- the same
    # problem StateWalker::Execute already solves for transitions, just via
    # an if-chain here rather than a registered object collection, since no
    # game is expected to have many sequences. Generated unconditionally
    # (even with zero <sequence>s) so DLLGame can call it uniformly; an
    # unrecognized name is a GameActionError, same convention every other
    # generated rejection already uses, so whatever calls this can catch it
    # exactly like DLLGame::HandleAction already catches one from a <body>.
    # Fixed names, not per-sequence ones, so collision is already covered by
    # the per-sequence-name reasoning above -- the only theoretical overlap
    # is a sequence literally named "Sequence", which would just overload
    # (different parameter count), not collide.
    print GIHPP "  int SequenceCount(const std::string &i_SequenceName,const std::string &i_PlayerName) const;\n";
    print GIHPP "  boost::json::array SequenceBody(const std::string &i_SequenceName,const std::string &i_PlayerName,size_t i_start,size_t i_end) const;\n";
    # Every defined sequence's current count, by name, all at once -- what
    # BuildStateWalkerEnvelope (gamecommon/DLLGame.hpp) actually calls to
    # populate the envelope's "sequences" object (docs/Sequences.txt: a
    # sequence-watching widget's whole mechanism for learning there's more
    # data to fetch is this count changing between deliveries). Plural, not
    # "SequenceCount" again -- no parameter collision risk either way, since
    # the arg lists differ, but the name itself stays unambiguous.
    print GIHPP "  boost::json::object SequenceCounts(const std::string &i_PlayerName) const;\n";
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

    print DLLCPP "boost::json::object ${name}GameInfo::GetClientState(const std::string &i_PlayerName,const StateName &i_StateName) const\n";
    print DLLCPP "{\n";
    print DLLCPP $tcom->{REFRESH},"\n";
    print DLLCPP "}\n";
    print DLLCPP "\n";

    foreach my $seqname (keys %{$tcom->{SEQUENCES}})
    {
	my $seq = $tcom->{SEQUENCES}->{$seqname};

	# <count> is a bare expression (like <allowed>/<auto>), so it needs
	# wrapping in a return statement; <body> is a full statement block
	# with its own return (like <refresh>), so it's spliced as-is.
	print DLLCPP "int ${name}GameInfo::${seqname}Count(const std::string &i_PlayerName) const\n";
	print DLLCPP "{\n";
	print DLLCPP "  return ",$seq->{COUNT},";\n";
	print DLLCPP "}\n";
	print DLLCPP "\n";

	print DLLCPP "boost::json::array ${name}GameInfo::${seqname}Body(const std::string &i_PlayerName,size_t i_start,size_t i_end) const\n";
	print DLLCPP "{\n";
	print DLLCPP $seq->{BODY},"\n";
	print DLLCPP "}\n";
	print DLLCPP "\n";
    }

    print DLLCPP "int ${name}GameInfo::SequenceCount(const std::string &i_SequenceName,const std::string &i_PlayerName) const\n";
    print DLLCPP "{\n";
    foreach my $seqname (keys %{$tcom->{SEQUENCES}})
    {
	print DLLCPP "  if (i_SequenceName == \"$seqname\") return ${seqname}Count(i_PlayerName);\n";
    }
    print DLLCPP "  throw GameActionError(\"Unknown sequence: \" + i_SequenceName);\n";
    print DLLCPP "}\n";
    print DLLCPP "\n";

    print DLLCPP "boost::json::array ${name}GameInfo::SequenceBody(const std::string &i_SequenceName,const std::string &i_PlayerName,size_t i_start,size_t i_end) const\n";
    print DLLCPP "{\n";
    foreach my $seqname (keys %{$tcom->{SEQUENCES}})
    {
	print DLLCPP "  if (i_SequenceName == \"$seqname\") return ${seqname}Body(i_PlayerName,i_start,i_end);\n";
    }
    print DLLCPP "  throw GameActionError(\"Unknown sequence: \" + i_SequenceName);\n";
    print DLLCPP "}\n";
    print DLLCPP "\n";

    print DLLCPP "boost::json::object ${name}GameInfo::SequenceCounts(const std::string &i_PlayerName) const\n";
    print DLLCPP "{\n";
    print DLLCPP "  boost::json::object result;\n";
    foreach my $seqname (keys %{$tcom->{SEQUENCES}})
    {
	print DLLCPP "  result[\"$seqname\"] = ${seqname}Count(i_PlayerName);\n";
    }
    print DLLCPP "  return result;\n";
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

	print GIHPP "  virtual void ExecuteAction(const StateName &i_StateName,${name}GameInfo &i_gi,\n";
        print GIHPP "                             const std::string &i_PlayerName,\n";
        print GIHPP "                             const ActionParser &i_ap) const;\n";


	print DLLCPP "void ${tname}_Transition::ExecuteAction(const StateName &i_StateName,${name}GameInfo &i_gi,\n";
        print DLLCPP "                           const std::string &i_PlayerName,\n";
        print DLLCPP "                           const ActionParser &i_ap) const\n";
	print DLLCPP "{\n";
        print DLLCPP "  i_gi.${tname}_ExecuteAction(i_PlayerName,i_ap,i_StateName);\n";
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

    close DLLCPP;
}

1;
