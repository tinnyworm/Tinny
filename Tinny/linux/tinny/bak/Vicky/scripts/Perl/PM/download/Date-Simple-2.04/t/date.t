#!/usr/bin/perl -w
use strict;
use Test;

BEGIN { plan tests => 164 }

use Date::Simple;

#------------------------------------------------------------------------------
# Check validate method
#------------------------------------------------------------------------------
ok(Date::Simple->new(2000, 12, 25));
ok(not Date::Simple->new(2000, 13, 25));
ok(not Date::Simple->new(2000, 0, 25));
ok(not Date::Simple->new(2000, 12, 32));
ok(not Date::Simple->new(2000, 12, 0));
ok(Date::Simple->new(1996, 02, 29));
ok(not Date::Simple->new(1900, 02, 29));

ok(Date::Simple->new('2000-12-25'));
ok(not Date::Simple->new('2000-13-25'));
ok(not Date::Simple->new('2000-00-25'));
ok(not Date::Simple->new('2000-12-32'));
ok(not Date::Simple->new('2000-12-00'));
ok(Date::Simple->new('1996-02-29'));
ok(not Date::Simple->new('1900-02-29'));

#------------------------------------------------------------------------------
# Check new method with parameters
#------------------------------------------------------------------------------

ok(not Date::Simple->new(2000, 2, 30));
ok(my $d = Date::Simple->new(2000, 2, 28));
ok(my $d2 = Date::Simple->new('2000-02-28'));
ok($d, $d2);

ok(2000, $d->year);
ok(2, $d->month);
ok(28, $d->day);

ok("$d" eq "2000-02-28");

#------------------------------------------------------------------------------
# Date arithmetic
#------------------------------------------------------------------------------

ok($d += 7);
ok("$d", "2000-03-06");

ok($d -= 14);
ok("$d", "2000-02-21");

ok($d cmp "2001-07-01", -1);
ok($d <=> [2001, 7, 1], -1);

ok($d2 = $d + 7);
ok("$d2", "2000-02-28");

ok($d2->prev, "2000-02-27");
ok($d2->next, "2000-02-29");

ok($d2-$d, 7);

ok( ($d + 0), $d);
ok( ($d + -3), ($d - 3));
ok( ($d - -3), ($d + 3));

#------------------------------------------------------------------------------
# try again with another date
#------------------------------------------------------------------------------

ok($d = Date::Simple->new('1998-02-28'));

ok(1998 == $d->year);
ok(2 == $d->month);
ok(28 == $d->day);

ok($d += 7);
ok("$d", "1998-03-07");

ok($d -= 14);
ok("$d", "1998-02-21");

ok($d2 = $d + 7);
ok("$d2", "1998-02-28");

ok($d2->prev, "1998-02-27");
ok($d2->next, "1998-03-01");

ok($d = Date::Simple->new('1972-01-17'));
ok($d->year, 1972);
ok($d->month, 1);
ok($d->day, 17);

ok($d->format, '1972-01-17');
# Don't assume much about how this locale spells 'Jan'.
ok($d->format('%d %b %Y') =~ m/17 \D+ 1972/);
ok($d->format('Foo'), 'Foo');

use Date::Simple ('date', 'd8');

$d = Date::Simple->new(1996, 10, 13);
ok ($d == Date::Simple->new ([1996, 10, 13]));
ok ($d > date(1996, 10, 12));
ok (date('1996-10-12') <= $d);
ok (Date::Simple->new(2000, 3, 12) - d8(19690219), 11344);

ok ($d = Date::Simple->new (2000, 2, 12));
ok ($d = $d + 17);
ok ($d->strftime("%Y %m %d"), "2000 02 29");
$d += 1;
ok ($d->as_d8, "20000301");
ok ($d - Date::Simple::ymd (2000, 2, 12), 18);
ok (($d - 18)->format("%Y %m %d"), "2000 02 12");

ok (Date::Simple::ymd(1966,10,15)->day_of_week, 6);

foreach (
	 [1969, 2,19,  1],
	 [1975, 6,14,  1],
	 [1999, 0, 1,  0],
	 [1999, 1, 1,  1],
	 [1999, 2,28,  1],
	 [1999, 2,29,  0],
	 [1999, 4,31,  0],
	 [1999, 4,30,  1],
	 [1999, 8, 1,  1],
	 [1999, 8,31,  1],  # produced '1999 09 00' due to buggy POSIX.xs
                            # in perl 5.005_63 and 5.5.560.
	 [1999, 8,32,  0],
	 [1999,12,31,  1],
	 [1999,13, 1,  0],
	 [2000, 1, 1,  1],
	 [2000, 2,12,  1],
	 [2000, 2,28,  1],
	 [2000, 2,29,  1],
	 [2000, 3, 1,  1],
	 [2001, 2,29,  0],
	 [2004, 2,29,  1],
	 [2100, 2,29,  0],
	)
{
    $d = Date::Simple->new(@$_[0,1,2]);
    ok (($d ? 1 : 0), $$_[3]);
    if ($$_[3]) {
	ok ($d->year, $$_[0]);
	ok ($d->month, $$_[1]);
	ok ($d->day, $$_[2]);
	ok ($d->strftime("%Y %m %d"), sprintf("%04d %02d %02d", @$_[0,1,2]));
	ok (join (' ', $d->as_ymd),
	    join (' ', Date::Simple::days_to_ymd (Date::Simple::ymd_to_days
						  (@$_[0,1,2]))));
    }
}

ok (Date::Simple::today());
ok (Date::Simple::days_in_month(2001,10), 31);

ok (d8 ('20021206') == 20021206);
ok (d8 ('20021206') eq '20021206');
ok (d8 ('20021206') eq '2002-12-06');
ok (d8 ('20021206') ne 'bla');
ok (d8 ('20021206') != 123);
