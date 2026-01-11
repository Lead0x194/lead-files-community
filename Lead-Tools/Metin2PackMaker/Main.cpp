#include <direct.h>
#include <sys/stat.h>
#include <io.h>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp> 
#include <fstream>

#pragma warning(push, 3)
#include <cryptopp/osrng.h>
#pragma warning(pop)

//#include <YmirBase/YmirBaseLink.h>
#include <cryptopp/cryptoppLibLink.h>
//#include <Mantle/MA_LibLink.h>
#include <lzo-2.03/lzoLibLink.h>
#pragma comment(lib, "winmm.lib")
//#pragma comment(lib, "d3d8.lib")

#ifdef _DEBUG
	#pragma comment(lib, "reducio_d.lib")
#else
	#pragma comment(lib, "reducio.lib")
#endif

#include <EterBase/Utils.h>
#include <EterBase/Filename.h>
#include <EterLib/TextFileLoader.h>
#include <EterPack/EterPackManager.h>
#include <Reducio/Reducio.h>

using std::string;
using std::vector;
using boost::unordered_map;
using std::size_t;

typedef unordered_map<string, bool, stringhash> NameDict;

string	g_st_packName;

NameDict g_map_ignoreFileName;
NameDict g_map_ignoreDirName;
NameDict g_map_ignoreBasePath;

typedef unordered_map<string, BYTE, stringhash> TStrMap;

TStrMap g_PackTypeByExtNameMap;
std::string g_strFolderName = "pack/";

static BYTE s_IV[32];

typedef unordered_map<string, string, stringhash> MapNameToSDBFile;

MapNameToSDBFile g_map_SDBFileList;
int				 iCompressTexQuality = 0;

bool IsSDBSupportRequired( const std::string& strFile, std::string& strMapName )
{
	MapNameToSDBFile::const_iterator cit = g_map_SDBFileList.find(strFile);

	if( cit != g_map_SDBFileList.end() )
	{
		strMapName = cit->second;
		return true;
	}
	
	return false;
}

//#define __ADD_CSHYBRID_ENCRYPT__

bool LoadList(const char* fileName, vector<string>* pvec_stLine)
{
	FILE* fp;

	if (0 != fopen_s(&fp, fileName, "r"))
		return false;

	while (1)
	{
		char line[256];
		if (!fgets(line, sizeof(line)-1, fp))
			break;

		size_t lineLen = strlen(line);

		if (line[lineLen-1] == '\n')
			line[lineLen-1] = '\0';

		if (line[lineLen-2] == '\r')
			line[lineLen-2] = '\0';

		pvec_stLine->push_back(line);		
	}
	fclose(fp);
	return true;
}

void IgnoreFileList_Append(const string& c_st_fileName)
{
	printf("ignore_file: [%s]\n", c_st_fileName.c_str());	
	
	if (c_st_fileName.find('*') >= 0) // strchr(c_st_fileName.c_str(), '*')
	{
		WIN32_FIND_DATA findData;
		HANDLE hFind;
		hFind = FindFirstFile(c_st_fileName.c_str(), &findData);		
		if (hFind != INVALID_HANDLE_VALUE)
		{
			std::string st_childName;

			do 
			{
				st_childName = findData.cFileName;
				StringPath(st_childName);

				g_map_ignoreFileName.insert(NameDict::value_type(st_childName, true));
			} 
			while (FindNextFile(hFind, &findData));
		}
		FindClose(hFind);
	}
	else
	{
		string st_fileName = c_st_fileName;
		StringPath(st_fileName);
		g_map_ignoreFileName.insert(NameDict::value_type(st_fileName, true));
	}
}

bool IgnoreFileList_Load(const char* fileName)
{
	vector<string> nameList;
	if (!LoadList(fileName, &nameList))
		return false;

	for_each(nameList.begin(), nameList.end(), IgnoreFileList_Append);	
	return true;
}

void IgnoreDirList_Append(const string& c_st_dirName)
{
	std::string st_dirName = c_st_dirName;
	StringPath(st_dirName);

	g_map_ignoreDirName.insert(NameDict::value_type(st_dirName, true));
}

