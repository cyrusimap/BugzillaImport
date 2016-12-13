#!/usr/bin/perl

use HTTP::Tiny;
use XML::Fast qw(xml2hash);

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
  my $item = $data->{bugzilla}{bug};
  next unless $item->{short_desc};
  print "$id: $item->{product} $item->{priority} $item->{short_desc} ($item->{bug_status})\n";
}
