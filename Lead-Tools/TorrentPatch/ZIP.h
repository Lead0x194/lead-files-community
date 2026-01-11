/**
 * @file	ZIP.h
 * @brief	ZIP 파일 구조체들
 * @author	Standil (cpascal@cpascal.net)
 *
 * @version	2003/10/25 cpascal - First release
 *
 * @see		http://www.pkware.com/documents/casestudies/APPNOTE.TXT
 */

#ifndef _ZIP_H_
#define _ZIP_H_

#pragma pack( push, 1 )

typedef struct tagZIP_MAIN_HEADER
{
	enum {
		SIGNATURE = 0x06054B50
	};
	DWORD	dwSignature;			// End of central directory signature (0x06054B50)
	WORD	wDisk;					// Number of this disk
	WORD	wStartDisk;				// Number of the disk with the start of the central directory
	WORD	nDirectoryEntries;		// Total number of entries in the central directory on this disk
	WORD	nTotalDirectoryEntries;	// Total number of entries in the central directory
	DWORD	dwDirectorySize;		// Size of the central directory
	DWORD	dwDirectoryOffset;		// Offset of start of central directory with respect to the starting disk number
	WORD	wCommentLength;			// Length of comment
} ZIP_MAIN_HEADER, *PZIP_MAIN_HEADER, FAR *LPZIP_MAIN_HEADER;

typedef struct tagZIP_LOCAL_FILE_HEADER
{
	enum {
		SIGNATURE = 0x04034B50,
	};
	DWORD	dwSignature;			// Local file header signature (0x04034B50)
	WORD	wVersionMadeBy;			// Version made by
	WORD	wVersionToExtract;		// Version needed to extract
	WORD	wCompression;			// Compression method
	WORD	wLastModificationTime;	// Last Modification File Time
	WORD	wLastModificationDate;	// Last Modification File Date
	DWORD	dwCRC32;				// CRC-32
	DWORD	wCompressedSize;		// Compressed size
	DWORD	wUncompressedSize;		// Uncompressed size
	WORD	wFileNameLength;		// FileName length
	WORD	wExtraLength;			// Extra field length
	char*	GetFileName() const		{ return (char *)(this + 1);   }
	char*	GetExtra() const		{ return GetFileName() + wFileNameLength; }
} ZIP_LOCAL_FILE_HEADER, *PZIP_LOCAL_FILE_HEADER, *LPZIP_LOCAL_FILE_HEADER;

typedef struct tagZIP_INDEX_ENTRY
{
	enum {
		SIGNATURE = 0x02014B50
	};
	DWORD	dwSignature;			// Central file header signature (0x02014B50)
	WORD	wVersionMadeBy;			// Version made by
	WORD	wVersionToExtract;		// Version needed to extract
	WORD	wFlag;					// General purpose bit flag
	WORD	wCompression;			// Compression method
	WORD	wLastModificationTime;	// Last Modification File Time
	WORD	wLastModificationDate;	// Last Modification File Date
	DWORD	dwCRC32;				// CRC-32
	DWORD	wCompressedSize;		// Compressed size
	DWORD	wUncompressedSize;		// Uncompressed size
	WORD	wFileNameLength;		// FileName length
	WORD	wExtraLength;			// Extra field length
	WORD	wCommentLength;			// File comment length
	WORD	wDiskStart;				// Disk number start
	WORD	wInternalAttributes;	// Internal file attributes
	DWORD	wExternalAttributes;	// External file attributes
	DWORD	dwOffsetOfLocalHeader;	// Relative offset of local header
	char*	GetFileName() const		{ return (char *)(this + 1);   }
	char*	GetExtra() const		{ return GetFileName() + wFileNameLength; }
	char*	GetComment() const		{ return GetExtra() + wExtraLength; }
} ZIP_INDEX_ENTRY, *PZIP_INDEX_ENTRY, *LPZIP_INDEX_ENTRY;

#pragma pack( pop )

#endif /* !_ZIP_H_ */
