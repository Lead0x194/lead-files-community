import app
import wndMgr
import systemSetting
import mouseModule
import grp
import ui
import chr
import background
import player

#wndMgr.SetOutlineFlag(TRUE)

class App:
	def __init__(self, title = "METIN2 UI TEST"):
		app.SetMouseHandler(mouseModule.mouseController)
		app.SetHairColorEnable(TRUE)
		wndMgr.SetMouseHandler(mouseModule.mouseController)

		width	= systemSetting.GetWidth()
		height	= systemSetting.GetHeight()
		wndMgr.SetScreenSize(width, height)
		app.Create(title, width, height, 1)
		mouseModule.mouseController.Create()

		self.OnInit()

	def MainLoop(self):
		app.Loop()

	def OnInit(self):
		pass


if __name__ == "__main__":
	App().MainLoop()

