#!/usr/bin/perl
# --------------------------------------------------------------------------
# Name:    elim.uptime.pl
#
# Purpose: LSF ELIM reports the host uptime. These metrics can be used 
#          for reporting, sorting, or creating scheduling thresholds.
#
# Author:  Larry Adams <adamsla@us.ibm.com>
#
# --------------------------------------------------------------------------

# --------------------------------------------------------------------------
# Change the one variable below to match lsf.shared
# --------------------------------------------------------------------------
my $sleep = 60;

while () {
	# -----------------------------------------------
	# Report uptime for the host
	# -----------------------------------------------
	my $days = $hours = $minutes = $seconds = 0;
	my $bootCount = 0;

	my $bootTime = `LC_ALL=POSIX ps -p 1 -o etime | grep -v ELAPSED | tr -d ' '`;
	chomp($bootTime);

	$bootCount = () = $bootTime =~ /:/g;
	if ($bootCount == 0) {
		$seconds = $bootTime;
	}elsif ($bootCount == 1) {
		($minutes, $seconds) = split(':', $bootTime);
	}elsif ($bootCount == 2) {
		($daysHours, $minutes, $seconds) = split(':', $bootTime);
		$bootCount = () = $daysHours =~ /-/g;

		if ($bootCount == 0) {
			$hours = $daysHours;
		}else{
			($days, $hours) = split('-', $daysHours);
		}
	}
   	my $uptime = ($days * 86400) + ($hours * 3600) + ($minutes * 60) + $seconds;

	print "1 uptime $uptime\n";

	sleep($sleep);
}

