#pragma once
#include<fbxsdk.h>
#include"CObject.h"
#include"CFbxExporter.h"
#pragma comment (lib,"libfbxsdk-md.lib")
#pragma comment (lib,"libxml2-md.lib")
#pragma comment (lib,"zlib-md.lib")

struct AnimDesc
{
public:
	UINT	m_iStartFrame = 0;
	UINT	m_iEndFrame = 0;
	UINT	m_iFrameSpeed = 30;
	UINT	m_iTickPerFrame = 160;

};

struct CFbxModel
{
	std::string		m_csName;
	TMatrix			m_WolrdMat;
	CFbxModel*		m_pParent = nullptr;
	
};

struct CFbxMesh : public CFbxModel
{
	UINT									iNumPolygon = 0;
	std::vector<std::vector<PNCT_VERTEX>>	m_TriangleList;
	std::vector<UINT>						m_TriangleOffSetList;
	std::vector<std::wstring>				m_szTextureFilename;
	std::vector<CFbxMesh*>					m_vChildList;
	std::vector<TMatrix>					m_vMatrixArr;
	bool Release()
	{
		for (auto child : m_vChildList)
		{
			child->Release();
			delete child;
		}
		m_vChildList.clear();
		return true;
	}
};		

class CFbxObj
{
public:
	std::wstring m_csName;
	AnimDesc    m_AnimInfo;

	ComPtr<ID3D11Buffer> m_pVertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> m_pIndexBuffer = nullptr;
	std::vector< PNCT_VERTEX> m_VertexList;
	std::vector< DWORD>       m_IndexList;
	std::vector<std::shared_ptr<CFbxMesh>>		    m_MeshList;

	bool Release()
	{
		m_VertexList.clear();
		m_IndexList.clear();
		m_MeshList.clear();
		return true;
	}
};



class MyFbxImport
{
public:
	AnimDesc    m_AnimInfo;

public:
	FbxManager*								m_FbxManager = nullptr;
	FbxImporter*							m_FbxImporter = nullptr;
	FbxScene*								m_FbxScene = nullptr;
	
	std::vector<FbxNode*>					m_pMeshNodeList;

public:
	


public:
	bool		Load(std::wstring filename, CFbxObj* fbxobj);
	bool		PreProcess(FbxNode* pNode, CFbxModel* pParent);
	bool		LoadMesh(FbxNode* pNode,CFbxMesh& mesh);
	FbxColor	ReadVertexColor(FbxLayerElementVertexColor* LayerCol, int VertexIndex, int uvIndex);
	FbxVector4  ReadVertexNormal(FbxLayerElementNormal* LayerNor, int VertexIndex, int uvIndex);
	FbxVector2  ReadTextureCoordinate(FbxLayerElementUV* LayerUV, int VertexIndex, int uvIndex);
	int			GetSubMaterialIndex(FbxLayerElementMaterial* LayerMtrl, int Polygon);
	std::string ParseMatarial(FbxSurfaceMaterial* pmtrl);
	TMatrix	    ParseTransform(FbxNode* pNode);
	TMatrix     ConvertAMatrix(FbxAMatrix& fbxamat);
	TMatrix		ConvertMatrix(FbxMatrix& fbxmat);
	TMatrix		ConvertToDxMat(TMatrix& mat);
	void		SaveData(const char* filename, CFbxObj* fbxobj);
	CFbxObj*	LoadData(const char* filename);
	void		GetAnimationData(CFbxObj* fbxobj);
public:
	bool Init();
	bool Release();




};

