#pragma once

#define WIN32_LEAN_AND_MEAN

#define _WIN32_DCOM

#pragma warning(disable:4710)	// not inlined
#pragma warning(disable:4786)	// Turn off anything that goes beyond character 255
#pragma warning(disable:4512)
#pragma warning(disable:4201)

#if _MSC_VER >= 1400
#pragma warning(disable:4201 4512 4238 4239)
#endif

#include <dx9/d3d9.h>
#include <d3dx9math_shim.h>

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

#pragma warning ( disable : 4201 )
#include <mmsystem.h>
#pragma warning ( default : 4201 )
#include <process.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <direct.h>
#include <malloc.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "../eterBase/StdAfx.h"
#include "../eterBase/Debug.h"
#include "../eterLocale/CodePageId.h"

#ifndef VC_EXTRALEAN
#include <winsock.h>
#endif


/*
#include "Pool.h"
#include "Dynamic.h"
#include "Event.h"
#include "FuncObject.h"
#include "ReferenceObject.h"
#include "Ref.h"

#include "Util.h"
#include "TextFileLoader.h"
#include "Parser.h"

#include "Resource.h"
#include "ResourceManager.h"

#include "MSWindow.h"
#include "MSApplication.h"
#include "Mutex.h"
#include "Thread.h"

#include "GrpBase.h"

#include "GrpDib.h"
#include "GrpMath.h"
#include "GrpDevice.h"

#include "CollisionData.h"
#include "GrpCollisionObject.h"
#include "GrpScreen.h"
#include "CullingManager.h"

// Attribute
#include "AttributeData.h"
#include "AttributeInstance.h"

#include "GrpObjectInstance.h"
#include "GrpRatioInstance.h"

#include "GrpD3DXBuffer.h"

#include "GrpTexture.h"
#include "GrpImageTexture.h"
#include "GrpFontTexture.h"

#include "GrpText.h"
#include "GrpImage.h"
#include "GrpSubImage.h"

#include "GrpIndexBuffer.h"
#include "GrpVertexBuffer.h"
#include "GrpVertexBufferStatic.h"
#include "GrpVertexBufferDynamic.h"


#include "GrpShadowTexture.h"
#include "GrpImageInstance.h"
#include "GrpExpandedImageInstance.h"
#include "GrpTextInstance.h"
#include "GrpLightManager.h"

#include "TargaResource.h"

#include "NetDevice.h"
#include "NetAddress.h"
// #include "NetStream.h"
#include "NetPacketHeaderMap.h"

#include "Input.h"
#include "IME.h"

#include "PathStack.h"
//#include "Property.h"

#include "Profiler.h"

#include "StateManager.h"

#include "ColorTransitionHelper.h"
#include "LensFlare.h"
#include "ScreenFilter.h"
#include "EnvironmentMap.h"

#include "lineintersect_utils.h"

#include "Decal.h"
*/
