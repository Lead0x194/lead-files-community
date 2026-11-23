#ifndef __INC_METIN_II_GAME_SECTREE_MANAGER_H__
#define __INC_METIN_II_GAME_SECTREE_MANAGER_H__

#include "sectree.h"


typedef struct SMapRegion
{
	int			index;
	int			sx, sy, ex, ey;
	PIXEL_POSITION	posSpawn;

	bool		bEmpireSpawnDifferent;
	PIXEL_POSITION	posEmpire[3];

	std::string		strMapName;
} TMapRegion;

struct TAreaInfo
{
	int sx, sy, ex, ey, dir;
	TAreaInfo(int sx, int sy, int ex, int ey, int dir)
		: sx(sx), sy(sy), ex(ex), ey(ey), dir(dir)
		{}
};

struct npc_info
{
	BYTE bType;
	std::string name;
	long x, y;
	npc_info(BYTE bType, const char* name, long x, long y)
		: bType(bType), name(name), x(x), y(y)
		{}
};

typedef std::map<std::string, TAreaInfo> TAreaMap;

typedef struct SSetting
{
	int			iIndex;
	int			iCellScale;
	int			iBaseX;
	int			iBaseY;
	int			iWidth;
	int			iHeight;

	PIXEL_POSITION	posSpawn;
} TMapSetting;

class SECTREE_MAP
{
	public:
		typedef std::map<DWORD, LPSECTREE> MapType;

		SECTREE_MAP();
		SECTREE_MAP(SECTREE_MAP & r);
		virtual ~SECTREE_MAP();

		bool Add(DWORD key, LPSECTREE sectree) {
			return map_.insert(MapType::value_type(key, sectree)).second;
		}

		LPSECTREE	Find(DWORD dwPackage);
		LPSECTREE	Find(DWORD x, DWORD y);
		void		Build();

		TMapSetting	m_setting;

		template< typename Func >
		void for_each( Func & rfunc )
		{
			// <Factor> Using snapshot copy to avoid side-effects
			FCollectEntity collector;
			std::map<DWORD, LPSECTREE>::iterator it = map_.begin();
			for ( ; it != map_.end(); ++it)
			{
				LPSECTREE sectree = it->second;
				sectree->for_each_entity(collector);
			}
			collector.ForEach(rfunc);
			/*
			std::map<DWORD,LPSECTREE>::iterator i = map_.begin();
			for (; i != map_.end(); ++i )
			{
				LPSECTREE pSec = i->second;
				pSec->for_each_entity( rfunc );
			}
			*/
		}

		void DumpAllToSysErr() {
			SECTREE_MAP::MapType::iterator i;
			for (i = map_.begin(); i != map_.end(); ++i)
			{
				sys_err("SECTREE %x(%u, %u)", i->first, i->first & 0xffff, i->first >> 16);
			}
		}

	private:
		MapType map_;
};

enum EAttrRegionMode
{
	ATTR_REGION_MODE_SET,
	ATTR_REGION_MODE_REMOVE,
	ATTR_REGION_MODE_CHECK,
};

class SECTREE_MANAGER : public singleton<SECTREE_MANAGER>
{
	public:
		SECTREE_MANAGER();
		virtual ~SECTREE_MANAGER();

		LPSECTREE_MAP GetMap(long lMapIndex);
		LPSECTREE 	Get(DWORD dwIndex, DWORD package);
		LPSECTREE 	Get(DWORD dwIndex, DWORD x, DWORD y);

		template< typename Func >
		void for_each( int iMapIndex, Func & rfunc )
		{
			LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap( iMapIndex );
			if ( pSecMap )
			{
				pSecMap->for_each( rfunc );
			}
		}
		
		int		LoadSettingFile(long lIndex, const char * c_pszSettingFileName, TMapSetting & r_setting);
		bool		LoadMapRegion(const char * c_pszFileName, TMapSetting & r_Setting, const char * c_pszMapName);
		int		Build(const char * c_pszListFileName, const char* c_pszBasePath);
		LPSECTREE_MAP BuildSectreeFromSetting(TMapSetting & r_setting);
		bool		LoadAttribute(LPSECTREE_MAP pkMapSectree, const char * c_pszFileName, TMapSetting & r_setting);
		void		LoadDungeon(int iIndex, const char * c_pszFileName);
		bool		GetValidLocation(int32_t lMapIndex, int32_t x, int32_t y, int32_t & r_lValidMapIndex, PIXEL_POSITION & r_pos, BYTE empire = 0);
		bool		GetSpawnPosition(long x, long y, PIXEL_POSITION & r_pos);
		bool		GetSpawnPositionByMapIndex(long lMapIndex, PIXEL_POSITION & r_pos);
		bool		GetRecallPositionByEmpire(int iMapIndex, BYTE bEmpire, PIXEL_POSITION & r_pos);

		const TMapRegion *	GetMapRegion(long lMapIndex);
		int			GetMapIndex(int32_t x, int32_t y);
		const TMapRegion *	FindRegionByPartialName(const char* szMapName);

