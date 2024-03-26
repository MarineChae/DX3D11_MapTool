#include "Sample.h"
#include "CoreInterface.h"
#include"SceneManager.h"
extern float gWindow_Width = 1380;
extern float gWindow_Height = 720;
extern float Map_XSize = 800;
extern float Map_YSize = 600;



bool Sample::Init()
{
	SceneManager::GetInstance().Init();
	

	return true;
}

bool Sample::Frame()
{
	SceneManager::GetInstance().Frame();

	return true;
}
bool Sample::Render() 
{
	SceneManager::GetInstance().Render();
	return true;
}
bool Sample::Release()
{
	SceneManager::GetInstance().Release();
	return true;
}

bool Sample::CreateResource()
{
	CoreInterface::g_pMainCamera->PerspectiveFovLH(1.0f, 1000.0f, PI * 0.25, (float)gWindow_Width / (float)gWindow_Height);
	CCore::CreateResource();
	return true;
}
bool Sample::DeleteResource()
{
	CCore::DeleteResource();
	return true;
}
GAME_PLAY(L"Map Tool", gWindow_Width, gWindow_Height);
