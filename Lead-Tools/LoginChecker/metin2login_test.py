import sys
sys.path.append("bin")

import logging

from metin2login import Client

from xml.dom import minidom

def GenServerList(filePath):
    xmlTree = minidom.parse(filePath)
    rootNodes = xmlTree.getElementsByTagName("server_list")
    for rootNode in rootNodes:
        regionNodes = rootNode.getElementsByTagName("region")
        for regionNode in regionNodes:
            regionName = regionNode.getAttribute("name")
            serverNodes = regionNode.getElementsByTagName("server")
            for serverIndex, serverNode in enumerate(serverNodes):
                serverName = serverNode.getAttribute("name")
                authIp = serverNode.getAttribute("auth_ip")
                authPort = int(serverNode.getAttribute("auth_port"))
                gamePort = serverNode.getAttribute("game_port")
                gamePort = int(gamePort) if gamePort else None
                channelNodes = serverNode.getElementsByTagName("channel")
                for channelIndex, channelNode in enumerate(channelNodes):
                    gameIp = channelNode.getAttribute("ip")
                    channelPort = channelNode.getAttribute("port")
                    channelPort = int(channelPort) if channelPort else gamePort
                    yield regionName, serverName, serverIndex+1, channelIndex+1, (authIp, authPort), (gameIp, channelPort)

# for detail log
#logging.basicConfig(level=logging.DEBUG)
logging.basicConfig(level=logging.INFO)

try:
    workings = open("metin2login_test.working.log", "rb").read().splitlines()
except IOError:
    workings = []

workingFile = open("metin2login_test.working.log", "wb")
workingFile.write("\n".join(workings))
workingFile.write("\n")
workingFile.flush()

errorFile = open("metin2login_test.error.log", "a")

workings = set(workings)
for regionName, serverName, serverIndex, channelIndex, authAddr, gameAddr in GenServerList("server_list.xml"):
    key = "%s:%s:%.2d:%.2d" % (regionName, serverName, serverIndex, channelIndex)

    if key in workings:
        print "skip:", key
    else:
        print regionName, serverIndex, channelIndex
        client = Client()
        client.SetLoginInfo("test11", "1234")
        client.SetAuthAddr(authAddr)
        client.SetGameAddr(gameAddr)
        result = client.MainLoop()
        if result == "OK":
            print "ok"
        else:
            print "ERROR:", result
            errorFile.write("%s:%s:%s:%s\n" % (key, authAddr, gameAddr, result))
            errorFile.flush()

        client.Close()
        workings.add(key)
        workingFile.write("%s\n" % (key.encode("utf8")))
        workingFile.flush()
        
