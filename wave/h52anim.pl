#!/usr/bin/perl

use strict;
use warnings;

my ($h5file, $var) = @ARGV;

unless (defined $h5file) {
  printf("*** error: no file specified.\n",
         "           usage:\n",
         "              $0 <hdf file> <variable name>\n");
  exit(1);
}

unless (defined $var) {
  printf("*** error: no variable specified.\n",
         "           usage:\n",
         "              $0 <hdf file> <variable name>\n");
  exit(1);
}

my $outname = $h5file . ".gif";

my ($m, $n, $nt);

open(H5META, "-|", "h5dump", "-d", $var, "-H", $h5file)
  or die("error while reading dataset metadata", $!);

while(my $line = <H5META>) {

  chomp $line;
  if ($line =~ m/DATASPACE *SIMPLE *\{ *\( *(\d+), *(\d+), *(\d+)/) {
    $m = $1;
    $n = $2;
    $nt = $3;
  }
}

close(H5META);

unless (defined $m && defined $n && defined $nt) {
  printf("*** error: could not determine dataset metadata.\n");
  exit(1);
}

my @images;

# start gnuplot, make plot
my $input = <<"EOT";
#set terminal pngcairo font "Arial,20" size 900,300 

set pm3d interpolate 1,1
unset key
set palette defined (  0 "blue", 19 "cyan", 20 "green", 21 "yellow", 40 "red")

set cbrange [-1.001:1.001]

#set hidden3d

set term png size 1000,600
set view 60,30
set view equal
set zrange [-1.001:1.001]

EOT

open(GNUPLOT, "|-", "gnuplot") or die("cannot start gnuplot");
print(GNUPLOT $input);


for my $it (0..$nt - 1) {

  # start a new plot
  print(GNUPLOT "set output 'tmp-$it.png'\n");
  print(GNUPLOT "splot '-' w pm3d\n");

  # start reading data
  open(H5DATA, "-|", "h5dump", "-d", $var, "-c", "$m,$n,1",
         "-s", "0,0,$it", $h5file)
    or die("error while reading dataset", $!);

  # skip until "DATA {"
  while (my $line = <H5DATA>) {
    last if $line =~ m/DATA \{/;
  }

  for my $i (1..$m) {

    my @values;

    while (@values < $n) {
      my $line = <H5DATA>;
      chomp $line;
      $line =~ s/^.*: *//;
      push(@values, split(/,/, $line));
    }

    for my $j (1..$n) {
      my $value = shift @values;
      printf(GNUPLOT "%4d %4d %8.3f\n", $i, $j, $value);
    }
    print(GNUPLOT "\n");
  }

  close(H5DATA);

  # end of the plot data
  print(GNUPLOT "e\n");

  # close output file
  print(GNUPLOT "set output\n");

  push(@images, "tmp-$it.png");
}

close(GNUPLOT);

# stack all images in one animated gif

# uses the "ImageMagick" command "convert"

# put a 10/100 = 0.1s delay between frames
system("convert", "-delay", "10", @images, "-loop", "0", $outname);

for my $image (@images) {
  unlink($image);
}
