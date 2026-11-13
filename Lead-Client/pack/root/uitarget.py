from __future__ import print_function
import app
import ui
import player
import net
import wndMgr
import messenger
import guild
import chr
import nonplayer
import localeInfo
import constInfo
import uiToolTip
import item

MONSTER_INFO_DATA = {}
def HAS_FLAG(value, flag):
	return (value & flag) == flag

class TargetBoard(ui.ThinBoard):
	class InfoBoard(ui.ThinBoard):
		class ItemListBoxItem(ui.ListBoxExNew.Item):
			def __init__(self, width):
				ui.ListBoxExNew.Item.__init__(self)

				image = ui.ExpandedImageBox()
				image.SetParent(self)
				image.Show()
				self.image = image

				nameLine = ui.TextLine()
				nameLine.SetParent(self)
				nameLine.SetPosition(32 + 5, 0)
				nameLine.Show()
				self.nameLine = nameLine

				self.SetSize(width, 32 + 5)
				self.baseTextY = 0 

			def LoadImage(self, image, name = None):
				self.image.LoadImage(image)
				
				imgWidth = self.image.GetWidth()
				imgHeight = self.image.GetHeight()
				
				calculatedHeight = max(32, imgHeight) + 5
				self.SetSize(self.GetWidth(), calculatedHeight)
				
				self.image.SetPosition(0, (calculatedHeight - imgHeight) // 2)
				self.baseTextY = (calculatedHeight - 15) // 2
				
				if name != None:
					self.SetText(name)
					self.nameLine.SetPosition(imgWidth + 5, self.baseTextY)

			def SetText(self, text):
				self.nameLine.SetText(text)

			def RefreshHeight(self):
				ui.ListBoxExNew.Item.RefreshHeight(self)

				fullHeight = float(self.GetHeight())
				if fullHeight == 0:
					return

				self.image.SetRenderingRect(
					0.0, 
					0.0 - float(self.removeTop) / fullHeight, 
					0.0, 
					0.0 - float(self.removeBottom) / fullHeight
				)
				self.image.SetPosition(0, -self.removeTop)
				
				if self.nameLine:
					imgWidth = self.image.GetWidth()
				
					currentTextY = self.baseTextY - self.removeTop
					self.nameLine.SetPosition(imgWidth + 5, currentTextY)

					currentVisibleHeight = fullHeight - self.removeTop - self.removeBottom
					textHeight = 15 
					if currentTextY < 0 or (currentTextY + textHeight) > currentVisibleHeight:
						self.nameLine.Hide()
					else:
						self.nameLine.Show()

		MAX_ITEM_COUNT = 5

		EXP_BASE_LVDELTA = [
			1,  #  -15 0
			5,  #  -14 1
			10, #  -13 2
			20, #  -12 3
			30, #  -11 4
			50, #  -10 5
			70, #  -9  6
			80, #  -8  7
			85, #  -7  8
			90, #  -6  9
			92, #  -5  10
			94, #  -4  11
			96, #  -3  12
			98, #  -2  13
			100,	#  -1  14
			100,	#  0   15
			105,	#  1   16
			110,	#  2   17
			115,	#  3   18
			120,	#  4   19
			125,	#  5   20
			130,	#  6   21
			135,	#  7   22
			140,	#  8   23
			145,	#  9   24
			150,	#  10  25
			155,	#  11  26
			160,	#  12  27
			165,	#  13  28
			170,	#  14  29
			180,	#  15  30
		]

		STONE_START_VNUM = 28030
		STONE_LAST_VNUM = 28042

		BOARD_WIDTH = 250

		def __init__(self):
			ui.ThinBoard.__init__(self)

			self.HideCorners(self.LT)
			self.HideCorners(self.RT)
			self.HideLine(self.T)

			self.race = 0
			self.hasItems = False

			self.itemTooltip = uiToolTip.ItemToolTip()
			self.itemTooltip.HideToolTip()

			self.stoneImg = None
			self.stoneVnum = None
			self.lastStoneVnum = 0
			self.nextStoneIconChange = 0

			self.race_flags_primary = []
			self.race_flags_elements = []
			self.__PrepareDynamicRaceFlags()

			self.SetSize(self.BOARD_WIDTH, 0)

		def __del__(self):
			ui.ThinBoard.__del__(self)

		def __UpdatePosition(self, targetBoard):
			self.SetPosition(targetBoard.GetLeft() + (targetBoard.GetWidth() - self.GetWidth()) // 2, targetBoard.GetBottom() - 17)

		def Open(self, targetBoard, race):
			self.__LoadInformation(race)

			self.SetSize(self.BOARD_WIDTH, self.yPos + 10)
			self.__UpdatePosition(targetBoard)

			self.Show()

		def Refresh(self):
			self.__LoadInformation(self.race)
			self.SetSize(self.BOARD_WIDTH, self.yPos + 10)

		def Close(self):
			self.itemTooltip.HideToolTip()
			self.Hide()

		def __PrepareDynamicRaceFlags(self):
			ELEMENT_START_FLAG = nonplayer.RACE_FLAG_ATT_ELEC

			for attr_name in dir(nonplayer):
				if not attr_name.startswith("RACE_FLAG_"):
					continue
					
				flag_value = getattr(nonplayer, attr_name)
				if flag_value == 0:
					continue

				is_element = (flag_value >= ELEMENT_START_FLAG)				
				pure_id = attr_name.replace("RACE_FLAG_", "").replace("ATT_", "")
				
				display_name = None
				
				if is_element:
					search_keys = ["TARGET_INFO_RACE_" + pure_id]
				else:
					search_keys = ["TARGET_INFO_RACE_" + pure_id, "TARGET_INFO_NEWRACE_" + pure_id]

				for key in search_keys:
					if hasattr(localeInfo, key):
						display_name = getattr(localeInfo, key)
						break
				
				if display_name:
					if is_element:
						self.race_flags_elements.append((flag_value, display_name))
					else:
						self.race_flags_primary.append((flag_value, display_name))

			self.race_flags_primary.sort()
			self.race_flags_elements.sort()

		def __LoadInformation(self, race):
			self.yPos = 7
			self.children = []
			self.race = race
			self.stoneImg = None
			self.stoneVnum = None
			self.nextStoneIconChange = 0

			self.__LoadInformation_Default(race)
			self.__LoadInformation_Race(race)
			self.__LoadInformation_Resists(race)
			self.__LoadInformation_Drops(race)

		def __LoadInformation_Default_GetHitRate(self, race):
			attacker_dx = nonplayer.GetMonsterDX(race)
			attacker_level = nonplayer.GetMonsterLevel(race)

			self_dx = player.GetStatus(player.DX)
			self_level = player.GetStatus(player.LEVEL)

			iARSrc = min(90, (attacker_dx * 4 + attacker_level * 2) // 6)
			iERSrc = min(90, (self_dx * 4 + self_level * 2) // 6)

			fAR = (float(iARSrc) + 210.0) / 300.0
			fER = (float(iERSrc) * 2 + 5) / (float(iERSrc) + 95) * 3.0 / 10.0

			return fAR - fER
			
		def __LoadInformation_Resists(self, race):
			self.AppendSeperator()
			self.AppendTextLine(localeInfo.TARGET_INFO_RESISTS)
			self.AppendTextLine(localeInfo.TARGET_INFO_RESISTS_LINE0 % (nonplayer.GetMonsterResist(race, nonplayer.MOB_RESIST_SWORD), nonplayer.GetMonsterResist(race, nonplayer.MOB_RESIST_TWOHAND), nonplayer.GetMonsterResist(race, nonplayer.MOB_RESIST_BELL)))
			self.AppendTextLine(localeInfo.TARGET_INFO_RESISTS_LINE1 % (nonplayer.GetMonsterResist(race, nonplayer.MOB_RESIST_DAGGER), nonplayer.GetMonsterResist(race, nonplayer.MOB_RESIST_FAN), nonplayer.GetMonsterResist(race, nonplayer.MOB_RESIST_BOW)))

		def __LoadInformation_Default(self, race):
			self.AppendSeperator()
			self.AppendTextLine(localeInfo.TARGET_INFO_MAX_HP % localeInfo.DottedNumber(nonplayer.GetMonsterMaxHP(race)))

			# calc att damage
			monsterLevel = nonplayer.GetMonsterLevel(race)
			fHitRate = self.__LoadInformation_Default_GetHitRate(race)
			iDamMin, iDamMax = nonplayer.GetMonsterDamage(race)
			iDamMin = int((iDamMin + nonplayer.GetMonsterST(race)) * 2 * fHitRate) + monsterLevel * 2
			iDamMax = int((iDamMax + nonplayer.GetMonsterST(race)) * 2 * fHitRate) + monsterLevel * 2
			iDef = player.GetStatus(player.DEF_GRADE) * (100 + player.GetStatus(player.DEF_BONUS)) // 100
			fDamMulti = nonplayer.GetMonsterDamageMultiply(race)
			iDamMin = int(max(0, iDamMin - iDef) * fDamMulti)
			iDamMax = int(max(0, iDamMax - iDef) * fDamMulti)
			if iDamMin < 1:
				iDamMin = 1
			if iDamMax < 5:
				iDamMax = 5
			self.AppendTextLine(localeInfo.TARGET_INFO_DAMAGE % (str(iDamMin), str(iDamMax)))

			idx = min(len(self.EXP_BASE_LVDELTA) - 1, max(0, (monsterLevel + 15) - player.GetStatus(player.LEVEL)))
			iExp = nonplayer.GetMonsterExp(race) * self.EXP_BASE_LVDELTA[idx] // 100
			self.AppendTextLine(localeInfo.TARGET_INFO_EXP % localeInfo.DottedNumber(iExp))

			self.AppendTextLine(localeInfo.TARGET_INFO_GOLD_MIN_MAX % (localeInfo.DottedNumber(nonplayer.GetMonsterGoldMin(race)), localeInfo.DottedNumber(nonplayer.GetMonsterGoldMax(race))))
			self.AppendTextLine(localeInfo.TARGET_INFO_REGEN_INFO % (nonplayer.GetMonsterRegenPercent(race), nonplayer.GetMonsterRegenCycle(race)))

		def __LoadInformation_Race(self, race):
			self.AppendSeperator()
			
			dwRaceFlag = nonplayer.GetMonsterRaceFlag(race)
			main_race_list = []
			element_list = []

			for flag, name in self.race_flags_primary:
				if dwRaceFlag & flag:
					main_race_list.append(name)
			
			if nonplayer.IsMonsterStone(race):
				main_race_list.append(localeInfo.TARGET_INFO_RACE_METIN)

			for flag, name in self.race_flags_elements:
				if dwRaceFlag & flag:
					element_list.append(name)

			main_race_str = ", ".join(main_race_list) if main_race_list else localeInfo.TARGET_INFO_NO_RACE
			element_str = ", ".join(element_list) if element_list else localeInfo.TARGET_INFO_NO_RACE

			self.AppendTextLine(localeInfo.TARGET_INFO_MAINRACE % main_race_str)
			self.AppendTextLine(localeInfo.TARGET_INFO_SUBRACE % element_str)

		def __LoadInformation_Drops(self, race):
			self.AppendSeperator()

			if race in MONSTER_INFO_DATA:
				if len(MONSTER_INFO_DATA[race]["items"]) == 0:
					self.AppendTextLine(localeInfo.TARGET_INFO_NO_ITEM_TEXT)
				else:
					itemListBox = ui.ListBoxExNew(32 + 5, self.MAX_ITEM_COUNT)
					itemListBox.SetSize(self.GetWidth() - 15 * 2 - ui.ScrollBar.SCROLLBAR_WIDTH, (32 + 5) * self.MAX_ITEM_COUNT)
					height = 0
					for curItem in MONSTER_INFO_DATA[race]["items"]:
						if "vnum_list" in curItem:
							height += self.AppendItem(itemListBox, curItem["vnum_list"], curItem["count"])
						else:
							height += self.AppendItem(itemListBox, curItem["vnum"], curItem["count"])
					if height < itemListBox.GetHeight():
						itemListBox.SetSize(itemListBox.GetWidth(), height)
					self.AppendWindow(itemListBox, 15)
					itemListBox.SetBasePos(0)

					if height > itemListBox.GetHeight():
						itemScrollBar = ui.ScrollBar()
						itemScrollBar.SetParent(self)
						itemScrollBar.SetPosition(itemListBox.GetRight(), itemListBox.GetTop())
						itemScrollBar.SetScrollBarSize(itemListBox.GetHeight())
						itemScrollBar.SetMiddleBarSize(float(itemListBox.GetHeight()) / float(height))
						itemScrollBar.Show()
						itemListBox.SetScrollBar(itemScrollBar)
			else:
				self.AppendTextLine(localeInfo.TARGET_INFO_NO_ITEM_TEXT)

		def AppendTextLine(self, text):
			textLine = ui.TextLine()
			textLine.SetParent(self)
			textLine.SetWindowHorizontalAlignCenter()
			textLine.SetHorizontalAlignCenter()
			textLine.SetText(text)
			textLine.SetPosition(0, self.yPos)
			textLine.Show()

			self.children.append(textLine)
			self.yPos += 17

		def AppendSeperator(self):
			img = ui.ImageBox()
			img.LoadImage("d:/ymir work/ui/seperator.tga")
			self.AppendWindow(img)
			img.SetPosition(img.GetLeft(), img.GetTop() - 15)
			self.yPos -= 15

		def AppendItem(self, listBox, vnums, count):
			if type(vnums) == int:
				vnum = vnums
			else:
				vnums.sort()
				vnum = vnums[0]

			item.SelectItem(vnum)
			itemName = item.GetItemName()

			if type(vnums) != int and len(vnums) > 1:
				if item.GetItemType() == item.ITEM_TYPE_METIN:
					realName = localeInfo.TARGET_INFO_STONE_NAME
					itemName = "%s +%d - +%d" % (realName, vnums[0] % 10, vnums[-1] % 10)
				else:
					pos = itemName.find("+")
					realName = itemName[:pos] if pos != -1 else itemName
					itemName = "%s +%d - +%d" % (realName.strip(), vnums[0] % 10, vnums[-1] % 10)
				
				vnum = vnums[-1]

			myItem = self.ItemListBoxItem(listBox.GetWidth())
			myItem.LoadImage(item.GetIconImageFileName())

			if count <= 1:
				myItem.SetText(itemName)
			else:
				myItem.SetText("%dx %s" % (count, itemName))

			myItem.SAFE_SetOverInEvent(self.OnShowItemTooltip, vnum)
			myItem.SAFE_SetOverOutEvent(self.OnHideItemTooltip)
			listBox.AppendItem(myItem)

			if item.GetItemType() == item.ITEM_TYPE_METIN:
				self.stoneImg = myItem
				self.stoneVnum = vnums
				self.lastStoneVnum = self.STONE_LAST_VNUM + vnums[-1] % 1000 // 100 * 100

			return myItem.GetHeight()

		def OnShowItemTooltip(self, vnum):
			item.SelectItem(vnum)
			if item.GetItemType() == item.ITEM_TYPE_METIN:
				self.itemTooltip.isStone = True
				self.itemTooltip.SetItemToolTip(self.lastStoneVnum)
			else:
				self.itemTooltip.isStone = False
				self.itemTooltip.SetItemToolTip(vnum)

		def OnHideItemTooltip(self):
			self.itemTooltip.HideToolTip()

		def AppendWindow(self, wnd, x = 0, width = 0, height = 0):
			if width == 0:
				width = wnd.GetWidth()
			if height == 0:
				height = wnd.GetHeight()

			wnd.SetParent(self)
			if x == 0:
				wnd.SetPosition((self.GetWidth() - width) // 2, self.yPos)
			else:
				wnd.SetPosition(x, self.yPos)
			wnd.Show()

			self.children.append(wnd)
			self.yPos += height + 5

		def OnUpdate(self):
			if self.stoneImg != None and self.stoneVnum != None and app.GetTime() >= self.nextStoneIconChange:
				nextImg = self.lastStoneVnum + 1
				if nextImg % 100 > self.STONE_LAST_VNUM % 100:
					nextImg -= (self.STONE_LAST_VNUM - self.STONE_START_VNUM) + 1
				self.lastStoneVnum = nextImg
				self.nextStoneIconChange = app.GetTime() + 2.5

				item.SelectItem(nextImg)
				itemName = item.GetItemName()
				realName = itemName[:itemName.find("+")]
				realName = realName + "+0 - +4"
				self.stoneImg.LoadImage(item.GetIconImageFileName(), realName)

				if self.itemTooltip.IsShow() and self.itemTooltip.isStone:
					self.itemTooltip.SetItemToolTip(nextImg)

	BUTTON_NAME_LIST = ( 
		localeInfo.TARGET_BUTTON_WHISPER, 
		localeInfo.TARGET_BUTTON_EXCHANGE, 
		localeInfo.TARGET_BUTTON_FIGHT, 
		localeInfo.TARGET_BUTTON_ACCEPT_FIGHT, 
		localeInfo.TARGET_BUTTON_AVENGE, 
		localeInfo.TARGET_BUTTON_FRIEND, 
		localeInfo.TARGET_BUTTON_INVITE_PARTY, 
		localeInfo.TARGET_BUTTON_LEAVE_PARTY, 
		localeInfo.TARGET_BUTTON_EXCLUDE, 
		localeInfo.TARGET_BUTTON_INVITE_GUILD,
		localeInfo.TARGET_BUTTON_DISMOUNT,
		localeInfo.TARGET_BUTTON_EXIT_OBSERVER,
		localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT,
		localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY,
		localeInfo.TARGET_BUTTON_BUILDING_DESTROY,
		localeInfo.TARGET_BUTTON_EMOTION_ALLOW,
		"VOTE_BLOCK_CHAT",
	)

	GRADE_NAME =	{
						nonplayer.PAWN : localeInfo.TARGET_LEVEL_PAWN,
						nonplayer.S_PAWN : localeInfo.TARGET_LEVEL_S_PAWN,
						nonplayer.KNIGHT : localeInfo.TARGET_LEVEL_KNIGHT,
						nonplayer.S_KNIGHT : localeInfo.TARGET_LEVEL_S_KNIGHT,
						nonplayer.BOSS : localeInfo.TARGET_LEVEL_BOSS,
						nonplayer.KING : localeInfo.TARGET_LEVEL_KING,
					}
	EXCHANGE_LIMIT_RANGE = 3000

	def __init__(self):
		ui.ThinBoard.__init__(self)

		name = ui.TextLine()
		name.SetParent(self)
		name.SetDefaultFontName()
		name.SetOutline()
		name.Show()

		hpGauge = ui.Gauge()
		hpGauge.SetParent(self)
		hpGauge.MakeGauge(130, "red")
		hpGauge.Hide()

		closeButton = ui.Button()
		closeButton.SetParent(self)
		closeButton.SetUpVisual("d:/ymir work/ui/public/close_button_01.sub")
		closeButton.SetOverVisual("d:/ymir work/ui/public/close_button_02.sub")
		closeButton.SetDownVisual("d:/ymir work/ui/public/close_button_03.sub")
		closeButton.SetPosition(30, 13)

		if localeInfo.IsARABIC():
			hpGauge.SetPosition(55, 17)
			hpGauge.SetWindowHorizontalAlignLeft()
			closeButton.SetWindowHorizontalAlignLeft()
		else:
			hpGauge.SetPosition(175, 17)
			hpGauge.SetWindowHorizontalAlignRight()
			closeButton.SetWindowHorizontalAlignRight()

			infoButton = ui.Button()
			infoButton.SetParent(self)
			infoButton.SetUpVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
			infoButton.SetOverVisual("d:/ymir work/ui/pattern/q_mark_02.tga")
			infoButton.SetDownVisual("d:/ymir work/ui/pattern/q_mark_01.tga")
			infoButton.SetEvent(ui.__mem_func__(self.OnPressedInfoButton))
			infoButton.Hide()

			infoBoard = self.InfoBoard()
			infoBoard.Hide()
			infoButton.showWnd = infoBoard

		closeButton.SetEvent(ui.__mem_func__(self.OnPressedCloseButton))
		closeButton.Show()

		self.buttonDict = {}
		self.showingButtonList = []
		for buttonName in self.BUTTON_NAME_LIST:
			button = ui.Button()
			button.SetParent(self)
		
			if localeInfo.IsARABIC():
				button.SetUpVisual("d:/ymir work/ui/public/Small_Button_01.sub")
				button.SetOverVisual("d:/ymir work/ui/public/Small_Button_02.sub")
				button.SetDownVisual("d:/ymir work/ui/public/Small_Button_03.sub")
			else:
				button.SetUpVisual("d:/ymir work/ui/public/small_thin_button_01.sub")
				button.SetOverVisual("d:/ymir work/ui/public/small_thin_button_02.sub")
				button.SetDownVisual("d:/ymir work/ui/public/small_thin_button_03.sub")
			
			button.SetWindowHorizontalAlignCenter()
			button.SetText(buttonName)
			button.Hide()
			self.buttonDict[buttonName] = button
			self.showingButtonList.append(button)

		self.buttonDict[localeInfo.TARGET_BUTTON_WHISPER].SetEvent(ui.__mem_func__(self.OnWhisper))
		self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE].SetEvent(ui.__mem_func__(self.OnExchange))
		self.buttonDict[localeInfo.TARGET_BUTTON_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_ACCEPT_FIGHT].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_AVENGE].SetEvent(ui.__mem_func__(self.OnPVP))
		self.buttonDict[localeInfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeInfo.TARGET_BUTTON_FRIEND].SetEvent(ui.__mem_func__(self.OnAppendToMessenger))
		self.buttonDict[localeInfo.TARGET_BUTTON_INVITE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyInvite))
		self.buttonDict[localeInfo.TARGET_BUTTON_LEAVE_PARTY].SetEvent(ui.__mem_func__(self.OnPartyExit))
		self.buttonDict[localeInfo.TARGET_BUTTON_EXCLUDE].SetEvent(ui.__mem_func__(self.OnPartyRemove))

		self.buttonDict[localeInfo.TARGET_BUTTON_INVITE_GUILD].SAFE_SetEvent(self.__OnGuildAddMember)
		self.buttonDict[localeInfo.TARGET_BUTTON_DISMOUNT].SAFE_SetEvent(self.__OnDismount)
		self.buttonDict[localeInfo.TARGET_BUTTON_EXIT_OBSERVER].SAFE_SetEvent(self.__OnExitObserver)
		self.buttonDict[localeInfo.TARGET_BUTTON_VIEW_EQUIPMENT].SAFE_SetEvent(self.__OnViewEquipment)
		self.buttonDict[localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY].SAFE_SetEvent(self.__OnRequestParty)
		self.buttonDict[localeInfo.TARGET_BUTTON_BUILDING_DESTROY].SAFE_SetEvent(self.__OnDestroyBuilding)
		self.buttonDict[localeInfo.TARGET_BUTTON_EMOTION_ALLOW].SAFE_SetEvent(self.__OnEmotionAllow)
		
		self.buttonDict["VOTE_BLOCK_CHAT"].SetEvent(ui.__mem_func__(self.__OnVoteBlockChat))

		self.name = name
		self.hpGauge = hpGauge
		self.infoButton = infoButton
		self.closeButton = closeButton
		self.nameString = 0
		self.nameLength = 0
		self.vnum = 0
		self.vid = 0
		self.eventWhisper = None
		self.isShowButton = False

		self.__Initialize()
		self.ResetTargetBoard()

	def __del__(self):
		ui.ThinBoard.__del__(self)

		print("===================================================== DESTROYED TARGET BOARD")

	def __Initialize(self):
		self.nameString = ""
		self.nameLength = 0
		self.vnum = 0
		self.vid = 0
		self.isShowButton = False

	def Destroy(self):
		self.eventWhisper = None
		self.infoButton = None
		self.closeButton = None
		self.showingButtonList = None
		self.buttonDict = None
		self.name = None
		self.hpGauge = None
		self.__Initialize()

	def RefreshMonsterInfoBoard(self):
		if not self.infoButton.showWnd.IsShow():
			return

		self.infoButton.showWnd.Refresh()

	def OnPressedInfoButton(self):
		vid = player.GetTargetVID()
		if vid:
			race = nonplayer.GetRaceNumByVID(vid)
			if race in MONSTER_INFO_DATA:
				del MONSTER_INFO_DATA[race]
		net.SendTargetInfoLoad(player.GetTargetVID())
		if self.infoButton.showWnd.IsShow():
			self.infoButton.showWnd.Close()
		elif self.vnum != 0:
			self.infoButton.showWnd.Open(self, self.vnum)

	def OnPressedCloseButton(self):
		player.ClearTarget()
		self.Close()

	def Close(self):
		self.__Initialize()
		self.infoButton.showWnd.Close()
		self.Hide()

	def Open(self, vid, name):
		if vid:
			if not constInfo.GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
				if not player.IsSameEmpire(vid):
					self.Hide()
					return

			if vid != self.GetTargetVID():
				self.ResetTargetBoard()
				self.SetTargetVID(vid)
				self.SetTargetName(name)

			if player.IsMainCharacterIndex(vid):
				self.__ShowMainCharacterMenu()		
			elif chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
				self.Hide()
			else:
				self.RefreshButton()
				self.Show()
		else:
			self.HideAllButton()
			self.__ShowButton(localeInfo.TARGET_BUTTON_WHISPER)
			self.__ShowButton("VOTE_BLOCK_CHAT")
			self.__ArrangeButtonPosition()
			self.SetTargetName(name)
			self.Show()
			
	def Refresh(self):
		if self.IsShow():
			if self.IsShowButton():			
				self.RefreshButton()		

	def RefreshByVID(self, vid):
		if vid == self.GetTargetVID():			
			self.Refresh()
			
	def RefreshByName(self, name):
		if name == self.GetTargetName():
			self.Refresh()

	def __ShowMainCharacterMenu(self):
		canShow=0

		self.HideAllButton()

		if player.IsMountingHorse():
			self.__ShowButton(localeInfo.TARGET_BUTTON_DISMOUNT)
			canShow=1

		if player.IsObserverMode():
			self.__ShowButton(localeInfo.TARGET_BUTTON_EXIT_OBSERVER)
			canShow=1

		if canShow:
			self.__ArrangeButtonPosition()
			self.Show()
		else:
			self.Hide()
			
	def __ShowNameOnlyMenu(self):
		self.HideAllButton()

	def SetWhisperEvent(self, event):
		self.eventWhisper = event

	def UpdatePosition(self):
		self.SetPosition(wndMgr.GetScreenWidth()//2 - self.GetWidth()//2, 10)

	def ResetTargetBoard(self):

		for btn in self.buttonDict.values():
			btn.Hide()

		self.__Initialize()

		self.name.SetPosition(0, 13)
		self.name.SetHorizontalAlignCenter()
		self.name.SetWindowHorizontalAlignCenter()
		self.hpGauge.Hide()
		self.infoButton.Hide()
		self.infoButton.showWnd.Close()
		self.SetSize(250, 40)

	def SetTargetVID(self, vid):
		self.vid = vid
		self.vnum = 0

	def SetEnemyVID(self, vid):
		self.SetTargetVID(vid)

		name = chr.GetNameByVID(vid)
		vnum = nonplayer.GetRaceNumByVID(vid)
		level = nonplayer.GetLevelByVID(vid)
		grade = nonplayer.GetGradeByVID(vid)

		nameFront = ""
		if -1 != level:
			nameFront += "Lv." + str(level) + " "
		if grade in self.GRADE_NAME:
			nameFront += "(" + self.GRADE_NAME[grade] + ") "

		self.SetTargetName(nameFront + name)

		(textWidth, textHeight) = self.name.GetTextSize()

		self.infoButton.SetPosition(textWidth + 25, 12)
		self.infoButton.SetWindowHorizontalAlignLeft()

		self.vnum = vnum
		self.infoButton.Show()

	def GetTargetVID(self):
		return self.vid

	def GetTargetName(self):
		return self.nameString

	def SetTargetName(self, name):
		self.nameString = name
		self.nameLength = len(name)
		self.name.SetText(name)

	def SetHP(self, hpPercentage):
		if not self.hpGauge.IsShow():

			self.SetSize(200 + 7*self.nameLength, self.GetHeight())

			if localeInfo.IsARABIC():
				self.name.SetPosition( self.GetWidth()-23, 13)
			else:
				self.name.SetPosition(23, 13)

			self.name.SetWindowHorizontalAlignLeft()
			self.name.SetHorizontalAlignLeft()
			self.hpGauge.Show()
			self.UpdatePosition()

		self.hpGauge.SetPercentage(hpPercentage, 100)

	def ShowDefaultButton(self):

		self.isShowButton = True
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_WHISPER])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_FIGHT])
		self.showingButtonList.append(self.buttonDict[localeInfo.TARGET_BUTTON_EMOTION_ALLOW])
		for button in self.showingButtonList:
			button.Show()

	def HideAllButton(self):
		self.isShowButton = False
		for button in self.showingButtonList:
			button.Hide()
		self.showingButtonList = []

	def __ShowButton(self, name):

		if name not in self.buttonDict:
			return

		self.buttonDict[name].Show()
		self.showingButtonList.append(self.buttonDict[name])

	def __HideButton(self, name):

		if name not in self.buttonDict:
			return

		button = self.buttonDict[name]
		button.Hide()

		for btnInList in self.showingButtonList:
			if btnInList == button:
				self.showingButtonList.remove(button)
				break

	def OnWhisper(self):
		if None != self.eventWhisper:
			self.eventWhisper(self.nameString)

	def OnExchange(self):
		net.SendExchangeStartPacket(self.vid)

	def OnPVP(self):
		net.SendChatPacket("/pvp %d" % (self.vid))

	def OnAppendToMessenger(self):
		net.SendMessengerAddByVIDPacket(self.vid)

	def OnPartyInvite(self):
		net.SendPartyInvitePacket(self.vid)

	def OnPartyExit(self):
		net.SendPartyExitPacket()

	def OnPartyRemove(self):
		net.SendPartyRemovePacket(self.vid)

	def __OnGuildAddMember(self):
		net.SendGuildAddMemberPacket(self.vid)

	def __OnDismount(self):
		net.SendChatPacket("/unmount")

	def __OnExitObserver(self):
		net.SendChatPacket("/observer_exit")

	def __OnViewEquipment(self):
		net.SendChatPacket("/view_equip " + str(self.vid))

	def __OnRequestParty(self):
		net.SendChatPacket("/party_request " + str(self.vid))

	def __OnDestroyBuilding(self):
		net.SendChatPacket("/build d %d" % (self.vid))

	def __OnEmotionAllow(self):
		net.SendChatPacket("/emotion_allow %d" % (self.vid))
		
	def __OnVoteBlockChat(self):
		cmd = "/vote_block_chat %s" % (self.nameString)
		net.SendChatPacket(cmd)

	def OnPressEscapeKey(self):
		self.OnPressedCloseButton()
		return True

	def IsShowButton(self):
		return self.isShowButton

	def RefreshButton(self):

		self.HideAllButton()

		if chr.INSTANCE_TYPE_BUILDING == chr.GetInstanceType(self.vid):
			#self.__ShowButton(localeInfo.TARGET_BUTTON_BUILDING_DESTROY)
			#self.__ArrangeButtonPosition()
			return
		
		if player.IsPVPInstance(self.vid) or player.IsObserverMode():
			# PVP_INFO_SIZE_BUG_FIX
			self.SetSize(200 + 7*self.nameLength, 40)
			self.UpdatePosition()
			# END_OF_PVP_INFO_SIZE_BUG_FIX			
			return	

		self.ShowDefaultButton()

		if guild.MainPlayerHasAuthority(guild.AUTH_ADD_MEMBER):
			if not guild.IsMemberByName(self.nameString):
				if 0 == chr.GetGuildID(self.vid):
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_GUILD)

		if not messenger.IsFriendByName(self.nameString):
			self.__ShowButton(localeInfo.TARGET_BUTTON_FRIEND)

		if player.IsPartyMember(self.vid):

			self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

			if player.IsPartyLeader(self.vid):
				self.__ShowButton(localeInfo.TARGET_BUTTON_LEAVE_PARTY)
			elif player.IsPartyLeader(player.GetMainCharacterIndex()):
				self.__ShowButton(localeInfo.TARGET_BUTTON_EXCLUDE)

		else:
			if player.IsPartyMember(player.GetMainCharacterIndex()):
				if player.IsPartyLeader(player.GetMainCharacterIndex()):
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)
			else:
				if chr.IsPartyMember(self.vid):
					self.__ShowButton(localeInfo.TARGET_BUTTON_REQUEST_ENTER_PARTY)
				else:
					self.__ShowButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)

			if player.IsRevengeInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeInfo.TARGET_BUTTON_AVENGE)
			elif player.IsChallengeInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)
				self.__ShowButton(localeInfo.TARGET_BUTTON_ACCEPT_FIGHT)
			elif player.IsCantFightInstance(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

			if not player.IsSameEmpire(self.vid):
				self.__HideButton(localeInfo.TARGET_BUTTON_INVITE_PARTY)
				self.__HideButton(localeInfo.TARGET_BUTTON_FRIEND)
				self.__HideButton(localeInfo.TARGET_BUTTON_FIGHT)

		distance = player.GetCharacterDistance(self.vid)
		if distance > self.EXCHANGE_LIMIT_RANGE:
			self.__HideButton(localeInfo.TARGET_BUTTON_EXCHANGE)
			self.__ArrangeButtonPosition()

		self.__ArrangeButtonPosition()

	def __ArrangeButtonPosition(self):
		showingButtonCount = len(self.showingButtonList)

		pos = -(showingButtonCount // 2) * 68
		if 0 == showingButtonCount % 2:
			pos += 34

		for button in self.showingButtonList:
			button.SetPosition(pos, 33)
			pos += 68

		self.SetSize(max(150, showingButtonCount * 75), 65)
		self.UpdatePosition()

	def OnUpdate(self):
		if self.isShowButton:

			exchangeButton = self.buttonDict[localeInfo.TARGET_BUTTON_EXCHANGE]
			distance = player.GetCharacterDistance(self.vid)

			if distance < 0:
				return

			if exchangeButton.IsShow():
				if distance > self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()

			else:
				if distance < self.EXCHANGE_LIMIT_RANGE:
					self.RefreshButton()