bool IgnoreDirList_Load(const char* fileName)
{
	vector<string> nameList;
	if (!LoadList(fileName, &nameList))
		return false;

	for_each(nameList.begin(), nameList.end(), IgnoreDirList_Append);	
	return true;
}

void IgnoreBasePathList_Append(const string& c_st_basePath)
{
	std::string st_basePath = c_st_basePath;
	StringPath(st_basePath);

	g_map_ignoreBasePath.insert(NameDict::value_type(st_basePath, true));
}

bool IgnoreBasePathList_Load(const char* fileName)
{
	vector<string> nameList;
	if (!LoadList(fileName, &nameList))
		return false;

	for_each(nameList.begin(), nameList.end(), IgnoreBasePathList_Append);	
	return true;
}

using namespace Reducio;

void RegisterPackTypeByExtName(const char * c_pszExt, BYTE packType, bool isOverwrite=false)
{
	if (isOverwrite)
		g_PackTypeByExtNameMap[c_pszExt] = packType;
	else
		g_PackTypeByExtNameMap.insert(TStrMap::value_type(c_pszExt, packType));
}

void RecursivePack(CEterPack & pack, const char * filename, std::vector<std::string>& vecCompressedTextures, std::vector<int>& vecCompressedTexMipMaps )
{
    WIN32_FIND_DATA	fdata;
    HANDLE			hFind;
	char			temp[512];

    if ((hFind = FindFirstFile(filename, &fdata)) != INVALID_HANDLE_VALUE)
    {
		do
		{
			if (fdata.cFileName[0] == '.')
				continue;

			if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				strcpy_s(temp, fdata.cFileName);
				StringPath(temp);

				if (g_map_ignoreDirName.end() != g_map_ignoreDirName.find(temp))
				{
					//printf("Exclude1 %s\n", fdata.cFileName);
					continue;
				}

				strcpy_s(temp, filename);
				char *p = strchr(temp, '*'); // 뒤에 * 을 없앤다.
				if (p) *p = '\0';

				StringPath(temp);

				if (g_map_ignoreBasePath.end() != g_map_ignoreBasePath.find(temp))
				{
					//printf("Exclude2 %s\n", fdata.cFileName);
					continue;
				}

				sprintf_s(p, sizeof(temp) - (p - temp), "%s/*", fdata.cFileName);

				RecursivePack(pack, temp, vecCompressedTextures, vecCompressedTexMipMaps);
				continue;
			}

			strcpy_s(temp, fdata.cFileName);
			StringPath(temp);

			if (g_map_ignoreFileName.end() != g_map_ignoreFileName.find(temp))
			{
				//printf("Exclude3 %s\n", fdata.cFileName);
				continue;
			}

			strcpy_s(temp, filename);
			char *p;

			if ((p = strrchr(temp, '*')))	// 뒤에 * 을 없앤다.
				*p = '\0';

			StringPath(temp);
			
			if (g_map_ignoreBasePath.end() != g_map_ignoreBasePath.find(temp))
			{
				//printf("Exclude4 %s\n", fdata.cFileName);
				continue;
			}

			if (p)
				sprintf_s(p, sizeof(temp) - (p - temp), "%s", fdata.cFileName);
			else
				sprintf_s(temp, sizeof(temp), "%s", fdata.cFileName);

			CFilename ext(temp);
			stl_lowers(ext);

			//FIXME : TEMP HARD CODE
			if( !ext.GetExtension().compare("wdp") )
				continue;

			BYTE packType = COMPRESSED_TYPE_NONE;
			{
				bool bSuccessCompTexture = false;

				if( !ext.GetExtension().compare("dds") && iCompressTexQuality > 0 /* && !IsSDBSupportRequired(temp, strRelatedMap)*/)
				{
					ImageHandle h = CreateImageStreamHandle();

					//Texture압축하기
					eReturnCode eRet;
					eRet = CompressWMPImageFromFile(h, temp, iCompressTexQuality);
					if( eRet == eSuccess )
					{
						CFilename wdpFileName = ext.NoExtension() + ".wdp";
						DumpWMPImageToFile( h, wdpFileName.c_str() );
						bSuccessCompTexture = true;
						strcpy(temp, wdpFileName.c_str() );

						Reducio::U8* pb;
						int nStreamLen, nMipMapCnt;
						GetImageStreamInfo( h, &pb, nStreamLen, nMipMapCnt );
						
						vecCompressedTextures.push_back(temp);
						vecCompressedTexMipMaps.push_back(nMipMapCnt);
					}
					else
					{
						CMakePackLog::GetSingleton().Writef("failed to compress tex: %s errorcode: %d \n", temp, eRet );
						CMakePackLog::GetSingleton().WriteErrorf("failed to compress tex: %s errorcode: %d \n", temp, eRet );
					}

					DestroyImageStreamHandle(h);
				}

				if( !bSuccessCompTexture )
				{
					unordered_map<std::string, BYTE, stringhash>::iterator it = g_PackTypeByExtNameMap.find(ext);

					if (g_PackTypeByExtNameMap.end() != it)
						packType = it->second;
				}

				std::string strRelatedMapName;

				if( packType == COMPRESSED_TYPE_HYBRIDCRYPT && IsSDBSupportRequired(temp, strRelatedMapName) )
					packType = COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB;

				if (pack.Put(temp, NULL, packType, strRelatedMapName))
				{
					CMakePackLog::GetSingleton().Writef("pack: %s\n", temp);
				}
				else
				{
					CMakePackLog::GetSingleton().Writef("pack failed: %s\n", temp);
					CMakePackLog::GetSingleton().WriteErrorf("pack failed: %s\n", temp);
				}

			}
		}
		while (FindNextFile(hFind, &fdata));
    }
}

