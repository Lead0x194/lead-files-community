import dbg
import app
import net
import ui
import ime
import snd
import wndMgr
import musicInfo
import serverInfo
import systemSetting
import ServerStateChecker
import localeInfo
import constInfo
import uiCommon
import time
import serverCommandParser
import ime
import uiScriptLocale
import uiTarget
import math

LOGIN_DELAY_SEC = 0.0
SKIP_LOGIN_PHASE = False
SKIP_LOGIN_PHASE_SUPPORT_CHANNEL = False
FULL_BACK_IMAGE = False

PASSPOD_MSG_DICT = {}

def IsFullBackImage():
	global FULL_BACK_IMAGE
	return FULL_BACK_IMAGE

def IsLoginDelay():
	global LOGIN_DELAY_SEC
	if LOGIN_DELAY_SEC > 0.0:
		return True
	else:
		return False

def GetLoginDelay():
	global LOGIN_DELAY_SEC
	return LOGIN_DELAY_SEC

app.SetGuildMarkPath("test")

class ConnectingDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__LoadDialog()
		self.eventTimeOver = lambda *arg: None
		self.eventExit = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __LoadDialog(self):
		try:
			PythonScriptLoader = ui.PythonScriptLoader()
			PythonScriptLoader.LoadScriptFile(self, "UIScript/ConnectingDialog.py")

			self.board = self.GetChild("board")
			self.message = self.GetChild("message")
			self.countdownMessage = self.GetChild("countdown_message")

		except:
			import exception
			exception.Abort("ConnectingDialog.LoadDialog.BindObject")

	def Open(self, waitTime):
		curTime = time.clock()
		self.endTime = curTime + waitTime

		self.Lock()
		self.SetCenterPosition()
		self.SetTop()
		self.Show()		

	def Close(self):
		self.Unlock()
		self.Hide()

	def Destroy(self):
		self.Hide()
		self.ClearDictionary()

	def SetText(self, text):
		self.message.SetText(text)

	def SetCountDownMessage(self, waitTime):
		self.countdownMessage.SetText("%.0f%s" % (waitTime, localeInfo.SECOND))

	def SAFE_SetTimeOverEvent(self, event):
		self.eventTimeOver = ui.__mem_func__(event)

	def SAFE_SetExitEvent(self, event):
		self.eventExit = ui.__mem_func__(event)

	def OnUpdate(self):
		lastTime = max(0, self.endTime - time.clock())
		if 0 == lastTime:
			self.Close()
			self.eventTimeOver()
		else:
			self.SetCountDownMessage(self.endTime - time.clock())

	def OnPressExitKey(self):
		#self.eventExit()
		return True

PORTAL_PATH = "locale/common/ui/login/portal/"
PORTAL_ORBIT_RADIUS = 234
PORTAL_UNIT_LIST = (2.0, 1.5, 1.25, 1.0)
PORTAL_ROTATE_TIME = 0.55
PORTAL_MESSAGE_INFO = 0
PORTAL_MESSAGE_OK = 1
PORTAL_MESSAGE_ERROR = 2

def PortalFont(size):
	return "%s:%d" % (localeInfo.UI_DEF_FONT.split(":")[0], size)

class PortalImage(ui.ExpandedImageBox):

	def __init__(self):
		ui.ExpandedImageBox.__init__(self)
		self.overInEvent = None
		self.overOutEvent = None
		self.imagePath = PORTAL_PATH
		self.index = 0

	def __del__(self):
		ui.ExpandedImageBox.__del__(self)

	def SetImage(self, fileName):
		self.LoadImage(self.imagePath + fileName)

	def OnMouseOverIn(self):
		if self.overInEvent:
			self.overInEvent(self.index)

	def OnMouseOverOut(self):
		if self.overOutEvent:
			self.overOutEvent(self.index)

