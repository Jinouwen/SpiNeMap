import sys
import os

#app_name = sys.argv[1]

def testFormat(argv):

    inputfile = ''
    outputfile = ''

    if len(argv) > 1:
        if argv[1] == '-h':
            print 'log_parser.py <inputFilePath> <outputFilePath>'
            sys.exit()

    if(len(argv) != 3):
        print 'Input format is ---> python<v> log_parser.py <inputFilePath> <outputfilePath>'
        sys.exit(2)

    else:
        inputfile = argv[1]
        outputfile = argv[2]

    print 'Input file is "', inputfile
    print 'Output file is "', outputfile
    return inputfile, outputfile


inputFilePath, outputFilePath = testFormat(sys.argv)

osfname = outputFilePath+'spike_info.txt'
ocfname = outputFilePath+'connection_info.txt'

isfname = inputFilePath+'spike_info.txt'
icfname = inputFilePath+'connection_info.txt'

snrn = {}
cnrn = {}

fp = open(icfname,'r')
lines = fp.readlines()
itr = 0 
while itr < len(lines)-1:
    line = lines[itr]
    line_array = line.strip().split()
    sourceNeuron = line_array[0]
    itr = itr+1
    line = lines[itr]
    line_array = line.strip().split()
    if len(line_array) != 0: 
        if sourceNeuron in cnrn.keys():
            for i in range(len(line_array)):
                cnrn[sourceNeuron].append(int(line_array[i]))
        else:
            #print(map(int, line_array[1:-1]))
            cnrn[sourceNeuron] = map(int, line_array)
    itr = itr+2

fp.close()

ocfp = open(ocfname,'w')

for sourceNeuron in cnrn:
    ocfp.write('%s ' % sourceNeuron)
    for destinationNeuron in cnrn[sourceNeuron]:
        ocfp.write('%d ' % destinationNeuron)
    ocfp.write('\n')

ocfp.close()


####################################################################

fp = open(isfname,'r')
lines = fp.readlines()
itr = 0 
while itr < len(lines):
    line = lines[itr]
    line_array = line.strip().split(',')
    sourceNeuron = line_array[0]
    itr = itr+1
    line = lines[itr]
    line_array = line.strip().split()
    if len(line_array) != 0: 
        if sourceNeuron in snrn.keys():
            for i in range(len(line_array)):
                snrn[sourceNeuron].append(line_array[i])
        else:
            #print(map(int, line_array[1:-1]))
            snrn[sourceNeuron] = map(int, line_array)
    else: 
        snrn[sourceNeuron] = ' '
    itr = itr+1

fp.close()

osfp = open(osfname,'w')

for sourceNeuron in snrn:
    osfp.write('%s ' % str(sourceNeuron))
    for spikeTime in snrn[sourceNeuron]:
        osfp.write('%s ' % str(spikeTime))
    osfp.write('\n')

osfp.close()

