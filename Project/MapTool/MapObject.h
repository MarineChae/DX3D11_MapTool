#pragma once
#include"CModelMgr.h"
#include<set>

struct T_VERTEX
{
	TVector3 Tangent;
};

class MapObject : public CObject
{
public:
	CFbxObj* m_pFbxObj;
	AnimDesc m_AnimInfo;
	TMatrix  m_Scale;
	TVector3  m_Rotation;
	TMatrix  m_Transform;
	wstring  m_MapObjName;
	TMatrix  m_SRTMat;
	TVector3 m_OriginMinSize;
	TVector3 m_OriginMaxSize;
	bool	 m_bIsUpdate = true;
	shared_ptr<CObject> TestBox;
public:
	float   m_fCurrentAnimTime = 0.0f;
	std::shared_ptr<CFbxMesh> m_pMesh;
	std::vector<const CTexture*> m_vTexArr;
	std::vector<const CTexture*> m_vNormalMapArr;
	std::vector<std::shared_ptr<MapObject>> m_vChildList;
	vector<vector<DWORD>>	 m_vObjectIndexList;
	vector<DWORD>			 m_vIndexListForTangent;
	vector<ComPtr<ID3D11Buffer>> m_vObjectIndexBuffer;

	vector<T_VERTEX> m_vTangentList;
	ComPtr<ID3D11Buffer> m_pTangentVB;

	
public:

	bool Frame();
	bool Render(TMatrix mat = TMatrix::Identity);
	bool PostRender();
	bool Release();

public:

	void LoadTextureArray(std::wstring filename);
	void LoadNormalTextureArray(std::wstring filename);

	
	void SetMapObj(CFbxObj* obj, AnimDesc& AnimInfo);
	void CreateObectIndexBuffer(shared_ptr<MapObject> obj, int num);
	void CreateTangentSpaceVectors(TVector3* v0,
		TVector3* v1, TVector3* v2, TVector2 uv0,
		TVector2 uv1, TVector2 uv2,
		TVector3* vTangent, TVector3* vBiNormal, TVector3* vNormal);
	void CreateTangentVector(shared_ptr<MapObject> obj);
	void CreateCollisionBox(shared_ptr<MapObject> obj,TVector3 Min, TVector3 Max);
	void UpdateCollisionBox(shared_ptr<MapObject> obj, TMatrix SRT);
	void CheckVertexMinMax(const TVector3 pos, TVector3& Min, TVector3& Max);
	bool CreateInputLayout() override;
	void DebugBox();
	
};