class LoginWindow(ui.ScriptWindow):

	def __init__(self, stream):
		ui.ScriptWindow.__init__(self)
		net.SetPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(self)

		self.stream = stream
		self.connectingDialog = None
		self.isNowCountDown = False
		self.idEditLine = None
		self.pwdEditLine = None
		self.loginFailureMsgDict = {}
		self.loginFailureFuncDict = {}
		self.objectList = []
		self.flagList = []
		self.serverItemDict = {}
		self.channelItemDict = {}
		self.selectedServerID = 0
		self.selectedChannelID = 0
		self.focusState = -1
		self.serverName = None
		self.serverIDList = []
		self.slotList = []
		self.messageLineList = []
		self.messageShown = False
		self.messageEndTime = 0.0
		self.ringImage = None
		self.errorField = ""
		self.errorSnapshot = ("", "")
		self.accountDict = {}
		self.loginButton = None
		self.unit = 1.0
		self.imagePath = PORTAL_PATH
		self.rotateIndex = -1
		self.rotateTarget = 0.0
		self.rotateStart = 0.0

	def __del__(self):
		net.ClearPhaseWindow(net.PHASE_WINDOW_LOGIN, self)
		net.SetAccountConnectorHandler(0)
		ui.ScriptWindow.__del__(self)

	def Open(self):
		ServerStateChecker.Create(self)

		self.__RefreshLanguageTexts()

		self.loginFailureFuncDict = {
			"WRONGPWD"	: self.__DisconnectAndInputPassword,
			"QUIT"		: app.Exit,
		}

		self.SetSize(wndMgr.GetScreenWidth(), wndMgr.GetScreenHeight())
		self.SetWindowName("LoginWindow")

		self.__BuildWindow()
		self.__LoadLoginInfo("loginInfo.xml")

		if musicInfo.loginMusic != "":
			snd.SetMusicVolume(systemSetting.GetMusicVolume())
			snd.FadeInMusic("BGM/" + musicInfo.loginMusic)

		snd.SetSoundVolume(systemSetting.GetSoundVolume())

		ime.AddExceptKey(91)
		ime.AddExceptKey(93)

		self.Show()
		self.idEditLine.SetFocus()
		app.ShowCursor()

	def Close(self):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		ServerStateChecker.Initialize(self)

		if musicInfo.loginMusic != "" and musicInfo.selectMusic != "":
			snd.FadeOutMusic("BGM/" + musicInfo.loginMusic)

		if self.idEditLine:
			self.idEditLine.SetTabEvent(0)
			self.idEditLine.SetReturnEvent(0)
		if self.pwdEditLine:
			self.pwdEditLine.SetReturnEvent(0)
			self.pwdEditLine.SetTabEvent(0)

		for flag in self.flagList + self.slotList:
			flag.overInEvent = None
			flag.overOutEvent = None
		if self.loginButton:
			self.loginButton.overInEvent = None
			self.loginButton.overOutEvent = None

		self.idEditLine = None
		self.pwdEditLine = None
		self.userField = None
		self.lockField = None
		self.loginButton = None
		self.languageName = None
		self.serverName = None
		self.messageLineList = []
		self.slotList = []
		self.channelRing = None
		self.flagList = []
		self.serverItemDict = {}
		self.channelItemDict = {}
		self.objectList = []

		self.KillFocus()
		self.Hide()

		self.stream.popupWindow.Close()
		self.loginFailureFuncDict = None

		ime.ClearExceptKey()
		app.HideCursor()

	def __X(self, dx):
		return self.centerX + int(round(dx * self.unit))

	def __Y(self, dy):
		return self.centerY + int(round(dy * self.unit))

	def __MakeImage(self, fileName, dx, dy, pick = False):
		image = PortalImage()
		image.SetParent(self)
		image.imagePath = self.imagePath
		image.SetImage(fileName)
		image.SetPosition(self.__X(dx), self.__Y(dy))
		if not pick:
			image.AddFlag("not_pick")
		image.Show()
		self.objectList.append(image)
		return image

	def __MakeText(self, parent, x, y, text, color, fontSize = 12):
		textLine = ui.TextLine()
		textLine.SetParent(parent)
		textLine.SetPosition(x, y)
		textLine.SetFontName(PortalFont(int(round(fontSize * self.unit))))
		textLine.SetPackedFontColor(color)
		textLine.SetText(text)
		textLine.AddFlag("not_pick")
		textLine.Show()
		self.objectList.append(textLine)
		return textLine

	def __MakeEditLine(self, dx, dy, width, secret):
		editLine = ui.EditLine()
		editLine.SetParent(self)
		editLine.SetPosition(self.__X(dx), self.__Y(dy))
		editLine.SetSize(int(width * self.unit), int(22 * self.unit))
		editLine.SetFontName(PortalFont(int(round(13 * self.unit))))
		editLine.SetMax(16)
		editLine.SetIMEFlag(0)
		if secret:
			editLine.SetSecret()
		editLine.SetPackedFontColor(0xfff5f2eb)
		editLine.Show()
		return editLine

	def __BuildWindow(self):
		screenWidth = wndMgr.GetScreenWidth()
		screenHeight = wndMgr.GetScreenHeight()
		fit = min(float(screenHeight) / 720.0, float(screenWidth) / 1180.0)
		self.unit = 1.0
		for unit in PORTAL_UNIT_LIST:
			if unit <= fit * 1.06:
				self.unit = unit
				break
		self.imagePath = PORTAL_PATH + "%d/" % int(self.unit * 100)
		self.centerX = screenWidth / 2
		self.centerY = screenHeight / 2 + int(12 * self.unit)

		background = ui.ExpandedImageBox()
		background.SetParent(self)
		background.LoadImage(PORTAL_PATH + "background.sub")
		background.SetScale(float(screenWidth) / 1920.0, float(screenHeight) / 1080.0)
		background.SetPosition(0, 0)
		background.AddFlag("not_pick")
		background.Show()
		self.objectList.append(background)

		self.ringImage = self.__MakeImage("ring.tga", -280, -280)

		self.userField = self.__MakeImage("field_user_1.tga", -143, -22 - 28)
		self.lockField = self.__MakeImage("field_lock_0.tga", -143, 32 - 28)

		self.idEditLine = self.__MakeEditLine(-89, -22 - 10, 200, False)
		self.pwdEditLine = self.__MakeEditLine(-89, 32 - 10, 168, True)

		self.loginButton = self.__MakeImage("arrow_0.tga", 135 - 20 - 18, 32 - 18, True)
		self.loginButton.overInEvent = ui.__mem_func__(self.__OnOverInLoginButton)
		self.loginButton.overOutEvent = ui.__mem_func__(self.__OnOverOutLoginButton)
		self.loginButton.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__OnClickLoginButton))

		self.idEditLine.SetReturnEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		self.idEditLine.SetTabEvent(ui.__mem_func__(self.pwdEditLine.SetFocus))
		self.pwdEditLine.SetReturnEvent(ui.__mem_func__(self.__OnClickLoginButton))
		self.pwdEditLine.SetTabEvent(ui.__mem_func__(self.idEditLine.SetFocus))

		self.__BuildLanguageRing()
		self.__BuildServerBlock()
		self.__BuildAccountSlots()
		self.__BuildStatus()

	def __OnOverInLoginButton(self, index):
		self.loginButton.SetImage("arrow_1.tga")

	def __OnOverOutLoginButton(self, index):
		self.loginButton.SetImage("arrow_0.tga")

	def __BuildLanguageRing(self):
		self.languageList = localeInfo.GetLanguageList()
		currentIndex = 0
		for index, (code, name) in enumerate(self.languageList):
			if code == app.GetLocaleName():
				currentIndex = index

		self.currentLanguageIndex = currentIndex
		self.__MakeImage("flag_ring.tga", -28, -PORTAL_ORBIT_RADIUS - 28)

		for index, (code, name) in enumerate(self.languageList):
			flag = self.__MakeImage("flag_%s_%d.tga" % (code, index == currentIndex), 0, 0, True)
			flag.code = code
			flag.index = index
			flag.overInEvent = ui.__mem_func__(self.__OnOverInLanguage)
			flag.overOutEvent = ui.__mem_func__(self.__OnOverOutLanguage)
			flag.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__OnSelectLanguage), index)
			self.flagList.append(flag)

		self.__PlaceFlags(0.0, 0.0)

		self.languageName = self.__MakeText(self, self.__X(0), self.__Y(128), "", 0xffc3ccd6, 11)
		self.languageName.SetHorizontalAlignCenter()

	def __OnOverInLanguage(self, index):
		self.__LightFlag(index, True)
		self.languageName.SetText(self.languageList[index][1])

	def __OnOverOutLanguage(self, index):
		if index != self.currentLanguageIndex and index != self.rotateIndex:
			self.__LightFlag(index, False)
		self.languageName.SetText("")

	def __OnSelectLanguage(self, index):
		if self.rotateIndex >= 0 or index == self.currentLanguageIndex:
			return

		count = max(1, len(self.languageList))
		target = -(index - self.currentLanguageIndex) * 2.0 * math.pi / count
		while target > math.pi:
			target -= 2.0 * math.pi
		while target < -math.pi:
			target += 2.0 * math.pi

		self.rotateIndex = index
		self.__LightFlag(index, True)
		self.rotateTarget = target
		self.rotateStart = app.GetTime()

	def __PlaceFlags(self, offset, progress):
		count = max(1, len(self.languageList))
		for index, flag in enumerate(self.flagList):
			angle = -math.pi / 2.0 + (index - self.currentLanguageIndex) * 2.0 * math.pi / count + offset
			flag.SetPosition(self.__X(PORTAL_ORBIT_RADIUS * math.cos(angle) - 17), self.__Y(PORTAL_ORBIT_RADIUS * math.sin(angle) - 17))

	def __LightFlag(self, index, bright):
		flag = self.flagList[index]
		flag.SetImage("flag_%s_%d.tga" % (flag.code, bright))

	def __UpdateRotation(self):
		progress = min(1.0, (app.GetTime() - self.rotateStart) / PORTAL_ROTATE_TIME)
		eased = progress * progress * (3.0 - 2.0 * progress)
		self.__PlaceFlags(self.rotateTarget * eased, eased)

		if progress < 1.0:
			return

		index = self.rotateIndex
		self.rotateIndex = -1
		if localeInfo.SetLanguage(self.languageList[index][0]):
			self.currentLanguageIndex = index
			self.__RefreshLanguageTexts()

		self.__PlaceFlags(0.0, 0.0)
		for flagIndex in xrange(len(self.flagList)):
			self.__LightFlag(flagIndex, flagIndex == self.currentLanguageIndex)

	def __RefreshLanguageTexts(self):
		self.loginFailureMsgDict = {
			"ALREADY"	: localeInfo.LOGIN_FAILURE_ALREAY,
			"NOID"		: localeInfo.LOGIN_FAILURE_NOT_EXIST_ID,
			"WRONGPWD"	: localeInfo.LOGIN_FAILURE_WRONG_PASSWORD,
			"FULL"		: localeInfo.LOGIN_FAILURE_TOO_MANY_USER,
			"SHUTDOWN"	: localeInfo.LOGIN_FAILURE_SHUTDOWN,
			"REPAIR"	: localeInfo.LOGIN_FAILURE_REPAIR_ID,
			"BLOCK"		: localeInfo.LOGIN_FAILURE_BLOCK_ID,
			"BESAMEKEY"	: localeInfo.LOGIN_FAILURE_BE_SAME_KEY,
			"NOTAVAIL"	: localeInfo.LOGIN_FAILURE_NOT_AVAIL,
			"NOBILL"	: localeInfo.LOGIN_FAILURE_NOBILL,
			"BLKLOGIN"	: localeInfo.LOGIN_FAILURE_BLOCK_LOGIN,
			"WEBBLK"	: localeInfo.LOGIN_FAILURE_WEB_BLOCK,
			"BADSCLID"	: localeInfo.LOGIN_FAILURE_WRONG_SOCIALID,
			"AGELIMIT"	: localeInfo.LOGIN_FAILURE_SHUTDOWN_TIME,
		}


	def __GetServerDict(self):
		return serverInfo.REGION_DICT.get(0, {})

	def __BuildServerBlock(self):
		serverDict = self.__GetServerDict()
		self.serverIDList = serverDict.keys()
		self.serverIDList.sort()

		loadServerID, loadChannelID = self.__LoadChannelInfo()
		if not serverDict.has_key(loadServerID):
			loadServerID = self.serverIDList[0] if self.serverIDList else 0

		self.serverName = self.__MakeText(self, self.__X(0), self.__Y(-116), "", 0xffffffff, 13)
		self.serverName.SetHorizontalAlignCenter()

		if len(self.serverIDList) > 1:
			for step, dx in ((-1, -76), (1, 52)):
				item = ui.Window()
				item.SetParent(self)
				item.SetPosition(self.__X(dx), self.__Y(-122))
				item.SetSize(int(24 * self.unit), int(26 * self.unit))
				item.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__StepServer), step)
				item.Show()
				item.textLine = self.__MakeText(item, int(12 * self.unit), int(2 * self.unit), "<" if step < 0 else ">", 0xffc3ccd6, 13)
				item.textLine.SetHorizontalAlignCenter()
				self.objectList.append(item)
		else:
			self.serverName.Hide()

		self.channelRing = self.__MakeImage("channel_on.tga", 0, 0)
		self.channelRing.Hide()

		self.selectedChannelID = loadChannelID
		self.__SelectServer(loadServerID)

	def __StepServer(self, step):
		if not self.serverIDList:
			return

		try:
			index = self.serverIDList.index(self.selectedServerID)
		except ValueError:
			index = 0

		self.__SelectServer(self.serverIDList[(index + step) % len(self.serverIDList)])

	def __SelectServer(self, serverID):
		serverDict = self.__GetServerDict()
		if not serverDict.has_key(serverID):
			return

		self.selectedServerID = serverID
		self.serverName.SetText(serverDict[serverID].get("name", ""))

		for item in self.channelItemDict.values():
			item.dot.Hide()
			item.Hide()
		self.channelItemDict = {}

		channelDict = serverDict[serverID].get("channel", {})
		channelIDList = channelDict.keys()
		channelIDList.sort()

		itemWidth = 64
		startDX = -len(channelIDList) * itemWidth / 2.0

		for index, channelID in enumerate(channelIDList):
			posDX = startDX + index * itemWidth

			item = ui.Window()
			item.SetParent(self)
			item.SetPosition(self.__X(posDX), self.__Y(-90))
			item.SetSize(int(itemWidth * self.unit), int(24 * self.unit))
			item.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__SelectChannel), channelID)
			item.Show()

			item.dot = self.__MakeImage("dot_none.tga", posDX + 8, -84)
			item.textLine = self.__MakeText(item, int(40 * self.unit), int(4 * self.unit), channelDict[channelID].get("name", str(channelID)), 0xffc3ccd6, 11)
			item.textLine.SetHorizontalAlignCenter()
			item.posDX = posDX
			self.channelItemDict[channelID] = item

		if not channelDict.has_key(self.selectedChannelID):
			self.selectedChannelID = channelIDList[0] if channelIDList else 0

		self.__SelectChannel(self.selectedChannelID)
		self.__RequestServerStateList()

	def __SelectChannel(self, channelID):
		if not self.channelItemDict.has_key(channelID):
			return

		self.selectedChannelID = channelID

		for eachChannelID, item in self.channelItemDict.items():
			if eachChannelID == channelID:
				item.textLine.SetPackedFontColor(0xffffffff)
				self.channelRing.SetPosition(self.__X(item.posDX + 2), self.__Y(-91))
				self.channelRing.Show()
			else:
				item.textLine.SetPackedFontColor(0xff9aa4b0)

	def __BuildAccountSlots(self):
		self.accountDict = self.__LoadAccounts()
		self.slotList = []

		for slot in xrange(9):
			dx = (slot - 4) * 30 - 13

			image = self.__MakeImage("slot_0.tga", dx, 92, True)
			image.index = slot
			image.overInEvent = ui.__mem_func__(self.__OnOverInSlot)
			image.overOutEvent = ui.__mem_func__(self.__OnOverOutSlot)
			image.SetMouseLeftButtonDownEvent(ui.__mem_func__(self.__OnClickSlot), slot)
			image.SetMouseRightButtonDownEvent(ui.__mem_func__(self.__ClearAccountSlot), slot)
			image.textLine = self.__MakeText(image, int(13 * self.unit), int(13 * self.unit) - 1, "F%d" % (slot + 1), 0xff9aa4b0, 8)
			image.textLine.SetHorizontalAlignCenter()
			image.textLine.SetVerticalAlignCenter()
			image.posDX = dx
			self.slotList.append(image)

		self.__RefreshAccountSlots()

	def __OnClickSlot(self, slot):
		if self.accountDict.has_key(slot):
			self.__UseAccountSlot(slot)
		else:
			self.__StoreAccountSlot(slot)

	def __RefreshAccountSlots(self):
		for slot, image in enumerate(self.slotList):
			if self.accountDict.has_key(slot):
				image.SetImage("slot_1.tga")
				image.textLine.SetPackedFontColor(0xffffffff)
			else:
				image.SetImage("slot_0.tga")
				image.textLine.SetPackedFontColor(0xff9aa4b0)

	def __OnOverInSlot(self, slot):
		if self.messageShown:
			return

		if self.accountDict.has_key(slot):
			self.languageName.SetText(self.accountDict[slot][0])
		else:
			self.slotList[slot].textLine.SetText("+")

	def __OnOverOutSlot(self, slot):
		self.slotList[slot].textLine.SetText("F%d" % (slot + 1))
		self.languageName.SetText("")

	def __ScrambleKey(self):
		return [ord(c) for c in "Lead:portal:slot"]

	def __Scramble(self, text):
		key = self.__ScrambleKey()
		return "".join(["%02x" % (ord(c) ^ key[i % len(key)]) for i, c in enumerate(text)])

	def __Unscramble(self, text):
		key = self.__ScrambleKey()
		return "".join([chr(int(text[i * 2:i * 2 + 2], 16) ^ key[i % len(key)]) for i in xrange(len(text) / 2)])

	def __LoadAccounts(self):
		accountDict = {}
		try:
			for line in open("accounts.inf").readlines():
				tokens = line.strip().split("\t")
				if len(tokens) == 3:
					accountDict[int(tokens[0])] = (tokens[1], self.__Unscramble(tokens[2]))
		except:
			pass
		return accountDict

	def __SaveAccounts(self):
		try:
			file = open("accounts.inf", "w")
			for slot, (id, pwd) in self.accountDict.items():
				file.write("%d\t%s\t%s\n" % (slot, id, self.__Scramble(pwd)))
			file.close()
		except:
			pass

	def __StoreAccountSlot(self, slot):
		id = self.idEditLine.GetText()
		pwd = self.pwdEditLine.GetText()
		if len(id) == 0 or len(pwd) == 0:
			return

		self.accountDict[slot] = (id, pwd)
		self.__SaveAccounts()
		self.__RefreshAccountSlots()

	def __ClearAccountSlot(self, slot):
		if self.accountDict.has_key(slot):
			del self.accountDict[slot]
			self.__SaveAccounts()
			self.__RefreshAccountSlots()
			self.languageName.SetText("")

	def __UseAccountSlot(self, slot):
		if not self.accountDict.has_key(slot):
			return

		id, pwd = self.accountDict[slot]
		self.idEditLine.SetText(id)
		self.pwdEditLine.SetText(pwd)
		self.__OnClickLoginButton()

	def OnKeyDown(self, key):
		slotKeyList = (app.DIK_F1, app.DIK_F2, app.DIK_F3, app.DIK_F4, app.DIK_F5, app.DIK_F6, app.DIK_F7, app.DIK_F8, app.DIK_F9)
		if key in slotKeyList and self.idEditLine:
			self.__OnClickSlot(slotKeyList.index(key))
		return True

	def __RequestServerStateList(self):
		try:
			channelDict = self.__GetServerDict()[self.selectedServerID]["channel"]
		except KeyError:
			return

		ServerStateChecker.Initialize()
		for channelID, channelDataDict in channelDict.items():
			ServerStateChecker.AddChannel(channelDataDict["key"], channelDataDict["ip"], channelDataDict["udp_port"])

		ServerStateChecker.Request()

	def NotifyChannelState(self, addrKey, state):
		try:
			stateName = serverInfo.STATE_DICT[state]
		except KeyError:
			stateName = serverInfo.STATE_NONE

		channelID = addrKey % 10

		try:
			self.__GetServerDict()[self.selectedServerID]["channel"][channelID]["state"] = stateName
		except KeyError:
			return

		item = self.channelItemDict.get(channelID)
		if item:
			item.dot.SetImage({1 : "dot_norm.tga", 2 : "dot_busy.tga", 3 : "dot_full.tga"}.get(state, "dot_none.tga"))

	def __SaveChannelInfo(self):
		try:
			file = open("channel.inf", "w")
			file.write("%d %d %d" % (self.selectedServerID, self.selectedChannelID, 0))
		except:
			pass

	def __LoadChannelInfo(self):
		try:
			tokens = open("channel.inf").readlines()[0].split()
			return int(tokens[0]), int(tokens[1])
		except:
			return 0, 0

	def __ApplyConnectInfo(self):
		serverDict = self.__GetServerDict()

		try:
			serverData = serverDict[self.selectedServerID]
			channelData = serverData["channel"][self.selectedChannelID]
		except KeyError:
			self.PopupNotifyMessage(localeInfo.CHANNEL_SELECT_CHANNEL)
			return False

		if channelData["state"] == serverInfo.STATE_DICT[3]:
			self.PopupNotifyMessage(localeInfo.CHANNEL_NOTIFY_FULL)
			return False

		self.__SaveChannelInfo()

		net.SetServerInfo(("%s, %s" % (serverData.get("name", ""), channelData["name"])).strip())

		try:
			accountData = serverInfo.REGION_AUTH_SERVER_DICT[0][self.selectedServerID]
			accountIP = accountData["ip"]
			accountPort = accountData["port"]
		except KeyError:
			accountIP = 0
			accountPort = 0

		markAddrValue = serverInfo.MARKADDR_DICT.get(self.selectedServerID * 10)
		if markAddrValue:
			net.SetMarkServer(markAddrValue["ip"], markAddrValue["tcp_port"])
			app.SetGuildMarkPath(markAddrValue["mark"])
			app.SetGuildSymbolPath(markAddrValue["symbol_path"])

		self.stream.SetConnectInfo(channelData["ip"], channelData["tcp_port"], accountIP, accountPort)
		return True

	def __ExitGame(self):
		app.Exit()

	def SetIDEditLineFocus(self):
		if self.idEditLine != None:
			self.idEditLine.SetFocus()

	def SetPasswordEditLineFocus(self):
		if self.pwdEditLine != None:
			self.pwdEditLine.SetText("")
			self.pwdEditLine.SetFocus()

	def OnEndCountDown(self):
		self.isNowCountDown = False
		self.OnConnectFailure()

	def OnConnectFailure(self):
		if self.isNowCountDown:
			return

		snd.PlaySound("sound/ui/loginfail.wav")

		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		self.PopupNotifyMessage(localeInfo.LOGIN_CONNECT_FAILURE, self.SetPasswordEditLineFocus)

	def OnHandShake(self):
		if not IsLoginDelay():
			snd.PlaySound("sound/ui/loginok.wav")
			self.__ShowMessage(localeInfo.LOGIN_CONNECT_SUCCESS, PORTAL_MESSAGE_OK)

	def OnLoginStart(self):
		if not IsLoginDelay():
			self.PopupDisplayMessage(localeInfo.LOGIN_PROCESSING)

	def OnLoginFailure(self, error):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		try:
			loginFailureMsg = self.loginFailureMsgDict[error]
		except KeyError:
			loginFailureMsg = PASSPOD_MSG_DICT.get(error, localeInfo.LOGIN_FAILURE_UNKNOWN + error)

		loginFailureFunc = self.loginFailureFuncDict.get(error, self.SetPasswordEditLineFocus)
		self.PopupNotifyMessage(loginFailureMsg, loginFailureFunc)

		if self.idEditLine:
			self.__SetErrorField({"WRONGPWD" : "pwd", "NOID" : "id"}.get(error, "pwd"))

		snd.PlaySound("sound/ui/loginfail.wav")

	def __DisconnectAndInputPassword(self):
		if self.connectingDialog:
			self.connectingDialog.Close()
		self.connectingDialog = None

		self.SetPasswordEditLineFocus()
		net.Disconnect()

	def Connect(self, id, pwd):
		if constInfo.SEQUENCE_PACKET_ENABLE:
			net.SetPacketSequenceMode()

		if IsLoginDelay():
			loginDelay = GetLoginDelay()
			self.connectingDialog = ConnectingDialog()
			self.connectingDialog.Open(loginDelay)
			self.connectingDialog.SAFE_SetTimeOverEvent(self.OnEndCountDown)
			self.connectingDialog.SAFE_SetExitEvent(self.OnPressExitKey)
			self.isNowCountDown = True
		else:
			self.__ShowMessage(localeInfo.LOGIN_CONNETING, PORTAL_MESSAGE_INFO)

		uiTarget.MONSTER_INFO_DATA.clear()
		self.stream.SetLoginInfo(id, pwd)
		self.stream.Connect()

	def __LoadLoginInfo(self, loginInfoFileName):
		loginInfo = {}
		try:
			execfile(loginInfoFileName, loginInfo)
		except IOError:
			pass

		id = loginInfo.get("id", "")
		pwd = loginInfo.get("pwd", "")
		addr = loginInfo.get("addr", "")
		port = loginInfo.get("port", 0)
		account_addr = loginInfo.get("account_addr", addr)
		account_port = loginInfo.get("account_port", port)

		self.stream.SetCharacterSlot(loginInfo.get("slot", 0))
		self.stream.isAutoLogin = loginInfo.get("autoLogin", 0)
		self.stream.isAutoSelect = loginInfo.get("autoSelect", 0)
		app.loggined = FALSE

		if addr and port:
			net.SetMarkServer(addr, port)
			net.SetServerInfo(addr + ":" + str(port))
			self.stream.SetConnectInfo(addr, port, account_addr, account_port)

			if self.stream.isAutoLogin:
				self.Connect(id, pwd)

	def __BuildStatus(self):
		self.messageLineList = []
		for index in xrange(2):
			textLine = self.__MakeText(self, self.__X(0), self.__Y(92), "", 0xffffffff, 14)
			textLine.SetHorizontalAlignCenter()
			textLine.Hide()
			self.messageLineList.append(textLine)

	def __WrapMessage(self, msg):
		lineList = []
		line = ""
		for word in msg.replace("[ENTER]", " ").split():
			if line and len([c for c in line + " " + word if (ord(c) & 0xC0) != 0x80]) > 30:
				lineList.append(line)
				line = word
			else:
				line = (line + " " + word).strip()
		if line:
			lineList.append(line)
		return lineList[:2]

	def __ShowMessage(self, msg, kind = PORTAL_MESSAGE_INFO):
		if not self.messageLineList:
			return

		lineList = self.__WrapMessage(msg)
		if not lineList:
			self.__HideMessage()
			return

		for slot in self.slotList:
			slot.Hide()
		self.languageName.SetText("")

		color = {PORTAL_MESSAGE_ERROR : 0xffff8f85, PORTAL_MESSAGE_OK : 0xff8ae89a}.get(kind, 0xffd6ecff)
		top = 104 - len(lineList) * 10
		for index, textLine in enumerate(self.messageLineList):
			if index < len(lineList):
				textLine.SetPosition(self.__X(0), self.__Y(top + index * 21))
				textLine.SetPackedFontColor(color)
				textLine.SetText(lineList[index])
				textLine.Show()
			else:
				textLine.Hide()

		self.ringImage.SetImage({PORTAL_MESSAGE_ERROR : "ring_error.tga", PORTAL_MESSAGE_OK : "ring_ok.tga"}.get(kind, "ring.tga"))
		self.messageShown = True
		self.messageEndTime = app.GetTime() + 5.0 if kind == PORTAL_MESSAGE_ERROR else 0.0

	def __HideMessage(self):
		if not self.messageShown or not self.messageLineList:
			return

		self.messageShown = False
		self.messageEndTime = 0.0
		for textLine in self.messageLineList:
			textLine.Hide()
		for slot in self.slotList:
			slot.Show()
		self.ringImage.SetImage("ring.tga")

	def __SetErrorField(self, field):
		self.errorField = field
		self.errorSnapshot = (self.idEditLine.GetText(), self.pwdEditLine.GetText())
		self.focusState = -1

	def __ClearError(self):
		self.errorField = ""
		self.focusState = -1
		self.__HideMessage()

	def PopupDisplayMessage(self, msg):
		self.__ShowMessage(msg, PORTAL_MESSAGE_INFO)

	def PopupNotifyMessage(self, msg, func = 0):
		self.__ShowMessage(msg, PORTAL_MESSAGE_ERROR)
		if func:
			func()

	def OnPressExitKey(self):
		self.stream.popupWindow.Close()
		self.stream.SetPhaseWindow(0)
		return True

	def OnExit(self):
		self.stream.popupWindow.Close()

	def OnUpdate(self):
		ServerStateChecker.Update()

		if not self.idEditLine:
			return

		if self.rotateIndex >= 0:
			self.__UpdateRotation()
			if not self.idEditLine:
				return

		if self.errorField and self.errorSnapshot != (self.idEditLine.GetText(), self.pwdEditLine.GetText()):
			self.__ClearError()

		if self.messageEndTime and app.GetTime() > self.messageEndTime:
			self.__HideMessage()

		focusState = 1 if self.pwdEditLine.IsFocus() else 0
		if focusState != self.focusState:
			self.focusState = focusState
			self.userField.SetImage("field_user_2.tga" if self.errorField == "id" else ("field_user_0.tga" if focusState else "field_user_1.tga"))
			self.lockField.SetImage("field_lock_2.tga" if self.errorField == "pwd" else ("field_lock_1.tga" if focusState else "field_lock_0.tga"))

	def EmptyFunc(self):
		pass

	def __OnClickLoginButton(self):
		id = self.idEditLine.GetText()
		pwd = self.pwdEditLine.GetText()

		self.__ClearError()

		if len(id) == 0:
			self.__SetErrorField("id")
			self.idEditLine.SetFocus()
			return

		if len(pwd) == 0:
			self.__SetErrorField("pwd")
			self.pwdEditLine.SetFocus()
			return

		if not self.__ApplyConnectInfo():
			return

		self.Connect(id, pwd)

	def SameLogin_OpenUI(self):
		self.stream.popupWindow.Close()
