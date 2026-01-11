#pragma once

class EL_SkinWindow;

enum EL_ESkinControl
{
	EL_ESkinControl_Text,
	EL_ESkinControl_Button,
	EL_ESkinControl_Gauge,
	EL_ESkinControl_IFrame,
	EL_ESkinControl_Max
};

#define SKINCONTROL_ATTRIBUTE_ENABLEONCOMPLETE		(1 << 0)

struct EL_SkinControlInfo
{
	EL_ESkinControl	type;
	std::wstring	identity;
	std::wstring	imageName;
	std::wstring	text;
	std::wstring	linkURL;
	std::wstring	execPath;
	std::wstring	execArgs;
	std::string		secKey;
	SIZE			pos;
	SIZE			size;
	DWORD			color;
	DWORD			bgColor;
	DWORD			attribute;
	wxFont			font;
};

struct EL_SkinConfig
{
	std::wstring background;
	std::vector<EL_SkinControlInfo> controls;
};

class EL_Skin
{
public:
	EL_Skin();
	virtual ~EL_Skin();

	bool Load( MA_LPCTSTR tszSkinName, Earth::EA_CStatus* pcStatus );

	wxBitmap& GetMainBitmap() { return m_bmpMain; }

	EL_SIZE GetControls() const { return m_config.controls.size(); }
	EL_SkinControlInfo GetControlInfo(const EL_SIZE index) const { return m_config.controls.at(index); }

	MA_BOOL	ApplySkinWindow( EL_SkinWindow* target, const EL_SkinControlInfo& stControlInfo );

protected:
	wxImage		LoadImageFile(MA_LPCTSTR tszPath);
	wxBitmap	LoadBitmapFile(MA_LPCTSTR tszPath);
	//bool _LoadControlSkin(const EL_ESkinControl type, EL_C_WCHAR_P path, EL_ControlSkin* outControlSkin);

protected:
	wxBitmap				m_bmpMain;
	MA_TCHAR				m_tszSkinDirectory[MA_MAX_PATH];
	EL_SkinConfig			m_config;
};

extern EL_Skin* g_pcSkin;

MA_BOOL SkinInitialize( MA_LPCTSTR SkinName, Earth::EA_CStatus* pcStatus );
void SkinDestroy();
