from __future__ import print_function
import numpy as np
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
            print('extract_spike_times.py <ApplicationDir> <outputFileDir>')
            sys.exit()

    if(len(argv) != 3):
        print('Input format is ---> python<v> extract_spike_times.py <ApplicationDir> <outputFileDir>')
        sys.exit(2)

    else:
        inputfile = argv[1]
        outputfile = argv[2]

    print('Input file is "', inputfile)
    print('Output file is "', outputfile)
    return inputfile, outputfile


if __name__ == "__main__":
    inputFilePath, outputFilePath = testFormat(sys.argv)

    a = np.load(inputFilePath+'/log/gui/test/log_vars/0.npz', allow_pickle=True)
    spikeInfo = a['spiketrains_n_b_l_t']
    lst = a.files

    for item in lst:
        if item == 'spiketrains_n_b_l_t':
            neuronId = 0
            file = open(outputFilePath+'extracted_spike_info.txt', 'w')
            print("Generating Spike Times File")
            for layer in spikeInfo:
                layerInfo = layer[0].shape
                #for time in range(layerInfo[0]):
                if len(layerInfo) == 5: 
                    for time in range(1):
                        for x_dim in range(layerInfo[1]):
                            for y_dim in range(layerInfo[2]):
                                for timex in range(layerInfo[3]):
                                    file.write(str(neuronId)+' ')
                                    #if np.count_nonzero(layer[0][time][x_dim][y_dim][timex]) == 0:
                                     #   file.write('\n')
                                    spikeTimes = list(layer[0][time][x_dim][y_dim][timex])
                                    neuronId = neuronId + 1
                                    for count, i in enumerate(spikeTimes):
                                        if(i>0):
                                            file.write(str(int(count))+' ')
                                    file.write('\n')
                elif len(layerInfo) == 3: 
                    for time in range(1):
                        for x_dim in range(layerInfo[1]):
                            file.write(str(neuronId)+' ')
                            spikeTimes = list(layer[0][time][x_dim])
                            neuronId = neuronId + 1
                            for count, i in enumerate(spikeTimes):
                                if(i>0):
                                    file.write(str(int(count))+' ')
                            file.write('\n')
            file.close()
