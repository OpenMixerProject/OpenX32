#!/usr/bin/perl -w
# decompresses files from DCP-binary
# based on compiler by T. Zint, MUSIC Group Services EU GmbH, 2012

use strict;
use Digest::MD5 qw (md5);
use File::Path qw(make_path); 
use File::Basename;

# constants
my $BLOCK_SIZE = 512;
my $DIR_ENTRY_SIZE = 87 + 1 + 4 + 4 + 16 + 16;
my $DIR_HEADER_SIZE = 128; 

if ($#ARGV != 0) {
    print "unpack updater image\n";
    print "\n";
    print "Usage:  $0 imagefile\n";
    print "\n";
    print "Extracted files will be written to the current directory, with subdirectories.\n";
    exit(1);
}

my $imagefile = $ARGV[0];

print "Open Image-File: $imagefile...\n";
open(FI, $imagefile) or die("Cannot open image file $imagefile $!\n");
binmode(FI);

# ... (Teil 1: read Directory Header) ...
my $directory_data;
read(FI, $directory_data, $DIR_HEADER_SIZE) or die("Error on reading directory-header\n");

my $nfiles_packed = unpack('L', substr($directory_data, 88, 4));
print "Found packed file: $nfiles_packed\n";

my $min_dir_size = $DIR_HEADER_SIZE + $nfiles_packed * $DIR_ENTRY_SIZE;
my $total_dir_blocks = int(($min_dir_size + $BLOCK_SIZE - 1) / $BLOCK_SIZE);
my $total_dir_size = $total_dir_blocks * $BLOCK_SIZE;

my $dir_rest_size = $total_dir_size - $DIR_HEADER_SIZE;
my $dir_rest;
read(FI, $dir_rest, $dir_rest_size) or die("Error on reading directory\n");
$directory_data .= $dir_rest;

# --- decompress files and create folders ---
my $offset = $DIR_HEADER_SIZE; 
my $current_block_offset = $total_dir_blocks;

for (my $i = 0; $i < $nfiles_packed; $i++) {
    my $entry = substr($directory_data, $offset, $DIR_ENTRY_SIZE);
    
    my $filename_packed = substr($entry, 0, 88); 
    my $block_num = unpack('L', substr($entry, 88, 4));
    my $filesize = unpack('L', substr($entry, 92, 4));
    
    my $full_target_path = (split("\0", $filename_packed))[0];
    
    print "  [$i] Entpacke '$full_target_path' (Größe: $filesize Bytes, Block: $block_num)...";
    
    my ($file_name, $dir_path) = fileparse($full_target_path);

    if ($dir_path ne '') {
        my @created_dirs = make_path($dir_path, { error => \my $err, verbose => 0 });

        if (@$err) {
            for my $diag (@$err) {
                my ($dir_name, $error) = %$diag;
                print "\nWARNING: Could not create directory '$dir_name': $error (try to continue...)\n";
            }
        }
    }
    # ----------------------------------------------------
    
    if ($block_num != $current_block_offset) {
        seek(FI, $block_num * $BLOCK_SIZE, 0) or die("Error on searching start-block of file $full_target_path\n");
        $current_block_offset = $block_num;
    }
    
    my $file_data;
    my $bytes_read = read(FI, $file_data, $filesize);
    if ($bytes_read != $filesize) {
        die("\nERROR: Could not read all $filesize bytes for file $full_target_path. Only $bytes_read bytes read.\n");
    }
    
    open(FO, ">$full_target_path") or die("\nCannot create output-file $full_target_path: $!\n");
    binmode(FO);
    print FO $file_data;
    close(FO);
    
    my $file_blocks = int(($filesize + $BLOCK_SIZE - 1) / $BLOCK_SIZE);
    $current_block_offset += $file_blocks;
    
    $offset += $DIR_ENTRY_SIZE;
    print " [OK]\n";
}

close(FI);

print "\nDecompression done. $nfiles_packed files extracted.\n";