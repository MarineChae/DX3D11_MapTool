#include "MyFbxImport.h"

bool MyFbxImport::Load(std::wstring filename, CFbxObj* fbxobj)
{
    string st = wtm(filename);
    bool bret = m_FbxImporter->Initialize(st.c_str());
    //bool bret = m_FbxImporter->Initialize(ship.FBX);
    string str = m_FbxImporter->GetStatus().GetErrorString();
    bret = m_FbxImporter->Import(m_FbxScene);

    FbxNode* FbxRootNode = m_FbxScene->GetRootNode();

    if (FbxRootNode)
    {
        PreProcess(FbxRootNode,nullptr);
    }
   
    for (int iNode = 0; iNode < m_pMeshNodeList.size(); ++iNode)
    {
        std::shared_ptr<CFbxMesh> NewMesh = std::make_shared<CFbxMesh>();
        LoadMesh(m_pMeshNodeList[iNode],*NewMesh.get());
        NewMesh->m_csName = m_pMeshNodeList[iNode]->GetName();
        NewMesh->m_WolrdMat = ParseTransform(m_pMeshNodeList[iNode]);
        fbxobj->m_MeshList.push_back(NewMesh);
        
    }
   
    GetAnimationData(fbxobj);
   
    return true;
}

void MyFbxImport::SaveData(const char* filename, CFbxObj* fbxobj)
{
    CFbxExporter  fileOpen;
    
    FILE* fp = fileOpen.CreateFbxFile(filename, "a+t");
    if (fp != NULL)
    {
       

        fwrite(&fbxobj->m_AnimInfo, sizeof(UINT)*4, 1, fp);

        UINT size;
        UINT loop = fbxobj->m_MeshList.size();
        fwrite(&loop, sizeof(UINT), 1, fp);


        for (int i = 0; i < loop; ++i)
        {
            fwrite(&fbxobj->m_MeshList[i]->iNumPolygon, sizeof(UINT), 1, fp);

            size = fbxobj->m_MeshList[i]->m_TriangleList.size();
            fwrite(&size, sizeof(UINT), 1, fp);
            for (int tri = 0; tri < size; ++tri)
            {
                UINT size2 = fbxobj->m_MeshList[i]->m_TriangleList[tri].size();
                fwrite(&size2, sizeof(UINT), 1, fp);
                for (int tri2 = 0; tri2 < size2; ++tri2)
                {
                   fwrite(&fbxobj->m_MeshList[i]->m_TriangleList[tri][tri2], sizeof(PNCT_VERTEX), 1, fp);
                }
                
            }
            size = fbxobj->m_MeshList[i]->m_TriangleOffSetList.size();
            fwrite(&size, sizeof(UINT), 1, fp);
            for (int tri = 0; tri < size; ++tri)
            {
              fwrite(&fbxobj->m_MeshList[i]->m_TriangleOffSetList[tri] , sizeof(UINT), 1, fp);
            }
            

            size = fbxobj->m_MeshList[i]->m_vMatrixArr.size();
            fwrite(&size, sizeof(UINT), 1, fp);
            for (int tri = 0; tri < size; ++tri)
            {
                fwrite(&fbxobj->m_MeshList[i]->m_vMatrixArr[tri], sizeof(TMatrix), 1, fp);
            }

            size = fbxobj->m_MeshList[i]->m_szTextureFilename.size();
            fwrite(&size, sizeof(UINT), 1, fp);
            for (int tri = 0; tri < size; ++tri)
            {
                
                fwrite(&fbxobj->m_MeshList[i]->m_szTextureFilename[tri], sizeof(std::wstring), 1, fp);
            }

        }
        

        

        fileOpen.CloseFile();

    }

}

