#include "CoreInterface.h"

ComPtr<ID3D11Device>		CoreInterface::g_pDevice = nullptr;
ComPtr<ID3D11DeviceContext> CoreInterface::g_pImmediateContext = nullptr;
shared_ptr<CCamera>				CoreInterface::g_pMainCamera = nullptr;