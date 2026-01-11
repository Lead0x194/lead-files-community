import copy
import struct
import logging

from random import randint

from select import select
from ctypes import create_string_buffer

from weakref import proxy

from metin2packet import PacketParser
from metin2packet import PacketMaker

#from metin2crypt import tea_decrypt
#from metin2crypt import tea_encrypt
#from metin2crypt import get_sequence

from socket import *
from socket import error as socket_error


import cipher

ENCRYPT_KEY = "1234abcd5678efgh"
DECRYPT_KEY = "1234abcd5678efgh"
RANDGEN_KEY = "".join(["%c" % c for c in [ 0xc9, 0x28, 0xb6, 0x8f, 0xff, 0x5d, 0x0c, 0x89, 0x35, 0x6b, 0x62, 0x87, 0xeb, 0x9c, 0xb8, 0x81]]) 
#RANDGEN_KEY = "JyTxtHljHJlVJHorRM301vf@4fvj10-v"

PHASE_HANDSHAKE = 1
PHASE_LOGIN = 2
PHASE_SELECT = 3
PHASE_AUTH = 10
KEY_AGREEMENT_MAX_DATA_LEN = 256

class Phase:
	def __init__(self, client): self.client = proxy(client)
	def OnEnterPhase(self): pass
	def OnLeavePhase(self): pass

class HandshakePhase(Phase):
	def GC_PHASE(self, header, phase):
		#print "GC_PHASE", phase
		logging.debug("GC_PHASE: %s" % phase)
		self.client.ChangePhase(phase)

	def GC_HANDSHAKE(self, header, code, time, delta):
		logging.debug("GC_HANDSHAKE: %s %s %s" % (code, time, delta))
		#print "GC_HANDSHAKE: %s %s %s" % (code, time, delta)
		self.client.SetServerBaseTime(time + delta)
		self.client.SendPacket("CG_HANDSHAKE", code=code, time=(time + delta * 2), delta=0)

	def GC_PING(self):
		logging.debug("GC_PING:")
		self.client.SendPacketWithSequence("CG_PONG")

	def GC_KEY_AGREEMENT(self, header, agreedLength, dataLength, data):
		##print header, agreedLength, dataLength
		preparedDataLength = cipher.new_intp()
		cipher.intp_assign(preparedDataLength, KEY_AGREEMENT_MAX_DATA_LEN)
		sendData = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
		preparedAgreedLength = self.client.PrepareCipher(sendData, preparedDataLength)
		res = cipher.intp_value(preparedDataLength)
		#print "After PrepareCipher", res
		self.client.ActivateCipher(data=data, dataLength=dataLength, agreedLength=agreedLength)
		#print "After ActivateCipher"
		self.client.SendPacket("CG_KEY_AGREEMENT", dataLength=dataLength, agreedLength=agreedLength, data=sendData)
		#print "After SendPacket"

	def GC_KEY_AGREEMENT_COMPLETED(self, header, data):
		#print "GC_KEY_AGREEMENT_COMPLETED"
		self.client.SetActiveCipher()

class AuthPhase(Phase):
	def OnEnterPhase(self):
		logging.debug("send_auth_login_packet")
		#print "AuthPhase:OnEnterPhase"
		self.client.SendPacketWithSequence("CG_LOGIN3", id=self.client.GetLoginID(), pw=self.client.GetLoginPW(), cryptKey=self.client.GetRandEncryptKey())

	def GC_PANAMA(self, header, packName, iv):
		logging.debug("GC_PANAMA")
		#print "AuthPhase:GC_PANAMA"

	def GC_LOGIN_SUCCESS(self, header, key, result):
		logging.debug("GC_LOGIN_SUCCESS: %s %s" % (key, result))
		if result:
			self.client.SetGameLoginKey(key)
			self.client.ConnectGame()
		else:
			self.client.PostExit("LOGIN_RESULT_ERROR")	  

	def GC_LOGIN_FAILURE(self, header, status):
		logging.debug("GC_LOGIN_FAILURE: %s" % status)
		self.client.PostExit("AUTH_LOGIN_FAILURE")	  

	def GC_HYBRIDCRYPT_KEYS(self, header, dynamicPacketSize, followingDataLen, followingData):
		#print "GC_HYBRIDCRYPT_KEYS" 
		pass

	def GC_HYBRIDCRYPT_SDB(self, header, dye, followingDataLen, followingData):
		#print "GC_HYBRIDCRYPT_SDB" 
		pass