CFbxObj* MyFbxImport::LoadData(const char* filename)
{

    CFbxExporter  fileOpen;

    FILE* fp = fileOpen.CreateFbxFile(filename, "rb");
    if (fp != NULL)
    {
     

         CFbxObj* NewNode = new CFbxObj;
         
         fread(&NewNode->m_AnimInfo, sizeof(UINT)*4, 1, fp);
       
         UINT size;
         UINT loop;
         fread(&loop, sizeof(UINT), 1, fp);
    
         for (int i = 0; i < loop; ++i)
         {
             std::shared_ptr<CFbxMesh>NewMesh = make_shared<CFbxMesh>();
             fread(&NewMesh->iNumPolygon, sizeof(UINT), 1, fp);

             
             fread(&size, sizeof(UINT), 1, fp);
             NewMesh->m_TriangleList.resize(size);

             for (int tri = 0; tri < size; ++tri)
             {
                 UINT Size2;
                 fread(&Size2, sizeof(UINT), 1, fp);
                 NewMesh->m_TriangleList[tri].resize(Size2);
                 for (int tri2 = 0; tri2 < Size2; ++tri2)
                 {
                     fread(&NewMesh->m_TriangleList[tri][tri2], sizeof(PNCT_VERTEX), 1, fp);
                 }
             }

            
             fread(&size, sizeof(UINT), 1, fp);
             for (int tri = 0; tri < size; ++tri)
             {
                 fread(&NewMesh->m_TriangleOffSetList[tri], sizeof(UINT) * size, 1, fp);
             }


             
             fread(&size, sizeof(UINT), 1, fp);
             NewMesh->m_vMatrixArr.resize(size);
             for (int tri = 0; tri < size; ++tri)
             {
             fread(&NewMesh->m_vMatrixArr[tri], sizeof(TMatrix), 1, fp);
             }

             fread(&size, sizeof(UINT), 1, fp);
             NewMesh->m_szTextureFilename.resize(size);
             for (int tri = 0; tri < size; ++tri)
             {
                 
                 fread(&NewMesh->m_szTextureFilename[tri], sizeof(std::wstring), 1, fp);
             }
           


            
             NewNode->m_MeshList.push_back(NewMesh);
         }
         
      


        fileOpen.CloseFile();
        return NewNode;
    }
    else
    {
        return nullptr;
    }


}


bool MyFbxImport::PreProcess(FbxNode* pNode, CFbxModel* pParent)
{ //트리를 순회하면서 정보를 탐색
    if (pNode == nullptr)return false;
    if (pNode->GetCamera() ||
        pNode->GetLight()) return false;

    //std::shared_ptr<CFbxModel> fbx = std::make_shared<CFbxModel>();
    //fbx->m_csName = pNode->GetName();
    //fbx->m_pParent = pParent;
    //fbx->m_WolrdMat = ParseTransform(pNode);
    //m_pNodeList.push_back(pNode);
    //m_vModelList.push_back(fbx);

    FbxMesh* fbxMesh = pNode->GetMesh();
    if (fbxMesh != nullptr)
    {
        m_pMeshNodeList.push_back(pNode);
    }
    UINT iNumChild = pNode->GetChildCount();

    for (int ichild = 0; ichild < iNumChild; ++ichild)
    {
        FbxNode* pChildeNode = pNode->GetChild(ichild);
        PreProcess(pChildeNode,nullptr);
    }

    return true;
}




