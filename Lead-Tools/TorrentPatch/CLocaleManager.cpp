#include "Metin2TorrentConsole.h"
#include "CLocaleManager.h"
#include "StringFromFile.h"
#include <YmirBase/TSimpleString.h>

EA_DEFINE_SINGLETON(CLocaleManager)

bool CLocaleManager::Load(MA_LPCTSTR tszFileName, Earth::EA_CStatus* pcStatus)
{
	pcStatus->AddNRf( Earth::EA_STATUS_INFO, _T("Loading language file (path='%s')"), tszFileName );

	std::string	strXMLContents;

	if( EL_StringFromFileW(strXMLContents, tszFileName, pcStatus) == false ) {
		pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("Cannot find language file") );
		return false;
	}

	TiXmlDocument CXMLDocument;

	CXMLDocument.Parse( strXMLContents.c_str() );

	if( CXMLDocument.Error() ) {
		pcStatus->AddNRf( Earth::EA_STATUS_ERROR, _T("Error while parsing XML") );
		return false;
	}

	TiXmlHandle CXMLHandle( &CXMLDocument );
	TiXmlElement* pkFirstElement = CXMLHandle.FirstChildElement().Element();
	if( NULL == pkFirstElement ) {
		pcStatus->AddNR( Earth::EA_STATUS_ERROR, _T("No first element in XML") );
		return false;
	}

	TiXmlHandle CXMLFirstHandle(pkFirstElement);
	{
		TiXmlElement* pkStringNode = CXMLFirstHandle.FirstChildElement("string").Element();
		for (; pkStringNode != NULL; pkStringNode = pkStringNode->NextSiblingElement("string"))
		{
			EL_C_ECHAR_P keyz = (EL_C_ECHAR_P)pkStringNode->Attribute("key");
			if (keyz)
			{
				// key
				Mantle::MA_CE2T keyBuf(keyz);
				EL_UINT32 keyHash = EL_SimpleString(keyBuf).CalcHash32();

				EL_C_ECHAR_P valz = (EL_C_ECHAR_P)pkStringNode->GetText();
				if (valz)
				{
					Mantle::MA_CE2T	tszValz(valz);
					std::wstring	wstrValz = (MA_LPCWSTR) tszValz;

					LocaleMap::value_type vtLocaleMap(keyHash, wstrValz);
					m_localeMap.insert(vtLocaleMap);
				}
			}
		}
	}

	return true;
}

MA_LPCWSTR CLocaleManager::Get(MA_LPCWSTR wszKeyPtr)
{
	EL_UINT32 uKeyHash = EL_SimpleString(wszKeyPtr).CalcHash32();
	LocaleMap::iterator it = m_localeMap.find(uKeyHash);

	if (it == m_localeMap.end())
		return wszKeyPtr;
	return it->second.c_str();
}

MA_LPCWSTR CLocaleManager::Get(MA_LPCASTR aszKeyPtr)
{
	Mantle::MA_CA2W	wszKey(aszKeyPtr);

	return Get((MA_LPCWSTR) wszKey);
}

bool CLocaleManager::Add(MA_LPCWSTR wszKey, MA_LPCWSTR wszValue)
{
	EL_UINT32 uKeyHash = EL_SimpleString(wszKey).CalcHash32();

	LocaleMap::iterator it = m_localeMap.find(uKeyHash);
	if (it != m_localeMap.end()) {
		it->second = wszValue;
	} else {
		LocaleMap::value_type vtLocaleMap(uKeyHash, wszValue);
		m_localeMap.insert(vtLocaleMap);
	}
	return true;
}

bool CLocaleManager::Add(MA_LPCASTR szKey, MA_LPCWSTR wszValue)
{
	Mantle::MA_CA2W	wszKey(szKey);

	return Add((MA_LPCWSTR) wszKey, wszValue);
}
