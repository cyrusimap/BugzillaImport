#!/usr/bin/perl

use HTTP::Tiny;
use XML::Fast qw(xml2hash);
use MIME::Base64 qw(decode_base64);

my $http = HTTP::Tiny->new();

my @n;
opendir(DH, "bugs");
while (my $item = readdir(DH)) {
    next unless $item =~ m/^(\d+)\.xml/;
    push @n, $1;
}
closedir(DH);

foreach my $id (sort { $a <=> $b } @n) {
  next unless -f "bugs/$id.xml";
  open(FH, "<bugs/$id.xml");
  local $/ = undef;
  my $xml = <FH>;
  close(FH);
  my $data = xml2hash($xml);
  my $bug = $data->{bugzilla}{bug};
  foreach my $attach (_ar($bug->{attachment})) {
    next if -d "attachments/$attach->{attachid}";
    my $data = delete $attach->{data};
    if (eval { $data->{"-encoding"} eq 'base64' }) {
      $data = decode_base64($data->{"#text"});
    }
    mkdir "attachments/$attach->{attachid}";
    use Data::Dumper;
    open(FH, ">attachments/$attach->{attachid}/$attach->{filename}");
    print FH $data;
    close(FH);
    print Dumper($attach);
  }
}

sub _ar {
  my $thing = shift;
  return unless defined $thing;
  return ($thing) unless ref($thing) eq 'ARRAY';
  return @$thing;
}