class LoginPhase(Phase):
	def OnEnterPhase(self):
		#print "LoginPhase:OnEnterPhase"
		logging.debug("send_game_login_packet")
		self.client.SendPacketWithSequence("CG_LOGIN2", id=self.client.GetLoginID(), loginKey=self.client.GetGameLoginKey(), cryptKey=self.client.GetRandEncryptKey())

	def GC_EMPIRE(self, header, empire):
		#print "LoginPhase:GC_EMPIRE"
		pass

	def GC_PHASE(self, header, phase):
		#print "LoginPhase:GC_PHASE, phase", phase
		logging.debug("GC_PHASE: %s" % phase)
		self.client.ChangePhase(phase)

	def GC_LOGIN_FAILURE(self, header, status):
		#print "LoginPhase:GC_LOGIN_FAILURE"
		logging.debug("GC_LOGIN_FAILURE: %s" % status)
		self.client.PostExit("GAME_LOGIN_FAILURE")

	def UNKNOWN_HEADER(self, headerCode):
		#print "LoginPhase:UNKNOWN_HEADER"
		logging.debug("UNKNOWN_HEADER: %d(0x%x)" % (headerCode, headerCode))
		self.client.PostExit("UNKNOWN_HEADER")

	def GC_HYBRIDCRYPT_KEYS(self, header, dynamicPacketSize, followingDataLen, followingData):
		#print "GC_HYBRIDCRYPT_KEYS" 
		pass

class SelectPhase(Phase):
	def GC_LOGIN_SUCCESS4(self, header, extra):
		#print "SelectPhase:GC_LOGIN_SUCCESS4"
		logging.debug("GC_LOGIN_SUCCESS4:")
		self.client.PostExit("OK")

class Stream:
	def __init__(self):
		self.sock = None
		self.sequence = 0
		self.cipher = cipher.Cipher()

	def Close(self):
		if self.sock:
			self.sock.close()
			self.sock = None

	def IsSecurityMode(self):
		return self.cipher.activated()

	def Connect(self, addr):
		logging.debug("connect: %s" % repr(addr))
		sock = socket(AF_INET, SOCK_STREAM)
		sock.connect(addr)
		self.sock = sock
		self.sequence = 0

	def RecvPacket(self):
		rawPacket = self.sock.recv(65536 * 2)
		#print "RecvPacket", len(rawPacket)
		if self.IsSecurityMode():
			self.cipher.Decrypt(rawPacket, len(rawPacket))
			return rawPacket
		else:
			return rawPacket

	def SendPacket(self, rawPacket):
		if self.IsSecurityMode():
			self.cipher.Encrypt(rawPacket, len(rawPacket))
			self.sock.send(rawPacket)
		else:
			self.sock.send(rawPacket)

	def SendPacketWithSequence(self, rawPacket):
		if self.IsSecurityMode():
			seq = cipher.get_sequence(self.sequence)
			self.sequence = self.sequence + 1

			rawPacket += struct.pack("=B", seq)
			self.cipher.Encrypt(rawPacket, len(rawPacket))
			self.sock.send(rawPacket)
		else:
			self.sock.send(rawPacket)

	def PrepareCipher(self, buffer_, length):
		return self.cipher.Prepare(buffer_, length)

	def ActivateCipher(self, agreed_length, buffer_, length):
		#print "ActivateCipher", agreed_length, length
		return self.cipher.Activate(True, agreed_length, buffer_, length)
	
	def SetActiveCipher(self):
		#print "SetActiveCipher"
		return self.cipher.set_activated(True);

	def fileno(self):
		return self.sock.fileno()

