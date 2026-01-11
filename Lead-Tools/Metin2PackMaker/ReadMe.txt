Diff Pack Disk
	pack = Pack_Open(packPath)
	filePaths = pack.GetFilePaths()
	for filePath in filePaths:
		pcrc = pack.GetPackFileCRC(filePath)
		dcrc = dcrcTable.get(filePath, 0)
		if not dcrc:
			dcrc = pack.GetDiskFileCRC(filePath)
			dcrcTable[filePath] = dcrc
		if pcrc == 0 and dcrc != 0:
			print "+"
		elif pcrc != 0 and dcrc == 0:
			print "-"
		elif pcrc != dcrc:
			printf "!"

	for path, flags, offset, size in pack.GetIndics():
		if Pack_IsRawType(flags):
		

	lzo_compress(open(path, "rb").read())