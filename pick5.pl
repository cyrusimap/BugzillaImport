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

my %have = map { $_ => {} } values %map;

foreach my $issue (@issues) {
    my $has_diceroll = 0;
    foreach my $label (@{$issue->{labels}}) {
        next unless $label->{name} eq 'diceroll';
        $has_diceroll = 1;
        last;
    }

    if ($has_diceroll) {
        foreach my $assignee (@{$issue->{assignees}}) {
            $have{$assignee->{login}}{$issue->{html_url}} = 1;
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
    foreach my $uri (sort keys %$have) {
        print " = $uri\n";
    }
    for (scalar(keys %$have)..4) {
        my $item = $list[$n++];
        my @labels = map { $_->{name} } @{$item->{labels}};
        push @labels, 'diceroll';
        print " + $item->{html_url}\n";
        $issue->update_issue($item->{number}, {
            assignee => $map{$name},
            labels => \@labels,
        }) if $doit eq 'doit';
    }
    print "\n";
}
