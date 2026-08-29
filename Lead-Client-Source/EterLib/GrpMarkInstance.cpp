#include "StdAfx.h"
#include "GrpMarkInstance.h"
#include "StateManager.h"
#include "ResourceManager.h"

#include "../eterBase/CRC32.h"

CDynamicPool<CGraphicMarkInstance> CGraphicMarkInstance::ms_kPool;

void CGraphicMarkInstance::SetImageFileName(const char* c_szFileName)
{
	m_stImageFileName = c_szFileName;
}

const std::string& CGraphicMarkInstance::GetImageFileName()
{
	return m_stImageFileName;
}

void CGraphicMarkInstance::CreateSystem(UINT uCapacity)
{
	ms_kPool.Create(uCapacity);
}

void CGraphicMarkInstance::DestroySystem()
{
	ms_kPool.Destroy();
}

CGraphicMarkInstance* CGraphicMarkInstance::New()
{
	return ms_kPool.Alloc();
}

void CGraphicMarkInstance::Delete(CGraphicMarkInstance* pkImgInst)
{
	pkImgInst->Destroy();
	ms_kPool.Free(pkImgInst);
}

void CGraphicMarkInstance::Render()
{
	if (IsEmpty())
		return;

	assert(!IsEmpty());

	OnRender();
}

void CGraphicMarkInstance::OnRender()
{
	CGraphicImage * pImage = m_roImage.GetPointer();
	CGraphicTexture * pTexture = pImage->GetTexturePointer();
	
	UINT uColCount = pImage->GetWidth() / MARK_WIDTH;

	if (uColCount == 0)
		return;

	UINT uCol = m_uIndex % uColCount;
	UINT uRow = m_uIndex / uColCount;

	// The guild-mark cache image is authored bottom-up: CGuildMarkImage writes each mark
	// with DevIL ilSetPixels (lower-left origin), so markID 0 sits in the BOTTOM-most mark
	// row of the texture, not the top. The upgraded DevIL 1.8.0 then saves the .tga
	// top-origin, so a naive top-down row (uRow*MARK_HEIGHT) samples the empty top strip and
	// every guild sign renders blank/white. Flip the sampled row to where the mark is stored.
	RECT kRect;
	kRect.left=uCol*MARK_WIDTH;
	kRect.top=pImage->GetHeight() - MARK_HEIGHT - uRow*MARK_HEIGHT;
	kRect.right=kRect.left+MARK_WIDTH;
	kRect.bottom=kRect.top+MARK_HEIGHT;

	float texReverseWidth = 1.0f / float(pTexture->GetWidth());
	float texReverseHeight = 1.0f / float(pTexture->GetHeight());
	float su = kRect.left * texReverseWidth;
	float sv = kRect.top * texReverseHeight;
	float eu = kRect.right * texReverseWidth;
	float ev = kRect.bottom * texReverseHeight;

	float fRenderWidth = (float)MARK_WIDTH * m_fScale;
	float fRenderHeight = (float)MARK_HEIGHT * m_fScale;

	TPDTVertex vertices[4];
	vertices[0].position.x	= m_v2Position.x-0.5f;
	vertices[0].position.y	= m_v2Position.y-0.5f;
	vertices[0].position.z	= 0.0f;
	vertices[0].texCoord	= TTextureCoordinate(su, sv);
	vertices[0].diffuse		= m_DiffuseColor;

	vertices[1].position.x	= m_v2Position.x + fRenderWidth -0.5f;
	vertices[1].position.y	= m_v2Position.y-0.5f;
	vertices[1].position.z	= 0.0f;
	vertices[1].texCoord	= TTextureCoordinate(eu, sv);
	vertices[1].diffuse		= m_DiffuseColor;

	vertices[2].position.x	= m_v2Position.x-0.5f;
	vertices[2].position.y	= m_v2Position.y + fRenderHeight -0.5f;
	vertices[2].position.z	= 0.0f;
	vertices[2].texCoord	= TTextureCoordinate(su, ev);
	vertices[2].diffuse		= m_DiffuseColor;

	vertices[3].position.x	= m_v2Position.x + fRenderWidth -0.5f;
	vertices[3].position.y	= m_v2Position.y + fRenderHeight -0.5f;
	vertices[3].position.z	= 0.0f;
	vertices[3].texCoord	= TTextureCoordinate(eu, ev);	
	vertices[3].diffuse		= m_DiffuseColor;

	if (CGraphicBase::SetPDTStream(vertices, 4))
	{
		CGraphicBase::SetDefaultIndexBuffer(CGraphicBase::DEFAULT_IB_FILL_RECT);
	
		STATEMANAGER.SetTexture(0, pTexture->GetD3DTexture());
		STATEMANAGER.SetTexture(1, NULL);
		if (CGraphicBase::BeginPDTShader())
		{
			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
			CGraphicBase::EndPDTShader();
		}
		else
		{
			STATEMANAGER.SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);
			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
		}
		//OLD: STATEMANAGER.DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, c_FillRectIndices, D3DFMT_INDEX16, vertices, sizeof(TPDTVertex));
	}
}

