#include "StdAfx.h"
#include "ImageFileDecoder.h"

#include <wincodec.h>

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

namespace
{
#pragma pack(push, 1)
	struct SDDSPixelFormat
	{
		DWORD dwSize;
		DWORD dwFlags;
		DWORD dwFourCC;
		DWORD dwRGBBitCount;
		DWORD dwRBitMask;
		DWORD dwGBitMask;
		DWORD dwBBitMask;
		DWORD dwABitMask;
	};

	struct SDDSHeader
	{
		DWORD dwSize;
		DWORD dwFlags;
		DWORD dwHeight;
		DWORD dwWidth;
		DWORD dwPitchOrLinearSize;
		DWORD dwDepth;
		DWORD dwMipMapCount;
		DWORD adwReserved1[11];
		SDDSPixelFormat kPixelFormat;
		DWORD dwCaps;
		DWORD dwCaps2;
		DWORD dwCaps3;
		DWORD dwCaps4;
		DWORD dwReserved2;
	};
#pragma pack(pop)

	const DWORD DDS_MAGIC = 0x20534444;			// "DDS "
	const DWORD DDSPF_ALPHAPIXELS = 0x00000001;
	const DWORD DDSPF_ALPHA = 0x00000002;
	const DWORD DDSPF_FOURCC = 0x00000004;
	const DWORD DDSPF_LUMINANCE = 0x00020000;

	// Per-channel mask -> 8-bit expansion, precomputed once per image.
	struct SChannelReader
	{
		DWORD dwMask;
		DWORD dwShift;
		DWORD dwMax;
		BYTE byDefault;

		void Setup(DWORD dwChannelMask, BYTE byDefaultValue)
		{
			dwMask = dwChannelMask;
			dwShift = 0;
			byDefault = byDefaultValue;
			if (dwMask)
			{
				DWORD dwProbe = dwMask;
				while (!(dwProbe & 1))
				{
					dwProbe >>= 1;
					++dwShift;
				}
				dwMax = dwProbe;
			}
			else
			{
				dwMax = 0;
			}
		}

		BYTE Read(DWORD dwPixel) const
		{
			if (!dwMask)
				return byDefault;
			return BYTE(((dwPixel & dwMask) >> dwShift) * 255 / dwMax);
		}
	};

	// Uncompressed DDS (masked RGB/luminance). The DXT formats never reach this
	// decoder - CDXTCImage claims them first - but D3DX also accepted plain
	// uncompressed .dds files, which the client's effect/UI assets use.
	bool DecodeUncompressedDDS(const BYTE* c_pbyBuf, UINT uBufSize, SDecodedImage* pkRetImage)
	{
		if (uBufSize < sizeof(DWORD) + sizeof(SDDSHeader))
			return false;

		const SDDSHeader* pkHeader = reinterpret_cast<const SDDSHeader*>(c_pbyBuf + sizeof(DWORD));
		if (pkHeader->dwSize != sizeof(SDDSHeader) || pkHeader->kPixelFormat.dwSize != sizeof(SDDSPixelFormat))
			return false;
		if (pkHeader->kPixelFormat.dwFlags & DDSPF_FOURCC)
			return false;

		const UINT uWidth = pkHeader->dwWidth;
		const UINT uHeight = pkHeader->dwHeight;
		const UINT uBitCount = pkHeader->kPixelFormat.dwRGBBitCount;
		if (0 == uWidth || 0 == uHeight)
			return false;
		if (uBitCount != 8 && uBitCount != 16 && uBitCount != 24 && uBitCount != 32)
			return false;

		const UINT uBytesPerPixel = uBitCount / 8;
		const UINT uPitch = uWidth * uBytesPerPixel;
		const UINT uDataOffset = sizeof(DWORD) + sizeof(SDDSHeader);
		if (uDataOffset + size_t(uPitch) * uHeight > uBufSize)
			return false;

		DWORD dwRMask = pkHeader->kPixelFormat.dwRBitMask;
		DWORD dwGMask = pkHeader->kPixelFormat.dwGBitMask;
		DWORD dwBMask = pkHeader->kPixelFormat.dwBBitMask;
		const DWORD dwAMask = (pkHeader->kPixelFormat.dwFlags & (DDSPF_ALPHAPIXELS | DDSPF_ALPHA)) ? pkHeader->kPixelFormat.dwABitMask : 0;

		if (pkHeader->kPixelFormat.dwFlags & DDSPF_LUMINANCE)
			dwRMask = dwGMask = dwBMask = pkHeader->kPixelFormat.dwRBitMask;

		SChannelReader kR, kG, kB, kA;
		kR.Setup(dwRMask, 0);
		kG.Setup(dwGMask, 0);
		kB.Setup(dwBMask, 0);
		kA.Setup(dwAMask, 0xFF);

		pkRetImage->uWidth = uWidth;
		pkRetImage->uHeight = uHeight;
		pkRetImage->kPixels.resize(size_t(uWidth) * uHeight * 4);

		const BYTE* pbySrcRow = c_pbyBuf + uDataOffset;
		BYTE* pbyDst = pkRetImage->kPixels.data();

		for (UINT y = 0; y < uHeight; ++y)
		{
			const BYTE* pbySrc = pbySrcRow;
			for (UINT x = 0; x < uWidth; ++x)
			{
				DWORD dwPixel = 0;
				for (UINT b = 0; b < uBytesPerPixel; ++b)
					dwPixel |= DWORD(pbySrc[b]) << (b * 8);
				pbySrc += uBytesPerPixel;

				pbyDst[0] = kB.Read(dwPixel);
				pbyDst[1] = kG.Read(dwPixel);
				pbyDst[2] = kR.Read(dwPixel);
				pbyDst[3] = kA.Read(dwPixel);
				pbyDst += 4;
			}
			pbySrcRow += uPitch;
		}

		return true;
	}

#pragma pack(push, 1)
	struct STGAHeader
	{
		BYTE byIDLength;
		BYTE byColorMapType;
		BYTE byImageType;
		BYTE abyColorMapSpec[5];
		WORD wXOrigin;
		WORD wYOrigin;
		WORD wWidth;
		WORD wHeight;
		BYTE byBitsPerPixel;
		BYTE byDescriptor;
	};
#pragma pack(pop)

