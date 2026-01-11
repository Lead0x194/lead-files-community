import os
import app
import locale
import debugInfo

CHINA_PORT = 50000

def BuildServerList(orderList):
	retMarkAddrDict = {}
	retAuthAddrDict = {}
	retRegion0 = {}

	ridx = 1
	for region, auth, mark, channels in orderList:
		cidx = 1
		channelDict = {}
		for channel in channels:
			key = ridx * 10 + cidx
			channel["key"] = key
			channelDict[cidx] = channel
			cidx += 1

		region["channel"] = channelDict

		retRegion0[ridx] = region
		retAuthAddrDict[ridx] = auth
		retMarkAddrDict[ridx*10] = mark
		ridx += 1

	return retRegion0, retAuthAddrDict, retMarkAddrDict

app.ServerName = None

if (locale.IsEUROPE() and app.GetLocalePath() == "locale/vn"):
	STATE_NONE = "..."
					
	STATE_DICT = {
		0 : "....",
		1 : "NORM",
		2 : "BUSY",
		3 : "FULL"
	}

	SERVER01_CHANNEL_DICT = {
		1:{"key":11,"name":"CH1   ","ip":"121.160.9.68","tcp_port":13002,"udp_port":13002,"state":STATE_NONE,},	
	}
	
	REGION_NAME_DICT = {
		0 : "Vietnam",		
	}

	REGION_AUTH_SERVER_DICT = {
		0 : {
			1 : { "ip":"121.160.9.68", "port":11002, },
	
		}		
	}

	REGION_DICT = {
		0 : {
			1 : { "name" :"Vietnam1", "channel" : SERVER01_CHANNEL_DICT, },						
		},
	}

	MARKADDR_DICT = {
		10 : { "ip" : "121.160.9.68", "tcp_port" : 13002, "mark" : "10.tga", "symbol_path" : "10", },
	}

	TESTADDR = { "ip" : "210.123.10.153", "tcp_port" : 50000, "udp_port" : 50000, }

