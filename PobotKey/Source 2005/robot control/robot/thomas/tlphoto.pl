#!/usr/bin/perl
#	Usage: tlphoto.pl [options] pidfile [interval]
#	Copyright (C) 2001, Nick Andrew <nick@nick-andrew.net>
#	Released under the terms of the GNU Public Licence.
#
#  Make timelapse photos from a video capture device every 'interval'
#  seconds. Photos will be taken when (time() % $interval) == 0, in
#  other words, if you ask for a photo every minute, then the photo
#  will be taken when seconds == 00.
#
#  Options:
#     -f format		Picture file format, default 'jpeg'
#     -k		Kill previous instance of tlphoto.pl
#     -l nnn		Exit after this number of pictures taken (i.e. limit)
#     -s source		Grab image from this source, default 'comp1'
#     -v		Verbose mode, log every picture taken
#
#  Pictures are taken using "vidcat". Output filenames are in the form
#  "xxxxxxxx.jpg" where 'xxxxxxxx' is the current unix timestamp in hex.
#
#  BUGS:
#  Filenames always end in .jpg even if the output format is changed.

use Getopt::Std;

use vars qw/$opt_f $opt_k $opt_l $opt_s $opt_v/;

getopt('f:kl:s:v');

my $video_source = $opt_s || 'comp1';
my $file_format = $opt_f || 'jpeg';
my $limit = $opt_l || 0;
my $VIDCAT = 'vidcat';

my $pidfile = shift @ARGV;
my $interval = $ARGV[0] || 60;

my $count = 0;

$SIG{'TERM'} = sub { unlink($pidfile); exit(4); };

# Check if there's a current instance, if so we exit

my $pid = instance_pid($pidfile);

if ($pid > 0) {
	if ($opt_k) {
		kill('TERM', $pid);
		exit(3);
	}
	if (kill(0, $pid)) {
		# Clean exit, code 2
		exit(2);
	}
}

open(P, ">$pidfile") or die "Unable to open $pidfile for write: $!";
print P "$$\n";
close(P);

# Unbuffer stdout
$| = 1;

my($now, $hex, $delay);

# Wait until the first interval starts.
sleep($interval - (time() % $interval));

# Now loop taking photos.

while (1) {
	$now = time();
	$hex = sprintf("%08x", $now);

	if ($opt_v) {
		if (($count % 8) == 0) {
			print "\n";
		}
		print "$hex  ";
	}

	system("$VIDCAT -i $video_source -b -f $file_format > $hex.jpg");
	$now = time();
	$delay = $interval - ($now % $interval); 
	sleep($delay);
	$count++;

	last if ($limit > 0 && $count >= $limit);
}

exit(0);

sub instance_pid {
	my $pidfile = shift;

	if (open(P, "<$pidfile")) {
		my $pid = <P>;
		print "pid is $pid\n";
		chomp $pid;
		if ($pid > 0) {
			return $pid;
		}
		close(P);
	}

	return 0;
}
