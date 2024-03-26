#pragma once
#include"CNode.h"
#include<set>
#include<queue>
#include"CHeightMap.h"

class StaticData
{
public:
	DWORD		dwLevel;
	std::vector<std::vector<DWORD>> IndexList;
	std::vector<ComPtr<ID3D11Buffer>> IndexBufferList;
public:
	StaticData()
	{

	}
	~StaticData()
	{
		for (int iList = 0; iList < IndexBufferList.size(); iList++)
		{
			IndexList[iList].clear();
		}
		IndexBufferList.clear();
		IndexList.clear();
	}


};
class MapLOD
{
public:
	CNode* m_pRootNode;
public:
	int m_iNumCell;
	int m_iPatchLodCount;
	int m_iNumFace;
	std::vector<vector<CNode*>>	m_LevelList;
	std::vector<CNode*> m_vDrawPatchNodeList;
	std::vector<std::shared_ptr<StaticData>> m_vLodIndexList;
public:
	//lod
	void InitLOD(CNode* rootNode, int maxDepth);
	void FineNeighborNode(CNode* node);
	void SetNeighborNode(CNode* node);
	void SetLOD(DWORD dwWidth, int iNumDepth);
	void ComputeStaticLodIndex(int iMaxCell);
	bool CreateIndexBuffer(std::shared_ptr<StaticData> data);
	DWORD GetIndexCounter(DWORD dwData, DWORD dwNumCell);

public:
	

	void GetDrawPatchNode(vector<CNode*> drawNodeList);
	void AddDrawPatchNode(CNode* pNode);
	DWORD GetLodSubIndex(CNode* pNode);
	float GetExpansionRatio(TVector3 pCenter);
	DWORD GetLODType(CNode* pNode);
};

