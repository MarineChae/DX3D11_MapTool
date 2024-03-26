#pragma once
#include"CQuadTree.h"

class SaveLoader
{
public:
	//높이맵, 마스킹 맵, 텍스쳐 경로, 오브젝트 갯수, 행렬값
	UINT							m_Mapsize[2];
	std::vector<float>				m_MapHeight;
	std::map<std::wstring, TMatrix> m_MapObject;
	std::vector<std::wstring>		m_SubTexture;
	std::wstring					m_AlphaTexture;
public:
	bool	SaveMap(shared_ptr<CQuadTree>, std::string filename, std::map<UINT, shared_ptr<InstanceObject>>list);
	bool	LoadMap(std::string filename, std::map<UINT, shared_ptr<InstanceObject>>list);

public:
	SaveLoader();
	~SaveLoader();
};