class Client:
	def __init__(self):
		self.phase = None

		packetParser = PacketParser(headerType="UINT8")
		packetParser.RegisterPacketStruct("GC_PHASE",	   0xfd, "header#UINT8,phase#UINT8")
		packetParser.RegisterPacketStruct("GC_HANDSHAKE",   0xff, "header#UINT8,code#UINT32,time#UINT32,delta#INT32")
		packetParser.RegisterPacketStruct("GC_KEY_AGREEMENT_COMPLETED",   0xfa, "header#UINT8,data#3B")
		packetParser.RegisterPacketStruct("GC_KEY_AGREEMENT", 0xfb, "header#UINT8,agreedLength#UINT16,dataLength#UINT16,data#CIPHER_KEY_DATA")
		packetParser.RegisterPacketStruct("GC_PING", 0xfe)
		packetParser.RegisterPacketStruct("GC_LOGIN_SUCCESS", 150, "header#UINT8,key#UINT32,result#UINT8")
		packetParser.RegisterPacketStruct("GC_LOGIN_FAILURE",   7, "header#UINT8,status#LOGIN_STATUS")
		packetParser.RegisterPacketStruct("GC_EMPIRE",		 90, "header#UINT8,empire#UINT8")
		packetParser.RegisterPacketStruct("GC_LOGIN_SUCCESS4", 32, "header#UINT8,extra#LOGIN_SUCCESS")
		packetParser.RegisterPacketStruct("GC_MAIN_CHARACTER3_BGM", 137, "header#UINT8,vid#UINT32,race#UINT16,charName#CHAR_NAME,bgmName#BGM_NAME,x#UINT32,y#UINT32,z#UINT32,empire#UINT8,job#UINT8")
		packetParser.RegisterPacketStruct("GC_PANAMA", 151, "header#UINT8,packName#PACK_NAME,iv#IV")
		packetParser.RegisterPacketStruct("GC_HYBRIDCRYPT_KEYS", 152, "header#UINT8,dynamicPacketSize#UINT16,followingDataLen#INT32", True)
		packetParser.RegisterPacketStruct("GC_HYBRIDCRYPT_SDB", 153, "header#UINT8,dynamicPacketSize#UINT16,followingDataLen#INT32", True)

		packetMaker = PacketMaker(headerType="UINT8")
		packetMaker.RegisterPacketStruct("CG_HANDSHAKE",	0xff, "header#UINT8,code#UINT32,time#UINT32,delta#INT32")
		packetMaker.RegisterPacketStruct("CG_KEY_AGREEMENT",	   0xfb, "header#UINT8,agreedLength#UINT16,dataLength#UINT16,data#CIPHER_KEY_DATA")
		packetMaker.RegisterPacketStruct("CG_PONG", 0xfe)
		packetMaker.RegisterPacketStruct("CG_LOGIN3",	   111, "header#UINT8,id#LOGIN_ID,pw#LOGIN_PW,cryptKey#CRYPT_KEY")
		packetMaker.RegisterPacketStruct("CG_LOGIN2",	   109, "header#UINT8,id#LOGIN_ID,loginKey#UINT32,cryptKey#CRYPT_KEY")


		self.packetParser = packetParser
		self.packetMaker = packetMaker
		self.serverBaseTime = 0
		self.newPhaseFuncd = {
				PHASE_HANDSHAKE : HandshakePhase,
				PHASE_AUTH : AuthPhase,
				PHASE_LOGIN : LoginPhase,
				PHASE_SELECT : SelectPhase,
				}
		self.authAddr = ("", 0)
		self.gameAddr = ("", 0)
		self.mainStream = None
		self.authStream = None
		self.gameStream = None
		self.gameLoginKey = None

		self.loginId = ""
		self.loginPw = ""
		self.authEncryptKey = ENCRYPT_KEY
		self.authDecryptKey = DECRYPT_KEY
		self.gameEncryptKey = ENCRYPT_KEY
		self.gameDecryptKey = DECRYPT_KEY
		self.randEncryptKey = "".join([chr(randint(0, 255)) for i in xrange(16)])

		self.streams = []

		self.isAlive = True
		self.exitCode = "INIT_ERROR"

	def Close(self):
		for stream in self.streams:
			stream.Close()

	def SetAuthAddr(self, addr):
		self.authAddr = addr

	def SetGameAddr(self, addr):
		self.gameAddr = addr

	def SetLoginInfo(self, id, pw):
		self.loginId = id
		self.loginPw = pw

	def MainLoop(self):
		try:
			self.ConnectAuth()
			while self.Proc():
				pass
			return self.exitCode
		except socket_error:
			logging.debug("SOCKET_ERROR")
			return "SOCKET_ERROR"

	def ConnectAuth(self):
		authStream = Stream()
		authStream.Connect(self.authAddr)
		self.streams.append(authStream)
		self.authStream = authStream
		self.mainStream = authStream
		self.ChangePhase(PHASE_HANDSHAKE)

	def SetAuthSecureMode(self):
		self.authStream.SetSecureMode(self.authEncryptKey, self.authDecryptKey)

	def ConnectGame(self):
		gameStream = Stream()
		gameStream.Connect(self.gameAddr)
		self.streams.append(gameStream)
		self.gameStream = gameStream
		self.mainStream = gameStream
		self.ChangePhase(PHASE_HANDSHAKE)

	def SetGameSecureMode(self):
		self.gameStream.SetSecureMode(self.gameEncryptKey, self.gameDecryptKey)

	def SetGameSecureMode2(self):
		self.gameEncryptKey = self.randEncryptKey
		self.gameDecryptKey = " " * 16
		tea_encrypt(self.gameDecryptKey, self.gameEncryptKey, RANDGEN_KEY, 16)
		self.gameStream.SetSecureMode(self.gameEncryptKey, self.gameDecryptKey)

	def SetGameLoginKey(self, key):
		self.gameLoginKey = key

	def PostExit(self, exitCode):
		self.isAlive = False
		self.exitCode = exitCode

	def Proc(self):
		if self.isAlive:
			rstreams, wstreams, estream = select(self.streams, self.streams, self.streams) 

			for stream in rstreams:
				packet = stream.RecvPacket()
				if packet:
					self.packetParser.Parse(packet, self)
				else:
					return False

			return True
		else:
			return False

	def PrepareCipher(self, buffer_, length):
		return self.mainStream.PrepareCipher(buffer_, length)

	def ActivateCipher(self, agreedLength, data, dataLength):
		self.mainStream.ActivateCipher(agreedLength, data, dataLength)
	
	def SetActiveCipher(self):
		self.mainStream.SetActiveCipher()

	def SendPacket(self, headerName, *args, **kwargs):
		rawPacket = self.packetMaker.Make(headerName, *args, **kwargs)
		if self.mainStream:
			self.mainStream.SendPacket(rawPacket)

	def SendPacketWithSequence(self, headerName, *args, **kwargs):
		rawPacket = self.packetMaker.Make(headerName, *args, **kwargs)
		if self.mainStream:
			self.mainStream.SendPacketWithSequence(rawPacket)

	def ChangePhase(self, newPhaseCode):
		oldPhase = self.phase
		if oldPhase:
			oldPhase.OnLeavePhase()

		newPhaseFunc = self.newPhaseFuncd.get(newPhaseCode, None)
		newPhase = newPhaseFunc(self) if newPhaseFunc else None
		self.phase = newPhase

		if newPhase:
			newPhase.OnEnterPhase()

	def SetServerBaseTime(self, serverBaseTime):
		self.serverBaseTime = serverBaseTime

	def GetPhaseMethod(self, name):
		try:
			return getattr(self.phase, name)
		except:
			return None

	def GetLoginID(self):
		return self.loginId

	def GetLoginPW(self):
		return self.loginPw

	def GetGameLoginKey(self):
		return self.gameLoginKey

	def GetRandEncryptKey(self):
		return self.randEncryptKey

"""
if __name__ == "__main__":
	client = Client()
	client.SetLoginInfo("myevan", "1234")
	client.SetAuthAddr(("210.123.10.153", 11000))
	client.SetGameAddr(("210.123.10.153", 13000))
	client.ConnectAuth()
	while client.Proc():
		pass
"""
