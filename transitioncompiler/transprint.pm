print "Name: ",$data->{NAME},"\n";
print "Status: ",$data->{STATUS},"\n";
print "Auto Recursion Limit: ",$data->{AUTOLIMIT},"\n";
print "States: \n";
foreach $key (keys %{$data->{STATE}})
{
    print "  $key: ", $data->{STATE}->{$key},"\n";
}
print "Transitions: \n";
foreach $key (keys %{$data->{TRANSITIONS}})
{
    print "  $key: \n";
    $data->{TRANSITIONS}->{$key}->Print;
}
print "Events: \n";
foreach $key (keys %{$data->{EVENTS}})
{
    print "  $key: \n";

    for ($i = 0 ; $i < scalar @{$data->{EVENTS}->{$key}} ; ++$i)
    {
	print "    ",$data->{EVENTS}->{$key}->[$i]->{TYPE}," ",$data->{EVENTS}->{$key}->[$i]->{NAME},";\n";
    }
}
print "Refresh: \n",$data->{REFRESH},"\n";