bool MyFbxImport::LoadMesh(FbxNode* pNode, CFbxMesh& mesh)
{
    FbxMesh* pFbxMesh = pNode->GetMesh();
    if (pFbxMesh == nullptr)return false;
    UINT iNumPolygonCount = pFbxMesh->GetPolygonCount();
    FbxVector4* pVertexPos = pFbxMesh->GetControlPoints();

    UINT iNumLayerCount = pFbxMesh->GetLayerCount();

    std::vector<FbxLayerElementUV*> VertexUvSet;
    std::vector<FbxLayerElementNormal*> VertexNormalSet;
    std::vector< FbxLayerElementVertexColor*> VertexColorSet;
    std::vector< FbxLayerElementTangent*> VertexTangentSet;
    std::vector< FbxLayerElementMaterial*> VertexMatrialSet;
    

    for (int iLayer = 0; iLayer < iNumLayerCount; ++iLayer)
    {
        FbxLayer* layer = pFbxMesh->GetLayer(iLayer);
        if (layer->GetUVs() != nullptr)
        {
            VertexUvSet.push_back(layer->GetUVs());
        }
        if (layer->GetNormals() != nullptr)
        {
            VertexNormalSet.push_back(layer->GetNormals());
        }
        if (layer->GetVertexColors() != nullptr)
        {
            VertexColorSet.push_back(layer->GetVertexColors());
        }
        if (layer->GetMaterials() != nullptr)
        {
            VertexMatrialSet.push_back(layer->GetMaterials());
        }
        if (layer->GetTangents() != nullptr)
        {
            VertexTangentSet.push_back(layer->GetTangents());
        }

    }


#pragma region Material

    int iNumMtrl = pNode->GetMaterialCount();
    if (iNumMtrl > 1)
    {

        mesh.m_TriangleList.resize(iNumMtrl);
    }
    else
    {
        mesh.m_TriangleList.resize(1);

    }
    for (int iMtrl = 0; iMtrl < iNumMtrl; ++iMtrl)
    {

        FbxSurfaceMaterial* pSurface = pNode->GetMaterial(iMtrl);
        if (pSurface)
        {

            std::string TextureName = ParseMatarial(pSurface);
            char Drive[255];
            char Dir[255];
            char Name[255];
            char Ext[255];
            _splitpath_s(TextureName.c_str(), Drive,255, Dir,255, Name,255, Ext,255);

            std::string TexName = Name;
            TexName += Ext;
            mesh.m_szTextureFilename.push_back(mtw(TexName));

        }


    }

#pragma endregion



#pragma region SetGeomatricMatrix

    FbxAMatrix GeometricMatrix;
    FbxAMatrix NormalMatrix;

    FbxVector4 t = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 s = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
    FbxVector4 r = pNode->GetGeometricRotation(FbxNode::eSourcePivot);


    GeometricMatrix.SetT(t);
    GeometricMatrix.SetR(r);
    GeometricMatrix.SetS(s);
   

    NormalMatrix = GeometricMatrix;
    NormalMatrix = NormalMatrix.Inverse();
    NormalMatrix = NormalMatrix.Transpose();
#pragma endregion





    UINT iBasePolygonIndex = 0;
    for (int iPoly = 0; iPoly < iNumPolygonCount; ++iPoly)
    {
        
        //polygon -> face가 2개인경우 -> 4개의 정점으로 구성 //제어점
        //polygon -> face가 1개인경우 - > 3개의 정점으로 구성//제어점
        int Submtrl = 0;
        if (iNumMtrl >= 1)
        {
            Submtrl = GetSubMaterialIndex(VertexMatrialSet[0],iPoly);
        }

        UINT iPolySize = pFbxMesh->GetPolygonSize(iPoly);
        UINT iTriangleCount = iPolySize - 2;
        UINT iCornerIndex[3];
        mesh.iNumPolygon += iTriangleCount;
        for (int iTriangle = 0; iTriangle < iTriangleCount; ++iTriangle)
        {
            iCornerIndex[0] = pFbxMesh->GetPolygonVertex(iPoly , 0);
            iCornerIndex[1] = pFbxMesh->GetPolygonVertex(iPoly , iTriangle+2);
            iCornerIndex[2] = pFbxMesh->GetPolygonVertex(iPoly , iTriangle+1);

            int UvIndex[3];
            UvIndex[0] = pFbxMesh->GetTextureUVIndex(iPoly,0);
            UvIndex[1] = pFbxMesh->GetTextureUVIndex(iPoly, iTriangle + 2);
            UvIndex[2] = pFbxMesh->GetTextureUVIndex(iPoly, iTriangle + 1);

            UINT LayerIndex[3] = { 0, iTriangle + 2 , iTriangle + 1 };


            for (int iVertex = 0; iVertex < 3; iVertex++)
            {
                UINT iVertexIndex = iCornerIndex[iVertex];
                PNCT_VERTEX pnct;
                FbxVector4 Vertex = pVertexPos[iVertexIndex];
                auto finalPos = GeometricMatrix.MultT(Vertex);
                pnct.Pos.x = finalPos.mData[0];
                pnct.Pos.y = finalPos.mData[2];
                pnct.Pos.z = finalPos.mData[1];

                pnct.Nor = { 0,0,0};
                if (VertexNormalSet.size() > 0)
                {
                    auto NormalLayer = VertexNormalSet[0];
                    FbxVector4 nor = ReadVertexNormal(NormalLayer, iVertexIndex, iBasePolygonIndex + LayerIndex[iVertex]);
                    
                    auto finalNormal = NormalMatrix.MultT(nor);
                    finalNormal.Normalize();

                    pnct.Nor.x = finalNormal.mData[0];
                    pnct.Nor.y = finalNormal.mData[2];
                    pnct.Nor.z = finalNormal.mData[1];
                }

                pnct.Col = { 1,1,1,1 };
                if (VertexColorSet.size() > 0)
                {
                    
                    FbxColor col = ReadVertexColor(VertexColorSet[0], iVertexIndex,
                                                iBasePolygonIndex + LayerIndex[iVertex]);
                    pnct.Col.x = col.mRed;
                    pnct.Col.y = col.mGreen;
                    pnct.Col.z = col.mBlue;
                    pnct.Col.w = 1.0f;


                }
                if (VertexUvSet.size() > 0)
                {
                    FbxLayerElementUV* layerUV = VertexUvSet[0];
                    FbxVector2 uv(0, 0);
                    uv = ReadTextureCoordinate(layerUV, iVertexIndex,UvIndex[iVertex]);
                    pnct.Tex = { (float)uv.mData[0],(float)(1.0f-uv.mData[1]) };

                }
                if (VertexTangentSet.size() > 0)
                {
                    int a = 0;
                }

                mesh.m_TriangleList[Submtrl].push_back(pnct);

            }

        }
        iBasePolygonIndex += iPolySize;

    }
   

    return true;
}
FbxVector4 MyFbxImport::ReadVertexNormal(FbxLayerElementNormal* LayerNor, int VertexIndex, int uvIndex)
{
    FbxVector4 Normal;

    switch (LayerNor->GetMappingMode())
    {
    case FbxLayerElementUV::eByControlPoint: //제어점 마다 하나의 매핑 좌표가 있다
    {                                        //하나의 사각형은 삼각형 두 개로 이루어지며 하나의 사각형에는 제어점이 4개 존재하지만
        switch (LayerNor->GetReferenceMode()) //정점은 6개가 존재한다. 평소의 VB IB 관계랑 비슷하다 생각하면 될듯
        {

             case FbxLayerElementUV::eDirect: //n번째 요소에 대한 매핑 정보가 directArray에서 n번째 위치에서 발견되었다.
             {
                 Normal = LayerNor->GetDirectArray().GetAt(VertexIndex);
                 break;
             }
             case FbxLayerElementUV::eIndexToDirect: // indexArray의 요소가 directArray에 인덱스가 존재한다.
             {
                 int id = LayerNor->GetIndexArray().GetAt(VertexIndex);
                 Normal = LayerNor->GetDirectArray().GetAt(id);
                 break;
             }

        }
        break;
    }
    case FbxLayerElementUV::eByPolygonVertex: // 정점마다 하나의 매핑 좌표가 있다
    {
        switch (LayerNor->GetReferenceMode())
        {
            case FbxLayerElementUV::eDirect:
            {
                Normal = LayerNor->GetDirectArray().GetAt(uvIndex);
                break;
            }
            case FbxLayerElementUV::eIndexToDirect:
            {
                int id = LayerNor->GetIndexArray().GetAt(uvIndex);
                Normal = LayerNor->GetDirectArray().GetAt(id);
                break;
            }
        }
        break;
    }

    }

    return Normal;
}
FbxColor MyFbxImport::ReadVertexColor(FbxLayerElementVertexColor* LayerCol, int VertexIndex, int uvIndex)
{
    FbxColor color(1,1,1,1);

    switch (LayerCol->GetMappingMode())
    {
    case FbxLayerElementUV::eByControlPoint: //제어점 마다 하나의 매핑 좌표가 있다
    {                                        //하나의 사각형은 삼각형 두 개로 이루어지며 하나의 사각형에는 제어점이 4개 존재하지만
        switch (LayerCol->GetReferenceMode()) //정점은 6개가 존재한다. 평소의 VB IB 관계랑 비슷하다 생각하면 될듯
        {

        case FbxLayerElementUV::eDirect: //n번째 요소에 대한 매핑 정보가 directArray에서 n번째 위치에서 발견되었다.
        {
            color = LayerCol->GetDirectArray().GetAt(VertexIndex);
            break;
        }
        case FbxLayerElementUV::eIndexToDirect: // indexArray의 요소가 directArray에 인덱스가 존재한다.
        {
            int id = LayerCol->GetIndexArray().GetAt(VertexIndex);
            color = LayerCol->GetDirectArray().GetAt(id);
            break;
        }

        }
        break;
    }
    case FbxLayerElementUV::eByPolygonVertex: // 정점마다 하나의 매핑 좌표가 있다
    {
        switch (LayerCol->GetReferenceMode())
        {
        case FbxLayerElementUV::eDirect:
        {
            color = LayerCol->GetDirectArray().GetAt(uvIndex);
            break;
        }
        case FbxLayerElementUV::eIndexToDirect:
        {
            int id = LayerCol->GetIndexArray().GetAt(uvIndex);
            color = LayerCol->GetDirectArray().GetAt(id);
            break;
        }
        }
    }

    }

    return color;
}
FbxVector2 MyFbxImport::ReadTextureCoordinate(FbxLayerElementUV* LayerUV,int VertexIndex ,int uvIndex)
{
    FbxVector2 uv;

    switch (LayerUV->GetMappingMode())
    {
    case FbxLayerElementUV::eByControlPoint: //제어점 마다 하나의 매핑 좌표가 있다
    {                                        //하나의 사각형은 삼각형 두 개로 이루어지며 하나의 사각형에는 제어점이 4개 존재하지만
        switch (LayerUV->GetReferenceMode()) //정점은 6개가 존재한다. 평소의 VB IB 관계랑 비슷하다 생각하면 될듯
        {

             case FbxLayerElementUV::eDirect: //n번째 요소에 대한 매핑 정보가 directArray에서 n번째 위치에서 발견되었다.
                 {
                      uv = LayerUV->GetDirectArray().GetAt(VertexIndex);
                      break;
                    
                 }
             case FbxLayerElementUV::eIndexToDirect: // indexArray의 요소가 directArray에 인덱스가 존재한다.
                 { 
                       int id = LayerUV->GetIndexArray().GetAt(VertexIndex);
                       uv = LayerUV->GetDirectArray().GetAt(id);
                     break;
                 }

        }
        break;
    }
    case FbxLayerElementUV::eByPolygonVertex: // 정점마다 하나의 매핑 좌표가 있다
    {
        switch (LayerUV->GetReferenceMode())
        {
        case FbxLayerElementUV::eDirect:
        case FbxLayerElementUV::eIndexToDirect:
        {
            uv = LayerUV->GetDirectArray().GetAt(uvIndex);
            break;
        }
        }
        break;
    }

    }

    return uv;
}
int MyFbxImport::GetSubMaterialIndex(FbxLayerElementMaterial* LayerMtrl, int Polygon)
{
    int submtrl=0;

    switch (LayerMtrl->GetMappingMode())
    {
    case FbxLayerElementUV::eByPolygon: // 정점마다 하나의 매핑 좌표가 있다
    {
        switch (LayerMtrl->GetReferenceMode())
        {
             case FbxLayerElementUV::eIndex: //n번째 요소에 대한 매핑 정보가 directArray에서 n번째 위치에서 발견되었다.
             {
                 submtrl = Polygon;
                 break;
             }
             case FbxLayerElementUV::eIndexToDirect: // indexArray의 요소가 directArray에 인덱스가 존재한다.
             {

                 submtrl = LayerMtrl->GetIndexArray().GetAt(Polygon);
                 break;
             }
             default:
             {
                 break;
             }
        }
    }

    }

    return submtrl;
}

