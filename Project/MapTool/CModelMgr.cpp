#include "CModelMgr.h"
#include"Coreinterface.h"

std::shared_ptr<CFbxObj> CModelMgr::Load(std::wstring FileName)
{
    std::size_t found = FileName.find_last_of(L"/");
    std::wstring path = FileName.substr(0, found + 1);
    std::wstring Key = FileName.substr(found + 1);
    std::shared_ptr<CFbxObj> data = GetPtr(Key);
    
    if (data != nullptr)
    {
        return data;

    }
    
    m_pFbxImpoter.Init();
    std::shared_ptr<CFbxObj> NewData = std::make_shared<CFbxObj>();
    NewData->m_csName = Key;
    if (m_pFbxImpoter.Load(FileName, NewData.get()))
    {
        m_FbxObjList.insert(std::make_pair(Key, NewData));
        m_pFbxImpoter.Release();
        return NewData;
    }
    m_pFbxImpoter.Release();

    return nullptr;
}
std::shared_ptr<CFbxObj> CModelMgr::GetPtr(std::wstring Key)
{
    auto iter = m_FbxObjList.find(Key);
    if (m_FbxObjList.end() == iter)
    {
        return nullptr;
    }

    return iter->second;
}

bool CModelMgr::Release()
{
    m_FbxObjList.clear();

    return true;
}

CModelMgr::CModelMgr()
{

}

CModelMgr::~CModelMgr()
{
    Release();
}
