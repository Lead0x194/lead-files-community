#pragma once

#include <string>

class CConfig
{
public:
	CConfig();
	~CConfig();

	void	Initialize(const char * filename);
	void	Destroy();
	void	Reload();

	char*	GetString(const char * name);
	int	GetNumber(const char * name);

private:
	struct word_list_s
	{
		char			*name;
		char			*string;
		struct word_list_s	*next;
	} *word_list;

	std::string			m_stFilename;
};
