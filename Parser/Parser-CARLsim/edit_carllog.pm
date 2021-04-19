#!/usr/bin/perl

my $inputFile = $ARGV[0];
my $outputDir = $ARGV[1];

open FILE, $inputFile;
open $out, '>', $outputDir.'carl.log' or die "Can't write new file: $!";

$logflag = 1;
$flag = 0;
$prevline = "";
while ($line = <FILE>)
{   
    
    $line=~s/^\[[A-Za-z]+ [._\/A-Za-z]+:[0-9]+] //gm;
    $line=~s/\(-[0-9].([0-9]+)\)//gm;
    $line=~s/\([0-9].([0-9]+)\)//gm;
    
    if($line=~/Running the simulation/)
    {
        $flag = 1;
    }
    
    if($line=~/Spike Times/)
    {
        $flag = 0;
        if($logflag==1)
        {
            $logflag=0
        }
    }
    
    if($flag==0)
    {
        if($logflag==0)
        {   
            print $out $prevline;
            $logflag = 2;
        }
        print $out $line;
    }
    $prevline = $line; 
}
