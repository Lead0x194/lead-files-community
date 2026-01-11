import uiScriptLocale

LOCALE_PATH = uiScriptLocale.LOGIN_PATH

window = {
	"name" : "LoginWindow",
	"sytle" : ("movable",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	(

		## Board
		{
			"name" : "bg1", "type" : "expanded_image", "x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1024.0, "y_scale" : float(SCREEN_HEIGHT) / 768.0,
			"image" : "locale/hongkong/ui/serverlist.sub",
		},
		{
			"name" : "bg2", "type" : "expanded_image", "x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1024.0, "y_scale" : float(SCREEN_HEIGHT) / 768.0,
			"image" : "locale/hongkong/ui/login.sub",
		},


		## ConnectBoard
		{
			"name" : "ConnectBoard",
			"type" : "thinboard",

			"x" : SCREEN_WIDTH - 275,
			"y" : SCREEN_HEIGHT - 195,
			"width" : 208,
			"height" : 30,

			"children" :
			(
				{
					"name" : "ConnectName",
					"type" : "text",

					"x" : 15,
					"y" : 0,
					"vertical_align" : "center",
					"text_vertical_align" : "center",

					"text" : uiScriptLocale.LOGIN_DEFAULT_SERVERADDR,
				},
				{
					"name" : "SelectConnectButton",
					"type" : "button",

					"x" : 150,
					"y" : 0,
					"vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",

					"text" : uiScriptLocale.LOGIN_SELECT_BUTTON,
				},
			),
		},

		## LoginBoard
		{
			"name" : "LoginBoard",
			"type" : "image",

			"x" : SCREEN_WIDTH - 275,
			"y" : SCREEN_HEIGHT - 155,

			"image" : LOCALE_PATH + "loginwindow.sub",

			"children" :
			(
				{
					"name" : "ID_EditLine",
					"type" : "editline",

					"x" : 77,
					"y" : 16,

					"width" : 120,
					"height" : 18,

					"input_limit" : 16,
					"enable_codepage" : 0,

					"r" : 1.0,
					"g" : 1.0,
					"b" : 1.0,
					"a" : 1.0,
				},
				{
					"name" : "Password_EditLine",
					"type" : "editline",

					"x" : 77,
					"y" : 43,

					"width" : 120,
					"height" : 18,

					"input_limit" : 16,
					"secret_flag" : 1,
					"enable_codepage" : 0,

					"r" : 1.0,
					"g" : 1.0,
					"b" : 1.0,
					"a" : 1.0,
				},
				{
					"name" : "LoginButton",
					"type" : "button",

					"x" : 15,
					"y" : 65,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.LOGIN_CONNECT,
				},
				{
					"name" : "LoginExitButton",
					"type" : "button",

					"x" : 105,
					"y" : 65,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.LOGIN_EXIT,
				},
			),
		},

		## ServerBoard
		{
			"name" : "ServerBoard",
			"type" : "thinboard",

			"x" : 0,
			"y" : 308 - 40,
			"width" : 375,
			"height" : 280, 
			"horizontal_align" : "center",

			"children" :
			(

				## Title
				{
					"name" : "Title",
					"type" : "text",

					"x" : 0,
					"y" : 12,
					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text" : uiScriptLocale.LOGIN_SELECT_TITLE,
				},

				## Horizontal
				{
					"name" : "HorizontalLine1",
					"type" : "line",

					"x" : 10,
					"y" : 34,
					"width" : 354,
					"height" : 0,
					"color" : 0xff777777,
				},
				{
					"name" : "HorizontalLine2",
					"type" : "line",

					"x" : 10,
					"y" : 35,
					"width" : 355,
					"height" : 0,
					"color" : 0xff111111,
				},

				## Vertical
				{
					"name" : "VerticalLine1",
					"type" : "line",

					"x" : 246,
					"y" : 38,
					"width" : 0,
					"height" : 280 - 45,
					"color" : 0xff777777,
				},
				{
					"name" : "VerticalLine2",
					"type" : "line",

					"x" : 247,
					"y" : 38,
					"width" : 0,
					"height" : 280 - 45,
					"color" : 0xff111111,
				},

				## ListBox
				{
					"name" : "ServerList",
					"type" : "listbox",

					"x" : 10,
					"y" : 40,
					"width" : 232,
					"height" : 171,

					"item_align" : 0,
				},
				{
					"name" : "ChannelList",
					"type" : "listbox",

					"x" : 255,
					"y" : 40,
					"width" : 109,
					"height" : 171,

					"item_align" : 0,
				},

				## Buttons
				{
					"name" : "ServerSelectButton",
					"type" : "button",

					"x" : 267,
					"y" : 280 - 50,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.OK,
				},
				{
					"name" : "ServerExitButton",
					"type" : "button",

					"x" : 267,
					"y" : 280 - 28,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.LOGIN_SELECT_EXIT,
				},

			),

		},

		## RUNUP_MATRIX_CARD
		{
			"name" : "RunupMatrixQuizBoard",
			"type" : "image",

			"x" : SCREEN_WIDTH	- 275,
			"y" : SCREEN_HEIGHT - 205,

			"image" : "locale/hongkong/ui/login/matrix.sub",

			"children" :
			(
				{"name" : "runup_matrix_back", "type" : "image", "x" : 75, "y" : 95, "image" : "locale/hongkong/ui/login/pwdbar.sub", },
				{"name" : "runup_account_label_shadow",	"type" : "text", "x" : 11, "y" : 11, "r" : 0/255.0, "g" : 0/255.0, "b" : 0/255.0, "text" : "Account"},
				{"name" : "runup_matrix_label_shadow",	"type" : "text", "x" : 11, "y" : 41, "r" : 0/255.0, "g" : 0/255.0, "b" : 0/255.0, "text" : "MatrixCode", },
				{"name" : "runup_password_label_shadow",	"type" : "text", "x" : 11, "y" : 76, "r" : 0/255.0, "g" : 0/255.0, "b" : 0/255.0, "text" : "MatrixPassword", },
				{"name" : "runup_account_label",	"type" : "text", "x" : 10, "y" : 10, "r" : 255/255.0, "g" : 230/255.0, "b" : 186/255.0, "text" : "Account"},
				{"name" : "runup_matrix_label",	"type" : "text", "x" : 10, "y" : 40, "r" : 255/255.0, "g" : 230/255.0, "b" : 186/255.0, "text" : "MatrixCode", },
				{"name" : "runup_password_label",	"type" : "text", "x" : 10, "y" : 75, "r" : 255/255.0, "g" : 230/255.0, "b" : 186/255.0, "text" : "MatrixPassword", },

				{"name" : "RunupMatrixID",	"type" : "text", "x" : 70, "y" : 20, "text" : "????????", },
				{"name" : "RunupMatrixCode",		"type" : "text", "x" : 70, "y" : 53, "text" : "[?,?][?,?][?,?][?,?]", },

				{
					"name" : "RunupMatrixAnswerInput", "type" : "editline", "x" : 77, "y" : 97, "width" : 120, "height" : 18,
					"input_limit" : 16, "secret_flag" : 1, "enable_codepage" : 0,
					"r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0,
				},
				{
					"name" : "RunupMatrixAnswerOK", "type" : "button",
					"x" : 15, "y" : 120,
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : "OK",
				},
				{
					"name" : "RunupMatrixAnswerCancel", "type" : "button",
					"x" : 105, "y" : 120,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : "CANCEL"
				},
			),
		}, # END_OF_TAIWAN_MATRIX_CARD

	),
}
