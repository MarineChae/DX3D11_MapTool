#pragma once
#include"CDefine.h"
#include"CMapObj.h"
#include"CInput.h"
#include"CSpriteObj.h"

class CScene
{
public:
	CSpriteUVObj* m_pPotal;
public:

	std::unique_ptr<CMapObj>m_pMapObj = nullptr;
	std::vector<std::unique_ptr<CMapObj>>	m_MapList;
	std::wstring BackGroundFile;




public:
	virtual bool CheckCollide(CRect& MapObj, CRect& other) { return true; };
	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();
	virtual void SetMatrix(CMatrix* WolrdMatrix, CMatrix* ViewMatrix, CMatrix* ProjMatrix);

public:
	virtual ~CScene() {};

};
//
//class CSceneTitle : public CScene
//{
//
//
//
//public:
//	virtual bool Init();
//	virtual bool Frame();
//	virtual bool Render();
//	virtual bool Release();
//public:
//
//	virtual ~CSceneTitle() {};
//};
//
//class CSceneLobby : public CScene
//{
//public:
//	std::vector<std::unique_ptr<CMapObj>>	m_MapList;
//
//public:
//
//	void CreateBackTile();
//	void CreateStaticObj();
//	void CreateFootHold();
//	bool CreateMapObj();
//
//public:
//	virtual bool Init();
//	virtual bool Frame();
//	virtual bool Render();
//	virtual bool Release();
//   
//};