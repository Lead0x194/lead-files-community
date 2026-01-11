import struct
import logging

from itertools import izip

PACKET_TYPE_TO_UNPACK_TYPE = {
		"UINT8" : "B",
		"UINT16" : "H",
		"UINT32" : "L",
		"INT32" : "l",
		"CRYPT_KEY" : "16s",
		"LOGIN_ID" : "31s",
		"LOGIN_PW" : "17s",
		"LOGIN_STATUS" : "9s",
		"LOGIN_SUCCESS" : "328s",
		"CHAR_NAME" : "25s",
		"BGM_NAME" : "25s",
		"PACK_NAME" : "256s",
		"IV" : "32s",
		"CIPHER_KEY_DATA" : "256s",
		"3B": "3B",
		}

class PacketMaker:
	def __init__(self, headerType):
		self.packetStructInfod = {}
		self.headerForm = PACKET_TYPE_TO_UNPACK_TYPE[headerType]
		self.headerSize = struct.calcsize(self.headerForm)

	def RegisterPacketStruct(self, headerName, headerCode, form=None):
		if form:
			cols = form.split(",")
			cols = [col.split("#") for col in cols]
			packetNames = [name for name, type in cols]
			packetForm = "=" + "".join([PACKET_TYPE_TO_UNPACK_TYPE[type] for name, type in cols])
		else: # only header packet
			packetNames = []
			packetForm = self.headerForm

		self.packetStructInfod[headerName] = (headerCode, packetForm, struct.calcsize(packetForm), packetNames)

	def Make(self, headerName, *args, **kwargs): 
		headerCode, packetForm, packetSize, packetNames = self.packetStructInfod[headerName]

		vars = [headerCode] + list(args) + [kwargs[name] for name in packetNames[1+len(args):]]
		return struct.pack(packetForm, *vars)

class PacketParser:
	def __init__(self, headerType):
		self.data = ""
		self.packetDispatchInfod = {}
		self.headerForm = PACKET_TYPE_TO_UNPACK_TYPE[headerType]
		self.headerSize = struct.calcsize(self.headerForm)

	def RegisterPacketStruct(self, headerName, headerCode, form=None, isDynamicPack=False):
		if form:
			cols = form.split(",")
			cols = [col.split("#") for col in cols]
			packetNames = [name for name, type in cols]
			packetForm = "=" + "".join([PACKET_TYPE_TO_UNPACK_TYPE[type] for name, type in cols])
		else: # only header packet
			packetNames = []
			packetForm = self.headerForm

		self.packetDispatchInfod[headerCode] = (headerName, packetForm, struct.calcsize(packetForm), packetNames, isDynamicPack)
	
	def Parse(self, newData, stateMachine=None):
		self.data += newData

		offset = 0

		while offset + self.headerSize <= len(self.data):
			headerCode, = struct.unpack(self.headerForm, self.data[offset:offset+self.headerSize])
#			print "headerCode", headerCode
			if headerCode:
				packetDispatchInfo = self.packetDispatchInfod.get(headerCode, None)
				if packetDispatchInfo:
					headerName, packetForm, packetSize, packetNames, isDynamicPack = packetDispatchInfo


					if offset + packetSize <= len(self.data):
						args = struct.unpack(packetForm, self.data[offset:offset+packetSize])
						kwargs = dict(izip(packetNames, args))
						offset += packetSize
						if isDynamicPack:
							followingDataLen = kwargs['followingDataLen']
#							print "followingDataLen", followingDataLen
							followingData = self.data[offset:offset+followingDataLen]
							args += (followingData,)
							kwargs['followingData'] = followingData
							offset += followingDataLen
						if offset > len(self.data):
							return
						self._HandlePacket(stateMachine, headerName, *args, **kwargs)
					else:
#						print "offset, packetSize, self.data", offset, packetSize, len(self.data)
						return
				else:
					offset = self._HandleUnknownHeader(stateMachine, headerCode, self.data, offset)
			else: # zero header is skip
				offset += 1
			self.data = self.data[offset:]
			offset = 0

#		self.data = self.data[offset:]

	def _HandlePacket(self, stateMachine, headerName, *args, **kwargs):
		method = stateMachine.GetPhaseMethod(headerName) if stateMachine else None
		if method:
			method(*[], **kwargs)
		else:
			logging.info("handle_packet(args=%s, kwargs=%s)" % (args, kwargs))

	def _HandleUnknownHeader(self, stateMachine, headerCode, data, offset):
		method = stateMachine.GetPhaseMethod("UNKNOWN_HEADER") if stateMachine else None
		if method:
			method(headerCode)
		else:
			logging.error("UNKNOWN_HEADER(header=%d,0x%x)" % (headerCode, headerCode))
		return offset + len(data) - offset

"""
if __name__ == "__main__":
	logging.basicConfig(level=logging.DEBUG)

	class PacketTester:
		def __init__(self, args, kwargs):
			self.args = args
			self.kwargs = kwargs
			self.result = False

		def __call__(self, *args, **kwargs):
			self.result = self.args == args or self.kwargs == kwargs

		def GetResult(self):
			return self.result

	class PacketTestMachine:
		def __init__(self, packetTester):
			self.packetTester = packetTester

		def GetPhaseMethod(self, name):
			return self.packetTester

	def TestPacketParsing(headerName, headerCode, form, data, checkArgs, checkResult=True):
		print "test_packet_parsing:", headerName, headerCode, form, ", ".join("0x%.2x" % ord(e) for e in data), checkArgs
		parser = PacketParser(headerType="UINT8")
		parser.RegisterPacketStruct(headerName, headerCode, form)

		tester = PacketTester(tuple(value for name, value in checkArgs), dict(checkArgs))
		testMachine = PacketTestMachine(tester)
		parser.Parse(data, testMachine)
		if tester.GetResult() == checkResult:
			print "\tok\n"
		else:
			print "\tERROR\n"
			raise RuntimeError

	TestPacketParsing("GC_PHASE", 0xfd, "header#UINT8,phase#UINT8", struct.pack("=BB", 0xfd, 5), [("header", 0xfd), ("phase", 5)])
	TestPacketParsing("GC_PHASE", 0xfd, "header#UINT8,phase#UINT8", struct.pack("=BB", 0xff, 5), [("header", 0xfd), ("phase", 5)], checkResult=False)
	TestPacketParsing("GC_HANDSHAKE", 0xff, "header#UINT8,code#UINT32,time#UINT32,delta#INT32", struct.pack("=BLLl", 0xff, 0, 1, -2), [("header", 0xff), ("code", 0), ("time", 1), ("delta", -2)])

	maker = PacketMaker(headerType="UINT8")
	maker.RegisterPacketStruct("CG_HANDSHAKE", 0xff, "header#UINT8,code#UINT32,time#UINT32,delta#INT32")
	len(maker.Make("CG_HANDSHAKE", code=1, time=2, delta=-3))

"""
