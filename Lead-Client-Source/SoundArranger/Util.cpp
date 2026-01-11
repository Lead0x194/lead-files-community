#include "StdAfx.h"
#include "Util.h"

void CreateHighColorImageList(int iBitmapID, CImageList * pImageList)
{
	CBitmap Bitmap;

	HINSTANCE hInstance = ::AfxFindResourceHandle(MAKEINTRESOURCE(iBitmapID), RT_BITMAP);
	HANDLE hImage = ::LoadImage(hInstance, MAKEINTRESOURCE(iBitmapID), IMAGE_BITMAP, 0, 0, (LR_DEFAULTSIZE | LR_CREATEDIBSECTION));
	Bitmap.Attach(hImage);

	// Replace
	BITMAP bmInfo;
	const RGBTRIPLE	kBackgroundColor = {192, 192, 192};

	VERIFY(Bitmap.GetBitmap(&bmInfo));

	const UINT numPixels (bmInfo.bmHeight * bmInfo.bmWidth);
    DIBSECTION ds;

    VERIFY (Bitmap.GetObject (sizeof (DIBSECTION), &ds) == sizeof (DIBSECTION));

	RGBTRIPLE * pixels = reinterpret_cast<RGBTRIPLE*>(ds.dsBm.bmBits);
	VERIFY (pixels != NULL);

	const COLORREF buttonColor (::GetSysColor(COLOR_BTNFACE));
	const RGBTRIPLE userBackgroundColor = { GetBValue (buttonColor), GetGValue (buttonColor), GetRValue (buttonColor) };

	for (UINT i = 0; i < numPixels; ++i)
	{
		if (pixels [i].rgbtBlue == kBackgroundColor.rgbtBlue &&
			pixels [i].rgbtGreen == kBackgroundColor.rgbtGreen &&
			pixels [i].rgbtRed == kBackgroundColor.rgbtRed)
		{
			pixels [i] = userBackgroundColor;
		}
	}

	// Create
	pImageList->Create(16, 16, ILC_COLOR24, 5, 0);
	pImageList->Add(&Bitmap, RGB(0, 0, 0));
}

void SetDialogFloatText(HWND hWnd, int iID, float fData)
{
	char szString[64 + 1];
	_snprintf(szString, 64, "%.3f", fData);
	SetDlgItemText(hWnd, iID, szString);
}
