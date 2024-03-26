#pragma once
#include"CCamera.h"

class CDebugCamera : public CCamera
{

public:
    float   m_fCameraYaw = 0.0f;
    float   m_fCameraPitch = 20.0f;
    float   m_fCameraRoll = 0.0f;
public:
    virtual bool  Init();
    virtual bool  Frame();
    virtual bool  Render();
    virtual bool  Release();
	




};

