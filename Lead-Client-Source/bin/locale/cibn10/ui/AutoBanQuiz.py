import uiScriptLocale

window = {
	"name" : "QuestionDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH/2 - 125,
	"y" : SCREEN_HEIGHT/2 - 52,

	"width" : 280,
	"height" : 220,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 280,
			"height" : 220,

			"children" :
			(
				{
					"name" : "msg1",
					"type" : "text",

					"x" : 0,
					"y" : 25,

					"text" : "MSG1",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "msg2",
					"type" : "text",

					"x" : 0,
					"y" : 25*2,

					"text" : "MSG2",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "status",
					"type" : "text",

					"x" : 0,
					"y" : 25*3,

					"text" : "STATUS",

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "select1",
					"type" : "radio_button",

					"x" : 0,
					"y" : 25*3+18,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
					"text" : "SELECT1",

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name" : "select2",
					"type" : "radio_button",

					"x" : 0,
					"y" : 25*3+18+25,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
					"text" : "SELECT2",

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name" : "select3",
					"type" : "radio_button",

					"x" : 0,
					"y" : 25*3+18+25*2,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
					"text" : "SELECT3",

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
                                        "name" : "answer",
					"type" : "button",

					"x" : -40,
					"y" : 25*3+18+25*3+10,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
                                        "text" : uiScriptLocale.AUTOBAN_QUIZ_ANSWER,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name" : "refresh",
					"type" : "button",

					"x" : +40,
					"y" : 25*3+18+25*3+10,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
                                        "text" : uiScriptLocale.AUTOBAN_QUIZ_REFRESH,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
			),
		},
	),
}
