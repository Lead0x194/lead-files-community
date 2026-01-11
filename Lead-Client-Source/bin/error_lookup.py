import sys							 
MAP_FILE = "metin2.map"
COD_HEAD = "..\\"
COD_MIDDLE = "..\\UserInterface\\distribute\\"

if len(sys.argv)>1:
	MAP_FILE = sys.argv[1]

f = file(MAP_FILE)
data = f.readline()
data = f.readline()
data = f.readline()
print data
data = f.readline()
data = f.readline()
data = f.readline()
while len(f.readline())>2:
	pass
#while f.readline()!='':
while len(f.readline())>2:
	pass

map_pieces = []

while 1:
	data = f.readline()
	if len(data)>2:
		a = data.split()
		# address mangling_name lib:module
		map_pieces.append((long(a[2],16),a[1],a[-1][:-4]))
	else:
		break

print 'Function count :', len(map_pieces)
print 'Parsing ready. Please input crashed offset (in HEX ex. 451f3):'

def read_cod(piece):
	lines=[]
	a = piece[2].split(':')
	if len(a)==2:
		# lib : name
		f = file("".join([COD_HEAD,a[0],COD_MIDDLE,a[1],".cod"]))
	else:
		# name
		f = file("".join([COD_HEAD,'UserInterface',COD_MIDDLE,piece[2],".cod"]))
	recorded = 0
	for line in f:
		if recorded:
			lines.append(line)
		if line.startswith(piece[1]):
			recorded = not recorded
			if not recorded:
				break
			lines.append(line)

	return lines

def show_code(piece):
	print 'Show code disabled.'
#	confirm = raw_input('Show code (Y/n)?')
#	if confirm.lower() in ['','y','ye','yes']:
#		lines = read_cod(piece)
#
#		app = Application(lines)
#		app.mainloop()
#
#		#startline = 0
#		#pageline = 15
#		#while 1:
#			#print
#			#print 'Crash Code offset :',delta,'(',hex(delta),')'
#			#print
#			#print "".join(lines[startline:startline+pageline])
#			#print
#			#cmd = raw_input('ENTER,f,b,u,d,q > ').strip()
#			#
#			#if cmd=='' or cmd=='f':
#				#mydelta = pageline
#			#elif cmd=='b':
#				#mydelta = -pageline
#			#elif cmd=='u':
#				#mydelta = -1
#			#elif cmd=='d':
#				#mydelta = 1
#			#elif cmd=='q':
#				#return
#			#
#			#startline+=mydelta
#			#if startline>len(lines): startline = len(lines)-pageline+1
#			#if startline<0:startline = 0



while 1:
	offset = raw_input('OFFSET, q > ').strip()

	if not offset or offset =='q':
		print 'Happy Debuging.'
		print 'Bye bye.'
		break

	if len(offset)>2:
		if offset[2]=='x':
			offset=offset[2:]
	x = long(offset,16)
	l = 0
	r = len(map_pieces)-1
	if x<map_pieces[l][0] or x>map_pieces[r][0]:
		print 'Invalid address.'
		continue
	while l<r:
		m = (l+r)/2
		if x<map_pieces[m][0]:
			r=m-1
		elif x>map_pieces[m][0]:
			l=m+1
		else:
			break
	while x>map_pieces[m][0]:
		m+=1
	while x<map_pieces[m][0]:
		m-=1
	print hex(map_pieces[m][0]), map_pieces[m][1:]
	delta = x-map_pieces[m][0]
	print
	print 'Crash Code offset :',delta,'(',hex(delta),')'
	show_code(map_pieces[m])