void AddIndex(const char * c_szName, const char * c_szDirectory)
{
	CFilename strIndexFileName = g_strFolderName + CFilename("Index.new");

	FILE * fp;
		
	if (0 != fopen_s(&fp, strIndexFileName.c_str(), "a+"))
	{
		printf("%s append open error", strIndexFileName.c_str());
		abort();
	}

	char szTmp[MAX_PATH + 1];
	strcpy_s(szTmp, c_szDirectory);

	char * p = strrchr(szTmp, '/');
	*(++p) = '\0';

	fprintf(fp, "%s\n%s\n", szTmp, c_szName);
	fclose(fp);
}

using namespace Reducio;

void MakeReducioHeader( std::vector<std::string>& textures, std::vector<int>& mipMapCounts )
{
	string st_HeaderFilePath;
	st_HeaderFilePath  = g_strFolderName;
	st_HeaderFilePath += g_st_packName;
	st_HeaderFilePath += ".rdch";
	
	std::ofstream outFile( st_HeaderFilePath.c_str(), ios_base::binary | ios_base::out );

	int nItemCount = textures.size();

	outFile.write((const char*)&nItemCount, sizeof(int) );

	for( int i = 0; i < nItemCount; ++i )
	{
		int iNameLength = textures[i].length();
		outFile.write((const char*)&iNameLength, sizeof(int) );
		outFile.write((const char*)&mipMapCounts[i], sizeof(int) );
		outFile.write(textures[i].c_str(), iNameLength );
	}

	outFile.close();
}


