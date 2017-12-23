


$state[$statecount++] = "InitialState";
$state[$statecount++] = "TerminalState";



while(<>)
{
    chomp;
    if (/\<state name=\"([^\"]+)\"/)
    {
	$state[$statecount++] = $1;
    }
    if (/\<transition name=\"([^\"]+)\"/)
    {
	$curtransition = $1;
    }
    if (/\<cyclic state=\"([^\"]+)\"/)
    {
	$newtrans = {};
	$newtrans->{NAME} = $curtransition;
	$newtrans->{FROM} = $1;
	$newtrans->{TO} = $1;
	$newtrans->{AUTOSTATE} = 0;
	push @trans,$newtrans;
    }
    if (/\<transit from=\"([^\"]+)\" to=\"([^\"]+)\"/)
    {
	$newtrans = {};
	$newtrans->{NAME} = $curtransition;
	$newtrans->{FROM} = $1;
	$newtrans->{TO} = $2;
	$newtrans->{AUTOSTATE} = 0;
	push @trans,$newtrans;
    }
    if (/\<allowed\>false\<\/allowed\>/)
    {
	$newtrans->{AUTOSTATE} += 1;
    }
    if (/\<auto\>true\<\/auto\>/)
    {
	$newtrans->{AUTOSTATE} += 2;
    }
    
}

$color[0] = "red";
$color[1] = "yellow";
$color[2] = "blue";
$color[3] = "green";

print "digraph Outpost {\n";
for ($i = 0 ; $i < $statecount ; ++$i)
{
    print $state[$i]," [label=\"",$state[$i],"\"];\n";
}
for ($i = 0 ; $i < @trans ; ++$i)
{
    print $trans[$i]->{FROM}," -> ",$trans[$i]->{TO}," [label=\"",$trans[$i]->{NAME},"\"";
    print " color = ",$color[$trans[$i]->{AUTOSTATE}];
    print "];\n";n
}

print "}\n";
