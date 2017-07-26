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

my $doit = shift || '';


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

my @issues = $issue->repos_issues({filter => 'all', state => 'open'});
push @issues, $issue->next_page() while $issue->has_next_page();
my @targets;

my %map = qw(
    Ken ksmurchison
    Robert rsto
    Partha ajaysusarla
    Bron brong
    ellie elliefm
);

my %have = map { $_ => [] } values %map;

foreach my $issue (sort { $a->{number} <=> $b->{number} } @issues) {
    # skip all shelved issues entirely
    next if grep { $_->{name} eq 'shelved' } @{$issue->{labels}};

    my $has_diceroll = grep { $_->{name} eq 'diceroll' } @{$issue->{labels}};

    if ($has_diceroll) {
        foreach my $assignee (@{$issue->{assignees}}) {
            push @{$have{$assignee->{login}}}, $issue;
        }
    }
    else {
        push @targets, $issue;
    }
}

my @list = shuffle @targets;

my $n = 0;

foreach my $name (sort keys %map) {
    print "$name ($map{$name})\n";
    my $have = $have{$map{$name}};
    foreach my $item (@$have) {
        printitem('=', $item);
    }
    for (scalar(@$have)..4) {
        my $item = $list[$n++];
        my @labels = map { $_->{name} } @{$item->{labels}};
        push @labels, 'diceroll';
        printitem('+', $item);
        $issue->update_issue($item->{number}, {
            assignee => $map{$name},
            labels => \@labels,
        }) if $doit eq 'doit';
    }
    print "\n";
}

sub printitem {
    my $char = shift;
    my $issue = shift;
    print " $char $issue->{html_url}\n";
    print "       $issue->{title}\n";
}
