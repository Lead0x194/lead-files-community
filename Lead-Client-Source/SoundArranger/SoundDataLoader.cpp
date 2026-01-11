#include "StdAfx.h"
#include "SoundDataLoader.h"

bool CSoundDataLoader::OnFolder(const char* c_szFilter, const char* c_szPathName, const char* c_szFileName)
{
	assert(m_pTreeControler);

	std::string stPathName = "";
	stPathName += c_szPathName;
	stPathName += c_szFileName;

	HTREEITEM hFolderItem = m_pTreeControler->InsertItem(c_szFileName, 0, 1, m_hParentItem, TVI_SORT);

	CSoundDataLoader SoundDataLoader;
	SoundDataLoader.SetTreeControler(m_pTreeControler);
	SoundDataLoader.SetParentItem(hFolderItem);
	SoundDataLoader.Create(c_szFilter, stPathName.c_str(), TRUE);
	return true;
}

bool CSoundDataLoader::OnFile(const char* c_szPathName, const char* c_szFileName)
{
	HTREEITEM hTreeItem = m_pTreeControler->InsertItem(c_szFileName, 2, 2, m_hParentItem, TVI_SORT);

	/////

	std::string strTemporaryFullFileName;
	std::string strFullFileName;
	strTemporaryFullFileName = c_szPathName;
	strTemporaryFullFileName += c_szFileName;
	StringPath(strTemporaryFullFileName.c_str(), strFullFileName);

	DWORD dwCRC32 = GetCaseCRC32(strFullFileName.c_str(), strFullFileName.length());
	m_pTreeControler->SetItemData(hTreeItem, dwCRC32);
	RegisterSoundFileName(dwCRC32, strFullFileName.c_str());

	return true;
}

void CSoundDataLoader::SetTreeControler(CTreeCtrl * pTreeControler)
{
	m_pTreeControler = pTreeControler;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          

void CSoundDataLoader::SetParentItem(HTREEITEM hParentItem)
{
	m_hParentItem = hParentItem;
}

CSoundDataLoader::CSoundDataLoader()
: m_pTreeControler(NULL), m_hParentItem(TVI_ROOT)
{
}
CSoundDataLoader::~CSoundDataLoader()
{
}