#pragma once
#include"MapLOD.h"
#include"Select.h"

enum class BrushType
{
	NONE,
	UPBRUSH,
	DOWNBRUSH,
	SMOOTHBRUSH,
	LAYER1,
	LAYER2,
	LAYER3,
	LAYER4,
	OBJECT1,
	OBJECT2,
	OBJECT3,

};



class CQuadTree : public MapLOD
{
public:
	std::queue<CNode*> m_pQueue;
	
	int m_iMaxDepthLimit =3;
	int m_iMaxDepth;
	int m_iMinSplitDistance = 2;
	int m_fMinDivideSize=1;
	bool m_bUsedIndexList = false;
	DWORD m_Width;
	DWORD m_Height;

	std::shared_ptr<CMap> m_pMap;
	std::vector<PNCT_VERTEX> m_vVertexList;
	std::vector<DWORD> m_vTreeIndexList;
	std::vector<CNode*>DrawNodeList;
	std::set<DWORD> m_ChangeAlphaList;
	std::vector<CNode*> m_vLeafNodeList;
	std::vector<shared_ptr<CObject>> TestBoxlist;
	BrushType			m_BrushType = BrushType::NONE;
	float				m_Brushstrength = 255.0f;
	float				m_HeightBrushstrength=5.0f;
	ComPtr<ID3D11ShaderResourceView>	m_pTexSRV[5];
	vector<CTexture*>   m_pTextureList;

public:
	void BuildQuadTree(std::shared_ptr<CMap> pMap , DWORD Width , DWORD Height, std::vector<PNCT_VERTEX> vVertexList);
	void CreateBoundingBox(CNode* pNode);
	TVector2 GetHeightFromNode(DWORD dwTL, DWORD dwTR, DWORD dwBL, DWORD dwBR);
	TVector2 GetHeightFromNode(CNode* pnode);
	bool SubDivide(CNode* pNode);
	void LevelOrder(CNode* pNode);
	void PostOrder(CNode* pNode);
	void PreOrder(CNode* pNode, CPlane* plane);
	void BuildTree(CNode* pNode);
	CNode* GetRootNode();
	CNode* CreateNode(CNode* pParent, DWORD LT, DWORD RT, DWORD LB, DWORD RB);
	bool UpdateVertexList(CNode* pNode);
	bool UpdateMapVertexList();
	//bool CreateIndexBuffer(CNode* pNode);*/
	DWORD CheckSize(DWORD dwSize);
	void FindDrawNode(CNode* pNode);
	void SetMatrix(TMatrix* WolrdMatrix, TMatrix* ViewMatrix, TMatrix* ProjMatrix);
	void UpdateNodeBoundingBox(CNode* pNode, float height);
	void FindNeighborVertex(CNode* node,Circle circle);
	void DebugBox(CBox box, CSphere sphere);
	void UpdateVertexAlpha(Circle circle);


public:
	bool Init();
	bool PreFrame();
	bool Frame(Select* select);
	bool PostFrame();
	bool Render(CPlane* plane);
	bool Release();

public:


	~CQuadTree()
	{
		
		
	}



};

