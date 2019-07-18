#!/usr/bin/perl
open FILE, "carlsim.log";
open $out, '>', "carl.log" or die "Can't write new file: $!";

$flag =0;
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
    }
    
    if($flag==0)
    {
        print $line;
        print $out $line;
    }
    
}


