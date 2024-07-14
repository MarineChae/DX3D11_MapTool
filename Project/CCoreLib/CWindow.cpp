#include "CWindow.h"
HWND   g_hWnd;
CWindow* g_pWindow = nullptr;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    LRESULT lr = g_pWindow->MsgProc(hWnd, message, wParam, lParam);
    if (lr > 0)return 0;
    switch (message)
    {

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
int CWindow::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        if (SIZE_MINIMIZED != wParam)
        {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            ResizeDevice(width, height);
        }
        break;
    }
    return -1;
}
bool CWindow::SetRegisterClassWindow(HINSTANCE hInstance)
{
    m_hInstance = hInstance;
    //윈도우 생성 예약
    WNDCLASSEXW wcex;
    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW ;
    wcex.lpfnWndProc = WndProc; // 윈도우프로시저 각종이벤트를 메세지 큐에 저장
    wcex.hInstance = hInstance;
    wcex.lpszClassName = L"윈도우";
    wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255,255,255));
    WORD ret = RegisterClassExW(&wcex);


	return true;
}

bool CWindow::SetWindow(const WCHAR* wsztitle,
                        DWORD dwWindowWidth,    
                        DWORD dwWindowHeight)
{
    m_dwWindowWidth = dwWindowWidth;
    m_dwWindowHeight = dwWindowHeight;

#ifndef _DEBUG
    m_dwWindowExStyle = WS_EX_APPWINDOW;
    //m_dwWindowStyle = WS_POPUPWINDOW;
#else
    m_dwWindowExStyle = WS_EX_APPWINDOW;

#endif 
    RECT rc = { 0,0, dwWindowWidth , dwWindowHeight };
    AdjustWindowRect(&rc, m_dwWindowStyle, FALSE);
    m_hWnd = CreateWindowEx(m_dwWindowExStyle, L"윈도우", wsztitle,
        m_dwWindowStyle,
        m_dwWindowPosX, m_dwWindowPosY,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, m_hInstance, nullptr);

  //  m_hWnd = CreateWindowEx(m_dwWindowExStyle, L"윈도우", wsztitle,
  //      m_dwWindowStyle,//윈도우 속성변경
  //      m_dwWindowPosX, m_dwWindowPosY,//윈도우 시작지점
  //      m_dwWindowWidth, m_dwWindowHeight,//윈도우 크기
  //      nullptr, nullptr, m_hInstance, nullptr);

    if (!m_hWnd)
    {
        return FALSE;
    }

    ShowWindow(m_hWnd, SW_SHOWNORMAL);
    g_hWnd = m_hWnd;

    GetClientRect(m_hWnd, &m_ClientRect);
    gWindow_Width = m_dwWindowWidth = m_ClientRect.right;
    gWindow_Height = m_dwWindowHeight = m_ClientRect.bottom;

    return true;
}


CWindow::CWindow()
{
    g_pWindow = this;
}