#pragma once

#include <EterBase/FileDir.h>
#include "Manager.h"

class CSoundDataLoader : public CDir, public CManager
{
	public:
		CSoundDataLoader();
		virtual ~CSoundDataLoader();

		void SetTreeControler(CTreeCtrl * pTreeControler);
		void SetParentItem(HTREEITEM hParentItem);

		bool OnFolder(const char* c_szFilter, const char* c_szPathName, const char* c_szFileName);
		bool OnFile(const char* c_szPathName, const char* c_szFileName);

	protected:
		CTreeCtrl * m_pTreeControler;
		HTREEITEM m_hParentItem;
};
