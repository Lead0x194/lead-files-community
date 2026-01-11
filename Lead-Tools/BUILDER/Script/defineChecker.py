import sys

defFileName = None
defName = None
additionalDefines = []
for arg in sys.argv[1:]:
	prefix = str.lower(arg[:2])
	s = arg[2:]
	if '-f' == prefix:
		defFileName = s
	elif '-c' == prefix:
		defName = s
	elif '-d' == prefix:
		additionalDefines.append(s.split('=')[0])

if not defFileName or not defName:
	exit(1)
	
defineChecker = open("defineChecker.c", "w")
defineChecker.write('#include <stdio.h>\n')
for ad in additionalDefines:
	defineChecker.write('#define %s\n' % ad)
defineChecker.write('#include "%s"\n' % defFileName)
defineChecker.write('\n')
defineChecker.write('int main()\n')
defineChecker.write('{\n')
defineChecker.write('#ifdef %s\n' % defName)
defineChecker.write('	printf("1");\n')
defineChecker.write('	return 1;\n')
defineChecker.write('#else\n')
defineChecker.write('	printf("0");\n')
defineChecker.write('	return 0;\n')
defineChecker.write('#endif\n')
defineChecker.write('}\n')
defineChecker.close()

from distutils.ccompiler import new_compiler
print additionalDefines

cc = new_compiler()
cc.define_macro(additionalDefines)
objs = cc.compile(["defineChecker.c"])
cc.link_executable(objs, "defineChecker")

import os
exit(os.system('defineChecker.exe'))
