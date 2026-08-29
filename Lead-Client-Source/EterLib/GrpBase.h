#pragma once

#include "GrpDetector.h"
#include "Ray.h"
#include <vector>

static const UINT SMALL_PDT_VERTEX_BUFFER_SIZE = 16;
static const UINT LARGE_PDT_VERTEX_BUFFER_SIZE = 768;

void PixelPositionToD3DXVECTOR3(const D3DXVECTOR3& c_rkPPosSrc, D3DXVECTOR3* pv3Dst);
void D3DXVECTOR3ToPixelPosition(const D3DXVECTOR3& c_rv3Src, D3DXVECTOR3* pv3Dst);

class CGraphicTexture;

typedef WORD TIndex;

typedef struct SFace
{
	TIndex indices[3];
} TFace;

typedef D3DXVECTOR3 TPosition;

typedef D3DXVECTOR3 TNormal;

typedef D3DXVECTOR2 TTextureCoordinate;

typedef DWORD TDiffuse;
typedef DWORD TAmbient;
typedef DWORD TSpecular;

typedef union UDepth
{
	float	f;
	long	l;
	DWORD	dw;
} TDepth;

typedef struct SVertex
{
	float x, y, z;
	DWORD color;
	float u, v;
} TVertex;

struct STVertex
{
	float x, y, z, rhw;
};

struct SPVertex
{
	float x, y, z;
};

typedef struct SPDVertex
{
	float x, y, z;
	DWORD color;
} TPDVertex;

struct SPDTVertexRaw
{
	float px, py, pz;
	DWORD diffuse;
	float u, v;
};

typedef struct SPTVertex
{
	TPosition position;
	TTextureCoordinate texCoord;
} TPTVertex;

typedef struct SPDTVertex
{
	TPosition	position;
	TDiffuse	diffuse;
	TTextureCoordinate texCoord;
} TPDTVertex;

typedef struct SPNTVertex
{
	TPosition			position;
	TNormal				normal;
	TTextureCoordinate	texCoord;
} TPNTVertex;

typedef struct SPNT2Vertex
{
	TPosition	position;
	TNormal		normal;
	TTextureCoordinate texCoord;
	TTextureCoordinate texCoord2;
} TPNT2Vertex;

typedef struct SPDT2Vertex
{	
	TPosition	position;
	DWORD		diffuse;	
	TTextureCoordinate texCoord;
	TTextureCoordinate texCoord2;
} TPDT2Vertex;

typedef struct SNameInfo
{
	DWORD	name;
	TDepth	depth;
} TNameInfo;

typedef struct SBoundBox
{
	float sx, sy, sz;
	float ex, ey, ez;
	int meshIndex;
	int boneIndex;
} TBoundBox;

const WORD c_FillRectIndices[6] = { 0, 2, 1, 2, 3, 1 };

/*
enum EIndexCount
{
	LINE_INDEX_COUNT = 2,
	TRIANGLE_INDEX_COUNT = 2*3,
	RECTANGLE_INDEX_COUNT = 2*4,
	CUBE_INDEX_COUNT = 2*4*3,
	FILLED_TRIANGLE_INDEX_COUNT = 3,
	FILLED_RECTANGLE_INDEX_COUNT = 3*2,
	FILLED_CUBE_INDEX_COUNT = 3*2*6,
};
*/

class CGraphicBase
{
	public:
		static DWORD GetAvailableTextureMemory();
		static const D3DXMATRIX& GetViewMatrix();
		static const D3DXMATRIX & GetIdentityMatrix();

		enum
		{			
			DEFAULT_IB_LINE, 
			DEFAULT_IB_LINE_TRI, 
			DEFAULT_IB_LINE_RECT, 
			DEFAULT_IB_LINE_CUBE, 
			DEFAULT_IB_FILL_TRI,
			DEFAULT_IB_FILL_RECT,
			DEFAULT_IB_FILL_CUBE,
			DEFAULT_IB_NUM,
		};

	public:
		CGraphicBase();
		virtual	~CGraphicBase();

