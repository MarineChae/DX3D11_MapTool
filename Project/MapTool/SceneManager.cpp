#include "SceneManager.h"

bool SceneManager::Init()
{
	m_CurrentScene = make_shared<MapToolScene>();
	m_CurrentScene->Init();


	return true;
}

bool SceneManager::Frame()
{
	m_CurrentScene->Frame();
	return true;
}

bool SceneManager::Render()
{
	m_CurrentScene->Render();
	return true;
}
bool SceneManager::Release()
{
	m_CurrentScene->Release();
	return true;
}

void SceneManager::SetScene(shared_ptr<Scene> scene)
{
	m_CurrentScene = scene;
}

shared_ptr<Scene> SceneManager::GetScene()
{
	return m_CurrentScene;
}

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}
