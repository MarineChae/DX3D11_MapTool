#include "RedoUndo.h"
#include"CInput.h"
void RedoUndo::SaveUndoData(std::vector<PNCT_VERTEX> ver, BYTE* pixeldata, map<UINT, shared_ptr<InstanceObject>>objlist)
{
    TempSaveData temp;
    //정점의 정보를 임시로 저장해 놓을 데이터에 넣어놓는다.
    if (!ver.empty())
    {
        temp.TempVertexList.resize(ver.size());
        std::copy(ver.begin(), ver.end(), temp.TempVertexList.begin());
    }
    //스플레팅 데이터
    if (pixeldata != NULL)
    {
        int iSize = m_pQuadTree->m_Width * m_pQuadTree->m_Height * 4;
        temp.TempPixelDataList = new BYTE[iSize];
        memcpy_s(temp.TempPixelDataList, iSize, pixeldata, iSize);
    }
    //오브젝트 데이터
    temp.TempInstanceObjList = objlist;
    for (auto& obj : objlist)
    {
        temp.TempMapObjList.insert(make_pair(obj.second, obj.second->m_InstanceList.first));
        temp.TempInstanceDataList.insert(make_pair(obj.second, obj.second->m_InstanceList.second));
    }
    //인스턴싱 데이터
    for (auto& inst : temp.TempInstanceObjList)
    {
        for (auto& obj : inst.second->m_InstanceList.first)
        {
            TMatrix mat;
            //scale
            mat._11 = obj->m_Scale._11;  mat._12 = obj->m_Scale._22; mat._13 = obj->m_Scale._33;
            //rotate
            mat._21 = obj->m_Rotation.x;  mat._22 = obj->m_Rotation.y;  mat._23 = obj->m_Rotation.z;
            //trasform
            mat._31 = obj->m_Transform._41;  mat._32 = obj->m_Transform._42;  mat._33 = obj->m_Transform._43;
            temp.TempObjectMatList.insert(make_pair(obj->m_MapObjName, mat));
        }
       
    }
    //되돌리는 목록에 추가해준다
    m_vUndoList.push_back(temp);

}
void RedoUndo::SaveRedoData(std::vector<PNCT_VERTEX> ver, BYTE* pixeldata, map<UINT, shared_ptr<InstanceObject>>objlist)
{
    TempSaveData temp;
    if (!ver.empty())
    {
        temp.TempVertexList.resize(ver.size());
        std::copy(ver.begin(), ver.end(), temp.TempVertexList.begin());
    }
    if (pixeldata != NULL)
    {
        int iSize = m_pQuadTree->m_Width * m_pQuadTree->m_Height * 4;
        temp.TempPixelDataList = new BYTE[iSize];
        memcpy_s(temp.TempPixelDataList, iSize, pixeldata, iSize);
    }

    temp.TempInstanceObjList = objlist;
    for (auto& obj : objlist)
    {
        temp.TempMapObjList.insert(make_pair(obj.second, obj.second->m_InstanceList.first));
        temp.TempInstanceDataList.insert(make_pair(obj.second, obj.second->m_InstanceList.second));
    }

    for (auto& inst : temp.TempInstanceObjList)
    {
        for (auto& obj : inst.second->m_InstanceList.first)
        {
            TMatrix mat;
            //scale
            mat._11 = obj->m_Scale._11;  mat._12 = obj->m_Scale._22; mat._13 = obj->m_Scale._33;
            //rotate
            mat._21 = obj->m_Rotation.x;  mat._22 = obj->m_Rotation.y;  mat._23 = obj->m_Rotation.z;
            //trasform
            mat._31 = obj->m_Transform._41;  mat._32 = obj->m_Transform._42;  mat._33 = obj->m_Transform._43;
            temp.TempObjectMatList.insert(make_pair(obj->m_MapObjName, mat));
        }
    }
    m_vRedoList.push_back(temp);

}
bool RedoUndo::UndoData(std::vector<PNCT_VERTEX>& ver, BYTE* pixeldata, map<UINT, shared_ptr<InstanceObject>>&objlist)
{
    //리스트가 비어있다면 실행하지 않는다.
    if (m_vUndoList.empty())
        return false;
    //작업내용을 취소하기 전에 되돌릴 데이터를 저장해 놓는다.
    SaveRedoData(ver, pixeldata,objlist);
    //리스트의 가장 최신 데이터를 가져온다.
    auto list = m_vUndoList.back();
    //각각의 정보들을 현재 데이터에 덮어 씌운다.
    if (!list.TempVertexList.empty())
        std::copy(list.TempVertexList.begin(), list.TempVertexList.end(), ver.begin());
    int iSize = m_pQuadTree->m_Width * m_pQuadTree->m_Height * 4;
    if (list.TempPixelDataList != NULL)
        memcpy_s(pixeldata, iSize, list.TempPixelDataList, iSize);


    for (auto& node : m_pQuadTree->m_vLeafNodeList)
    {
        m_pQuadTree->UpdateVertexList(node);
    }
    objlist = list.TempInstanceObjList;
    for (auto& obj : objlist)
    {
        auto objlist = list.TempMapObjList.find(obj.second);
        auto datalist = list.TempInstanceDataList.find(obj.second);
        obj.second->m_InstanceList.first = objlist->second;
        obj.second->m_InstanceList.second = datalist->second;

    }
   
    


    for (auto& inst : objlist)
    {

        for (auto& obj : inst.second->m_InstanceList.first)
        {
            auto& objName = obj->m_MapObjName;
            for (auto& mat : list.TempObjectMatList)
            {
                if (mat.first == objName)
                {
                    obj->m_Scale._11 = mat.second._11;
                    obj->m_Scale._22 = mat.second._12;
                    obj->m_Scale._33 = mat.second._13;
                    obj->m_Rotation.x = mat.second._21;
                    obj->m_Rotation.y = mat.second._22;
                    obj->m_Rotation.z = mat.second._23;
                    obj->m_Transform._41 = mat.second._31;
                    obj->m_Transform._42 = mat.second._32;
                    obj->m_Transform._43 = mat.second._33;
                    break;
                }
            }
        }
    }

    //사용한 임시데이터 들은 삭제해준다.
    if (!list.TempVertexList.empty())
        list.TempVertexList.clear();
    if (list.TempPixelDataList != NULL)
        delete[] list.TempPixelDataList;
    if (!list.TempInstanceObjList.empty())
        list.TempInstanceObjList.clear();
    if (!list.TempObjectMatList.empty())
        list.TempObjectMatList.clear();
    if (!list.TempInstanceDataList.empty());
        list.TempInstanceDataList.clear();
    if (!list.TempMapObjList.empty());
         list.TempMapObjList.clear();

    m_vUndoList.pop_back();
    return true;
}
bool RedoUndo::RedoData(std::vector<PNCT_VERTEX>& ver, BYTE* pixeldata, map<UINT, shared_ptr<InstanceObject>>&objlist)
{
    if (m_vRedoList.empty())
        return false;
    SaveUndoData(ver, pixeldata, objlist);
    auto list = m_vRedoList.back();
    if (!list.TempVertexList.empty())
        std::copy(list.TempVertexList.begin(), list.TempVertexList.end(), ver.begin());

    if (list.TempPixelDataList != NULL)
    {
        int iSize = m_pQuadTree->m_Width * m_pQuadTree->m_Height * 4;
        memcpy_s(pixeldata, iSize, list.TempPixelDataList, iSize);
    }

    for (auto& node : m_pQuadTree->m_vLeafNodeList)
    {
        m_pQuadTree->UpdateVertexList(node);
    }
    objlist = list.TempInstanceObjList;
    for (auto& obj : objlist)
    {
        auto objlist = list.TempMapObjList.find(obj.second);
        auto datalist = list.TempInstanceDataList.find(obj.second);
        obj.second->m_InstanceList.first = objlist->second;
        obj.second->m_InstanceList.second = datalist->second;
    }
 
    for (auto& inst : list.TempInstanceObjList)
    {

        for (auto& obj : inst.second->m_InstanceList.first)
        {
            auto& objName = obj->m_MapObjName;
            for (auto& mat : list.TempObjectMatList)
            {
                if (mat.first == objName)
                {
                    obj->m_Scale._11 = mat.second._11;
                    obj->m_Scale._22 = mat.second._12;
                    obj->m_Scale._33 = mat.second._13;
                    obj->m_Rotation.x = mat.second._21;
                    obj->m_Rotation.y = mat.second._22;
                    obj->m_Rotation.z = mat.second._23;
                    obj->m_Transform._41 = mat.second._31;
                    obj->m_Transform._42 = mat.second._32;
                    obj->m_Transform._43 = mat.second._33;
                    break;
                }
            }
        }
        
    }
    if (!list.TempVertexList.empty())
        list.TempVertexList.clear();
    if (list.TempPixelDataList != NULL)
        delete[] list.TempPixelDataList;
    if (!list.TempInstanceObjList.empty())
        list.TempInstanceObjList.clear();
    if (!list.TempObjectMatList.empty())
        list.TempObjectMatList.clear();
    if (!list.TempInstanceDataList.empty());
         list.TempInstanceDataList.clear();
    if (!list.TempMapObjList.empty());
         list.TempMapObjList.clear();
        
    m_vRedoList.pop_back();
    return true;
}

