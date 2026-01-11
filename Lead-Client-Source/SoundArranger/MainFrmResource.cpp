#include "StdAfx.h"
#include "MainFrm.h"

CResource * NewImage(const char* c_szFileName)
{
	return new CGraphicImage(c_szFileName);
}

CResource * NewSubImage(const char* c_szFileName)
{
	return new CGraphicSubImage(c_szFileName);
}

CResource * NewText(const char* c_szFileName)
{
	return new CGraphicText(c_szFileName);
}

CResource * NewThing(const char* c_szFileName)
{
	return new CGraphicThing(c_szFileName);
}

CResource * NewEffectMesh(const char* c_szFileName)
{
	return new CEffectMesh(c_szFileName);
}

void CMainFrame::RegisterResource()
{
	m_resourceManager.RegisterResourceNewFunctionPointer("sub", NewSubImage);
	m_resourceManager.RegisterResourceNewFunctionPointer("dds", NewImage);
	m_resourceManager.RegisterResourceNewFunctionPointer("jpg", NewImage);
	m_resourceManager.RegisterResourceNewFunctionPointer("tga", NewImage);
	m_resourceManager.RegisterResourceNewFunctionPointer("bmp", NewImage);
	m_resourceManager.RegisterResourceNewFunctionPointer("fnt", NewText);
	m_resourceManager.RegisterResourceNewFunctionPointer("gr2", NewThing);
	m_resourceManager.RegisterResourceNewFunctionPointer("mde", NewEffectMesh);
}