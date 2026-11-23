#include "stdafx.h"
#include "constants.h"
#include "banword.h"
#include "config.h"

extern void SendLog(const char * c_pszBuf);		// Notice only to the operator

CBanwordManager::CBanwordManager()
{
}

CBanwordManager::~CBanwordManager()
{
}

bool CBanwordManager::Initialize(TBanwordTable * p, WORD wSize)
{
	m_hashmap_words.clear();

	for (WORD i = 0; i < wSize; ++i, ++p)
		m_hashmap_words[p->szWord] = true;

	char szBuf[256];
	snprintf(szBuf, sizeof(szBuf), "Banword reloaded! (total %zu banwords)", m_hashmap_words.size());
	SendLog(szBuf);
	return true;
}

bool CBanwordManager::Find(const char * c_pszString)
{
	return m_hashmap_words.end() != m_hashmap_words.find(c_pszString);
}

bool CBanwordManager::CheckString(const char * c_pszString, size_t _len)
{
	if (m_hashmap_words.empty())
		return false;

	typeof(m_hashmap_words.begin()) it = m_hashmap_words.begin();

	while (it != m_hashmap_words.end())
	{
		const std::string & r = it->first;
		const char * tmp = c_pszString;
		ssize_t len = _len;

		while (len > 0)
		{
			if (!strncmp(tmp, r.c_str(), r.size()))
				return true;

			const int step = utf8_char_length(tmp);
			tmp += step;
			len -= step;
		}

		it++;
	}

	return false;
}

void CBanwordManager::ConvertString(char * c_pszString, size_t _len)
{
	typeof(m_hashmap_words.begin()) it = m_hashmap_words.begin();

	while (it != m_hashmap_words.end())
	{
		const std::string & r = it->first;

		char * tmp = c_pszString;
		ssize_t len = _len;

		while (len > 0)
		{
			if (*tmp == '*')
			{
				++tmp;
				--len;
				continue;
			}

			if (!strncmp(tmp, r.c_str(), r.size()))
			{
				memset(tmp, '*', r.size());
				tmp += r.size();
				len -= r.size();
			}
			else
			{
				const int step = utf8_char_length(tmp);
				tmp += step;
				len -= step;
			}
		}

		it++;
	}
}