bool MakePackFiles(const std::vector<std::string>& filePaths, CEterPackManager* pPackManager)
{
	string st_packName = g_st_packName.empty() ? "noname" : g_st_packName;

	string st_packFilePath;
	st_packFilePath  = g_strFolderName;
	st_packFilePath += st_packName;
	
	CMakePackLog::GetSingleton().Writef("\n + Making %s\n", st_packName.c_str());

	CEterFileDict fileDict;
	CEterPack* pPack = new CEterPack;

	if (!pPack->Create(fileDict, st_packFilePath.c_str(), g_strFolderName.c_str(), false, s_IV))
		return false;

	std::vector<std::string>::const_iterator i = filePaths.begin();
	std::vector<std::string>::const_iterator e = filePaths.end();

	bool bAddToIndex = false;

	std::vector<std::string> vecCompressedTextures;
	std::vector<int>		 vecCompressedTexMipMaps;

	bool bSuccess = true;
	while (i != e)
	{
		const std::string& path = *i;
		std::string strRelatedMap;

		char temp[1024];
		strcpy_s(temp, path.c_str());

		StringPath(temp);

		CFilename lowerFilePath(temp);
		stl_lowers(lowerFilePath);

		BYTE packType = COMPRESSED_TYPE_NONE;

		CFilename ext = lowerFilePath.GetExtension();

		bool bSuccessCompTexture = false;

		if( !ext.compare("wdp") )
		{
			i++;
			continue;
		}

		if( !ext.compare("dds") && iCompressTexQuality > 0 /* && !IsSDBSupportRequired(temp, strRelatedMap)*/)
		{
			ImageHandle h = CreateImageStreamHandle();

			//Texture압축하기
			eReturnCode eRet;
			eRet = CompressWMPImageFromFile(h, temp, iCompressTexQuality);
			if( eRet == eSuccess )
			{
				CFilename wdpFileName = lowerFilePath.NoExtension() + ".wdp";
				DumpWMPImageToFile( h, wdpFileName.c_str() );
				bSuccessCompTexture = true;
				strcpy(temp, wdpFileName.c_str() );

				Reducio::U8* pb;
				int nStreamLen, nMipMapCnt;
				GetImageStreamInfo( h, &pb, nStreamLen, nMipMapCnt );
				
				vecCompressedTextures.push_back(temp);
				vecCompressedTexMipMaps.push_back(nMipMapCnt);
			}
			else
			{
				CMakePackLog::GetSingleton().Writef("failed to compress tex: %s errorcode: %d \n", temp, eRet );
				CMakePackLog::GetSingleton().WriteErrorf("failed to compress tex: %s errorcode: %d \n", temp, eRet );
				if (eRet != eFail_UnSupportedFormat)
					bSuccess = false;
			}

			DestroyImageStreamHandle(h);
		}

		if( !bSuccessCompTexture )
		{
			unordered_map<std::string, BYTE, stringhash>::iterator it = g_PackTypeByExtNameMap.find(ext);

			if (g_PackTypeByExtNameMap.end() != it)
				packType = it->second;
		}

		if( packType == COMPRESSED_TYPE_HYBRIDCRYPT )
		{
			bAddToIndex = true;
			if( IsSDBSupportRequired(temp, strRelatedMap) )
			{
				packType = COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB;
			}
		}

		if (pPack->Put(temp, NULL, packType, strRelatedMap))
		{
			CMakePackLog::GetSingleton().Writef("pack: %s\n", temp);
		}
		else
		{
			CMakePackLog::GetSingleton().Writef("pack failed: %s\n", temp);
			CMakePackLog::GetSingleton().WriteErrorf("pack failed: %s\n", temp);
			bSuccess = false;
		}
		i++;
	}
	
	pPackManager->RegisterPackWhenPackMaking(st_packFilePath.c_str(), g_strFolderName.c_str(), pPack );

	if( bAddToIndex )
	{
		AddIndex(st_packName.c_str(), g_strFolderName.c_str());
	}

	pPack->DeleteUnreferencedData();
	pPack->EncryptIndexFile();

	//write it
	if( vecCompressedTextures.size() > 0 )
	{
		MakeReducioHeader( vecCompressedTextures, vecCompressedTexMipMaps );
	}

	return bSuccess;
}

void MakePack(const char * c_szPackName, const char * c_szDirectory, CEterPackManager* pPackManager)
{
	string st_packName = g_st_packName.empty() ? c_szPackName : g_st_packName;
	string st_packFilePath;
	st_packFilePath  = g_strFolderName;
	st_packFilePath += st_packName;
	
	CMakePackLog::GetSingleton().Writef("\n + Making %s\n", st_packName.c_str());

	//CEterPack pack;
	CEterFileDict fileDict;
	CEterPack* pPack = new CEterPack;

	if (!pPack->Create(fileDict, st_packFilePath.c_str(), g_strFolderName.c_str(), false, s_IV))
		return;
	
	std::string stFolder(c_szDirectory);
	stl_lowers(stFolder);

	std::vector<std::string> vecCompressedTextures;
	std::vector<int>		 vecCompressedTexMipMaps;

	RecursivePack(*pPack, stFolder.c_str(), vecCompressedTextures, vecCompressedTexMipMaps);
	AddIndex(st_packName.c_str(), stFolder.c_str());

	pPack->DeleteUnreferencedData();
	pPack->EncryptIndexFile();

	//write it
	if( vecCompressedTextures.size() > 0 )
	{
		MakeReducioHeader( vecCompressedTextures, vecCompressedTexMipMaps );
	}

	pPackManager->RegisterPackWhenPackMaking(st_packFilePath.c_str(), g_strFolderName.c_str(), pPack );
}