std::string MyFbxImport::ParseMatarial(FbxSurfaceMaterial* pmtrl)
{
    std::string TextureName;
    auto ProPerty = pmtrl->FindProperty(FbxSurfaceMaterial::sDiffuse);
    if (ProPerty.IsValid())
    {
        FbxFileTexture* Texture = ProPerty.GetSrcObject<FbxFileTexture>(0);
        if (Texture != nullptr)
        {
            std::string Filename = Texture->GetFileName();
            return Filename;
        }

    }
    
    return TextureName;
   
}

TMatrix MyFbxImport::ParseTransform(FbxNode* pNode)
{
    FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
    FbxTime time;
    time.SetFrame(0.0f, TimeMode);
    
    FbxAMatrix fbxmat = pNode->EvaluateGlobalTransform(time);
    TMatrix mat = ConvertAMatrix(fbxmat);
    mat = ConvertToDxMat(mat);
    return mat;
}
TMatrix MyFbxImport::ConvertMatrix(FbxMatrix& fbxmat)
{
    TMatrix mat;

    float* pMatArray = reinterpret_cast<float*>(&mat);
    double* pSrcArray = reinterpret_cast<double*>(&fbxmat);

    for (int i = 0; i < 16; ++i)
    {
        pMatArray[i] = pSrcArray[i];
    }

    return mat;

}
TMatrix MyFbxImport::ConvertToDxMat(TMatrix& mat)
{
    TMatrix ret;

    ret._11 = mat._11;  ret._12 = mat._13;  ret._13 = mat._12; ret._14 = 0.0f;
    ret._21 = mat._31;  ret._22 = mat._33;  ret._23 = mat._32; ret._24 = 0.0f;
    ret._31 = mat._21;  ret._32 = mat._23;  ret._33 = mat._22; ret._34 = 0.0f;
    ret._41 = mat._41;  ret._42 = mat._43;  ret._43 = mat._42; ret._44 = 1.0f;

    return ret;
}
void MyFbxImport::GetAnimationData(CFbxObj* fbxobj)
{
    FbxTime::SetGlobalTimeMode(FbxTime::eFrames50);
    FbxAnimStack* AnimStack = m_FbxScene->GetSrcObject<FbxAnimStack>();
    if (AnimStack == nullptr)return;
    FbxString TakeName = AnimStack->GetName();
    FbxTakeInfo* TakeInfo = m_FbxScene->GetTakeInfo(TakeName);
    FbxTimeSpan TimeSpan = TakeInfo->mLocalTimeSpan;
    FbxTime StartTime = TimeSpan.GetStart();
    FbxTime EndTime = TimeSpan.GetStop();

    FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
    FbxLongLong StartFrame = StartTime.GetFrameCount();
    FbxLongLong EndFrame = EndTime.GetFrameCount();

     fbxobj->m_AnimInfo.m_iStartFrame = StartFrame;
     fbxobj->m_AnimInfo.m_iEndFrame = EndFrame;
     fbxobj->m_AnimInfo.m_iFrameSpeed = 30;
     fbxobj->m_AnimInfo.m_iTickPerFrame = 160;
          


    FbxTime time;
    for (int iTime = StartFrame; iTime < EndFrame; ++iTime)
    {
        time.SetFrame(iTime, TimeMode);
        for (int iNode = 0; iNode < m_pMeshNodeList.size(); ++iNode)
        {
            std::shared_ptr<CFbxMesh> mesh = fbxobj->m_MeshList[iNode];
            std::vector<FbxAMatrix> MatrixFrame;

            FbxAMatrix fbxmat = m_pMeshNodeList[iNode]->EvaluateGlobalTransform(time);
            TMatrix mat = ConvertAMatrix(fbxmat);
            mat = ConvertToDxMat(mat);
            mesh->m_vMatrixArr.push_back(mat);


        }
    }
   



}
TMatrix MyFbxImport::ConvertAMatrix(FbxAMatrix& fbxamat)
{
    TMatrix mat;

    float* pMatArray=reinterpret_cast<float*>(&mat);
    double* pSrcArray=reinterpret_cast<double*>(&fbxamat);

    for (int i = 0; i < 16; ++i)
    {
        pMatArray[i] = pSrcArray[i];
    }



    return mat;

}

bool MyFbxImport::Init()
{

    
    m_FbxManager=FbxManager::Create();
    m_FbxImporter=FbxImporter::Create(m_FbxManager, IOSROOT);
    m_FbxScene=FbxScene::Create(m_FbxManager,"");


    return true;
}

bool MyFbxImport::Release()
{
   if(m_FbxScene)    m_FbxScene->Destroy();
   if (m_FbxImporter) m_FbxImporter->Destroy();
   if (m_FbxManager)  m_FbxManager->Destroy();

   m_FbxScene = nullptr;
   m_FbxImporter = nullptr;
   m_FbxManager = nullptr;
   if (!m_pMeshNodeList.empty())
   {
       m_pMeshNodeList.clear();
   }
 
   return true;
}
