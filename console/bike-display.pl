#!/usr/bin/env perl

use strict;
use warnings;

use Time::HiRes 'time';

my $SERIAL    = '/dev/cu.usbmodem12341';
my $WHEEL_DIA = 16;
my $REV_LIMIT = 3;

my $PI = 3.14159265358979;

my $dist     = 0;
my $time     = 0;
my $last_rev = 0;

open my $serial, '<', $SERIAL;

while (my $rpm = <$serial>) {
  my $elapsed = time - $last_rev;
  $last_rev = time;

  $time += $elapsed if $elapsed < $REV_LIMIT;
  $dist += $WHEEL_DIA * $PI / 5280 / 12;

  my $mph = $rpm * $WHEEL_DIA * $PI * 60 / 5280 / 12;
  my $hr  = $time / 3600;
  my $min = ($time / 60) % 60;
  my $sec = $time % 60;

  printf "\r%3.1f MPH | %5.2f mi | %1u:%02u:%02u", $mph, $dist, $hr, $min, $sec;
}

close $serial;
