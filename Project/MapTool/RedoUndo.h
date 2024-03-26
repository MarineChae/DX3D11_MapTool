#pragma once
#include"CQuadTree.h"

struct TempSaveData
{
	std::vector<PNCT_VERTEX>				TempVertexList;
	BYTE*									TempPixelDataList;
	map<UINT, shared_ptr<InstanceObject>>   TempInstanceObjList;
	map<shared_ptr<InstanceObject>, vector<INSTANCEDATA>>	TempInstanceDataList;
	map<shared_ptr<InstanceObject>,vector<shared_ptr<MapObject>>>	TempMapObjList;
	std::map<wstring, TMatrix>				TempObjectMatList;
	~TempSaveData()
	{

	}
};

class RedoUndo
{
public:
	std::vector<TempSaveData>  m_vUndoList;
	std::vector<TempSaveData>  m_vRedoList;
	std::shared_ptr<CQuadTree> m_pQuadTree;
public:

	void SaveUndoData(std::vector<PNCT_VERTEX> ver = {}, BYTE* pixeldata = nullptr, map<UINT, shared_ptr<InstanceObject>>objlist = {});
	void SaveRedoData(std::vector<PNCT_VERTEX> ver = {}, BYTE* pixeldata = nullptr, map<UINT, shared_ptr<InstanceObject>>objlist = {});
	bool UndoData(std::vector<PNCT_VERTEX>& ver, BYTE* pixeldata, map<UINT, shared_ptr<InstanceObject>>&objlist);
	bool RedoData(std::vector<PNCT_VERTEX>& ver, BYTE* pixeldata, map<UINT, shared_ptr<InstanceObject>>&objlist);

public:
	bool Init(std::shared_ptr<CQuadTree> tree);
	bool Frame(shared_ptr<CMap> map, std::map<UINT, shared_ptr<InstanceObject>>list);
	bool Release();
public:
	~RedoUndo();
};

