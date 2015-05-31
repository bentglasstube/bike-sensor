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
my $time : shared     = 0;
my $last_rev : shared = 0;
my $rpm : shared      = 0;

sub debug {
  my ($message) = @_;
  print STDERR $message, "\n" if $VERBOSE;
}

my $reader = threads->create(
  sub {
    my $bike = IO::File->new($SERIAL, 'r');

    unless ($bike) {
      print "\nCould not open $SERIAL: $!";
      return;
    }

    while (defined(my $line = $bike->getline())) {
      chomp $line;
      debug "Got data $line";

      $rpm = $line +0;

      my $elapsed = time - $last_rev;
      $last_rev = time;

      $dist += $WHEEL_DIA * $PI / 5280 / 12;
    }

    print "\nDevice closed";
  }
);

my $running = 1;
local $SIG{INT} = sub { $running = undef; };

STDOUT->autoflush(1);

while ($running) {
  last unless $reader->is_running;

  if (time - $last_rev > $TIMEOUT) {
    debug 'Pedalling stopped';
    $rpm = 0;
  }

  my $mph = $rpm * $WHEEL_DIA * $PI * 60 / 5280 / 12;

  my $hr  = $time / 3600;
  my $min = ($time / 60) % 60;
  my $sec = $time % 60;

  print "\r" unless $VERBOSE;
  printf '%.1f MPH | %.2f mi | %1u:%02u:%02u', $mph, $dist, $hr, $min, $sec;
  print "\n" if $VERBOSE;

  $time++ if $rpm > 0;

  sleep 1;
}

$reader->detach();

print "\nHave a nice day.\n";
