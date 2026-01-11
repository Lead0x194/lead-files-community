#include "Metin2TorrentConsole.h"
#include "CConfig.h"
#include <Mantle/MA_String.h>

CConfig::CConfig() : word_list(NULL)
{
}

void CConfig::Initialize(const char *filename)
{
	FILE *					fp;

	if( (fp = fopen(filename, "rt")) == NULL )
		return;

	struct word_list_s * word;

	char	szBuffer[256], szLine[256], token[256], value_string[256];

	while (fgets(szBuffer, MA_ARRAYCOUNT(szBuffer), fp))
	{
		MA_StringTrimA(szLine, MA_ARRAYCOUNT(szLine), szBuffer);

		// ÁÖ¼®
		if (*szLine == '#')
			continue;
		char* szDelimeter = (char*) MA_StringSearchCharacterA(szLine, ':');
		if( szDelimeter == NULL )
			continue;
		*szDelimeter = 0;

		MA_StringTrimA(token, MA_ARRAYCOUNT(token), szLine);
		MA_StringTrimA(value_string, MA_ARRAYCOUNT(value_string), szDelimeter+1);

		word			= new struct word_list_s;

		word->name		= MA_StringAllocA( strlen(token) );
		word->string	= MA_StringAllocA( strlen(value_string) );

		strcpy(word->name, token);
		strcpy(word->string, value_string);

		word->next		= word_list;
		word_list		= word;
	}

	fclose(fp);

	m_stFilename = filename;
}

void CConfig::Reload()
{
	if (m_stFilename.length() == 0)
		return;

	Destroy();
	Initialize(m_stFilename.c_str());
}

void CConfig::Destroy()
{
	struct word_list_s	*word;

	while ((word = word_list) != NULL)
	{
		word_list = word->next;

		MA_SAFE_FREE(word->name);
		MA_SAFE_FREE(word->string);
		MA_SAFE_DELETE(word);
	}

	word_list = NULL;
}

CConfig::~CConfig()
{
	Destroy();
}

char * CConfig::GetString(const char *name)
{
	struct word_list_s	*word;

	for (word = word_list; word; word = word->next)
	{
		if (MA_StringCompareA(word->name, name) == 0)
			return (word->string);
	}

	return NULL;
}

int CConfig::GetNumber(const char *name)
{
	struct word_list_s	*word;

	for (word = word_list; word; word = word->next)
	{
		if (MA_StringCompareA(word->name, name) == 0)
			return (atoi(word->string));
	}

	return (0);
}
