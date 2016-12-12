#!/usr/bin/perl

use HTTP::Tiny;

my $http = HTTP::Tiny->new();

foreach my $id (1..3938) {
  next if -f "bugs/$id.xml";
  print "Getting $id\n";
  my $data = $http->post_form("https://bugzilla.cyrusimap.org/show_bug.cgi", [ctype => 'xml', id => $id]);
  open(FH, ">bugs/$id.xml");
  print FH $data->{content};
  close(FH);
}