const CGraphicTexture & CGraphicMarkInstance::GetTextureReference() const
{
	return m_roImage->GetTextureReference();
}

CGraphicTexture * CGraphicMarkInstance::GetTexturePointer()
{
	return m_roImage->GetTexturePointer();
}

CGraphicImage * CGraphicMarkInstance::GetGraphicImagePointer()
{
	return m_roImage.GetPointer();
}

void CGraphicMarkInstance::SetScale(float fScale)
{
	m_fScale=fScale;
}

void CGraphicMarkInstance::SetIndex(UINT uIndex)
{
	m_uIndex=uIndex;
}

int CGraphicMarkInstance::GetWidth()
{
	if (IsEmpty())
		return 0;
	
	//return m_roImage->GetWidth();
	return 16;
}

int CGraphicMarkInstance::GetHeight()
{
	if (IsEmpty())
		return 0;
	
	//return m_roImage->GetHeight();
	return 12;
}

void CGraphicMarkInstance::SetDiffuseColor(float fr, float fg, float fb, float fa)
{
	m_DiffuseColor.r = fr;
	m_DiffuseColor.g = fg;
	m_DiffuseColor.b = fb;
	m_DiffuseColor.a = fa;
}

void CGraphicMarkInstance::SetPosition(float fx, float fy)
{
	m_v2Position.x = fx;
	m_v2Position.y = fy;
}

void CGraphicMarkInstance::Load()
{
	if (GetImageFileName().empty())
		return;

	CResource * pResource = CResourceManager::Instance().GetResourcePointer(GetImageFileName().c_str());

	if (!pResource)
	{
		TraceError("CGraphicMarkinstance::Load - [%s] NOT EXIST", GetImageFileName().c_str());
		return;
	}

	if (pResource->IsType(CGraphicImage::Type()))
		SetImagePointer(static_cast<CGraphicImage*>(pResource));		
}

void CGraphicMarkInstance::SetImagePointer(CGraphicImage * pImage)
{
	m_roImage.SetPointer(pImage);

	OnSetImagePointer();
}

bool CGraphicMarkInstance::IsEmpty() const
{
	if (!m_roImage.IsNull() && !m_roImage->IsEmpty())
		return false;

	return true;
}

bool CGraphicMarkInstance::operator == (const CGraphicMarkInstance & rhs) const
{
	return (m_roImage.GetPointer() == rhs.m_roImage.GetPointer());
}

DWORD CGraphicMarkInstance::Type()
{
	static DWORD s_dwType = GetCRC32("CGraphicMarkInstance", strlen("CGraphicMarkInstance"));
	return (s_dwType);
}

BOOL CGraphicMarkInstance::IsType(DWORD dwType)
{
	return OnIsType(dwType);
}

BOOL CGraphicMarkInstance::OnIsType(DWORD dwType)
{
	if (CGraphicMarkInstance::Type() == dwType)
		return TRUE;

	return FALSE;
}

void CGraphicMarkInstance::OnSetImagePointer()
{
}

void CGraphicMarkInstance::Initialize()
{
	m_DiffuseColor.r = m_DiffuseColor.g = m_DiffuseColor.b = m_DiffuseColor.a = 1.0f;
	m_v2Position.x = m_v2Position.y = 0.0f;
	m_uIndex = 0;
	m_fScale = 1.0f;
}

void CGraphicMarkInstance::Destroy()
{
	m_roImage.SetPointer(NULL); // The reference count must fall from CRef.
	Initialize();
}

CGraphicMarkInstance::CGraphicMarkInstance()
{
	Initialize();
}

CGraphicMarkInstance::~CGraphicMarkInstance()
{
	Destroy();
}
