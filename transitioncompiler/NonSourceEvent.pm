package NonSourceEvent;
use Event;
@ISA=qw(Event);

sub new
{
    my ($class,$name,$node) = @_;
    my $result = new Event($name,0,$node);
    bless $result,$class;
    return $result;
}

1;

sub GetMessageArgList
{
    my ($this) = @_;
    return join(",",map {"const " . $_->{TYPE} . "& " . $_->{NAME} } @{$this->{VARS}});
}

sub GetMessageStream
{
    my ($this) = @_;
    return join("",map {"\n  << ',' << UnCommaStringify(" . $_->{NAME} . ")" } @{$this->{VARS}});
}

sub GetActArgs()
{
    my ($this) = @_;
    return join(",",map { $_->{NAME} } @{$this->{VARS}});
}