		void		SetSimpleCamera(float x, float y, float z, float pitch, float roll);
		void		SetEyeCamera(float xEye, float yEye, float zEye, float xCenter, float yCenter, float zCenter, float xUp, float yUp, float zUp);
		void		SetAroundCamera(float distance, float pitch, float roll, float lookAtZ = 0.0f);
		void		SetPositionCamera(float fx, float fy, float fz, float fDistance, float fPitch, float fRotation);
		void		MoveCamera(float fdeltax, float fdeltay, float fdeltaz);

		void		GetTargetPosition(float * px, float * py, float * pz);
		void		GetCameraPosition(float * px, float * py, float * pz);
		void		SetOrtho2D(float hres, float vres, float zres);
		void		SetOrtho3D(float hres, float vres, float zmin, float zmax);
		void		SetPerspective(float fov, float aspect, float nearz, float farz);
		float		GetFOV();
		void		GetClipPlane(float * fNearY, float * fFarY)
		{
			*fNearY = ms_fNearY;
			*fFarY = ms_fFarY;
		}

		////////////////////////////////////////////////////////////////////////
		void		PushMatrix();

		void		MultMatrix( const D3DXMATRIX* pMat );
		void		MultMatrixLocal( const D3DXMATRIX* pMat );
	
		void		Translate(float x, float y, float z);
		void		Rotate(float degree, float x, float y, float z);
		void		RotateLocal(float degree, float x, float y, float z);
		void		RotateYawPitchRollLocal(float fYaw, float fPitch, float fRoll);
		void		Scale(float x, float y, float z);
		void		PopMatrix();		
		void		LoadMatrix(const D3DXMATRIX & c_rSrcMatrix);		
		void		GetMatrix(D3DXMATRIX * pRetMatrix) const;
		const		D3DXMATRIX * GetMatrixPointer() const;

		// Special Routine
		void		GetSphereMatrix(D3DXMATRIX * pMatrix, float fValue = 0.1f);

		////////////////////////////////////////////////////////////////////////
		void		InitScreenEffect();
		void		SetScreenEffectWaving(float fDuringTime, int iPower);
		void		SetScreenEffectFlashing(float fDuringTime, const D3DXCOLOR & c_rColor);

		////////////////////////////////////////////////////////////////////////
		DWORD		GetColor(float r, float g, float b, float a = 1.0f);

		DWORD		GetFaceCount();
		void		ResetFaceCount();
		HRESULT		GetLastResult();

		void		UpdateProjMatrix();
		void		UpdateViewMatrix();
		
		void		SetViewport(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, float fMinZ, float fMaxZ);
		static void		GetBackBufferSize(UINT* puWidth, UINT* puHeight);
		static bool		IsTLVertexClipping();
		static bool		IsFastTNL();
		static bool		IsLowTextureMemory();
		static bool		IsHighTextureMemory();

		static void SetDefaultIndexBuffer(UINT eDefIB);
		static bool SetPDTStream(SPDTVertexRaw* pVertices, UINT uVtxCount);
		static bool SetPDTStream(SPDTVertex* pVertices, UINT uVtxCount);

		// Fixed-function replacement shaders (DX12 migration); default off.
		static void SetUseShaderFFP(bool bEnable);
		static bool IsUseShaderFFP();
		static bool BeginPDTShader();		// true = shader pipeline bound; else use the FVF path
		static bool BeginPDTDiffuseShader();	// variant for NULL-texture draws (diffuse passthrough)
		static bool BeginPTTextureShader();	// XYZ|TEX1 draws (texture passthrough)
		static bool BeginPDTTextureShader();	// XYZ|DIFFUSE|TEX1 draws where FFP selects TEXTURE only
		static bool BeginPDTModulateTexAlphaShader();	// rgb = texture*diffuse, alpha = texture
		static bool BeginPDTCloudShader();	// scrolling-UV cloud layer (MODULATEINVALPHA_ADDCOLOR)
		static bool BeginEffectShader(DWORD dwColorOp);	// effect COLOROP(TFACTOR, TEXTURE); false = unsupported op, use FFP
		static bool BeginGrannyMeshShader();	// lit PNT mesh w/ base modulate cascade; false = keep FFP
		static bool BeginTerrainSplatShader(bool bBase);	// terrain patch splat layers; false = keep FFP
		static bool BeginTerrainFogFlatShader();	// beyond-fog terrain patches (TFACTOR fill)
		static bool BeginTerrainAttrShader();	// attr/marked-area overlay; false = keep FFP
		static void EndPDTShader();		// call only after a successful Begin*Shader
		static void DestroyShaderPool();

