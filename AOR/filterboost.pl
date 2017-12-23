
my @targets;

while(<>)
{
    chomp;
    if (/\\$/) { chop; }
    split;

    for ($i = 0 ; $i < @_ ; ++$i)
    {
	if ($_[$i] =~ /(.*):$/)
	{
	    $tname = $1;
	    my $t = {};
	    $t->{TNAME} = $1;
	    $t->{DEPS}=[];
	    unshift @targets,$t;
	}
	elsif ($_[$i] !~ /boost-1_33_1/)
	{
	    push @{$targets[0]->{DEPS}},$_[$i];
	}
    }
}

foreach $target (reverse @targets)
{
    $tname = $target->{TNAME};
    print $tname,":";
    $occount = length($tname) + 1;
    $ccount = $occount;

    for ($i = 0 ; $i < @{$target->{DEPS}} ; ++$i)
    {
	$len = length($target->{DEPS}->[$i]);

	if ($len + $ccount > 78)
	{
	    print " \\\n";
	    print " " x (length($tname) + 1);
	    $ccount = $occount;
	}
	
	print " ",$target->{DEPS}->[$i];
	$ccount += $len + 1;
    }
    print "\n";
}

	    
