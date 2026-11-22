#include "StdAfx.h"

#include <windows.h>
#include <mmsystem.h>
#include <io.h>
#include <assert.h>

#include "EterPack.h"
#include "Inline.h"
#include "EterPackPolicy_CSHybridCrypt.h"

#pragma warning(push, 3)
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/tiger.h>
#include <cryptopp/sha.h>
#include <cryptopp/ripemd.h>
#include <cryptopp/whrlpool.h>
#include <cryptopp/panama.h>

#pragma warning(pop)

#include "../EterBase/utils.h"
#include "../EterBase/Debug.h"
#include "../EterBase/CRC32.h"

#include <iostream>
#include <fstream>

void CMakePackLog::SetFileName(const char* c_szFileName)
{
	m_stFileName=c_szFileName;
	m_stFileName +=".log";
	m_stErrorFileName = c_szFileName;
	m_stErrorFileName += ".err";
}

CMakePackLog& CMakePackLog::GetSingleton()
{
	static CMakePackLog s_kMakePackLog;
	return s_kMakePackLog;
}

CMakePackLog::CMakePackLog()
{
	m_fp=NULL;
	m_fp_err = NULL;
}

CMakePackLog::~CMakePackLog()
{
	if (NULL!=m_fp)
	{
		fclose(m_fp);
		m_fp=NULL;
	}
	if (NULL != m_fp_err)
	{
		fclose(m_fp_err);
		m_fp_err = NULL;
	}
}

bool CMakePackLog::__IsLogMode()
{
	if (m_stFileName.empty())
		return false;

	return true;
}

void CMakePackLog::Writef(const char* c_szFormat, ...)
{
	if (!__IsLogMode())
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[1024];
	int nBufLen = _vsnprintf_s(szBuf, sizeof(szBuf), _TRUNCATE, c_szFormat, args);
	szBuf[nBufLen++] = '\0';
	__Write(szBuf, nBufLen);
}

void CMakePackLog::Writenf(const char* c_szFormat, ...)
{
	if (!__IsLogMode())
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[1024+1];
	int nBufLen = _vsnprintf_s(szBuf, sizeof(szBuf), sizeof(szBuf)-1, c_szFormat, args);
	if (nBufLen > 0)
	{
		szBuf[nBufLen++] = '\n';
		szBuf[nBufLen++] = '\0';
	}
	__Write(szBuf, nBufLen);
}

void CMakePackLog::Write(const char* c_szBuf)
{
	if (!__IsLogMode())
		return;

	__Write(c_szBuf, static_cast<int>(strlen(c_szBuf)+1));
}

void CMakePackLog::__Write(const char* c_szBuf, int nBufLen)
{
	if (!__IsLogMode())
		return;

	if (NULL==m_fp)
		fopen_s(&m_fp, m_stFileName.c_str(), "w");

	fwrite(c_szBuf, nBufLen, 1, m_fp);

	printf("%s", c_szBuf);
}



void CMakePackLog::WriteErrorf(const char* c_szFormat, ...)
{
	if (!__IsLogMode())
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[1024];
	int nBufLen = _vsnprintf_s(szBuf, sizeof(szBuf), _TRUNCATE, c_szFormat, args);
	szBuf[nBufLen++] = '\0';
	__WriteError(szBuf, nBufLen);
}

void CMakePackLog::WriteErrornf(const char* c_szFormat, ...)
{
	if (!__IsLogMode())
		return;

	va_list args;
	va_start(args, c_szFormat);

	char szBuf[1024+1];
	int nBufLen = _vsnprintf_s(szBuf, sizeof(szBuf), sizeof(szBuf)-1, c_szFormat, args);
	if (nBufLen > 0)
	{
		szBuf[nBufLen++] = '\n';
		szBuf[nBufLen++] = '\0';
	}
	__WriteError(szBuf, nBufLen);
}

void CMakePackLog::WriteError(const char* c_szBuf)
{
	if (!__IsLogMode())
		return;

	__WriteError(c_szBuf, static_cast<int>(strlen(c_szBuf)+1));
}

void CMakePackLog::__WriteError(const char* c_szBuf, int nBufLen)
{
	if (!__IsLogMode())
		return;

	if (NULL==m_fp_err)
		fopen_s(&m_fp_err, m_stErrorFileName.c_str(), "w");

	fwrite(c_szBuf, nBufLen, 1, m_fp_err);

	printf("Error: %s", c_szBuf);
}

void CMakePackLog::FlushError()
{
	std::wifstream iFile(m_stErrorFileName.c_str());
    std::istream_iterator <std::wstring, wchar_t> iit(iFile);
    std::istream_iterator <std::wstring, wchar_t> eos;

    std::vector <std::wstring> vText;

    std::copy (iit, eos, std::back_inserter(vText));

    std::ostream_iterator <std::wstring, wchar_t, std::char_traits <wchar_t> > oit(std::wcout);

    std::sort (vText.begin(), vText.end());
    std::copy (vText.begin(), vText.end(), oit);
}
#ifdef __MAKE_PACK__
FILE * CEterPack::ms_PackLogFile = NULL;
#endif
///////////////////////////////////////////////////////////////////////////////
CEterPack::CEterPack() : m_indexCount(0), m_indexData(NULL), m_FragmentSize(0), m_bEncrypted(false), m_bReadOnly(false), m_bDecrypedIV(false)
{
	m_pCSHybridCryptPolicy = new EterPackPolicy_CSHybridCrypt;
	
}

CEterPack::~CEterPack()
{
	Destroy();
	
	delete m_pCSHybridCryptPolicy;
	m_pCSHybridCryptPolicy = NULL;
}

