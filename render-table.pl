# ./uniquevalues rev_sequential | perl render-table.pl

use common::sense;

my $n;
my $h;

while(<>) {
  if (/^N = (\d+)/) {
    $n = $1;
    next;
  }

  if (/^distinct_count_(\w+).*? ([\d.]+) cycles/) {
    $h->{$n}->{$1} = $2;
  }

  die "not expected found in output" if /not expected/;
}

say "<table>";
say "  <tr><th>N</th><th>hash</th><th>sort</th><th>judy</th></tr>";
for my $i (sort { $a <=> $b } keys %$h) {
  my $v = $h->{$i};
  print "  <tr>";
  print "<td>$i</td>";
  for (qw/hash sort judy/) {
    print "<td>$v->{$_}</td>";
  }
  say "</tr>";
}
say "</table>";
