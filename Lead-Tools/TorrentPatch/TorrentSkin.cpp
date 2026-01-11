#include "Metin2TorrentConsole.h"
#include "TorrentSkin.h"

#include <wx-2.8.10/mstream.h>
#include <wx-2.8.10/image.h>

#include "CMiniUnZIP.h"
#include "CConfig.h"
#include "StringFromFile.h"
#include "Helper.h"

#include "SkinButton.h"
#include "SkinGauge.h"

EL_Skin* g_pcSkin = NULL;

MA_BOOL SkinInitialize( MA_LPCTSTR SkinName, Earth::EA_CStatus* pcStatus )
{
	if( g_pcSkin )
		SkinDestroy();

	g_pcSkin = new EL_Skin();
	if( false == g_pcSkin->Load( SkinName, pcStatus ) ) {
		SkinDestroy();
		return MA_FALSE;
	}
	return MA_TRUE;
}

void SkinDestroy()
{
	EL_SAFE_DELETE(g_pcSkin);
}

EL_Skin::EL_Skin()
{
	MA_StringCopy( m_tszSkinDirectory, MA_ARRAYCOUNT(m_tszSkinDirectory), _T("LauncherSkin/") );
}

EL_Skin::~EL_Skin()
{
}

bool LoadSkinConfig( MA_LPCTSTR tszSkinName, EL_SkinConfig* OutConfig, Earth::EA_CStatus* pcStatus )
{
	if( OutConfig == NULL )
		return false;

	std::wstring basePath = tszSkinName;
	basePath.append(EL_LTEXT("/"));

	const EL_WCHAR_P APP_SKIN_CONFIG_PATH = L"skin.xml";

	std::string configData;

#ifdef _SFX_ENABLE
	CMiniUnZIP	cZIP;

	TCHAR		tszSelfFileName[MAX_PATH];
	::GetModuleFileName( NULL, tszSelfFileName, MAX_PATH );
	if( cZIP.OpenArchive( tszSelfFileName ) )
	{
		DWORD dwFileIndex = cZIP.GetFileIndex(APP_SKIN_CONFIG_PATH);
		if (dwFileIndex != CMiniUnZIP::FILE_NOT_FOUND)
		{
			DWORD dwFileSize = cZIP.GetFileLength(dwFileIndex);
			configData.resize(dwFileSize);
			cZIP.ReadFile(dwFileIndex, (&configData[0]) );
		}

		basePath.clear();
	}
#endif
	if (configData.empty())
		EL_StringFromDiskFileW(configData, (basePath + APP_SKIN_CONFIG_PATH).c_str(), NULL);

	if (configData.empty()) {
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("ERROR_NO_SKIN") );
		return false;
	}

	CConfig cPatchCfg;

	cPatchCfg.Initialize("patch.cfg");
	TiXmlDocument cXMLDocument;
	cXMLDocument.Parse(configData.c_str());

	if( cXMLDocument.Error() ) {
		if( pcStatus )
			pcStatus->AddNR( Earth::EA_STATUS_ERROR, EL_LTEXT("XML_PARSING_ERROR") );
		return false;
	}

	TiXmlElement* skinElement = cXMLDocument.FirstChildElement();

	const char* background = skinElement->Attribute("background_image");

	OutConfig->background = basePath + EL_ToWString(background) + _T(".png");

	TiXmlElement* control = skinElement->FirstChildElement("control");
	while (control)
	{
		const char* type		= control->Attribute("type");
		const char* id			= control->Attribute("id");
		const char* image		= control->Attribute("image");
		const char* text		= control->Attribute("text");
		const char* link_url	= control->Attribute("link_url");
		const char* exec_path	= control->Attribute("exec_path");
		const char* exec_args	= control->Attribute("exec_args");
		const char* sec_key		= control->Attribute("sec_key");
		const char* color		= control->Attribute("color");
		const char* bgColor		= control->Attribute("bg_color");
		const char* enable_on_complete = control->Attribute("enable_on_complete");

		const char* fontFace	= control->Attribute("font");
		const char* fontSize	= control->Attribute("fontsize");
		const char* fontItalic	= control->Attribute("italic");
		const char* fontBold	= control->Attribute("bold");

		int x = 0, y = 0, sx = 0, sy = 0;

		control->Attribute("x", &x);
		control->Attribute("y", &y);
		control->Attribute("width", &sx);
		control->Attribute("height", &sy);

		EL_SkinControlInfo info;
		info.color = 0;
		info.bgColor = 0;
		info.identity = EL_ToWString(id);
		info.attribute = 0;

		if (image)
			info.imageName = basePath + EL_ToWString(image);

		if (text)
			info.text = CLocaleManager::sGetActive()->Get( EL_ToWString(text).c_str() );

		if (link_url) {
			char* szAlternateLinkURL = cPatchCfg.GetString(id);

			if(szAlternateLinkURL)
				info.linkURL = EL_ToWString(szAlternateLinkURL);
			else
				info.linkURL = CLocaleManager::sGetActive()->Get( EL_ToWString(link_url).c_str() );
		}

		if (exec_path)
			info.execPath = EL_ToWString(exec_path);

		if (exec_args)
			info.execArgs = EL_ToWString(exec_args);

		if (sec_key)
			info.secKey	= sec_key;

		if (fontFace || fontSize || fontItalic || fontBold) {
			wxFont font;
			if (fontFace)
				font.SetFaceName( EL_ToWString(fontFace).c_str() );
			if (fontSize)
				font.SetPointSize( atoi(fontSize) );
			if (fontItalic && (strcmp(fontItalic, "false") != 0))
				font.SetStyle(wxFONTSTYLE_ITALIC);
			if (fontBold && (strcmp(fontBold, "false") != 0))
				font.SetWeight(wxFONTWEIGHT_BOLD);
			info.font = font;
		}

		if (color) {
			DWORD c = strtoul(color+1, NULL, 16);
			info.color = ((c >> 16) & 0xFF) | (c & 0xFF00) | ((c << 16) & 0xFF0000);
		}

		if (bgColor) {
			DWORD c = strtoul(bgColor+1, NULL, 16);
			info.bgColor = ((c >> 16) & 0xFF) | (c & 0xFF00) | ((c << 16) & 0xFF0000);
		}

		if (enable_on_complete) {
			if (_stricmp(enable_on_complete, "false") && _stricmp(enable_on_complete, "0"))
				info.attribute |= SKINCONTROL_ATTRIBUTE_ENABLEONCOMPLETE;
		}

		info.type = EL_ESkinControl_Max;
		if (0 == _stricmp(type, "text"))
			info.type = EL_ESkinControl_Text;
		else if (0 == _stricmp(type, "button"))
			info.type = EL_ESkinControl_Button;
		else if (0 == _stricmp(type, "gauge"))
			info.type = EL_ESkinControl_Gauge;
		else if (0 == _stricmp(type, "iframe"))
			info.type = EL_ESkinControl_IFrame;

		if (EL_ESkinControl_Max != info.type)
		{
			info.pos.cx = x;
			info.pos.cy = y;
			info.size.cx = sx;
			info.size.cy = sy;

			OutConfig->controls.push_back(info);
		}

		control = control->NextSiblingElement();
	}

	return true;
}