bool MakeRecursivePack(const char * c_szPackName, CTextFileLoader & rTextFileLoader, CEterPackManager* pPackManager)
{
	string st_packName = g_st_packName.empty() ? c_szPackName : g_st_packName;
	string st_packFilePath;
	st_packFilePath  = g_strFolderName;
	st_packFilePath += st_packName;
	
	string st_testName = g_strFolderName + CFilename(c_szPackName);
	assert(st_testName == st_packFilePath);

	//CEterPack pack;
	CEterFileDict fileDict;
	CEterPack* pPack = new CEterPack;

	std::vector<std::string> vecCompressedTextures;
	std::vector<int>		 vecCompressedTexMipMaps;

	if (!pPack->Create(fileDict, st_packFilePath.c_str(), g_strFolderName.c_str(), false, s_IV))
		return false;

	for (DWORD i = 0; i < rTextFileLoader.GetChildNodeCount(); ++i)
	{
		if (rTextFileLoader.SetChildNode(i))
		{
			bool bAddIndex = false;
			std::string strAddIndex;
			if (rTextFileLoader.GetTokenString("addindex", &strAddIndex))
				if (0 == strAddIndex.compare("TRUE"))
					bAddIndex = true;

			std::string strFileName;
			if (!rTextFileLoader.GetTokenString("filename", &strFileName))
				continue;

			RecursivePack(*pPack, strFileName.c_str(), vecCompressedTextures, vecCompressedTexMipMaps);

			if (bAddIndex)
			{
				AddIndex(c_szPackName, strFileName.c_str());
			}

			rTextFileLoader.SetParentNode();
		}
	}

	pPack->DeleteUnreferencedData();
	pPack->EncryptIndexFile();

	//write it
	if( vecCompressedTextures.size() > 0 )
	{
		MakeReducioHeader( vecCompressedTextures, vecCompressedTexMipMaps );
	}

	pPackManager->RegisterPackWhenPackMaking(st_packFilePath.c_str(), g_strFolderName.c_str(), pPack );
	return true;
}

