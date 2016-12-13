use strict;
use warnings;
use Net::GitHub;
use Data::Dumper;
use XML::Fast;
use HTML::Entities;
use DateTime::Format::DateParse;
use Encode qw(decode_utf8 encode_utf8);
use JSON;

BEGIN {
    Net::GitHub::V3::Issues->__build_methods(
        import_issue => { url => "/repos/%s/%s/import/issues", method => 'POST', args => 1 },
    );
};

open(FH, "<oauth_token.txt");
my $token = <FH>;
chomp($token);
close(FH);

my $gh = Net::GitHub::V3->new(
  access_token => $token,
);

$gh->ua->default_header(Accept => 'application/vnd.github.golden-comet-preview+json');

my $issue = $gh->issue;

$issue->set_default_user_repo('cyrusimap', 'cyrus-imapd');

use HTTP::Message;
$issue->ua->default_header('Accept-Encoding' => scalar HTTP::Message::decodable());
# for debugging
#$issue->ua->add_handler("request_send",  sub { shift->dump; return });
#$issue->ua->add_handler("response_done", sub { shift->dump; return });

my @labels = $issue->labels();
my %labels = map { $_->{name} => $_ } @labels;

my %have;
my @issues = $issue->repos_issues({filter => 'all', state => 'all'});
push @issues, $issue->next_page() while $issue->has_next_page();
foreach my $issue (@issues) {
    next unless $issue->{body} =~ m/Bugzilla-Id: (\d+)/s;
    $have{$1} = 1;
}

my @n;
opendir(DH, "bugs");
while (my $item = readdir(DH)) {
    next unless $item =~ m/^(\d+)\.xml/;
    push @n, $1;
}
closedir(DH);

my %ownermap = qw(
   brong@fastmail.fm brong
   ellie@fastmail.com elliefm
   murch@andrew.cmu.edu ksmurchison
);

foreach my $id (sort { $a <=> $b } @n) {
  next unless -f "bugs/$id.xml";
  next if $have{$id};
  open(FH, "<bugs/$id.xml");
  local $/ = undef;
  my $xml = <FH>;
  close(FH);
  my $data = xml2hash($xml);
  my $bug = $data->{bugzilla}{bug};
  next unless $bug->{short_desc};
  next unless $bug->{product} eq 'Cyrus IMAP';
  my @these = ($bug->{component}, $bug->{priority}, $bug->{bug_severity});
  foreach my $label (@these) {
    $labels{$label} ||= $issue->create_label({
      name => $label,
      color => '000000',
    });
  }
  my $owner = $ownermap{$bug->{assigned_to}{"#text"}};
  my @bits;
  push @bits, (assignee => $owner) if $owner;
  push @bits, (closed_at => _ts($bug->{delta_ts})) if $bug->{bug_status} eq 'CLOSED';
  push @bits, (updated_at => _ts($bug->{delta_ts}));
  push @bits, (closed => JSON::true) if $bug->{bug_status} eq 'CLOSED';
  my @comments;
  my %attachments;
  foreach my $attach (_ar($bug->{attachment})) {
     $attachments{$attach->{attachid}} = $attach;
  }
  foreach my $item (_ar($bug->{long_desc})) {
    push @comments, _make_comment($item, \%attachments);
  }

  my $hash = {
    issue => {
        title => $bug->{short_desc},
        body => _make_body($bug),
        labels => \@these,
        created_at => _ts($bug->{creation_ts}),
        @bits,
    },
    comments => [ sort { $a->{created_at} cmp $b->{created_at} } @comments ],
  };
  my $res = $issue->import_issue($hash);

  print "DID $bug->{bug_id}\n";
}

sub _make_body {
  my $bug = shift;
  my $from = _name($bug->{reporter});
  my $text = <<EOF;
From: $from
Bugzilla-Id: $bug->{bug_id}
Version: $bug->{version}
EOF
  $text .= "Owner: " . _name($bug->{assigned_to}) ."\n";
  return $text;
}

sub _make_comment {
  my $item = shift;
  my $attachments = shift;
  my $from = _name($item->{who});
  my $comment = enc($item->{thetext});
  my $body = <<EOF;
From: $from

$comment
EOF
  if ($comment =~ m/Created an attachment \(id=(\d+)\)/s) {
    my $item = $attachments->{$1};
    my $comment = enc($item->{desc});
    $body = <<EOF;
Attachment-Id: $item->{attachid}
From: $from
Type: $item->{type}
File: [$item->{filename}](https://github.com/cyrusimap/BugzillaImport/blob/master/attachments/$item->{attachid}/$item->{filename})

$comment
EOF
  }
  return {
    created_at => _ts($item->{bug_when}),
    body => $body,
  };
}

sub _ar {
  my $thing = shift;
  return unless defined $thing;
  return ($thing) unless ref($thing) eq 'ARRAY';
  return @$thing;
}

sub enc {
  my $str = shift;
  return encode_entities(encode_utf8($str));
}

sub _ts {
  my $when = DateTime::Format::DateParse->parse_datetime(shift);
  $when->set_time_zone('UTC');
  return $when->iso8601() . 'Z',
}

sub _name {
  my $item = shift;
  return $item unless ref($item);
  my $name = enc($item->{'-name'});
  my $email = $item->{'#text'};
  return "[$name]($email)";
}
