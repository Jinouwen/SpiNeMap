from __future__ import print_function
import os 
import sys
import numpy
import collections
import re
import math
import string
import operator
import time
import random as rn


def testFormat(argv):

    inputfile = ''
    outputfile = ''

    if len(argv) > 1:
        if argv[1] == '-h':
            print('parser.py <inputfilePath> <outputFilePath>')
            sys.exit()

    if(len(argv) != 3):
        print('Input format is ---> python<v> parser.py <inputfileDir> <outputfileDir>')
        sys.exit(2)

    else:
        inputfile = argv[1]
        outputfile = argv[2]

    print('Input file is "', inputfile)
    print('Output file is "', outputfile)
    return inputfile, outputfile

if __name__ == "__main__":
    
    inputFilePath, outputFilePath = testFormat(sys.argv)
    arch = sys.argv[1]

    sfname = inputFilePath+'extracted_spike_info.txt'
    cfname = inputFilePath+'connection_info.txt'

    sf = open(sfname, 'r')
    cf = open(cfname, 'r')

    osf = open(outputFilePath+'spike_info.txt', 'a')

    flag = 0
    line = cf.readline()
    data = collections.OrderedDict()

    array = []
    while(line):
        if(flag):
            # read the source neuron
            line = cf.readline()

        flag = 1
        if(len(line)==0):
            break

        line = line.strip().split()
        array.append(int(line[0]))
    line = sf.readline()
    flag=0

    while(line):
        if(flag):
            # read the source neuron
            line = sf.readline()

        flag = 1
        if(len(line)==0):
            break

        line1 = line.strip().split()

        if int(line1[0]) in array:
            osf.write(line)

