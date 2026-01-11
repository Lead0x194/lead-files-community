import uiScriptLocale

window = {
	"name" : "GuildWindow_MemberPage",

	"x" : 8,
	"y" : 30,

	"width" : 360,
	"height" : 298,

	"children" :
	(
		## ScrollBar
		{
			"name" : "ScrollBar",
			"type" : "scrollbar",

			"x" : 341,
			"y" : 20,
			"scrollbar_type" : "normal",
			"size" : 270,
		},

		## Grade
		{
			"name" : "IndexName", "type" : "text", "x" : 53, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_NAME,
		},
		{
			"name" : "IndexGrade", "type" : "text", "x" : 129, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_RANK,
		},
		{
			"name" : "IndexJob", "type" : "text", "x" : 182, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_JOB,
		},
		{
			"name" : "IndexLevel", "type" : "text", "x" : 227, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_LEVEL,
		},
		{
			"name" : "IndexOffer", "type" : "text", "x" : 261, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_SPECIFIC_GRAVITY,
		},
		{
			"name" : "IndexGeneral", "type" : "text", "x" : 314, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_KNIGHT,
		},

	),
}
