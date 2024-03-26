#pragma once
#define _CRTDBG_MAP_ALLOC
#include"Mymath.h"
#include "DirectXTex.h"
#include<memory>
#include <d3d11.h>
#include <dxgi.h>
#include <codecvt>
#include<tchar.h>
#include "CUtiliy.h"
#include<d3dcompiler.h>
#include <iostream>
#include<wrl.h>
#include <wincodec.h>
#include <crtdbg.h>
#include"imgui.h"
#include"imgui_impl_dx11.h"
#include"imgui_impl_win32.h"
#include"ImGuiFileDialog.h"

#pragma comment (lib,"d3d11.lib")
#pragma comment (lib,"dxgi.lib")
#pragma comment (lib,"D3DCompiler.lib")
#pragma comment (lib, "DirectXTex.lib")
#pragma comment (lib, "DirectXTex_debug.lib")
#pragma comment (lib,"CCoreLib.lib")
#define randstep(fmin,fmax) ((float)fmin + ((float)fmax-(float)fmin) * rand()/RAND_MAX)

using namespace Microsoft::WRL;
using namespace std;
extern float gWindow_Width;
extern float gWindow_Height;
extern float Map_XSize;
extern float Map_YSize;

#define GAME_INIT int APIENTRY wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR lpCmdLine,int nCmdShow){{ Sample window;
#define GAME_REGISTER 				     window.SetRegisterClassWindow(hInstance);
#define GAME_WINDOW(Title,Width,Height)  window.SetWindow(Title, Width, Height);
#define GAME_RUN 						 window.Run();}   _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#define GAME_END					     return 0;}
	
#define GAME_PLAY(Title,Width,Height)  GAME_INIT GAME_REGISTER GAME_WINDOW(Title,Width,Height) GAME_RUN GAME_END


extern double g_fSecondPerFrame;
extern double g_fGameTime;
extern HWND  g_hWnd; 

static void DebugString(const WCHAR* msg)
{
#ifdef _DEBUG
	OutputDebugString(msg);
#endif
}
static std::wstring mtw(std::string str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.from_bytes(str);
}

static std::string wtm(std::wstring str)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	return conv.to_bytes(str);
}

