use strict;
use warnings;
use Net::GitHub;
use Data::Dumper;
use XML::Fast;
use HTML::Entities;
use DateTime::Format::DateParse;
use Encode qw(decode_utf8 encode_utf8);
use JSON;
use List::Util qw(shuffle);

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
my @issues = $issue->repos_issues({filter => 'all', state => 'open'});
push @issues, $issue->next_page() while $issue->has_next_page();
my @list = shuffle grep { not has_diceroll($_) } @issues;

my $n = 0;

my %map = qw(
    Ken ksmurchison
    Robert rsto
    Partha ajaysusarla
    Bron brong
    ellie elliefm
);

delete $map{Bron};

foreach my $name (sort keys %map) {
    print "$name ($map{$name})\n";
    for (1..5) {
        my $item = $list[$n++];
        my @labels = map { $_->{name} } @{$item->{labels}};
        push @labels, 'diceroll';
        print " * $item->{html_url}\n";
        $issue->update_issue($item->{number}, {
            assignee => $map{$name},
            labels => \@labels,
        });
    }
    print "\n";
}

sub has_diceroll {
    my $issue = shift;
    foreach my $label (@{$issue->{labels}}) {
        next unless $label->{name} eq 'diceroll';
        return 1;
    }
    return 0;
}
