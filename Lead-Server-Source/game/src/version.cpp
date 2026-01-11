#include <stdio.h>

void WriteVersion()
{
#ifndef __WIN32__
	FILE* fp = fopen("ver.txt", "w");

	if (fp)
	{
		fprintf(fp, "emulated game server revision: %s\n", __SVN_VERSION__);
		//fprintf(fp, "%s@%s:%s\n", __USER__, __HOSTNAME__, __PWD__);
		fclose(fp);
	}
#endif
}

