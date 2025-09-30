#!/usr/bin/perl -w
# T. Zint, MUSIC Group Services EU GmbH, 2012
#
# Published with the kind permission of T. Zint. Thank you!

use strict;
use File::Find;
use Digest::MD5 qw (md5);

if ($#ARGV < 1) {
  print "create updater image\n";
  print "\n";
  print "Usage:  $0  path1:name1,path2:name2,... imagefile\n";
  print "\n";
  print "Written by T. Zint, MUSIC Group Services EU GmbH, 2012\n";
  exit();
}

print "Compiling binaries into DCP-Format for X32...\n";

# dont change revstring when loading unencrypted files
my ($filelist, $image, $revfile) = @ARGV; 
my $mask = "";
my $revstring = "INTERNAL_DEVELOPMENT_VERSION_DO_NOT_DISTRIBUTE";

$revstring = substr($revstring, 0, 87);
my $revstringshrt = $revstring;
fixedlength(\$revstring, 87);
$revstring .= "\0";
my $md5string = "DCP bootloader";
my $crseed = " - designed by Thomas Zint - ";

my @files = sort(split(",", $filelist));
my $nfiles = $#files + 1;
my $block = int(($nfiles + 1 + 3) / 4);
my $startblock = $block;
my $directory = "";  
my @dirfiles;
fixedlength(\$directory, 128);

for (my $i = 0; $i < $nfiles; $i++) {
  my ($srcname,$dir) = split(":", $files[$i]);
  if (not defined $dir) {($dir) = $srcname =~ /([^\/]+)\z/};
  my $size = -s $srcname;
  open(F,$srcname) or die("Cannot open file $srcname\n");
  binmode(F);       
  my $fdata;
  read(F,$fdata,512);
  fixedlength(\$fdata, 512);
  seek(F,0,0);
  my $md5a = md5($fdata);
  my $md5b = Digest::MD5->new->addfile(*F)->digest;
  close(F);
  push (@dirfiles, $dir);
  fixedlength(\$dir, 87);
  $dir .= "\0" . pack('L',$block) . pack('L',$size) . $md5a . $md5b;
  $directory .= $dir;
  $block += int(($size + 511) / 512);
}
my $packsize = $block;

open(FI,"+>".$image) or die("Cannot write image\n");
binmode(FI);

# write directory
while (length($directory) % 512) { $directory .= "\0"; }
substr($directory,0,88) = $revstring;
substr($directory,88,4) = pack('L', $nfiles);
substr($directory,92,4) = pack('L', $packsize);
substr($directory,96,16) = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
substr($directory,112,16) = md5($md5string);
substr($directory,112,16) = md5($directory);
print FI $directory;

# write files
for (my $i = 0; $i < $nfiles; $i++) {
  my ($srcname,$dir) = split(":", $files[$i]);
  if (not defined $dir) {$dir = $srcname};
  open(F, $srcname) or die("Cannot read file $srcname\n");
  binmode(F);
  my $buf;                    
  my $crs = Digest::MD5->new->add($revstringshrt, $dirfiles[$i], $crseed)->digest;
  my @key = unpack('LLLL',$crs);
  my $modf = tell(FI) >> 9;
  while(not eof(F)) {
    read(F, $buf, 512);
    fixedlength(\$buf, 512);
    print FI $buf;
  }
  close(F);
}

# update full md5
seek(FI,0,0);
my $md5f = Digest::MD5->new->add($crseed)->addfile(*FI)->digest;
seek(FI,96,0);
print FI $md5f;

close(FI);

print "Done.\n";


sub fixedlength { # stringref, length
  my ($sr,$l) = @_;
  while (length($$sr) < $l) { $$sr .= "\0"; }
  $$sr = substr($$sr,0,$l);
}


sub findfunc {
  -d and return;
  -f and /$mask/ or return;
  push(@files, $File::Find::name);
}

__END__