		bool		GetMapBasePosition(long x, long y, PIXEL_POSITION & r_pos);
		bool		GetMapBasePositionByMapIndex(long lMapIndex, PIXEL_POSITION & r_pos);
		bool		GetMovablePosition(long lMapIndex, long x, long y, PIXEL_POSITION & pos);
		bool		IsMovablePosition(long lMapIndex, long x, long y);
		bool		GetCenterPositionOfMap(long lMapIndex, PIXEL_POSITION & r_pos);
		bool        GetRandomLocation(long lMapIndex, PIXEL_POSITION & r_pos, DWORD dwCurrentX = 0, DWORD dwCurrentY = 0, int iMaxDistance = 0);

		long		CreatePrivateMap(long lMapIndex);	// returns new private map index, returns 0 when fail
		void		DestroyPrivateMap(long lMapIndex);

		TAreaMap&	GetDungeonArea(long lMapIndex);
		void		SendNPCPosition(LPCHARACTER ch);
		void		InsertNPCPosition(long lMapIndex, BYTE bType, const char* szName, long x, long y);

		BYTE		GetEmpireFromMapIndex(long lMapIndex);

		void		PurgeMonstersInMap(long lMapIndex);
		void		PurgeStonesInMap(long lMapIndex);
		void		PurgeNPCsInMap(long lMapIndex);
		size_t		GetMonsterCountInMap(long lMapIndex);
		size_t		GetMonsterCountInMap(long lMpaIndex, DWORD dwVnum);

		/// About the area Sectree of Attribute Perform specific processing on .
		/**
		 * @param [in]	lMapIndex apply Map index
		 * @param [in]	lStartX Leftmost coordinate of the rectangular area
		 * @param [in]	lStartY Top coordinate of rectangular area
		 * @param [in]	lEndX Rightmost coordinate of the rectangular area
		 * @param [in]	lEndY Bottom coordinate of rectangular area
		 * @param [in]	lRotate Angle to rotate about the area
		 * @param [in]	dwAttr apply Attribute
		 * @param [in]	mode Attribute to deal with type
		 */
		bool		ForAttrRegion(long lMapIndex, long lStartX, long lStartY, long lEndX, long lEndY, long lRotate, DWORD dwAttr, EAttrRegionMode mode);

		bool		SaveAttributeToImage(int lMapIndex, const char * c_pszFileName, LPSECTREE_MAP pMapSrc = NULL);

	private:

		/// For a rectangular area Sectree of Attribute Perform specific processing on .
		/**
		 * @param [in]	lMapIndex apply Map index
		 * @param [in]	lCX leftmost part of the rectangular area Cell coordinates of
		 * @param [in]	lCY top of rectangular area Cell coordinates of
		 * @param [in]	lCW square area Cell unit width
		 * @param [in]	lCH square area Cell unit height
		 * @param [in]	lRotate angle to rotate ( right angle )
		 * @param [in]	dwAttr apply Attribute
		 * @param [in]	mode Attribute to deal with type
		 */
		bool		ForAttrRegionRightAngle( long lMapIndex, long lCX, long lCY, long lCW, long lCH, long lRotate, DWORD dwAttr, EAttrRegionMode mode );

		/// For rectangular areas other than right angles Sectree of Attribute Perform specific processing on .
		/**
		 * @param [in]	lMapIndex apply Map index
		 * @param [in]	lCX leftmost part of the rectangular area Cell coordinates of
		 * @param [in]	lCY top of rectangular area Cell coordinates of
		 * @param [in]	lCW square area Cell unit width
		 * @param [in]	lCH square area Cell unit height
		 * @param [in]	lRotate angle to rotate ( angles other than right angles )
		 * @param [in]	dwAttr apply Attribute
		 * @param [in]	mode Attribute to deal with type
		 */
		bool		ForAttrRegionFreeAngle( long lMapIndex, long lCX, long lCY, long lCW, long lCH, long lRotate, DWORD dwAttr, EAttrRegionMode mode );

		/// one Cell of Attribute Perform specific processing on .
		/**
		 * @param [in]	lMapIndex apply Map index
		 * @param [in]	lCX apply Cell of X coordinate
		 * @param [in]	lCY apply Cell of Y coordinate
		 * @param [in]	dwAttr apply Attribute
		 * @param [in]	mode Attribute to deal with type
		 */
		bool		ForAttrRegionCell( long lMapIndex, long lCX, long lCY, DWORD dwAttr, EAttrRegionMode mode );

		static WORD			current_sectree_version;
		std::map<DWORD, LPSECTREE_MAP>	m_map_pkSectree;
		std::map<int, TAreaMap>	m_map_pkArea;
		std::vector<TMapRegion>		m_vec_mapRegion;
		std::map<DWORD, std::vector<npc_info> > m_mapNPCPosition;

		// <Factor> Circular private map indexing
		typedef TR1_NS::unordered_map<long, int> PrivateIndexMapType;
		PrivateIndexMapType next_private_index_map_;
};

#endif

