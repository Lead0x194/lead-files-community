#ifndef _CVERSIONFILE_H_
#define _CVERSIONFILE_H_

class CVersionFile
{
public:
	CVersionFile();
	CVersionFile( MA_LPCTSTR tszVersionFileName );
	~CVersionFile();

	bool	SetFileName( MA_LPCTSTR tszVersionFileName );
	bool	IsMatchVersion( MA_LPCTSTR tszNewVersion );
	bool UpdateVersion( MA_LPCTSTR tszNewVersion );

protected:
	MA_TCHAR	m_tszVersionFileName[MA_MAX_PATH];
};

#endif /* !_CVERSIONFILE_H_ */