//void EL_Skin::ApplyButtonSkin(EL_SkinButton* target, const EL_ControlSkin& skin)
//{
//	if (skin.label.IsOk()) {
//		const wxImage& image = skin.label.ConvertToImage();
//		target->SetNormalImage(image);
//		target->SetSize(image.GetWidth(), image.GetHeight());
//	}
//	//if (skin.focused.IsOk())
//	//	target->SetBitmapFocus(wxImage(skin.focused.ConvertToImage()));
//	if (skin.selected.IsOk())
//		target->SetPressedImage(wxImage(skin.selected.ConvertToImage()));
//	if (skin.disabled.IsOk())
//		target->SetDisabledImage(wxImage(skin.disabled.ConvertToImage()));
//	if (skin.hover.IsOk())
//		target->SetOverImage(wxImage(skin.hover.ConvertToImage()));
//}

bool EL_Skin::Load(MA_LPCTSTR tszSkinName, Earth::EA_CStatus* pcStatus)
{
	EL_SkinConfig cSkinConfig;

	if( false == LoadSkinConfig( tszSkinName, &cSkinConfig, pcStatus ) )
		return false;
	m_config = cSkinConfig;
	m_bmpMain = LoadBitmapFile( cSkinConfig.background.c_str() );
	return true;
}