	// TGA has no magic number; accept only the shapes the client ships
	// (true-color, optionally RLE, 24/32bpp, no color map).
	bool IsPlausibleTGA(const BYTE* c_pbyBuf, UINT uBufSize)
	{
		if (uBufSize < sizeof(STGAHeader))
			return false;

		const STGAHeader* pkHeader = reinterpret_cast<const STGAHeader*>(c_pbyBuf);
		if (pkHeader->byColorMapType != 0)
			return false;
		if (pkHeader->byImageType != 2 && pkHeader->byImageType != 10)
			return false;
		if (pkHeader->byBitsPerPixel != 24 && pkHeader->byBitsPerPixel != 32)
			return false;
		if (pkHeader->wWidth == 0 || pkHeader->wHeight == 0)
			return false;

		return true;
	}

	bool DecodeTGA(const BYTE* c_pbyBuf, UINT uBufSize, SDecodedImage* pkRetImage)
	{
		const STGAHeader* pkHeader = reinterpret_cast<const STGAHeader*>(c_pbyBuf);

		const UINT uWidth = pkHeader->wWidth;
		const UINT uHeight = pkHeader->wHeight;
		const UINT uSrcPixelSize = pkHeader->byBitsPerPixel / 8;
		const bool bTopDown = (pkHeader->byDescriptor & 0x20) != 0;
		const bool bRLE = (pkHeader->byImageType == 10);

		UINT uPos = UINT(sizeof(STGAHeader)) + pkHeader->byIDLength;
		if (uPos > uBufSize)
			return false;

		pkRetImage->uWidth = uWidth;
		pkRetImage->uHeight = uHeight;
		pkRetImage->kPixels.resize(size_t(uWidth) * uHeight * 4);

		const UINT uPixelCount = uWidth * uHeight;
		UINT uDecoded = 0;
		BYTE abyPixel[4] = { 0, 0, 0, 0xFF };

		// Decode into TGA storage order first (row 0 = bottom unless top-down).
		std::vector<BYTE>& rkOut = pkRetImage->kPixels;
		auto emit = [&](void) {
			// Storage row -> top-down row.
			UINT uRow = uDecoded / uWidth;
			UINT uCol = uDecoded % uWidth;
			if (!bTopDown)
				uRow = uHeight - 1 - uRow;
			BYTE* pbyDst = &rkOut[(size_t(uRow) * uWidth + uCol) * 4];
			pbyDst[0] = abyPixel[0];	// B
			pbyDst[1] = abyPixel[1];	// G
			pbyDst[2] = abyPixel[2];	// R
			pbyDst[3] = abyPixel[3];	// A
			++uDecoded;
		};

		if (!bRLE)
		{
			if (uPos + size_t(uPixelCount) * uSrcPixelSize > uBufSize)
				return false;

			for (UINT i = 0; i < uPixelCount; ++i)
			{
				abyPixel[0] = c_pbyBuf[uPos];
				abyPixel[1] = c_pbyBuf[uPos + 1];
				abyPixel[2] = c_pbyBuf[uPos + 2];
				abyPixel[3] = (uSrcPixelSize == 4) ? c_pbyBuf[uPos + 3] : 0xFF;
				uPos += uSrcPixelSize;
				emit();
			}
		}
		else
		{
			while (uDecoded < uPixelCount)
			{
				if (uPos >= uBufSize)
					return false;

				const BYTE byPacket = c_pbyBuf[uPos++];
				const UINT uRunLength = (byPacket & 0x7F) + 1;

				if (byPacket & 0x80)
				{
					if (uPos + uSrcPixelSize > uBufSize || uDecoded + uRunLength > uPixelCount)
						return false;

					abyPixel[0] = c_pbyBuf[uPos];
					abyPixel[1] = c_pbyBuf[uPos + 1];
					abyPixel[2] = c_pbyBuf[uPos + 2];
					abyPixel[3] = (uSrcPixelSize == 4) ? c_pbyBuf[uPos + 3] : 0xFF;
					uPos += uSrcPixelSize;

					for (UINT i = 0; i < uRunLength; ++i)
						emit();
				}
				else
				{
					if (uPos + size_t(uRunLength) * uSrcPixelSize > uBufSize || uDecoded + uRunLength > uPixelCount)
						return false;

					for (UINT i = 0; i < uRunLength; ++i)
					{
						abyPixel[0] = c_pbyBuf[uPos];
						abyPixel[1] = c_pbyBuf[uPos + 1];
						abyPixel[2] = c_pbyBuf[uPos + 2];
						abyPixel[3] = (uSrcPixelSize == 4) ? c_pbyBuf[uPos + 3] : 0xFF;
						uPos += uSrcPixelSize;
						emit();
					}
				}
			}
		}

		return true;
	}

