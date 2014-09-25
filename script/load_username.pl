use strict;
use warnings;
use utf8;
use DBIx::Sunny;
use Cache::Memcached::Fast;

my $memd = Cache::Memcached::Fast->new({servers => [ "localhost:11212" ]});
my $dbh = DBIx::Sunny->connect(
  "dbi:mysql:database=isucon;host=localhost;port=3336", "isucon", "", {
      RaiseError => 1,
      PrintError => 0,
      AutoInactiveDestroy => 1,
      mysql_enable_utf8   => 1,
      mysql_auto_reconnect => 1,
  },
);

my $all = $dbh->select_all("SELECT id,username FROM users;");

for my $user (@$all) {
  $memd->set('uid-' . $user->{id}, $user->{username});
}
