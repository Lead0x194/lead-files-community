import app
import net
import player
import item
import ui
import uiToolTip
import mouseModule
import localeInfo
import uiCommon
import constInfo

class RefineDialog(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.__Initialize()
		self.isLoaded = False

	def __Initialize(self):
		self.dlgQuestion = None
		self.children = []
		self.materialData = []
		self.vnum = 0
		self.targetItemPos = 0
		self.dialogHeight = 0
		self.cost = 0
		self.percentage = 0
		self.type = 0

	def __LoadScript(self):

		try:
			pyScrLoader = ui.PythonScriptLoader()
			pyScrLoader.LoadScriptFile(self, "uiscript/refinedialog.py")

		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.LoadObject")

		try:
			self.board = self.GetChild("Board")
			self.titleBar = self.GetChild("TitleBar")
			self.probText = self.GetChild("SuccessPercentage")
			self.costText = self.GetChild("Cost")
			self.successPercentage = self.GetChild("SuccessPercentage")
			self.GetChild("AcceptButton").SetEvent(self.OpenQuestionDialog)
			self.GetChild("CancelButton").SetEvent(self.CancelRefine)
		except:
			import exception
			exception.Abort("RefineDialog.__LoadScript.BindObject")

		self.successPercentage.Show()

		toolTip = uiToolTip.ItemToolTip()
		toolTip.SetParent(self)
		toolTip.SetFollow(False)
		toolTip.SetPosition(15, 38)
		toolTip.Show()
		self.toolTip = toolTip

		self.slotList = []
		for i in range(3):
			slot = self.__MakeSlot()
			slot.SetParent(toolTip)
			slot.SetWindowVerticalAlignCenter()
			self.slotList.append(slot)

		itemImage = self.__MakeItemImage()
		itemImage.SetParent(toolTip)
		itemImage.SetWindowVerticalAlignCenter()
		itemImage.SetPosition(-35, 0)
		self.itemImage = itemImage

		self.titleBar.SetCloseEvent(ui.__mem_func__(self.CancelRefine))
		self.isLoaded = True

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def __MakeSlot(self):
		slot = ui.ImageBox()
		slot.LoadImage("d:/ymir work/ui/public/slot_base.sub")
		slot.Show()
		self.children.append(slot)
		return slot

	def __MakeItemImage(self):
		itemImage = ui.ImageBox()
		itemImage.Show()
		self.children.append(itemImage)
		return itemImage

	def __MakeThinBoard(self):
		thinBoard = ui.ThinBoard()
		thinBoard.SetParent(self)
		thinBoard.Show()
		self.children.append(thinBoard)
		return thinBoard

	def Destroy(self):
		self.ClearDictionary()
		self.dlgQuestion = None
		self.board = 0
		self.probText = 0
		self.costText = 0
		self.titleBar = 0
		self.toolTip = 0
		self.successPercentage = None
		self.slotList = []
		self.children = []
		self.materialData = []

	def Open(self, targetItemPos, nextGradeItemVnum, cost, prob, type):

		if False == self.isLoaded:
			self.__LoadScript()

		self.__Initialize()

		self.targetItemPos = targetItemPos
		self.vnum = nextGradeItemVnum
		self.cost = cost
		self.percentage = prob
		self.type = type

		self.materialData = []

		self.probText.SetText(localeInfo.REFINE_SUCCESS_PROBALITY % (self.percentage))
		self.costText.SetText(localeInfo.REFINE_COST % (localeInfo.NumberToMoneyString(self.cost)))

		self.toolTip.ClearToolTip()
		metinSlot = []
		for i in range(player.METIN_SOCKET_MAX_NUM):
			metinSlot.append(player.GetItemMetinSocket(targetItemPos, i))

		attrSlot = []
		for i in range(player.ATTRIBUTE_SLOT_MAX_NUM):
			attrSlot.append(player.GetItemAttribute(targetItemPos, i))
		self.toolTip.AddRefineItemData(nextGradeItemVnum, metinSlot, attrSlot)

		item.SelectItem(nextGradeItemVnum)
		self.itemImage.LoadImage(item.GetIconImageFileName())
		xSlotCount, ySlotCount = item.GetItemSize()
		for slot in self.slotList:
			slot.Hide()
		for i in range(min(3, ySlotCount)):
			self.slotList[i].SetPosition(-35, i*32 - (ySlotCount-1)*16)
			self.slotList[i].Show()

		self.dialogHeight = self.toolTip.GetHeight() + 46
		self.UpdateDialog()

		self.SetTop()
		self.Show()

	def Close(self):
		self.dlgQuestion = None
		self.Hide()

	def AppendMaterial(self, vnum, count):
		slot = self.__MakeSlot()
		slot.SetParent(self)
		slot.SetPosition(15, self.dialogHeight)

		itemImage = self.__MakeItemImage()
		itemImage.SetParent(slot)
		item.SelectItem(vnum)
		itemImage.LoadImage(item.GetIconImageFileName())

		thinBoard = self.__MakeThinBoard()
		thinBoard.SetPosition(50, self.dialogHeight)
		thinBoard.SetSize(191, 20)

		textLine = ui.TextLine()
		textLine.SetParent(thinBoard)
		textLine.SetFontName(localeInfo.UI_DEF_FONT)
		textLine.SetText("")
		textLine.SetOutline()
		textLine.SetFeather(False)
		textLine.SetWindowVerticalAlignCenter()
		textLine.SetVerticalAlignCenter()

		if localeInfo.IsARABIC():
			(x,y) = textLine.GetTextSize()
			textLine.SetPosition(x, 0)
		else:
			textLine.SetPosition(15, 0)

		textLine.Show()
		self.children.append(textLine)

		self.materialData.append([vnum, count, textLine])

		self.dialogHeight += 34
		self.UpdateDialog()

	def OnUpdate(self):
		if not self.IsShow():
			return
			
		for data in self.materialData:
			vnum = data[0]
			needed_count = data[1]
			text_line = data[2]
			
			current_count = player.GetItemCountByVnum(vnum)
			
			item.SelectItem(vnum)
			itemName = item.GetItemName()
			
			newText = "%d/%d x %s" % (current_count, needed_count, itemName)
			
			if text_line.GetText() != newText:
				text_line.SetText(newText)
				
				if current_count >= needed_count:
					text_line.SetPackedFontColor(0xff82ce4d)
				else:
					text_line.SetPackedFontColor(0xffff6f6f)

	def UpdateDialog(self):
		newWidth = self.toolTip.GetWidth() + 60
		newHeight = self.dialogHeight + 75

		newHeight -= 8

		if localeInfo.IsARABIC():
			self.board.SetPosition( newWidth, 0 )

			(x, y) = self.titleBar.GetLocalPosition()
			self.titleBar.SetPosition( newWidth - 15, y )

		self.board.SetSize(newWidth, newHeight)
		self.toolTip.SetPosition(15 + 35, 38)
		self.titleBar.SetWidth(newWidth-15)
		self.SetSize(newWidth, newHeight)

		(x, y) = self.GetLocalPosition()
		self.SetPosition(x, y)

	def OpenQuestionDialog(self):

		if 100 == self.percentage:
			self.Accept()
			return

		if 5 == self.type:
			self.Accept()
			return

		dlgQuestion = uiCommon.QuestionDialog2()
		dlgQuestion.SetText2(localeInfo.REFINE_WARNING2)
		dlgQuestion.SetAcceptEvent(ui.__mem_func__(self.Accept))
		dlgQuestion.SetCancelEvent(ui.__mem_func__(dlgQuestion.Close))

		if 3 == self.type:
			dlgQuestion.SetText1(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_1)
			dlgQuestion.SetText2(localeInfo.REFINE_DESTROY_WARNING_WITH_BONUS_PERCENT_2)
		elif 2 == self.type:
			dlgQuestion.SetText1(localeInfo.REFINE_DOWN_GRADE_WARNING)
		else:
			dlgQuestion.SetText1(localeInfo.REFINE_DESTROY_WARNING)

		dlgQuestion.Open()
		self.dlgQuestion = dlgQuestion

	def Accept(self):
		net.SendRefinePacket(self.targetItemPos, self.type)
		self.Close()

	def CancelRefine(self):
		net.SendRefinePacket(255, 255)
		self.Close()

	def OnPressEscapeKey(self):
		self.CancelRefine()
		return True
