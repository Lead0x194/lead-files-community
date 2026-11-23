#include "stdafx.h"
#include "start_position.h"


char g_nation_name[4][32] =
{
	"",
	"Shinsoo",
	"Chunjo",
	"Jinno",
};

//	"[LS;793]"
//	"[LS;794]"
//	"[LS;795]"

long g_start_map[4] =
{
	0,	// reserved
	1,	// sour hydrangea
	21,	// Cheonjo-guk
	41	// Jinnoguk
};

DWORD g_start_position[4][2] =
{
	{      0,      0 },	// reserved
	{ 469300, 964200 },	// sour hydrangea
	{  55700, 157900 },	// Cheonjo-guk
	{ 969600, 278400 }	// Jinnoguk
};


DWORD arena_return_position[4][2] =
{
	{       0,  0       },
	{   347600, 882700  }, // Jayanghyeon
	{   138600, 236600  }, // Bok Jeong-hyeon
	{   857200, 251800  }  // Park Ra-hyeon
};


DWORD g_create_position[4][2] = 
{
	{		0,		0 },
	{ 459800, 953900 },
	{ 52070, 166600 },
	{ 957300, 255200 },	
};