	bool DecodeWIC(const BYTE* c_pbyBuf, UINT uBufSize, SDecodedImage* pkRetImage)
	{
		// WIC needs COM; tolerate an already-initialized thread with a different model.
		HRESULT hrCoInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		const bool bBalanceCoInit = SUCCEEDED(hrCoInit);

		bool bResult = false;
		IWICImagingFactory* pkFactory = NULL;
		IWICStream* pkStream = NULL;
		IWICBitmapDecoder* pkDecoder = NULL;
		IWICBitmapFrameDecode* pkFrame = NULL;
		IWICBitmapSource* pkConverted = NULL;

		do
		{
			if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
										IID_IWICImagingFactory, (void**)&pkFactory)))
				break;
			if (FAILED(pkFactory->CreateStream(&pkStream)))
				break;
			if (FAILED(pkStream->InitializeFromMemory(const_cast<BYTE*>(c_pbyBuf), uBufSize)))
				break;
			if (FAILED(pkFactory->CreateDecoderFromStream(pkStream, NULL, WICDecodeMetadataCacheOnDemand, &pkDecoder)))
				break;
			if (FAILED(pkDecoder->GetFrame(0, &pkFrame)))
				break;
			if (FAILED(WICConvertBitmapSource(GUID_WICPixelFormat32bppBGRA, pkFrame, &pkConverted)))
				break;

			UINT uWidth = 0, uHeight = 0;
			if (FAILED(pkConverted->GetSize(&uWidth, &uHeight)) || 0 == uWidth || 0 == uHeight)
				break;

			pkRetImage->uWidth = uWidth;
			pkRetImage->uHeight = uHeight;
			pkRetImage->kPixels.resize(size_t(uWidth) * uHeight * 4);

			if (FAILED(pkConverted->CopyPixels(NULL, uWidth * 4, UINT(pkRetImage->kPixels.size()), pkRetImage->kPixels.data())))
				break;

			bResult = true;
		} while (false);

		if (pkConverted)
			pkConverted->Release();
		if (pkFrame)
			pkFrame->Release();
		if (pkDecoder)
			pkDecoder->Release();
		if (pkStream)
			pkStream->Release();
		if (pkFactory)
			pkFactory->Release();
		if (bBalanceCoInit)
			CoUninitialize();

		return bResult;
	}
}

bool DecodeImageFileFromMemory(const void* c_pvBuf, UINT uBufSize, SDecodedImage* pkRetImage)
{
	const BYTE* c_pbyBuf = static_cast<const BYTE*>(c_pvBuf);

	// Known magics first; TGA has none, so it is probed last.
	const bool bDDS = uBufSize >= 4 && *(const DWORD*)c_pbyBuf == DDS_MAGIC;
	const bool bJPG = uBufSize >= 2 && c_pbyBuf[0] == 0xFF && c_pbyBuf[1] == 0xD8;
	const bool bBMP = uBufSize >= 2 && c_pbyBuf[0] == 'B' && c_pbyBuf[1] == 'M';
	const bool bPNG = uBufSize >= 8 && 0 == memcmp(c_pbyBuf, "\x89PNG\r\n\x1a\n", 8);

	if (bDDS)
		return DecodeUncompressedDDS(c_pbyBuf, uBufSize, pkRetImage);

	if (bJPG || bBMP || bPNG)
		return DecodeWIC(c_pbyBuf, uBufSize, pkRetImage);

	if (IsPlausibleTGA(c_pbyBuf, uBufSize))
		return DecodeTGA(c_pbyBuf, uBufSize, pkRetImage);

	return false;
}
