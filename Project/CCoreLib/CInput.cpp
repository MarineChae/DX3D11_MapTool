#include "CInput.h"


bool CInput::Init()
{

    ZeroMemory(&m_dwKeyState, sizeof(m_dwKeyState));
    return true;
}
bool CInput::Frame()
{
    ::GetCursorPos(&m_MousePos);
    ::ScreenToClient(g_hWnd, &m_MousePos);

    m_vOffSet.x = m_MousePos.x - m_BeforeMousePos.x;
    m_vOffSet.y = m_MousePos.y - m_BeforeMousePos.y;
    for (int iKey = 0; iKey < 256; ++iKey)
    {

        SHORT sh = GetAsyncKeyState(iKey);
        if (sh & 0x8000)
        {
            if (m_dwKeyState[iKey] == KEY_NONE)
            {
                m_dwKeyState[iKey] = KEY_PUSH;
            }
            else
            {
                m_dwKeyState[iKey] = KEY_HOLD;
            }
        }
        else
        {
            if (m_dwKeyState[iKey] == KEY_PUSH || m_dwKeyState[iKey] == KEY_HOLD)
            {
                m_dwKeyState[iKey] = KEY_UP;
            }
            else
            {
                m_dwKeyState[iKey] = KEY_NONE;
            }

        }
    }
    m_BeforeMousePos = m_MousePos;
    return true;
}
bool CInput::Render()
{
#ifdef _DEBUG
    std::wstring mousePos = std::to_wstring(m_MousePos.x);
    mousePos += L",";
    mousePos += std::to_wstring(m_MousePos.y);
    mousePos += L"\n";
//    DebugString(mousePos.c_str());
#endif
    return true;
}
bool CInput::Release()
{
    return true;
}