if (locale.IsEUROPE() and app.GetLocalePath() == "locale/sg"):
	STATE_NONE = "..."
					
	STATE_DICT = {
		0 : "....",
		1 : "NORM",
		2 : "BUSY",
		3 : "FULL"
	}

	SERVER01_CHANNEL_DICT = {
		1:{"key":11,"name":"CH1   ","ip":"120.29.208.231","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		2:{"key":12,"name":"CH2   ","ip":"120.29.208.232","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		3:{"key":13,"name":"CH3   ","ip":"120.29.208.233","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		4:{"key":14,"name":"CH4   ","ip":"120.29.208.234","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
	}
	
	REGION_NAME_DICT = {
		0 : "Singapore",		
	}

	REGION_AUTH_SERVER_DICT = {
		0 : {
			1 : { "ip":"120.29.208.227", "port":11000, },
	
		}		
	}

	REGION_DICT = {
		0 : {
			1 : { "name" :"Singapore", "channel" : SERVER01_CHANNEL_DICT, },						
		},
	}

	MARKADDR_DICT = {
		10 : { "ip" : "120.29.208.231", "tcp_port" : 13000, "mark" : "10.tga", "symbol_path" : "10", },
	}

	TESTADDR = { "ip" : "210.123.10.153", "tcp_port" : 50000, "udp_port" : 50000, }

if (locale.IsEUROPE() and app.GetLocalePath() == "locale/ca"):
	STATE_NONE = "..."
					
	STATE_DICT = {
		0 : "....",
		1 : "NORM",
		2 : "BUSY",
		3 : "FULL"
	}

	SERVER01_CHANNEL = [
		{"name":"CH11   ","ip":"74.200.6.201","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH12   ","ip":"74.200.6.202","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH13   ","ip":"74.200.6.203","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH14   ","ip":"74.200.6.204","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH15   ","ip":"74.200.6.205","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH16   ","ip":"74.200.6.206","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
	]
	SERVER02_CHANNEL =[
		{"name":"CH21   ","ip":"74.200.6.211","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH22   ","ip":"74.200.6.212","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH23   ","ip":"74.200.6.213","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH24   ","ip":"74.200.6.214","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH25   ","ip":"74.200.6.215","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"CH26   ","ip":"74.200.6.216","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
	]
	
	SERVER01_MARK = { "ip" : "74.200.6.202", "tcp_port" : 13000, "mark" : "10.tga", "symbol_path" : "10",}
	SERVER02_MARK = { "ip" : "74.200.6.212", "tcp_port" : 13000, "mark" : "20.tga", "symbol_path" : "20",}

	SERVER01_AUTH = {  "ip":"74.200.6.209", "port":11001, }			#Freekingdom
	SERVER02_AUTH = {  "ip":"74.200.6.209", "port":11002, }			#new world

	SERVER01 = { "name" : "FREE KINGDOM" } 
	SERVER02 = { "name" : "NEW WORLD" } 

	TESTADDR = { "ip" : "210.123.10.153", "tcp_port" : 50000, "udp_port" : 50000, }

	REGION0_ORDER_LIST = [
		(SERVER02, SERVER02_AUTH, SERVER02_MARK, SERVER02_CHANNEL),
		(SERVER01, SERVER01_AUTH, SERVER01_MARK, SERVER01_CHANNEL),
		]
		
	# BUILD
	NEW_REGION0, NEW_REGION0_AUTH_SERVER_DICT, NEW_MARKADDR_DICT = BuildServerList(REGION0_ORDER_LIST)

	# RESULT
	NEW_REGION_NAME_DICT = {
		0 : "CANADA",		
	}

	NEW_REGION_AUTH_SERVER_DICT = {
		0 : NEW_REGION0_AUTH_SERVER_DICT,
	}

	NEW_REGION_DICT = {
		0 : NEW_REGION0,
	}

	MARKADDR_DICT = NEW_MARKADDR_DICT
	REGION_DICT = NEW_REGION_DICT
	REGION_NAME_DICT = NEW_REGION_NAME_DICT
	REGION_AUTH_SERVER_DICT = NEW_REGION_AUTH_SERVER_DICT

if (locale.IsEUROPE() and app.GetLocalePath() == "locale/br"):
	STATE_NONE = "..."
					
	STATE_DICT = {
		0 : "....",
		1 : "NORM",
		2 : "BUSY",
		3 : "FULL"
	}

	SHUNZO_CHANNEL = [
		{"name":"SHUNZO-1   ","ip":"201.77.235.53","tcp_port":13100,"udp_port":13100,"state":STATE_NONE,},	
		{"name":"SHUNZO-2   ","ip":"201.77.235.55","tcp_port":13100,"udp_port":13100,"state":STATE_NONE,},	
		{"name":"SHUNZO-3   ","ip":"201.77.235.51","tcp_port":13100,"udp_port":13100,"state":STATE_NONE,},	
		{"name":"SHUNZO-4   ","ip":"201.77.235.57","tcp_port":13100,"udp_port":13100,"state":STATE_NONE,},	
		{"name":"SHUNZO-5   ","ip":"201.77.235.54","tcp_port":13100,"udp_port":13100,"state":STATE_NONE,},	
		{"name":"SHUNZO-6   ","ip":"201.77.235.52","tcp_port":13100,"udp_port":13100,"state":STATE_NONE,},	
		{"name":"SHUNZO-7   ","ip":"201.77.235.56","tcp_port":13100,"udp_port":13100,"state":STATE_NONE,},	
	]

	HORAN_CHANNEL = [
		{"name":"HORAN-1   ","ip":"201.77.235.51","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"HORAN-2   ","ip":"201.77.235.52","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"HORAN-3   ","ip":"201.77.235.53","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"HORAN-4   ","ip":"201.77.235.54","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"HORAN-5   ","ip":"201.77.235.55","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"HORAN-6   ","ip":"201.77.235.56","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"HORAN-7   ","ip":"201.77.235.57","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
	]

	RAIZAN_CHANNEL = [

		{"name":"RAIZAN-1   ","ip":"201.77.235.58","tcp_port":13200,"udp_port":13200,"state":STATE_NONE,},
		{"name":"RAIZAN-2   ","ip":"201.77.235.59","tcp_port":13200,"udp_port":13200,"state":STATE_NONE,},
		{"name":"RAIZAN-3   ","ip":"201.77.235.58","tcp_port":13210,"udp_port":13210,"state":STATE_NONE,},
		{"name":"RAIZAN-4   ","ip":"201.77.235.59","tcp_port":13210,"udp_port":13210,"state":STATE_NONE,},
	]
	SHUNZO_MARK = { "ip" : "201.77.235.52", "tcp_port" : 13100, "mark" : "20.tga", "symbol_path" : "20", }	
	HORAN_MARK = { "ip" : "201.77.235.52", "tcp_port" : 13000, "mark" : "10.tga", "symbol_path" : "10", }	
	RAIZAN_MARK = { "ip" : "201.77.235.59", "tcp_port" : 13210, "mark" : "30.tga", "symbol_path" : "30", }	
		
	SHUNZO_AUTH = { "ip":"201.77.235.50", "port":11101, }
	HORAN_AUTH = { "ip":"201.77.235.50", "port":11001, }
	RAIZAN_AUTH = { "ip":"201.77.235.50", "port":11201, }
	

	RAIZAN = { "name" : "RAIZAN" }
	SHUNZO = { "name" : "SHUNZO" }						
	HORAN= { "name" : "HORAN"}					


	TESTADDR = { "ip" : "210.123.10.153", "tcp_port" : 50000, "udp_port" : 50000, }
	
	REGION0_ORDER_LIST = [
		(RAIZAN, RAIZAN_AUTH, RAIZAN_MARK, RAIZAN_CHANNEL),
		(SHUNZO, SHUNZO_AUTH, SHUNZO_MARK, SHUNZO_CHANNEL),
		(HORAN, HORAN_AUTH, HORAN_MARK, HORAN_CHANNEL),
		]
	# BUILD
	NEW_REGION0, NEW_REGION0_AUTH_SERVER_DICT, NEW_MARKADDR_DICT = BuildServerList(REGION0_ORDER_LIST)

	# RESULT
	NEW_REGION_NAME_DICT = {
		0 : "BRAZIL",		
	}

	NEW_REGION_AUTH_SERVER_DICT = {
		0 : NEW_REGION0_AUTH_SERVER_DICT,
	}

	NEW_REGION_DICT = {
		0 : NEW_REGION0,
	}

	MARKADDR_DICT = NEW_MARKADDR_DICT
	REGION_DICT = NEW_REGION_DICT
	REGION_NAME_DICT = NEW_REGION_NAME_DICT
	REGION_AUTH_SERVER_DICT = NEW_REGION_AUTH_SERVER_DICT

if locale.IsNEWCIBN():
	STATE_NONE = "..."
					
	STATE_DICT = {
		0 : "Œ¨ª§÷–",
		1 : "’˝≥£",
		2 : "∑±√¶",
		3 : "±¨¬˙"
	}


	TE1_CHANNELS = [
		{"name":"“ª∑˛","ip":"218.240.37.86","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"∂˛∑˛","ip":"218.240.37.87","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"»˝∑˛","ip":"218.240.37.88","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
		{"name":"Àƒ∑˛","ip":"218.240.37.89","tcp_port":13000,"udp_port":13000,"state":STATE_NONE,},	
	]

	TE1_MARK = { "ip" : "218.240.37.87",	"tcp_port" : 13000, "mark" : "50.tga", "symbol_path" : "50", }
		
	TE1_AUTH = { "ip":"218.240.37.85",	"port":11061, }

	TE1 = { "name" : "À´œﬂ◊€∫œ«¯"}

	TESTADDR = { "ip" : "210.123.10.153", "tcp_port" : 50000, "udp_port" : 50000, }

	# ORDER
	REGION0_ORDER_LIST = [
		(TE1, TE1_AUTH, TE1_MARK, TE1_CHANNELS),
	]

	# BUILD
	NEW_REGION0, NEW_REGION0_AUTH_SERVER_DICT, NEW_MARKADDR_DICT = BuildServerList(REGION0_ORDER_LIST)

	# RESULT
	NEW_REGION_NAME_DICT = {
		0 : "CHINA_NEWCIBN",		
	}

	NEW_REGION_AUTH_SERVER_DICT = {
		0 : NEW_REGION0_AUTH_SERVER_DICT,
	}

	NEW_REGION_DICT = {
		0 : NEW_REGION0,
	}

	MARKADDR_DICT = NEW_MARKADDR_DICT
	REGION_DICT = NEW_REGION_DICT
	REGION_NAME_DICT = NEW_REGION_NAME_DICT
	REGION_AUTH_SERVER_DICT = NEW_REGION_AUTH_SERVER_DICT

elif locale.IsJAPAN():
	STATE_NONE = "..."
					
	STATE_DICT = {
		0 : "...",
		1 : "ê≥èÌ",
		2 : "ç¨éG",
		3 : "FULL"
	}

	SERVER01_CHANNEL_DICT = {
		1:{"key":11,"name":"CHANNEL1   ","ip":"202.229.16.24","tcp_port":11000,"udp_port":11000,"state":STATE_NONE,},
		#2:{"key":12,"name":"CHANNEL2   ","ip":"202.229.16.24","tcp_port":12000,"udp_port":12000,"state":STATE_NONE,},
	}

	MARKADDR_DICT = {
		10 : { "ip" : "202.229.16.24", "tcp_port" : 11000, "mark" : "10.tga", "symbol_path" : "10", },
	}

	REGION_NAME_DICT = {
		0 : "JAPAN",
	}

	REGION_AUTH_SERVER_DICT = {
		0 : {
			1 : { "ip":"202.229.16.24", "port":10001, },
		}
	}

	REGION_DICT = {
		0 : {
			1 : { "name" : "âûó≥", "channel" : SERVER01_CHANNEL_DICT, },
		},
	}

	TESTADDR = { "ip" : "202.229.16.4", "tcp_port" : 50000, "udp_port" : 50000, }


elif locale.IsYMIR():
	STATE_NONE = "..."
					
					
	STATE_DICT = {
		0 : "¡°∞À",
		1 : "∫∏≈Î",
		2 : "»•¿‚",
		3 : "FULL"
	}

	SERVER01_CHANNEL_DICT = {
		1:{"key":11,"name":"√§≥Œ 1   ","ip":"202.31.212.51","tcp_port":50010,"udp_port":50010,"state":STATE_NONE,},
		2:{"key":12,"name":"√§≥Œ 2   ","ip":"202.31.212.51","tcp_port":50020,"udp_port":50020,"state":STATE_NONE,},
		3:{"key":13,"name":"√§≥Œ 3   ","ip":"202.31.212.51","tcp_port":50030,"udp_port":50030,"state":STATE_NONE,},
		4:{"key":14,"name":"√§≥Œ 4   ","ip":"202.31.212.51","tcp_port":50040,"udp_port":50040,"state":STATE_NONE,},
#		5:{"key":15,"name":"√§≥Œ 5   ","ip":"202.31.212.51","tcp_port":50051,"udp_port":50051,"state":STATE_NONE,},		
#		5:{"key":15,"name":"π´«—¥Î¿¸ ","ip":"220.95.239.35","tcp_port":50100,"udp_port":50100,"state":STATE_NONE,},		
	}

	#6:{"key":16,"name":"¥Î∑√ ¿Ã∫•∆Æ","ip":"220.95.239.35","tcp_port":50100,"udp_port":50100,"state":STATE_NONE,},

	REGION_NAME_DICT = {
		0 : "KOREA",		
	}

	REGION_AUTH_SERVER_DICT = {
		0 : {
			1 : { "ip":"202.31.212.51", "port":51000, },
			2 : { "ip":"202.31.212.15", "port":51000, },
		}		
	}

	REGION_DICT = {
		0 : {
			1 : { "name" : "√µ∏∂ º≠πˆ", "channel" : SERVER01_CHANNEL_DICT, },
		},
	}

	MARKADDR_DICT = {
		10 : { "ip" : "202.31.212.51", "tcp_port" : 50040, "mark" : "01.tga", "symbol_path" : "10", },
	}

	TESTADDR = { "ip" : "220.95.239.62", "tcp_port" : 50000, "udp_port" : 50000, }

elif locale.IsWE_KOREA():
	STATE_NONE = "..."
					
					
	STATE_DICT = {
		0 : "¡°∞À",
		1 : "∫∏≈Î",
		2 : "»•¿‚",
		3 : "FULL"
	}


	SERVER01_CHANNEL_DICT = {
		1:{"key":11,"name":"√§≥Œ 1   ","ip":"202.31.212.15","tcp_port":50010,"udp_port":50010,"state":STATE_NONE,},	
		2:{"key":12,"name":"√§≥Œ 2   ","ip":"202.31.212.15","tcp_port":50020,"udp_port":50020,"state":STATE_NONE,},	
	}

	REGION_NAME_DICT = {
		0 : "KOREA",		
	}

	REGION_AUTH_SERVER_DICT = {
		0 : {
			1 : { "ip":"202.31.212.15", "port":51000, },
		}		
	}

	REGION_DICT = {
		0 : {
			1 : { "name" : "ƒËµµ º≠πˆ", "channel" : SERVER01_CHANNEL_DICT, },
		},
	}

	MARKADDR_DICT = {
		10 : { "ip" : "202.31.212.15", "tcp_port" : 50040, "mark" : "02.tga", "symbol_path" : "20", },
	}

	TESTADDR = { "ip" : "220.95.239.62", "tcp_port" : 50000, "udp_port" : 50000, }

elif locale.IsTAIWAN():
	STATE_NONE = "..."

	STATE_DICT = {
		0 : "....",
		1 : "NORM",
		2 : "BUSY",
		3 : "FULL"
	}

	SERVER01_CHANNEL_DICT = {
		1:{"key":11,"name":"CHANNEL1   ","ip":"203.69.141.201","tcp_port":50010,"udp_port":50010,"state":STATE_NONE,},
		2:{"key":12,"name":"CHANNEL2   ","ip":"203.69.141.201","tcp_port":50020,"udp_port":50020,"state":STATE_NONE,},
	}

	MARKADDR_DICT = {
		10 : { "ip" : "203.69.141.201", "tcp_port" : 50010, "mark" : "10.tga", "symbol_path" : "10", },
	}

	REGION_NAME_DICT = {
		0 : "TAIWAN",
	}

	REGION_AUTH_SERVER_DICT = {
		0 : {
			1 : { "ip":"203.69.141.201", "port":51000, },
		}
	}

	REGION_DICT = {
		0 : {
			1 : { "name" : "¿sæs", "channel" : SERVER01_CHANNEL_DICT, },
		},
	}

	TESTADDR = { "ip" : "203.69.141.201", "tcp_port" : 50000, "udp_port" : 50000, }
	
if locale.IsEUROPE():
	name = app.GetLocalePath().replace("/", "_") + ".addr"
	path = os.sep.join(("pack", name))
	if os.access(path, os.R_OK):
		print "load_locale_addr:", path

		data = app.LoadLocaleAddr(path)

		import cPickle
		import cStringIO
		info = cPickle.load(cStringIO.StringIO(data))

		STATE_NONE = "..."

		STATE_DICT = {
			0 : "....",
			1 : "NORM",
			2 : "BUSY",
			3 : "FULL"
			}
		
		SERVER_ID_DICT = info["SERVERID"]
		REGION_NAME_DICT = info["NAME"]
		REGION_AUTH_SERVER_DICT = info["AUTHADDR"]
		REGION_DICT = info["GAMEADDR"]
		MARKADDR_DICT = info["MARKADDR"]