int main(int argc, char **argv)
{	
	if (argc < 2)
	{
		puts("USAGE:");
		printf("%s [command]\n", argv[0]);
		return 0;
	}

	boost::shared_ptr<CLZO> lzo(new CLZO);
	boost::shared_ptr<CEterPackManager> packMgr(new CEterPackManager);

	if (!strcmp(argv[1], "--createiv"))
	{
		// .exe --createiv <filename>
		if (argc != 3)
		{
			printf("Usage: %s --createiv <filename>\n", argv[0]);
			return 1;
		}

		CFileBase diskFile;

		if (diskFile.Create(argv[2], CFileBase::FILEMODE_WRITE))
		{
			BYTE iv[32];

			CryptoPP::AutoSeededRandomPool prng;
			prng.GenerateBlock(iv, sizeof(iv));

			diskFile.Write(iv, sizeof(iv));
			diskFile.Close();

			char szHex[32*2+1];

			for (int i = 0; i < 32; ++i)
				sprintf_s(szHex + i * 2, sizeof(szHex) - i * 2, "%02x", iv[i]);

			printf("IV %s created (hex %s)\n", argv[2], szHex);
			_chmod(argv[2], _S_IREAD); // 읽기 전용으로 만들기
			return 0;
		}

		printf("Cannot open %s, File may already exist\n", argv[2]);
		return 1;
	}
	else if (!strcmp(argv[1], "--openiv"))
	{
		// .exe --openiv <filename>
		if (argc != 3)
		{
			printf("Usage: %s --openiv <filename>\n", argv[0]);
			return 1;
		}

		CMappedFile file;
		const BYTE* iv;
		
		if (file.Create(argv[2], (const void**) &iv, 0, 32))
		{
			char szHex[32*2+1];

			for (int i = 0; i < 32; ++i)
				sprintf_s(szHex + i * 2, sizeof(szHex) - i * 2, "%02x", iv[i]);

			printf("IV %s loaded (hex %s)", argv[2], szHex);
			return 0;
		}

		printf("File not found or invalid format: %s\n", argv[3]);
		return 1;
	}
	else if (!strcmp(argv[1], "--extract"))
	{
		// .exe --extract <packname> [<IV filename>]
		if (argc < 3)
		{
			printf("Usage: %s --extract <packname> [<IV filename>]\n", argv[0]);
			return 1;
		}

		CMappedFile file;
		const BYTE* iv = NULL;

		if (argc >= 4)
		{	
			if (!file.Create(argv[3], (const void**) &iv, 0, 32))				
			{
				printf("Cannot load IV file %s\n", argv[3]);
				return 1;
			}
		}


		CEterPack pack;
		CEterFileDict dict;

		if (pack.Create(dict, argv[2], "", true, iv))
		{

			pack.Extract();
			return 0;
		}

		printf("Cannot extract pack %s\n", argv[2]);
		return 1;
	}

	HANDLE hThread = GetCurrentThread();
	SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
	SetLogLevel(1);	

	if (2 != argc)
	{
		printf("Usage: %s [ScriptFileName]\n", argv[0]);
		return 1;
	}

	char* pcExt = strrchr(argv[1], '.');

	if (pcExt)
	{
		std::string stLogFileName(argv[1], pcExt);
		CMakePackLog::GetSingleton().SetFileName(stLogFileName.c_str());
	}

	CTextFileLoader TextFileLoader;

	if (!TextFileLoader.Load(argv[1]))
	{
		printf("Failed to load pack script\n");
		return 1;
	}

	if (TextFileLoader.GetTokenString("foldername", &g_strFolderName))
	{
		_mkdir(g_strFolderName.c_str());
		g_strFolderName += "/";
	}

	if (TextFileLoader.GetTokenString("packname", &g_st_packName))
	{
		CMakePackLog::GetSingleton().Writef("\nPackName: %s\n", g_st_packName.c_str());
	}

	TextFileLoader.GetTokenInteger("compresstexture", &iCompressTexQuality);
	////////////////////////////////////////////////////////////
	IgnoreDirList_Load("ignore_dir_list.txt");
	IgnoreFileList_Load("ignore_file_list.txt");
	IgnoreBasePathList_Load("ignore_basepath_list.txt");

	CFilename strIndexFileName = g_strFolderName + CFilename("Index.new");
	FILE * fp;
	
	if (0 != fopen_s(&fp, strIndexFileName.c_str(), "w"))
	{
		char szCurDir[MAX_PATH + 1];
		GetCurrentDirectory(MAX_PATH, szCurDir);

		printf("%s 파일을 열 수 없습니다. 디렉토리가 없거나 파일이 이미 사용 중 입니다.\n"
			   "현재 디렉토리: %s\n", strIndexFileName.c_str(), szCurDir);
		return 1;
	}

	fprintf(fp, "PACK\n");	// Search Mode, 1 == 팩에서 먼저 읽음
	fclose(fp);

	////////////////////////////////////////////////////////////

	CTokenVector * pTokenVector;

	// ExcludedFolderNameList - 해당 폴더 이름은 팩하지 않는다.
	if (TextFileLoader.GetTokenVector("excludedfoldernamelist", &pTokenVector))
	{
		//printf("\n - ExcludedFolderNameList\n");
		CMakePackLog::GetSingleton().Writef("\n - ExcludedFolderNameList\n");

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;
			//printf(" %s\n", rstrName.c_str());
			CMakePackLog::GetSingleton().Writef(" %s\n", rstrName.c_str());

			IgnoreDirList_Append("CVS");
		}
	}

	// ExcludedPathList - 패스 내에 모든 것을 팩하지 않는다.
	if (TextFileLoader.GetTokenVector("excludedpathlist", &pTokenVector))
	{
		//printf("\n - ExcludedPathList\n");
		CMakePackLog::GetSingleton().Write("\n - ExcludedPathList\n");

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;
			//printf(" %s\n", rstrName.c_str());
			CMakePackLog::GetSingleton().Writef(" %s\n", rstrName.c_str());

			IgnoreBasePathList_Append("d:/ymir work/sound/");
		}
	}

	// ExcludedFileNameList - 모든 폴더에 대해 해당 파일이름은 팩하지 않는다.
	if (TextFileLoader.GetTokenVector("excludedfilenamelist", &pTokenVector))
	{
		//printf("\n - ExcludedFileNameList\n");
		CMakePackLog::GetSingleton().Writef("\n - ExcludedFileNameList\n");

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;
			//printf(" %s\n", rstrName.c_str());
			CMakePackLog::GetSingleton().Writef(" %s\n", rstrName.c_str());

			IgnoreFileList_Append(rstrName.c_str());
		}
	}
	
	// CompressExtNameList - 해당 확장자를 가진 파일은 compress 한다.
	if (TextFileLoader.GetTokenVector("compressextnamelist", &pTokenVector))
	{
		CMakePackLog::GetSingleton().Writef("\n - CompressExtNameList\n");

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;
			CMakePackLog::GetSingleton().Writef(" %s\n", rstrName.c_str());

			RegisterPackTypeByExtName(rstrName.c_str(), COMPRESSED_TYPE_COMPRESS);
		}
	}

	// SecurityExtNameList - 해당 확장자를 가진 파일은 encrypt 한다.
	if (TextFileLoader.GetTokenVector("securityextnamelist", &pTokenVector))
	{
		CMakePackLog::GetSingleton().Writef("\n - SecurityExtNameList\n");

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;
			CMakePackLog::GetSingleton().Writef(" %s\n", rstrName.c_str());

			RegisterPackTypeByExtName(rstrName.c_str(), COMPRESSED_TYPE_SECURITY);
		}
	}

	// OldCompressExtNameList - 해당 확장자를 가진 파일은 암호화 한다.
	if (TextFileLoader.GetTokenVector("oldcompressextnamelist", &pTokenVector))
	{
		CMakePackLog::GetSingleton().Writef("\n - OldCompressExtNameList\n");

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;
			CMakePackLog::GetSingleton().Writef(" %s\n", rstrName.c_str());

			// OldCompressExtNameList는 압축을 하는 것이 아니라 암호화를 해야하므로
			// 의도적으로 COMPRESSED_TYPE_SECURITY를 넣었음
			RegisterPackTypeByExtName(rstrName.c_str(), COMPRESSED_TYPE_SECURITY);
		}
	}

	std::string stIVFileName;

	if (TextFileLoader.GetTokenString("iv", &stIVFileName))
	{
		CMappedFile mappedFile;
		BYTE * pIV;

		if (mappedFile.Create(stIVFileName.c_str(), (const void **) &pIV, 0, 32))
			memcpy(s_IV, pIV, 32);
		else
		{
			TraceError("IV open error: %s", stIVFileName.c_str());
			return 1;
		}
	}

	// PanamaExtNameList - 새로운 암호화
	if (TextFileLoader.GetTokenVector("panamaextnamelist", &pTokenVector))
	{
		CMakePackLog::GetSingleton().Writef("\n - PanamaExtNameList\n");

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;
			CMakePackLog::GetSingleton().Writef(" %s\n", rstrName.c_str());

			RegisterPackTypeByExtName(rstrName.c_str(), COMPRESSED_TYPE_PANAMA, true);
		}
	}

	if (TextFileLoader.GetTokenVector("cshybridencryptexenamelist", &pTokenVector))
	{
		CMakePackLog::GetSingleton().Writef("\n - C/S Hybrid Encrypt ExtNameList\n");

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;
			CMakePackLog::GetSingleton().Writef(" %s\n", rstrName.c_str());

			RegisterPackTypeByExtName(rstrName.c_str(), COMPRESSED_TYPE_HYBRIDCRYPT, true);
		}
	}

	if (TextFileLoader.SetChildNode("rootpackitemlist"))
	{
		CMakePackLog::GetSingleton().Writef("\n + Making RootPack\n");
		MakeRecursivePack("root", TextFileLoader, packMgr.get());
		TextFileLoader.SetParentNode();
	}

	if (TextFileLoader.SetChildNode("localpathpack"))
	{
		CMakePackLog::GetSingleton().Writef("\n + Making RootPack\n");
		MakeRecursivePack(g_st_packName.c_str(), TextFileLoader, packMgr.get());
		TextFileLoader.SetParentNode();
	}

	if (TextFileLoader.SetChildNode("etcpackitemlist"))
	{
		CMakePackLog::GetSingleton().Writef("\n + Making ETCPack\n");
		MakeRecursivePack("ETC", TextFileLoader, packMgr.get());
		TextFileLoader.SetParentNode();
	}

	if (TextFileLoader.SetChildNode("soundpackitemlist"))
	{
		CMakePackLog::GetSingleton().Writef("\n + Making SoundPack\n");
		MakeRecursivePack("sound", TextFileLoader, packMgr.get());
		TextFileLoader.SetParentNode();
	}

	if (TextFileLoader.SetChildNode("sound2packitemlist"))
	{
		CMakePackLog::GetSingleton().Writef("\n + Making Sound2Pack\n");
		MakeRecursivePack("sound2", TextFileLoader, packMgr.get());
		TextFileLoader.SetParentNode();
	}

	CMakePackLog::GetSingleton().Writef("\n + PackList \n");

	if (TextFileLoader.SetChildNode("packlist"))
	{
		for (DWORD i = 0; i < TextFileLoader.GetChildNodeCount(); ++i)
		{
			if (TextFileLoader.SetChildNode(i))
			{
				std::string strNodeName;
				if (!TextFileLoader.GetCurrentNodeName(&strNodeName))
					continue;

				std::string strPathName;
				if (!TextFileLoader.GetTokenString("pathname", &strPathName))
					continue;

				MakePack(strNodeName.c_str(), strPathName.c_str(), packMgr.get());

				TextFileLoader.SetParentNode();
			}
		}
	}

	// SDB ( Supplementary Data Block Required File List ) 
	if (TextFileLoader.GetTokenVector("sdbfilelist", &pTokenVector))
	{
		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;		

			std::string rstrNameLower = rstrName;
			stl_lowers(rstrNameLower);

			std::vector<std::string> strs; 
			//mapname:sdbfilename
			boost::split(strs, rstrNameLower, boost::is_any_of("?")); 

			if( strs.size() == 1 )
			{
				strs.push_back(strs[0]);
				strs[0] = "none";
			}

			if( strs.size() != 2 )
			{
				CMakePackLog::GetSingleton().Writef("\n SDB File Usage: MapName:SDBFileName \n");
				return -1;
			}

			StringPath(strs[1]);
			g_map_SDBFileList[strs[1]] = strs[0];
		}
	}

	// filelist에 섹션에 입력되어 있는 모든 파일을 팩하기
	if (TextFileLoader.GetTokenVector("filelist", &pTokenVector))
	{
		std::vector<std::string> filePaths;

		CTokenVector::iterator itor = pTokenVector->begin();
		for (; pTokenVector->end() != itor; ++itor)
		{
			std::string & rstrName = *itor;	
			std::vector<std::string>::iterator it = std::find( filePaths.begin(), filePaths.end(), rstrName );
			if( it == filePaths.end() )
			{
				filePaths.push_back(rstrName);
			}
		}

		if(!MakePackFiles(filePaths, packMgr.get()))
		{
			CMakePackLog::GetSingleton().FlushError();
			return -1;
		}
	}

	//make key file for CS hybrid crypt
	std::string strCryptFileName = g_strFolderName + "/" + "cshybridcrypt_" + g_st_packName + ".dat";

	packMgr->WriteHybridCryptPackInfo(strCryptFileName.c_str());

	CFilename strOldIndexFileName = g_strFolderName + CFilename("Index");

	DeleteFile(strOldIndexFileName.c_str());
	MoveFile(strIndexFileName.c_str(), strOldIndexFileName.c_str());

	TextFileLoader.Destroy();
	CTextFileLoader::DestroySystem();
	return 0;
}