MA_BOOL EL_Skin::ApplySkinWindow( EL_SkinWindow* pcSkinWindow, const EL_SkinControlInfo& stControlInfo )
{
	const std::wstring basePath = L"";
	const std::wstring ext = _T(".png");

	switch (stControlInfo.type)
	{
	case EL_ESkinControl_Button:
		pcSkinWindow->SetNormalImage( LoadImageFile( (basePath + stControlInfo.imageName + _T("_up") + ext).c_str() ) );
		pcSkinWindow->SetOverImage( LoadImageFile( (basePath + stControlInfo.imageName + _T("_on") + ext).c_str() ) );
		pcSkinWindow->SetDisabledImage( LoadImageFile( (basePath + stControlInfo.imageName + _T("_ds") + ext).c_str() ) );
		((EL_SkinButton*) pcSkinWindow)->SetPressedImage( LoadImageFile( (basePath + stControlInfo.imageName + _T("_dn") + ext).c_str() ) );
		{
			wxImage& cImage = pcSkinWindow->GetNormalImage();
			if(cImage.GetWidth() && cImage.GetHeight() )
					pcSkinWindow->SetSize( cImage.GetWidth(), cImage.GetHeight() );
		}
		break;
	case EL_ESkinControl_Gauge:
		((EL_SkinGauge*) pcSkinWindow)->SetGaugeImage( LoadImageFile( (basePath + stControlInfo.imageName + ext).c_str() ) );
		{
			wxImage& cImage = ((EL_SkinGauge*) pcSkinWindow)->GetGaugeImage();
			if(cImage.GetWidth() && cImage.GetHeight() )
					pcSkinWindow->SetSize( cImage.GetWidth(), cImage.GetHeight() );
		}
		break;
	case EL_ESkinControl_Text :
	case EL_ESkinControl_IFrame :
		break;
	default:
		return false;
	}
	
	return true;
}

wxBitmap EL_Skin::LoadBitmapFile(MA_LPCTSTR tszPath)
{
	std::vector<BYTE> buffer;

	bool bLoaded = false;

#ifdef _SFX_ENABLE
	CMiniUnZIP	cZIP;

	TCHAR		tszSelfFileName[MAX_PATH];
	::GetModuleFileName( NULL, tszSelfFileName, MAX_PATH );
	if( cZIP.OpenArchive( tszSelfFileName ) ) {
		DWORD dwFileIndex = cZIP.GetFileIndex( tszPath );
		if( dwFileIndex != CMiniUnZIP::FILE_NOT_FOUND )
		{
			DWORD dwFileSize = cZIP.GetFileLength( dwFileIndex );
			buffer.resize( dwFileSize );
			cZIP.ReadFile( dwFileIndex, (&buffer[0]) );
			bLoaded	= true;
		}
		cZIP.CloseArchive();
	}
#endif

	if (false == bLoaded) {
		FILE* fp;

		if (_wfopen_s(&fp, tszPath, _T("rb")))
			return wxNullImage;
		fseek(fp, 0, SEEK_END);
		const long fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buffer.resize(fileSize);
		fread(&(*buffer.begin()), fileSize, 1, fp);
		fclose(fp);
	}

	wxMemoryInputStream stream(&*buffer.begin(), buffer.size());
	wxImage image(stream, wxBITMAP_TYPE_ANY);
	//if(image.HasAlpha()) {
	//	image.ConvertAlphaToMask();
	//}
	if(image.IsOk() == false)
		return wxNullImage;

	return wxBitmap( image );
}

wxImage EL_Skin::LoadImageFile(MA_LPCTSTR tszPath)
{
	std::vector<BYTE> buffer;

	bool bLoaded = false;

#ifdef _SFX_ENABLE
	CMiniUnZIP	cZIP;

	TCHAR		tszSelfFileName[MAX_PATH];
	::GetModuleFileName( NULL, tszSelfFileName, MAX_PATH );
	if( cZIP.OpenArchive( tszSelfFileName ) ) {
		DWORD dwFileIndex = cZIP.GetFileIndex( tszPath );
		if( dwFileIndex != CMiniUnZIP::FILE_NOT_FOUND )
		{
			DWORD dwFileSize = cZIP.GetFileLength( dwFileIndex );
			buffer.resize( dwFileSize );
			cZIP.ReadFile( dwFileIndex, (&buffer[0]) );
			bLoaded	= true;
		}
		cZIP.CloseArchive();
	}
#endif

	if (false == bLoaded) {
		FILE* fp;

		if (_wfopen_s(&fp, tszPath, _T("rb")))
			return wxNullImage;
		fseek(fp, 0, SEEK_END);
		const long fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buffer.resize(fileSize);
		fread(&(*buffer.begin()), fileSize, 1, fp);
		fclose(fp);
	}

	wxMemoryInputStream stream(&*buffer.begin(), buffer.size());
	wxImage image(stream, wxBITMAP_TYPE_ANY);
	//if(image.HasAlpha()) {
	//	image.ConvertAlphaToMask();
	//}
	if(image.IsOk() == false)
		return wxNullImage;
	return image;
}