void CEterPack::Destroy()
{
	m_bReadOnly = false;
	m_bEncrypted = false;
	m_indexCount = 0;
	m_DataPositionMap.clear();

	for (int i = 0; i < FREE_INDEX_MAX_SIZE + 1; ++i)
		m_FreeIndexList[i].clear();

	SAFE_DELETE_ARRAY(m_indexData);

	m_FragmentSize = 0;

	memset(m_dbName, 0, sizeof(m_dbName));
	memset(m_indexFileName, 0, sizeof(m_indexFileName));	
}

const std::string& CEterPack::GetPathName()
{
	return m_stPathName;
}

bool CEterPack::Create(CEterFileDict& rkFileDict, const char * dbname, const char* pathName, bool bReadOnly, const BYTE* iv, bool bPrefixPath)
{
	m_bPrefixPath = bPrefixPath;

	if (iv)
	{
		m_stIV_Panama.assign((const char*) iv, 32);
		m_bDecrypedIV = false;
	}

	m_stPathName = pathName;

	strncpy_s(m_dbName, sizeof(m_dbName), dbname, _TRUNCATE);

	strncpy_s(m_indexFileName, sizeof(m_indexFileName), dbname, _TRUNCATE);
	strcat_s(m_indexFileName, sizeof(m_indexFileName), ".eix");

	m_stDataFileName = dbname;
	m_stDataFileName += ".epk";
	
	m_bReadOnly = bReadOnly;

	// Creation will fail if the database is not opened in bReadOnly mode.
	if (!CreateIndexFile())
		return false;

	if (!CreateDataFile())
		return false;

	bool bOverwrite = (iv != NULL);
	__BuildIndex(rkFileDict, bOverwrite);

	if (m_bReadOnly)
	{
		//m_bIsDataLoaded = true;		
		//if (!m_file.Create(m_stDataFileName.c_str(), (const void**)&m_file_data, 0, 0))
		//	return false;
	}
	else
	{
		DecryptIndexFile();
	}

	return true;
}

bool CEterPack::DecryptIV(DWORD dwPanamaKey)
{	
	if (m_stIV_Panama.length() != 32)
		return false;

	if (m_bDecrypedIV) // If the encryption has already been released, it will not be processed again.
		return true;

	DWORD* ivs = (DWORD*)&m_stIV_Panama[0];
	for (int i = 0; i != m_stIV_Panama.length() / sizeof(DWORD); ++i)
	{
		ivs[i] ^= dwPanamaKey + i * 16777619;		
	}

	m_bDecrypedIV = true;
	return true;
}

bool CEterPack::DecryptIndexFile()
{
	if (!m_bEncrypted)
		return true;

	CFileBase file;
	
	if (!file.Create(m_indexFileName, CFileBase::FILEMODE_WRITE))
		return false;

	file.Write(&eterpack::c_IndexCC, sizeof(DWORD));
	file.Write(&eterpack::c_Version, sizeof(DWORD));
	file.Write(&m_indexCount, sizeof(m_indexCount));
	file.Write(m_indexData, sizeof(TEterPackIndex) * m_indexCount);

	file.Close();

	m_bEncrypted = false;
	return true;
}

static DWORD s_adwEterPackKey[] =
{
	45129401,
	92367215,
	681285731,
	1710201,
};

static DWORD s_adwEterPackSecurityKey[] =
{
	78952482,
	527348324,
	1632942,
	486274726,
};

bool CEterPack::EncryptIndexFile()
{
	CMappedFile file;
	LPCVOID pvData;

	if (NULL == file.Create(m_indexFileName, &pvData, 0, 0))
	{
		TraceError("EncryptIndex: Cannot open pack index file! %s", m_dbName);
		return false;
	}

	BYTE * pbData = new BYTE[file.Size()];
	memcpy(pbData, pvData, file.Size());

	CLZObject zObj;
	
	if (!CLZO::Instance().CompressEncryptedMemory(zObj, pbData, file.Size(), s_adwEterPackKey))
	{
		TraceError("EncryptIndex: Cannot encrypt! %s", m_dbName);
		SAFE_DELETE_ARRAY(pbData);
		return false;
	}

	file.Destroy();

	while (!DeleteFile(m_indexFileName));

	FILE * fp;

	fopen_s(&fp, m_indexFileName, "wb");

	if (!fp)
	{
		TraceError("EncryptIndex: Cannot open file for writing! %s", m_dbName);
		SAFE_DELETE_ARRAY(pbData);
		return false;
	}

	if (1 != fwrite(zObj.GetBuffer(), zObj.GetSize(), 1, fp))
	{
		TraceError("Encryptindex: Cannot write to file! %s", m_indexFileName);
		SAFE_DELETE_ARRAY(pbData);
		fclose(fp);
		return false;
	}
	
	fclose(fp);

	m_bEncrypted = true;
	delete [] pbData;
	return true;
}

