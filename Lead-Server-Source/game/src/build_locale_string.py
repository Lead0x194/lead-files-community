#!/usr/local/bin/python
import sys
import glob

def IsHangulInLine(line):
    for ch in line:
	if ord(ch) >= 128:
	    return True
    return False

hanList = []

for fileName in glob.glob("*.cpp"):
    isComment = False

    lines = open(fileName).readlines()
    for line in lines:
	linePos = lines.index(line)
	if isComment: 
	    commentEnd = line.find("*/")
	    if commentEnd < 0:
		continue
	    else:
		line = line[commentEnd+2:]
		isComment = False


	else:
	    commentBegin = line.find("/*")
	    if commentBegin >= 0:
		commentEnd = line.find("*/")
		if commentEnd >= 0:
		    line = line[:commentBegin] + line[commentEnd+2:]
		else:
		    isComment = True

	while True:
	    pos = line.find("TEXT")

	    if pos < 0:
		break

	    if len(line) < pos + 5:
		break
    
	
	    if line[pos+4] != "(":
		break

	    pos += 5
	    if line[pos] != '"':
		break

	    endPos = line[pos+1:].find('"')
	    if endPos < 0:
		raise

	    endPos += pos
	    endPos += 2

	    han = line[pos+1:endPos-1]
	    if not han in hanList:
		hanList.append(han)

	    line = line[endPos:]

out = open("locale_string.txt", "w")
for han in hanList:
    out.write("%s\n" % (han))
    print han
