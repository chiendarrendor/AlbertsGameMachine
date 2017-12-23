package SourceEvent;
use Event;
@ISA=qw(Event);
sub new
{
    my ($class,$name,$node) = @_;
    my $result = new Event($name,1,$node);
    bless $result,$class;
    return $result;
}

sub GetMessageArgList
{
    my ($this) = @_;
    return join(",",map {"const " . $_->{TYPE} . "& " . $_->{NAME} } @{$this->{ITERS}});
}

sub GetMessageStream
{
    my ($this) = @_;
    return join("",map {"\n  << ',' << UnCommaStringify(" . $_->{SOURCE} . ")" } @{$this->{VARS}});
}

sub GetIterOpen
{
    my ($this) = @_;
    return join("\n",
		map { "for (" . 
		      $_->{TYPE} . " " . $_->{NAME} . "=" . $_->{START} . ";" .
		      $_->{NAME} . "!=" . $_->{STOP} . ";" .
		      "++" . $_->{NAME} . ") {" } 
		@{$this->{ITERS}});
}

sub GetIterClose
{
    my ($this) = @_;
    return join("",map { "}" }  @{$this->{ITERS}});
}

sub GetActArgs()
{
    my ($this) = @_;
    return join(",",map { $_->{NAME} } @{$this->{ITERS}});
}

    


1;
