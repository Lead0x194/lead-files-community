import uiScriptLocale

window = {
	"name" : "InputDialog",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 370,
	"height" : 130,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : 370,
			"height" : 130,

			"title" : uiScriptLocale.GUILD_WAR_DECLARE,

			"children" :
			(
				## Input Slot
				{
					"name" : "InputName",
					"type" : "text",

					"x" : 30,
					"y" : 40,

					"text" : uiScriptLocale.GUILD_WAR_ENEMY,
				},
				{
					"name" : "InputSlot",
					"type" : "slotbar",

					"x" : 115,
					"y" : 37,
					"width" : 130,
					"height" : 18,

					"children" :
					(
						{
							"name" : "InputValue",
							"type" : "editline",

							"x" : 3,
							"y" : 3,

							"width" : 90,
							"height" : 18,

							"input_limit" : 12,
						},
					),
				},
				## Input Slot
				{
					"name" : "GameType", "x" : 10, "y" : 65, "width" : 85+87*4, "height" : 20,
					
					"children" :
					(
						{"name" : "GameTypeLabel", "type" : "text", "x" : 20, "y" : 3, "text" : uiScriptLocale.GUILD_WAR_BATTLE_TYPE,},
						{
							"name" : "NormalButton",
							"type" : "radio_button",

							"x" : 90,
							"y" : 0,

							"text" : uiScriptLocale.GUILD_WAR_NORMAL,

							"default_image" : "d:/ymir work/ui/public/Large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/Large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/Large_button_03.sub",
						},
						{
							"name" : "WarpButton",
							"type" : "radio_button",

							"x" : 90+87*1,
							"y" : 0,

							"text" : uiScriptLocale.GUILD_WAR_WARP,
							
							"default_image" : "d:/ymir work/ui/public/Large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/Large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/Large_button_03.sub",
						},
						{
							"name" : "CTFButton",
							"type" : "radio_button",

							"x" : 90+87*2,
							"y" : 0,

							"text" : uiScriptLocale.GUILD_WAR_CTF,

							"default_image" : "d:/ymir work/ui/public/Large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/Large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/Large_button_03.sub",
						},
					),
				},
				## Button
				{
					"name" : "AcceptButton",
					"type" : "button",

					"x" : - 61 - 5 + 40,
					"y" : 95,
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OK,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 5 + 28,
					"y" : 95,
					"horizontal_align" : "center",

					"text" : uiScriptLocale.CANCEL,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
			),
		},
	),
}
