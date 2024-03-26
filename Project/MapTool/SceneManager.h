#pragma once
#include"CDefine.h"
#include"CoreInterface.h"
#include"MapToolScene.h"


class SceneManager
{
public:
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pImmediateContext;
	std::map < std::wstring, shared_ptr<Scene>> m_List;

public:
	shared_ptr<Scene> m_CurrentScene;


	bool Init();
	bool Frame();
	bool Render();
	bool Release();
	void SetScene(shared_ptr<Scene> scene);
	shared_ptr<Scene> GetScene();

	static SceneManager& GetInstance()
	{
		static SceneManager Scene;
		return Scene;

	}

private:
	SceneManager();
public:

	virtual ~SceneManager();


};
