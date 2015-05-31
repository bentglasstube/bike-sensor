#!/usr/bin/env perl

use strict;
use warnings;
use threads;
use threads::shared;

use IO::File;
use Time::HiRes 'time';

my $SERIAL    = '/dev/cu.usbmodem12341';
my $WHEEL_DIA = 16;
my $TIMEOUT   = 3;
my $VERBOSE   = 0;

my $PI = 3.14159265358979;

my $dist : shared     = 0;
my $last_rev : shared = 0;
my $rpm : shared      = 0;

my $time    = 0;
my $max_mph = 0;

sub debug {
  my ($message) = @_;
  print STDERR $message, "\n" if $VERBOSE;
}

sub tstring {
  my ($seconds) = @_;

  sprintf '%u:%02u:%02u', $seconds / 3600, ($seconds / 60) % 60, $seconds % 60;
}

my $reader = threads->create(
  sub {
    my $bike = IO::File->new($SERIAL, 'r');

    unless ($bike) {
      print "\nCould not open $SERIAL: $!";
      return;
    }

    while (1) {
      my $line = $bike->getline() or next;

      chomp $line;
      debug "Got data $line";

      $rpm = $line +0;

      my $elapsed = time - $last_rev;
      $last_rev = time;

      $dist += $WHEEL_DIA * $PI / 5280 / 12;
    }
  }
);

my $running = 1;
local $SIG{INT} = sub { $running = undef; };

STDOUT->autoflush(1);

while ($running) {
  if (time - $last_rev > $TIMEOUT) {
    debug 'Pedalling stopped';
    $rpm = 0;
  }

  my $mph = $rpm * $WHEEL_DIA * $PI * 60 / 5280 / 12;
  $max_mph = $mph if $mph > $max_mph;

  print "\r" unless $VERBOSE;
  printf '%.1f MPH | %.2f mi | %s     ', $mph, $dist, tstring($time);
  print "\n" if $VERBOSE;

  $time++ if $rpm > 0;

  last unless $reader->is_running;

  sleep 1;
}

$reader->detach();

print "\n";

if ($time > 0) {
  print "\nRide summary:\n";
  printf "Total distance: %.2f mi\n", $dist;
  printf "Average speed:  %.1f MPH\n", $dist / $time * 3600;
  printf "Maximum speed:  %.1f MPH\n", $max_mph;
  printf "Total time:     %s\n", tstring($time);
}
