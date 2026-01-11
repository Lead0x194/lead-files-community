#!/usr/local/bin/python
import sys

def ReadLocaleLines(fileName):
    dstLines = []

    srcLines = open(fileName).readlines()
    for srcLine in srcLines:
	if srcLine[-2:] == "\r\n":
	    srcLine = srcLine[:-2]
	elif srcLine[-1:] == "\n":
	    srcLine = srcLine[:-1]

	dstLines.append(srcLine)

    return dstLines

if len(sys.argv) != 3:
	print "usage:"
	print "%s [han_file_name] [locale_file_name]" % (sys.argv[0])
	sys.exit()

srcList = ReadLocaleLines(sys.argv[1])
dstList = ReadLocaleLines(sys.argv[2])

outList = []
for (srcLine, dstLine) in zip(srcList, dstList):
    outList.append('"%s";'% (srcLine))
    outList.append('"%s";'% (dstLine))
    outList.append('')

outFile = open("locale_string_out.txt", "w")
outFile.write("\n".join(outList))