bool CEterPack::__BuildIndex(CEterFileDict& rkFileDict, bool bOverwrite)
{
	//DWORD dwBeginTime = ELTimer_GetMSec();
	CMappedFile file;
	LPCVOID pvData;
	CLZObject zObj;

	if (NULL == file.Create(m_indexFileName, &pvData, 0, 0))
	{
		TraceError("Cannot open pack index file! %s", m_dbName);
		return false;
	}

	if (file.Size() < eterpack::c_HeaderSize)
	{
		TraceError("Pack index file header error! %s", m_dbName);
		return false;
	}

	DWORD fourcc = *(DWORD *) pvData;

	BYTE * pbData;
	UINT uiFileSize;

	if (fourcc == eterpack::c_IndexCC)
	{
		pbData = (BYTE *) pvData;
		uiFileSize = file.Size();
	}
	else if (fourcc == CLZObject::ms_dwFourCC)
	{
		m_bEncrypted = true;

		if (!CLZO::Instance().Decompress(zObj, (const BYTE *) pvData, s_adwEterPackKey))
			return false;

		if (zObj.GetSize() < eterpack::c_HeaderSize)
			return false;

		pbData = zObj.GetBuffer();
		uiFileSize = zObj.GetSize();
	}
	else
	{
		TraceError("Pack index file fourcc error! %s", m_dbName);
		return false;
	}

	pbData += sizeof(DWORD);

	DWORD ver = *(DWORD *) pbData;
	pbData += sizeof(DWORD);

	if (ver != eterpack::c_Version)
	{
		TraceError("Pack index file version error! %s", m_dbName);
		return false;
	}

	m_indexCount = *(int *) pbData;
	pbData += sizeof(int);

	if (uiFileSize < eterpack::c_HeaderSize + sizeof(TEterPackIndex) * m_indexCount)
	{
		TraceError("Pack index file size error! %s, indexCount %d", m_dbName, m_indexCount);
		return false;
	}

	//Tracef("Loading Pack file %s elements: %d ... ", m_dbName, m_indexCount);

	m_indexData = new TEterPackIndex[m_indexCount];
	memcpy(m_indexData, pbData, sizeof(TEterPackIndex) * m_indexCount);

	TEterPackIndex * index = m_indexData;

	for (int i = 0; i < m_indexCount; ++i, ++index)
	{
		if (!index->filename_crc)
		{
			PushFreeIndex(index);
		}
		else
		{
			if (index->real_data_size > index->data_size)
				m_FragmentSize += index->real_data_size - index->data_size;

			if (m_bPrefixPath)
			{
				std::string stFileName(m_stPathName);
				stFileName.append(index->filename, strnlen(index->filename, FILENAME_MAX_LEN));
				stl_lowers(stFileName);
				std::replace(stFileName.begin(), stFileName.end(), '\\', '/');

				strncpy_s(index->filename, sizeof(index->filename), stFileName.c_str(), _TRUNCATE);
				index->filename_crc = GetCRC32(index->filename, strlen(index->filename));
			}

			m_DataPositionMap.insert(TDataPositionMap::value_type(index->filename_crc, index));

			if (bOverwrite) // The server-linked packing file will come later, but must be registered at the top level.
				rkFileDict.UpdateItem(this, index);
			else
				rkFileDict.InsertItem(this, index);
		}
	}

	//Tracef("Done. (spent %dms)\n", ELTimer_GetMSec() - dwBeginTime);
	return true;
}
//
//void CEterPack::UpdateLastAccessTime()
//{
//	m_tLastAccessTime = time(NULL);
//}
//
//void CEterPack::ClearDataMemoryMap()
//{
// // m_file is a data file...
//	m_file.Destroy();
//	m_tLastAccessTime = 0;
//	m_bIsDataLoaded = false;
//}

