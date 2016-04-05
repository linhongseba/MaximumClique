import sys
import json
import os


def check_dict(string):
    global dictionary
    global counter
    # print dict
    if string in dictionary:
        return str(dictionary[string])
    else:
        counter = counter + 1
        dictionary[string] = counter
        return str(counter)



def create_node_nums(x):
    source = x.split("\t")[0]
    destinaton = x.split("\t")[1]
    res = ''
    res = res + check_dict(source) + " " + check_dict(destinaton)
    return res


dictionary = {}
counter = -1

if __name__ == '__main__':
    inputFileName = sys.argv[1]
    outputFileName = sys.argv[2]
    inputFile = open(inputFileName,'r')
    tempFile = open(outputFileName + '_temp','w')
    dicFile = open(outputFileName + '_dict','w')
    reversedicFile = open(outputFileName + '_rev-dict','w')

    for line in inputFile:
        line = line.strip()
        tempFile.write(create_node_nums(line)+"\n")
    tempFile.close()
    edges = open(outputFileName+'_temp','r').read()

    outputFile = open(outputFileName,'w')
    outputFile.write(str(counter+1) + "\n")
    outputFile.write(edges)

    dicFile.write(json.dumps(dictionary,indent=4))

    rev_dict = dict([[v,k] for k,v in dictionary.items() ])
    reversedicFile.write(json.dumps(rev_dict,indent=4))
    os.remove(outputFileName + '_temp')
    print "counter is ",counter