	private:
		static void __UploadGrannyLightingConstants();
		static void __UploadOmniLightingConstants();
	public:
		
	protected:
		static D3DXMATRIX				ms_matIdentity;

		static D3DXMATRIX				ms_matView;
		static D3DXMATRIX				ms_matProj;
		static D3DXMATRIX				ms_matInverseView;
		static D3DXMATRIX				ms_matInverseViewYAxis;

		static D3DXMATRIX				ms_matWorld;
		static D3DXMATRIX				ms_matWorldView;

	protected:
		//void		UpdatePrePipeLineMatrix();
		void		UpdatePipeLineMatrix();

	protected:
		// Various D3DX Meshes (used to display collusion data, etc.)
		static LPD3DXMESH				ms_lpSphereMesh;
		static LPD3DXMESH				ms_lpCylinderMesh;

	protected:
		static HRESULT					ms_hLastResult;

		static bool						ms_bUseShaderFFP;

		static int						ms_iWidth;
		static int						ms_iHeight;

		static UINT						ms_iD3DAdapterInfo;
		static UINT						ms_iD3DDevInfo;
		static UINT						ms_iD3DModeInfo;		
		static D3D_CDisplayModeAutoDetector				ms_kD3DDetector;

		static HWND						ms_hWnd;
		static HDC						ms_hDC;
		static LPDIRECT3D9EX				ms_lpd3d;
		static LPDIRECT3DDEVICE9EX		ms_lpd3dDevice;
		static ID3DXMatrixStack*		ms_lpd3dMatStack;
		static D3DVIEWPORT9				ms_Viewport;

		static DWORD					ms_faceCount;
		static D3DCAPS9					ms_d3dCaps;
		static D3DPRESENT_PARAMETERS	ms_d3dPresentParameter;
		
		static DWORD					ms_dwD3DBehavior;
		static LPDIRECT3DVERTEXDECLARATION9 ms_ptDecl;
		static LPDIRECT3DVERTEXDECLARATION9 ms_pntDecl;
		static LPDIRECT3DVERTEXDECLARATION9 ms_pnt2Decl;

		static D3DXMATRIX				ms_matScreen0;
		static D3DXMATRIX				ms_matScreen1;
		static D3DXMATRIX				ms_matScreen2;
		//static D3DXMATRIX				ms_matPrePipeLine;

		static D3DXVECTOR3				ms_vtPickRayOrig;
		static D3DXVECTOR3				ms_vtPickRayDir;

		static float					ms_fFieldOfView;
		static float					ms_fAspect;
		static float					ms_fNearY;
		static float					ms_fFarY;

		// 2004.11.18.myevan.Replaced with DynamicVertexBuffer
		/*
		static std::vector<TIndex>		ms_lineIdxVector;
		static std::vector<TIndex>		ms_lineTriIdxVector;
		static std::vector<TIndex>		ms_lineRectIdxVector;
		static std::vector<TIndex>		ms_lineCubeIdxVector;

		static std::vector<TIndex>		ms_fillTriIdxVector;
		static std::vector<TIndex>		ms_fillRectIdxVector;
		static std::vector<TIndex>		ms_fillCubeIdxVector;
		*/

		// Screen Effect - Waving, Flashing and so on..
		static DWORD					ms_dwWavingEndTime;
		static int						ms_iWavingPower;
		static DWORD					ms_dwFlashingEndTime;
		static D3DXCOLOR				ms_FlashingColor;

		// Ray for terrain picking... Version using CCamera... Requires integration with existing Ray...
 		static CRay						ms_Ray;

		// 
		static bool						ms_bSupportDXT;
		static bool						ms_isLowTextureMemory;
		static bool						ms_isHighTextureMemory;

		static LPDIRECT3DVERTEXBUFFER9	ms_smallPdtVertexBuffer;
		static LPDIRECT3DVERTEXBUFFER9	ms_largePdtVertexBuffer;
		static LPDIRECT3DINDEXBUFFER9	ms_alpd3dDefIB[DEFAULT_IB_NUM];
};
