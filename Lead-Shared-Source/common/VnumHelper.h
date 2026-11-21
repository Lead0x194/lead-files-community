#ifndef __HEADER_VNUM_HELPER__
#define	__HEADER_VNUM_HELPER__

/**
	Items, mobs and so on, existing or yet to be added, are currently all identified in the source by
	hardcoded identifiers (number = VNum), which makes the code very hard to read.

	Added on Seungcheol's suggestion, so that the source alone tells which item (or mob) is meant.

		* This file is expected to change often; in the PCH every change would force a full rebuild,
		so for now it is included only by the cpp files that need it.

		* cpp implementation would need compiling and linking, so only a header was put into common. (to be used by both the game and db projects)

	@date	2011. 8. 29.
*/


class CItemVnumHelper
{
public:
	/// Phoenix summon ticket for the German DVD
	static	const bool	IsPhoenix(DWORD vnum)				{ return 53001 == vnum; }		// NOTE: the phoenix summon item is 53001, but the mob vnum is 34001.

	/// Ramadan event crescent ring (originally a special item for the Ramadan event, but it is to be reused in several ways from now on)
	static	const bool	IsRamadanMoonRing(DWORD vnum)		{ return 71135 == vnum; }

	/// Halloween candy (same spec as the crescent ring)
	static	const bool	IsHalloweenCandy(DWORD vnum)		{ return 71136 == vnum; }

	/// Christmas ring of happiness
	static	const bool	IsHappinessRing(DWORD vnum)		{ return 71143 == vnum; }

	/// Valentine pendant of love
	static	const bool	IsLovePendant(DWORD vnum)		{ return 71145 == vnum; }
};

class CMobVnumHelper
{
public:
	/// Mob number of the phoenix for the German DVD
	static	bool	IsPhoenix(DWORD vnum)				{ return 34001 == vnum; }
	static	bool	IsIcePhoenix(DWORD vnum)				{ return 34003 == vnum; }
	/// PetSystem-managed pet?
	static	bool	IsPetUsingPetSystem(DWORD vnum)	{ return (IsPhoenix(vnum) || IsReindeerYoung(vnum)) || IsIcePhoenix(vnum); }

	/// 2011 Christmas event pet (baby reindeer)
	static	bool	IsReindeerYoung(DWORD vnum)	{ return 34002 == vnum; }

	/// Black horse (20119) as Ramadan event reward .. clone of the Ramadan black horse for the Halloween event (same spec, 20219)
	static	bool	IsRamadanBlackHorse(DWORD vnum)		{ return 20119 == vnum || 20219 == vnum || 22022 == vnum; }
};

class CVnumHelper
{
};


#endif	//__HEADER_VNUM_HELPER__