bool RedoUndo::Init(std::shared_ptr<CQuadTree> tree)
{
    m_pQuadTree = tree;
    return true;
}

bool RedoUndo::Frame(shared_ptr<CMap> map, std::map<UINT, shared_ptr<InstanceObject>>list)
{
    if (m_pQuadTree->m_BrushType != BrushType::OBJECTSELECT
        &&CInput::GetInstance().m_dwKeyState[VK_RBUTTON] == KEY_PUSH)
    {
        m_vRedoList.clear();
        SaveUndoData(map->m_vVertexList, map->m_fLookup, list);
    }
    
    return true;
}

bool RedoUndo::Release()
{
    for (auto& list : m_vUndoList)
    {
        if (!list.TempVertexList.empty())
            list.TempVertexList.clear();
        if (list.TempPixelDataList != NULL)
            delete[] list.TempPixelDataList;
        if (!list.TempInstanceObjList.empty())
            list.TempInstanceObjList.clear();
    }
    for (auto& list : m_vRedoList)
    {
        if (!list.TempVertexList.empty())
            list.TempVertexList.clear();
        if (list.TempPixelDataList != NULL)
            delete[] list.TempPixelDataList;
        if (!list.TempInstanceObjList.empty())
            list.TempInstanceObjList.clear();
    }

    m_vUndoList.clear();
    m_vRedoList.clear();
    return true;
}

RedoUndo::~RedoUndo()
{
    Release();
}
