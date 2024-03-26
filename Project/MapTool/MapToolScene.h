#pragma once
#include"CCore.h"
#include"CDebugCamera.h"
#include"CCamera.h"
#include"Select.h"
#include"SaveLoader.h"
#include"SkyBox.h"
#include"RedoUndo.h"
#include"InstanceObject.h"
#include"Scene.h"
#include"ProJectionShadow.h"
#include"WaterMap.h"
#define FOGMODE_NONE    0
#define FOGMODE_LINEAR  1
#define FOGMODE_EXP     2
#define FOGMODE_EXP2    3


enum class MenuState
{
	Height,
	Texture,
	Object,

};


struct cbFog
{
	TVector4			g_ClipPlane;
	TVector4			g_vCameraPos;
	TVector4			g_FogColor;
	TVector4			g_FogInfo;
	//float				g_fogMode;		x(감쇠방법)
	//float				g_fogDensity;	y(농도)
	//float				g_fogStart;		z(시작포그거리)
	//float				g_fogEnd;		w(종료포그거리)
};
class MapToolScene : public Scene
{
public:
	//utility
	Select								     m_Select;
	RedoUndo							     m_RedoUndo;
	SaveLoader							     m_SaveLoader;
	MenuState								 m_MenuState;
public:
	std::shared_ptr<CHeightMap> 		     m_pHeightMap;
	std::shared_ptr<CQuadTree>			     m_pQuadTree = nullptr;
	std::shared_ptr<SkyBox>				     m_pSkyBox;
	RenderTarget							m_RT;
	WaterMap								m_WaterMap;
	float									m_fWaterHeight= 10.0f;
	
	
	cbFog									m_Fog;
	ComPtr<ID3D11Buffer>					m_pFogCB;
public:
	//FbxObject
	shared_ptr<MapObject>					 m_SelectMapObj;
	std::shared_ptr<MyFbxImport>			 m_FbxImporter;
	std::vector<std::shared_ptr<CFbxObj>>		ObjList;
	map<UINT, shared_ptr<InstanceObject>>    m_InstanceObjList;
	shared_ptr<InstanceObject>				m_SelectInstanceObject;
	int										m_BeforeKey;
public:
	//Debug
	std::vector<CObject*>				     TestBoxlist;
	std::shared_ptr<CCamera>			     m_pDebugCamera;
public:
	//light shadow
	ComPtr<ID3D11Buffer>				     m_pLightBuffer;
	ComPtr<ID3D11Buffer>					 m_pChangeEvreyFrameCB;
	ChangeEveryFrame						 ChangeEvreyFrameDesc;
	ProJectionShadow						 m_ProjectionShadow;
	
	LightDesc								lightDesc;
	


public:
	void SelectMenu();
	void DebugCameraData();
	void DebugPlane();
	void SaveLoad();
	//void CreateDSS();
	template<typename T>
	HRESULT CreateConstantBuffer(T desc, ID3D11Buffer** buffer);
	void DrawReflectMap(int num, TMatrix* world, TMatrix* view, TMatrix* proj);

public:

	bool Init() override;
	bool Frame() override;
	bool Render() override;
	bool Release() override;


};

template<typename T>
inline HRESULT MapToolScene::CreateConstantBuffer(T desc, ID3D11Buffer** buffer)
{

	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = sizeof(T);
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &lightDesc;

	HRESULT rs = CoreInterface::g_pDevice->CreateBuffer(
		&Desc,
		&InitData,
		buffer);


	return rs;
}

