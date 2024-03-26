#pragma once
#include"CQuadTree.h"

class SaveLoader
{
public:
	//���̸�, ����ŷ ��, �ؽ��� ���, ������Ʈ ����, ��İ�
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

