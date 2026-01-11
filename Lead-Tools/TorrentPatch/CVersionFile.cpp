#include "Metin2TorrentConsole.h"
#include "CVersionFile.h"
#include <Mantle/MA_String.h>
#include <Earth/EA_CTextFile.h>

CVersionFile::CVersionFile()
{
	m_tszVersionFileName[0] = _T('\0');
}

CVersionFile::CVersionFile( MA_LPCTSTR tszVersionFileName )
{
	SetFileName( tszVersionFileName );
}

CVersionFile::~CVersionFile()
{
}

bool	CVersionFile::SetFileName( MA_LPCTSTR tszVersionFileName )
{
	MA_StringCopy( m_tszVersionFileName, MA_ARRAYCOUNT(m_tszVersionFileName), tszVersionFileName );
	return EL_TRUE;
}

bool	CVersionFile::IsMatchVersion( MA_LPCTSTR tszNewVersion )
{
	Earth::EA_CTextFile cVersionFile;

	if( cVersionFile.Open( m_tszVersionFileName ) == EA_FALSE )
		return EL_FALSE;
	MA_TCHAR tszOldVersion[256];
	MA_TCHAR tszLine[256];
	cVersionFile.ReadLine( tszLine, MA_ARRAYCOUNT(tszLine) );
	MA_StringTrim( tszOldVersion, MA_ARRAYCOUNT(tszOldVersion), tszLine );
	bool bRet = MA_StringCompareCI( tszOldVersion, tszNewVersion ) == 0;
	cVersionFile.Close();
	return bRet;
}

bool CVersionFile::UpdateVersion( MA_LPCTSTR tszNewVersion )
{
	Earth::EA_CTextFile cVersionFile;

	if( cVersionFile.Open( m_tszVersionFileName, Earth::modeCreate ) == EA_FALSE )
		return EL_FALSE;
	cVersionFile.WriteLine( tszNewVersion );
	cVersionFile.Close();
	return EL_TRUE;
}