bool CEterPack::Get(CMappedFile& out_file, const char * filename, LPCVOID * data)
{
	TEterPackIndex * index = FindIndex(filename);
	
	if (!index)
	{
		return false;
	}

	//UpdateLastAccessTime();
	//if (!m_bIsDataLoaded)
	//{
	//	if (!m_file.Create(m_stDataFileName.c_str(), (const void**)&m_file_data, 0, 0))
	//		return false;
	//	
	//	m_bIsDataLoaded = true;
	//}
	
	// Previously, CEterPack placed the epk on the memory map, and when a request was made, the part was linked and handed over.
	// Now, when a request comes, only the necessary parts are uploaded to the memory map and released when the request is completed.
	out_file.Create(m_stDataFileName.c_str(), data, index->data_position, index->data_size);
	
	bool bIsSecurityCheckRequired = ( index->compressed_type == COMPRESSED_TYPE_SECURITY ||
									  index->compressed_type == COMPRESSED_TYPE_PANAMA );

	if( bIsSecurityCheckRequired )
	{
#ifdef CHECKSUM_CHECK_MD5
		MD5_CTX context;
		GenerateMD5Hash( (BYTE*)(*data), index->data_size, context );

		if( memcmp( index->MD5Digest, context.digest, 16 ) != 0 )
		{
			return false;
		}	
#else
		DWORD dwCrc32 = GetCRC32((const char*)(*data), index->data_size);

		if( index->data_crc != dwCrc32 )
		{
			return false;
		}
#endif
	}


	if (COMPRESSED_TYPE_COMPRESS == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;

		if (!CLZO::Instance().Decompress(*zObj, static_cast<const BYTE *>(*data)))
		{
			TraceError("Failed to decompress : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject(zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_SECURITY == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;

		if (!CLZO::Instance().Decompress(*zObj, static_cast<const BYTE *>(*data), s_adwEterPackSecurityKey))
		{
			TraceError("Failed to encrypt : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject(zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_PANAMA == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;
		__Decrypt_Panama(filename, static_cast<const BYTE*>(*data), index->data_size, *zObj);
		out_file.BindLZObjectWithBufferedSize(zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_HYBRIDCRYPT == index->compressed_type || COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB == index->compressed_type)
	{
		auto stFilename = std::string(filename);
		CLZObject * zObj = new CLZObject;

		if( !m_pCSHybridCryptPolicy->DecryptMemory(stFilename, static_cast<const BYTE*>(*data), index->data_size, *zObj) )
		{
			return false;
		}

		out_file.BindLZObjectWithBufferedSize(zObj);
		if( COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB == index->compressed_type)
		{
			BYTE* pSDBData;
			int   iSDBSize;

			if( !m_pCSHybridCryptPolicy->GetSupplementaryDataBlock(stFilename, pSDBData, iSDBSize) )
			{
				return false;
			}

			*data = out_file.AppendDataBlock( pSDBData, iSDBSize );
		}
		else
		{
			*data = zObj->GetBuffer();
		}
	}
	return true;
}

bool CEterPack::Get2(CMappedFile& out_file, const char * filename, TEterPackIndex * index, LPCVOID * data)
{
	if (!index)
	{
		return false;
	}

	//UpdateLastAccessTime();
	//if (!m_bIsDataLoaded)
	//{
	//	if (!m_file.Create(m_stDataFileName.c_str(), (const void**)&m_file_data, 0, 0))
	//		return false;
	//	
	//	m_bIsDataLoaded = true;
	//}
	out_file.Create(m_stDataFileName.c_str(), data, index->data_position, index->data_size);

	bool bIsSecurityCheckRequired = ( index->compressed_type == COMPRESSED_TYPE_SECURITY ||
									  index->compressed_type == COMPRESSED_TYPE_PANAMA );

	if( bIsSecurityCheckRequired )
	{
#ifdef CHECKSUM_CHECK_MD5
		MD5_CTX context;
		GenerateMD5Hash( (BYTE*)(*data), index->data_size, context );

		if( memcmp( index->MD5Digest, context.digest, 16 ) != 0 )
		{
			return false;
		}	
#else
		DWORD dwCrc32 = GetCRC32((const char*)(*data), index->data_size);

		if( index->data_crc != dwCrc32 )
		{
			return false;
		}
#endif
	}


	if (COMPRESSED_TYPE_COMPRESS == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;

		if (!CLZO::Instance().Decompress(*zObj, static_cast<const BYTE *>(*data)))
		{
			TraceError("Failed to decompress : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject(zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_SECURITY == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;

		if (!CLZO::Instance().Decompress(*zObj, static_cast<const BYTE *>(*data), s_adwEterPackSecurityKey))
		{
			TraceError("Failed to encrypt : %s", filename);
			delete zObj;
			return false;
		}

		out_file.BindLZObject(zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_PANAMA == index->compressed_type)
	{
		CLZObject * zObj = new CLZObject;
		__Decrypt_Panama(filename, static_cast<const BYTE*>(*data), index->data_size, *zObj);
		out_file.BindLZObjectWithBufferedSize(zObj);
		*data = zObj->GetBuffer();
	}
	else if (COMPRESSED_TYPE_HYBRIDCRYPT == index->compressed_type || COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB == index->compressed_type)
	{
		auto stFilename = std::string(filename);
		CLZObject * zObj = new CLZObject;

		if( !m_pCSHybridCryptPolicy->DecryptMemory(stFilename, static_cast<const BYTE*>(*data), index->data_size, *zObj) )
		{
			return false;
		}

		out_file.BindLZObjectWithBufferedSize(zObj);

		if( COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB == index->compressed_type)
		{
			BYTE* pSDBData;
			int   iSDBSize;
			
			if( !m_pCSHybridCryptPolicy->GetSupplementaryDataBlock(stFilename, pSDBData, iSDBSize) )
			{
				return false;
			}

			*data = out_file.AppendDataBlock( pSDBData, iSDBSize );
		}
		else
		{
			*data = zObj->GetBuffer();
		}
	}

	return true;
}


bool CEterPack::Delete(TEterPackIndex * pIndex)
{
	CFileBase fileIndex;

	if (!fileIndex.Create(m_indexFileName, CFileBase::FILEMODE_WRITE))
		return false;

	PushFreeIndex(pIndex);
	WriteIndex(fileIndex, pIndex);
	return true;
}

bool CEterPack::Delete(const char * filename)
{
	TEterPackIndex * pIndex = FindIndex(filename);

	if (!pIndex)
		return false;

	return Delete(pIndex);
}

bool CEterPack::Extract()
{
	CMappedFile dataMapFile;
	LPCVOID		data;

	if (!dataMapFile.Create(m_stDataFileName.c_str(), &data, 0, 0))
		return false;

	CLZObject zObj;
	
	for (TDataPositionMap::iterator i = m_DataPositionMap.begin();
		i != m_DataPositionMap.end();
		++i)
	{
		TEterPackIndex* index = i->second;
		CFileBase writeFile;
		
		inlinePathCreate(index->filename);
		printf("%s\n", index->filename);
		
		writeFile.Create(index->filename, CFileBase::FILEMODE_WRITE);

		if (COMPRESSED_TYPE_COMPRESS == index->compressed_type)
		{
			if (!CLZO::Instance().Decompress(zObj, (const BYTE *) data + index->data_position))
			{
				printf("cannot decompress");
				return false;
			}

			writeFile.Write(zObj.GetBuffer(), zObj.GetSize());
			zObj.Clear();
		}
		else if (COMPRESSED_TYPE_SECURITY == index->compressed_type)
		{
			if (!CLZO::Instance().Decompress(zObj, (const BYTE *) data + index->data_position, s_adwEterPackSecurityKey))
			{
				printf("cannot decompress");
				return false;
			}

			writeFile.Write(zObj.GetBuffer(), zObj.GetSize());
			zObj.Clear();
		}
		else if (COMPRESSED_TYPE_PANAMA == index->compressed_type)
		{
			__Decrypt_Panama(index->filename, (const BYTE *) data + index->data_position, index->data_size, zObj);
			writeFile.Write(zObj.GetBuffer(), zObj.GetBufferSize());
			zObj.Clear();
		}
		else if (COMPRESSED_TYPE_HYBRIDCRYPT == index->compressed_type || COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB == index->compressed_type)
		{
			auto stFilename = std::string(index->filename);
			if( !m_pCSHybridCryptPolicy->DecryptMemory(stFilename, (const BYTE *) data + index->data_position, index->data_size, zObj) )
				return false;

			if( COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB == index->compressed_type)
			{
				dataMapFile.BindLZObjectWithBufferedSize(&zObj);

				BYTE* pSDBData;
				int   iSDBSize;

				if( !m_pCSHybridCryptPolicy->GetSupplementaryDataBlock(stFilename, pSDBData, iSDBSize) )
					return false;

				dataMapFile.AppendDataBlock( pSDBData, iSDBSize );
				writeFile.Write(dataMapFile.AppendDataBlock( pSDBData, iSDBSize ),dataMapFile.Size());
			}
			else
			{
				writeFile.Write(zObj.GetBuffer(), zObj.GetBufferSize());
			}
			zObj.Clear();
		}
		else if (COMPRESSED_TYPE_NONE == index->compressed_type)
			writeFile.Write((const char *) data + index->data_position, index->data_size);
		
		writeFile.Destroy();
	}
	return true;
}

bool CEterPack::GetNames(std::vector<std::string>* retNames)
{
	CMappedFile dataMapFile;
	LPCVOID		data;

	if (!dataMapFile.Create(m_stDataFileName.c_str(), &data, 0, 0))
		return false;

	CLZObject zObj;
	
	for (TDataPositionMap::iterator i = m_DataPositionMap.begin();
		i != m_DataPositionMap.end();
		++i)
	{
		TEterPackIndex* index = i->second;
		
		inlinePathCreate(index->filename);

		retNames->push_back(index->filename);
	}
	return true;
}

bool CEterPack::Put(const char * filename, const char * sourceFilename, BYTE packType, const std::string& strRelateMapName )
{
	CMappedFile mapFile;
	LPCVOID		data;

	if (sourceFilename)
	{
		if (!mapFile.Create(sourceFilename, &data, 0, 0))
		{
			return false;
		}
	}
	else if (!mapFile.Create(filename, &data, 0, 0))
	{
		return false;
	}

	BYTE*  pMappedData    = (BYTE*)data;
	int	   iMappedDataSize = mapFile.Size();

	auto stFilename = std::string(filename);

	if( packType == COMPRESSED_TYPE_HYBRIDCRYPT || packType == COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB )
	{
		m_pCSHybridCryptPolicy->GenerateCryptKey(stFilename);

		if( packType == COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB )
		{
			if( !m_pCSHybridCryptPolicy->GenerateSupplementaryDataBlock(stFilename, strRelateMapName, (const BYTE*)data, mapFile.Size(), pMappedData, iMappedDataSize ))
			{
				return false;
			}
		}
	}

	return Put(filename, pMappedData, iMappedDataSize, packType);
}

#ifdef CHECKSUM_CHECK_MD5
void CEterPack::GenerateMD5Hash( BYTE* pData, int nLength, IN OUT MD5_CTX& mdContext )
{
	MD5Init (&mdContext);

	const int nBlockSize = 1024;

	int nLoopCnt   = nLength / nBlockSize;
	int nRemainder = nLength % nBlockSize;
	int i;

	for(i = 0; i < nLoopCnt; ++i )
	{
		MD5Update (&mdContext, reinterpret_cast<BYTE*>(pData + i*nBlockSize), nBlockSize);
	}

	if( nRemainder > 0 ) 
		MD5Update (&mdContext, reinterpret_cast<BYTE*>(pData + i*nBlockSize), nRemainder);

	MD5Final (&mdContext);
}
#endif
bool CEterPack::Put(const char * filename, LPCVOID data, long len, BYTE packType)
{
	if (m_bEncrypted)
	{
		TraceError("EterPack::Put : Cannot put to encrypted pack (filename: %s, DB: %s)", filename, m_dbName);
		return false;
	}

	CFileBase fileIndex;

	if (!fileIndex.Create(m_indexFileName, CFileBase::FILEMODE_WRITE))
	{
		return false;
	}

	CFileBase fileData;

	if (!fileData.Create(m_stDataFileName.c_str(), CFileBase::FILEMODE_WRITE))
	{
		return false;
	}

	TEterPackIndex * pIndex;
	pIndex = FindIndex(filename);

	CLZObject zObj;
	std::string encryptStr;

	if (packType == COMPRESSED_TYPE_SECURITY || 
		packType == COMPRESSED_TYPE_COMPRESS)
	{
		if (packType == COMPRESSED_TYPE_SECURITY)
		{
			if (!CLZO::Instance().CompressEncryptedMemory(zObj, data, len, s_adwEterPackSecurityKey))
			{
				return false;
			}
		}
		else
		{
			if (!CLZO::Instance().CompressMemory(zObj, data, len))
			{
				return false;
			}
		}

		data = zObj.GetBuffer();
		len = zObj.GetSize();
	}
	else if (packType == COMPRESSED_TYPE_PANAMA)
	{
		if (!__Encrypt_Panama(filename, (const BYTE *) data, len, zObj))
		{
			return false;
		}

		data = zObj.GetBuffer();
		len = zObj.GetBufferSize();
	}
	else if (packType == COMPRESSED_TYPE_HYBRIDCRYPT || packType == COMPRESSED_TYPE_HYBRIDCRYPT_WITHSDB )
	{
		auto stFilename = std::string(filename);
		if( !m_pCSHybridCryptPolicy->EncryptMemory(stFilename, (const BYTE *)data, len, zObj ) )
		{
			return false;
		}

		data = zObj.GetBuffer();
		len = zObj.GetBufferSize();
	}


#ifdef CHECKSUM_CHECK_MD5
	MD5_CTX context;
	GenerateMD5Hash( (BYTE*)(data), len, context );
#else
	DWORD data_crc;
	data_crc = GetCRC32((const char *) data, len);
#endif

	// If you have existing data...
	if (pIndex)
	{
		// If the existing data size can accommodate the data size to be inserted
		if (pIndex->real_data_size >= len)
		{
			++m_map_indexRefCount[pIndex->id];

			// Save only when the length is wrong or the checksum is wrong.
			if ( (pIndex->data_size != len) || 
#ifdef CHECKSUM_CHECK_MD5
				(memcmp( pIndex->MD5Digest, context.digest, 16 ) != 0) )
#else
				(pIndex->data_crc != data_crc) )
#endif
			{
#ifdef __MAKE_PACK__
				if (ms_PackLogFile)
					fprintf(ms_PackLogFile, "Overwrite[%d/%d] %s\n", bCompress, bSecurity, pIndex->filename);
				printf("Overwrite[%d/%d] %s\n", bCompress, bSecurity, pIndex->filename);
#endif
				pIndex->data_size = len;
#ifdef CHECKSUM_CHECK_MD5
				memcpy( pIndex->MD5Digest, context.digest, 16 );

#else
				pIndex->data_crc = data_crc;
#endif

				pIndex->compressed_type = packType;

				CMakePackLog::GetSingleton().Writef("Overwrite[type:%u] %s\n", pIndex->compressed_type, pIndex->filename);

				WriteIndex(fileIndex, pIndex);
				WriteData(fileData, pIndex, data);
			}

			return true;
		}

		// If the existing data size is smaller than the new one, allocate a new index.
		// You have to put it in. The original index is left empty.
		PushFreeIndex(pIndex);
		WriteIndex(fileIndex, pIndex);
	}

	// new data
	pIndex = NewIndex(fileIndex, filename, len);
	pIndex->data_size = len;

#ifdef CHECKSUM_CHECK_MD5
		memcpy( pIndex->MD5Digest, context.digest, 16 );
#else
		pIndex->data_crc = data_crc;
#endif


	pIndex->data_position = GetNewDataPosition(fileData);
	pIndex->compressed_type = packType;

	WriteIndex(fileIndex, pIndex);
	WriteNewData(fileData, pIndex, data);

	++m_map_indexRefCount[pIndex->id];

#ifdef __MAKE_PACK__
	if (ms_PackLogFile)
		fprintf(ms_PackLogFile, "Write[%d/%d] %s\n", bCompress, bSecurity, pIndex->filename);
	printf("Write[%d/%d] %s, position %ld realsize %ld size %ld\n",
			bCompress, bSecurity, pIndex->filename, pIndex->data_position, pIndex->real_data_size, pIndex->data_size);
#endif
	CMakePackLog::GetSingleton().Writef("Write[type:%u] %s\n", pIndex->compressed_type, pIndex->filename);

	return true;
}

long CEterPack::GetFragmentSize()
{
	return m_FragmentSize;
}

// Private methods
bool CEterPack::CreateIndexFile()
{
	FILE * fp;

	fopen_s(&fp, m_indexFileName, "rb");
	if (NULL != fp)
	{
		fclose(fp);
		return true;
	}
	else if (m_bReadOnly)
		return false;

	//
	// The file does not exist, so create a new one.
	//
	fopen_s(&fp, m_indexFileName, "wb");

	if (!fp)
		return false;

	fwrite(&eterpack::c_IndexCC, sizeof(DWORD), 1, fp);
	fwrite(&eterpack::c_Version, sizeof(DWORD), 1, fp);
	fwrite(&m_indexCount, sizeof(m_indexCount), 1, fp);

	fclose(fp);
	return true;
}


void CEterPack::WriteIndex(CFileBase & file, TEterPackIndex * index)
{
	file.Seek(sizeof(DWORD) + sizeof(DWORD));
	file.Write(&m_indexCount, sizeof(m_indexCount));
	file.Seek(eterpack::c_HeaderSize + (index->id * sizeof(TEterPackIndex)));

	if (!file.Write(index, sizeof(TEterPackIndex)))
	{
		assert(!"WriteIndex: fwrite failed");
		return;
	}
}

/*
 * Free Block refers to the erased portion of data. Free Blocks are divided into FREE_INDEX_BLOCK_SIZE (32768) units and managed as a list.  For example, 128k data is 128 * 1024 / FREE_INDEX_BLOCK_SIZE = 4, so it ultimately goes into m_FreeIndexList[4].  The maximum value of FREE_INDEX_BLOCK_SIZE is FREE_INDEX_MAX_SIZE(512). Therefore, data over 16MB is unconditionally placed at position 512 of the array.
 */
int CEterPack::GetFreeBlockIndex(long size)
{
	return min(FREE_INDEX_MAX_SIZE, size / FREE_INDEX_BLOCK_SIZE);
}

void CEterPack::PushFreeIndex(TEterPackIndex* index)
{
	if (index->filename_crc != 0)
	{
		TDataPositionMap::iterator i = m_DataPositionMap.find(index->filename_crc);

		if (i != m_DataPositionMap.end())
			m_DataPositionMap.erase(i);

		index->filename_crc = 0;
		memset(index->filename, 0, sizeof(index->filename));
	}

	int blockidx = GetFreeBlockIndex(index->real_data_size);
	m_FreeIndexList[blockidx].push_back(index);
	m_FragmentSize += index->real_data_size;
	//printf("FreeIndex: size %d: blockidx: %d\n", index->real_data_size, blockidx);
}

long CEterPack::GetNewIndexPosition(CFileBase & file)
{
	long pos = (file.Size() - eterpack::c_HeaderSize) / sizeof(TEterPackIndex);
	++m_indexCount;
	return (pos);
}

TEterPackIndex* CEterPack::NewIndex(CFileBase& file, const char* filename, long size)
{
	TEterPackIndex* index = NULL;
	int block_size = size + (DATA_BLOCK_SIZE - (size % DATA_BLOCK_SIZE));
// if ((index = FindIndex(filename))) // Check if the index already exists
//		return index;

	int blockidx = GetFreeBlockIndex(block_size);

	for (TFreeIndexList::iterator i = m_FreeIndexList[blockidx].begin();
		 i != m_FreeIndexList[blockidx].end();
		 ++i)
	{
		 if ((*i)->real_data_size >= size)
		 {
			 index = *i;
			 m_FreeIndexList[blockidx].erase(i);

			 assert(index->filename_crc == 0);
			 break;
		 }
	}

	if (!index)
	{
		index = new TEterPackIndex;
		index->real_data_size = block_size;
		index->id = GetNewIndexPosition(file);
	}

	strncpy_s(index->filename, sizeof(index->filename), filename, _TRUNCATE);
	index->filename[FILENAME_MAX_LEN] = '\0';
	inlineConvertPackFilename(index->filename);

	index->filename_crc	= GetCRC32(index->filename, strlen(index->filename));

	m_DataPositionMap.insert(TDataPositionMap::value_type(index->filename_crc, index));
	return index;
}

TEterPackIndex* CEterPack::FindIndex(const char * filename)
{
	static char tmpFilename[MAX_PATH + 1];
	strncpy_s(tmpFilename, sizeof(tmpFilename), filename, _TRUNCATE);
	inlineConvertPackFilename(tmpFilename);

	DWORD filename_crc = GetCRC32(tmpFilename, strlen(tmpFilename));
	TDataPositionMap::iterator i = m_DataPositionMap.find(filename_crc);

	if (i == m_DataPositionMap.end())
		return NULL;

	return (i->second);
}

bool CEterPack::IsExist(const char * filename)
{
	return FindIndex(filename) ? true : false;
}

bool CEterPack::CreateDataFile()
{
	FILE * fp;

	fopen_s(&fp, m_stDataFileName.c_str(), "rb");
	if (NULL != fp)
	{
		fclose(fp);
		return true;
	}
	else if (m_bReadOnly)
		return false;

	fopen_s(&fp, m_stDataFileName.c_str(), "wb");

	if (!fp)
		return false;

	fclose(fp);
	return true;
}

long CEterPack::GetNewDataPosition(CFileBase& file)
{
	return file.Size();
}

bool CEterPack::ReadData(CFileBase & file, TEterPackIndex* index, LPVOID data, long maxsize)
{
	if (index->data_size > maxsize)
		return false;

	file.Seek(index->data_position);
	file.Read(data, index->data_size);
	return true;
}

bool CEterPack::WriteData(CFileBase & file, TEterPackIndex* index, LPCVOID data)
{
	file.Seek(index->data_position);
	
	if (!file.Write(data, index->data_size))
	{
		assert(!"WriteData: fwrite data failed");
		return false;
	}

	return true;
}

bool CEterPack::WriteNewData(CFileBase& file, TEterPackIndex* index, LPCVOID data)
{
	file.Seek(index->data_position);
	
	if (!file.Write(data, index->data_size))
	{
		assert(!"WriteData: fwrite data failed");
		return false;
	}

	int empty_size = index->real_data_size - index->data_size;

	if (empty_size < 0)
	{
		printf("SYSERR: WriteNewData(): CRITICAL ERROR: empty_size lower than 0!\n");
		exit(1);
	}

	if (empty_size == 0)
		return true;

	char * empty_buf = (char *) calloc(empty_size, sizeof(char));

	if (!file.Write(empty_buf, empty_size))
	{
		assert(!"WriteData: fwrite empty data failed");
		return false;
	}

	free(empty_buf);
	return true;
}

TDataPositionMap & CEterPack::GetIndexMap()
{
	return m_DataPositionMap;
}

DWORD CEterPack::DeleteUnreferencedData()
{
	TDataPositionMap::iterator i = m_DataPositionMap.begin();
	DWORD dwCount = 0;

	while (i != m_DataPositionMap.end())
	{
		TEterPackIndex * pIndex = (i++)->second;

		if (0 == m_map_indexRefCount[pIndex->id])
		{
			printf("Unref File %s\n", pIndex->filename);
			Delete(pIndex);
			++dwCount;
		}
	}

	return dwCount;
}

const char * CEterPack::GetDBName()
{
	return m_dbName;
}

void CEterPack::__CreateFileNameKey_Panama(const char * filename, BYTE * key, unsigned int keySize)
{
	// key encryption
	if (keySize != 32)
		return;

	std::string SrcStringForKey(filename);
	unsigned int idx = GetCRC32(SrcStringForKey.c_str(), SrcStringForKey.length()) & 3;

	CryptoPP::HashTransformation* hm1 = NULL;
	CryptoPP::HashTransformation* hm2 = NULL;

	static CryptoPP::Tiger tiger;
	static CryptoPP::SHA1 sha1;
	static CryptoPP::RIPEMD128 ripemd128;
	static CryptoPP::Whirlpool whirlpool;

	switch (idx & 3)
	{
		case 0:
			hm1 = &whirlpool;
			break;

		case 1:
			hm1 = &tiger;
			break;

		case 2:
			hm1 = &sha1;
			break;

		case 3:
			hm1 = &ripemd128;
			break;
	}

	CryptoPP::StringSource(SrcStringForKey, true,
					 new CryptoPP::HashFilter(*hm1, 
						//new CryptoPP::HexEncoder(
							new CryptoPP::ArraySink(key, 16)
						//) // HexEncoder
					 ) // HashFilter
				 ); // StringSource

	// Select the next 16-byte key generation algorithm based on the first 4 bytes of the created key.
	unsigned int idx2 = *(unsigned int*) key;

	switch (idx2 & 3)
	{
		case 0:
			hm2 = &sha1;
			break;

		case 1:
			hm2 = &ripemd128;
			break;

		case 2:
			hm2 = &whirlpool;
			break;

		case 3:
			hm2 = &tiger;
			break;
	}

	CryptoPP::StringSource(SrcStringForKey, true,
					 new CryptoPP::HashFilter(*hm2, 
						//new CryptoPP::HexEncoder(
							new CryptoPP::ArraySink(key + 16, 16)
						//) // HexEncoder
					 ) // HashFilter
				 ); // StringSource
	// Key creation complete
}

bool CEterPack::__Encrypt_Panama(const char* filename, const BYTE* data, SIZE_T dataSize, CLZObject& zObj)
{
	if (32 != m_stIV_Panama.length())
	{
		// This message is output only in debug in case a hacker sees this message and gets a hint.
#ifdef _DEBUG
		TraceError("IV not set (filename: %s)", filename);
#endif
		return false;
	}

	CryptoPP::PanamaCipher<CryptoPP::LittleEndian>::Encryption Encryptor;

	if (dataSize < Encryptor.MandatoryBlockSize())
	{
#ifdef _DEBUG
		TraceError("Type 3 pack file must be bigger than %u bytes (filename: %s)", Encryptor.MandatoryBlockSize(), filename);
#endif
		return false;
	}

	BYTE key[32];

	__CreateFileNameKey_Panama(filename, key, sizeof(key));
	Encryptor.SetKeyWithIV(key, sizeof(key), (const BYTE*) m_stIV_Panama.c_str(), 32);

	// Divide it into MandatoryBlockSize and only have a maximum of 2048 bytes.
	DWORD cryptSize = static_cast<DWORD>(dataSize - (dataSize % Encryptor.MandatoryBlockSize()));
	cryptSize = cryptSize > 2048 ? 2048 : cryptSize;

	std::string tmp;

	tmp.reserve(cryptSize);

	CryptoPP::ArraySource(data, cryptSize, true,
		new CryptoPP::StreamTransformationFilter(Encryptor,
			new CryptoPP::StringSink(tmp)
		)
	);

	if (tmp.length() != cryptSize)
	{
#ifdef _DEBUG
		TraceError("Type 3 pack crypt buffer size error (out %u should be %u)", tmp.length(), cryptSize);
#endif
		return false;
	}

	zObj.AllocBuffer(static_cast<DWORD>(dataSize));
	memcpy(zObj.GetBuffer(), tmp.c_str(), cryptSize);

	if (dataSize - cryptSize > 0)
		memcpy(zObj.GetBuffer() + cryptSize, data + cryptSize, dataSize - cryptSize);

	return true;
}

bool CEterPack::__Decrypt_Panama(const char* filename, const BYTE* data, SIZE_T dataSize, CLZObject& zObj)
{
	if (32 != m_stIV_Panama.length())
	{
		// This message is output only in debug in case a hacker sees this message and gets a hint.
#ifdef _DEBUG
		TraceError("IV not set (filename: %s)", filename);
#endif
		return false;
	}

	CryptoPP::PanamaCipher<CryptoPP::LittleEndian>::Decryption Decryptor;

	BYTE key[32];

	__CreateFileNameKey_Panama(filename, key, sizeof(key));
	Decryptor.SetKeyWithIV(key, sizeof(key), (const BYTE*) m_stIV_Panama.c_str(), 32);

	// Divide it into MandatoryBlockSize and only have a maximum of 2048 bytes.
	DWORD cryptSize = static_cast<DWORD>(dataSize - (dataSize % Decryptor.MandatoryBlockSize()));
	cryptSize = cryptSize > 2048 ? 2048 : cryptSize;

	std::string tmp;

	tmp.reserve(cryptSize);

	CryptoPP::ArraySource(data, cryptSize, true,
		new CryptoPP::StreamTransformationFilter(Decryptor,
			new CryptoPP::StringSink(tmp)
		)
	);

	if (tmp.length() != cryptSize)
	{
#ifdef _DEBUG
		TraceError("Type 3 pack crypt buffer size error (out %u should be %u)", tmp.length(), cryptSize);
#endif
		return false;
	}

	zObj.AllocBuffer(static_cast<DWORD>(dataSize));
	memcpy(zObj.GetBuffer(), tmp.c_str(), cryptSize);

	if (dataSize - cryptSize > 0)
		memcpy(zObj.GetBuffer() + cryptSize, data + cryptSize, dataSize - cryptSize);

	return true;
}

EterPackPolicy_CSHybridCrypt* CEterPack::GetPackPolicy_HybridCrypt() const
{
	return  m_pCSHybridCryptPolicy;
}


/////////////////////////

void CEterFileDict::InsertItem(CEterPack* pkPack, TEterPackIndex* pkInfo)
{
	Item item;

	item.pkPack = pkPack;
	item.pkInfo = pkInfo;

	m_dict.insert(TDict::value_type(pkInfo->filename_crc, item));
}

void CEterFileDict::UpdateItem(CEterPack* pkPack, TEterPackIndex* pkInfo)
{
	Item item;

	item.pkPack = pkPack;
	item.pkInfo = pkInfo;

	TDict::iterator f = m_dict.find(pkInfo->filename_crc);
	if (f == m_dict.end())	
		m_dict.insert(TDict::value_type(pkInfo->filename_crc, item));
	else
	{
		if (strcmp(f->second.pkInfo->filename, item.pkInfo->filename) == 0)
		{
			f->second = item;
		}
		else
		{
			TraceError("NAME_COLLISION: OLD: %s NEW: %s", f->second.pkInfo->filename, item.pkInfo->filename);			
		}
		
	}
}

CEterFileDict::Item* CEterFileDict::GetItem(DWORD dwFileNameHash, const char * c_pszFileName)
{
	std::pair<TDict::iterator, TDict::iterator> iter_pair = m_dict.equal_range(dwFileNameHash);

	TDict::iterator iter = iter_pair.first;

	while (iter != iter_pair.second)
	{
		Item& item = iter->second;

		if (0 == strcmp(c_pszFileName, item.pkInfo->filename))
			return &item;

		++iter;
	}

	return NULL;
}
