#pragma once
#include"MapObject.h"

struct INSTANCEDATA
{
	TMatrix matWorld;
	float	Emissive = 0.0f;
};

class InstanceObject : public CObject
{
public:
	wstring m_szName;
	bool	 m_bBeforeSelect = false;
	pair<vector<shared_ptr<MapObject>>, vector<INSTANCEDATA>> m_InstanceList;
	ComPtr<ID3D11Buffer>		    m_InstanceBuffer;
public:
	bool Init();
	bool Frame();
	bool Render(TMatrix* world = nullptr , TMatrix* view =nullptr, TMatrix* proj = nullptr);
	bool CreateInstanceBuffer(TMatrix mat);
};

