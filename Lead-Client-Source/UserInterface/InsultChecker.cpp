#include "StdAfx.h"
#include "InsultChecker.h"
#include "../EterLib/Util.h"

CInsultChecker& CInsultChecker::GetSingleton()
{
	static CInsultChecker s_kInsultChecker;
	return s_kInsultChecker;
}

CInsultChecker::CInsultChecker()
{
}

CInsultChecker::~CInsultChecker()
{
}

void CInsultChecker::Clear()
{
	m_kList_stInsult.clear();
}

void CInsultChecker::AppendInsult(const std::string& c_rstInsult)
{
	if (c_rstInsult.length()>0)
		m_kList_stInsult.push_back(c_rstInsult);
}

bool CInsultChecker::__GetInsultLength(const char* c_szWord, UINT* puInsultLen)
{
	std::list<std::string>::iterator i;
	for (i=m_kList_stInsult.begin(); i!=m_kList_stInsult.end(); ++i)
	{
		std::string& rstInsult=*i;

		int ret=LocaleService_StringCompareCI(c_szWord, rstInsult.c_str(), rstInsult.length());		
		if (0==ret)
		{
			*puInsultLen=static_cast<UINT>(rstInsult.length());
			return true;
		}
	}

	return false;
}

bool CInsultChecker::__IsInsult(const char* c_szWord)
{
	UINT uInsultLen;
	return __GetInsultLength(c_szWord, &uInsultLen);
}

void CInsultChecker::FilterInsult(char* szLine, UINT uLineLen)
{
	const char INSULT_FILTER_CHAR = '*'; 
	for (UINT uPos=0; uPos<uLineLen;)
	{
		UINT uInsultLen;
		if (__GetInsultLength(szLine+uPos, &uInsultLen))
		{
			memset(szLine+uPos, INSULT_FILTER_CHAR, uInsultLen);
			uPos += uInsultLen;
		}
		else
		{
			uPos += GetDefaultCodePageCharLength(szLine + uPos);
		}
	}
}

bool CInsultChecker::IsInsultIn(const char* c_szLine, UINT uLineLen)
{
	for (UINT uPos=0; uPos<uLineLen;)
	{
		if (__IsInsult(c_szLine+uPos))
			return true;

		uPos += GetDefaultCodePageCharLength(c_szLine + uPos);
	}

	return false;
}
