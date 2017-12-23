while(<>)
{
    chomp;
    $isfinal = 1;
    if (/\\$/)
    {
	chop;
	$isfinal = 0;
    }
    @items = split();
    for ($i = 0 ; $i < @items ; ++$i)
    {
	next if ($items[$i] =~ /boost/);
	if ($items[$i] !~ /:/) { print ' '; }
	print $items[$i];
    }
    if ($isfinal) { print "\n";
    }
}

    
	
