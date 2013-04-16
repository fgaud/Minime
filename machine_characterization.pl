#!/usr/bin/perl

use strict;
use warnings;

use Data::Dumper;
use File::Temp qw/ tempfile /;
use IPC::Run qw( run );
use Sys::Hostname;

## Config
my $duration = 30; ## in seconds
my $output = hostname."_characterization";

sub run_cmd {
   my $cmd = $_[0];
   my $global_th;
   my $global_lat;

   $cmd =~ s/^\s+//;
   my @c = split /\s+/, $cmd;

   my ($fh, $fname) = tempfile();
   run( \@c, ">", "$fname", "2>&1" );

   open FILE_TEMP, $fname or die $!;
   my @lines = <FILE_TEMP>;
   close FILE_TEMP;

   open FILE_RAW, ">>", "$output.raw" or die $!;
   print FILE_RAW ("-"x10)."\n";
   for my $line (@lines) {
      print FILE_RAW $line;
      if($line =~ m/\[GLOBAL\] throughput: (\d+\.\d+) MB\/s/) {
         $global_th = $1;
      }
      elsif($line =~ m/\[GLOBAL\] Average latency: (\d+) cycles/) {
         $global_lat = $1;
      }
   }
   print FILE_RAW "\n\n";
   close FILE_RAW;

   system "rm $fname";

   return ($global_th, $global_lat);
}

## Remove old files
system "rm $output $output.raw";

## Discovery the topology
my @node_files = </sys/devices/system/node/node*/cpulist>;
my %cpumap;

print "Found the following architecture:\n";
for my $n (@node_files) {
   (my $node_id) = ($n =~ m/node(\d+)/);
   my $cpulist = `cat $n`;
   chop($cpulist);

   $cpumap{$node_id} = $cpulist;

   print "\tNode $node_id: $cpumap{$node_id}\n";
}

## First let's benchmark the throughput
open FILE, ">", "$output"; 
for(my $n_src = 0; $n_src < scalar(keys %cpumap); $n_src++) {
   for (my $n_dest = 0; $n_dest < scalar(keys %cpumap); $n_dest++) {
      my $cmd = "./memory_test -t 0 -c $cpumap{$n_src} -m $n_dest -T $duration";
      my $l = "Evaluating throughput from node $n_src to node $n_dest: ";

      print $l;

      my @res = run_cmd("$cmd");

      my $el = "$res[0] MB/s\n";
      print $el;
      print FILE $l.$el;
   }
}

## Second let's benchmark latencies
for(my $n_src = 0; $n_src < scalar(keys %cpumap); $n_src++) {
   for (my $n_dest = 0; $n_dest < scalar(keys %cpumap); $n_dest++) {
      (my $first_core) = ($cpumap{$n_src} =~ m/^(\d+)/);
   
      my $cmd = "./memory_test -t 2 -c $first_core -m $n_dest -T $duration";
      my $l = "Evaluating latency from node $n_src to node $n_dest: ";

      print $l;

      my @res = run_cmd("$cmd");

      my $el = "$res[1] cycles\n";
      print $el;
      print FILE $l.$el;
   }
}

close FILE;
