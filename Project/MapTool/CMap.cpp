#include "CMap.h"
#include"CoreInterface.h"
#include"CTextureMgr.h"
#include"CShaderMgr.h"

bool CMap::Load(CMapDesc& MapDesc,std::vector<float> heightlist)
{
	CreateAlphaTexture(MapDesc.iCol, MapDesc.iRow);
	if (!CreateMap(MapDesc,heightlist))
	{
		return false;
	}

	return true;
}



bool CMap::GenerateVertexNormal()
{
	InitFaceNormal();
	GenNormalLookupTable();
	CalcPerVertexNormalsFastLookup();

	return true;
}

void CMap::CalcVertexColor(TVector3 vLightDir)
{
	for (int iRow = 0; iRow < m_iRow; iRow++)
	{
		for (int iCol = 0; iCol < m_iCol; iCol++)
		{

			int  iVertexIndex = iRow * m_iCol + iCol;
			
			float fDot = D3DXVec3Dot(&vLightDir, &m_vVertexList[iVertexIndex].Nor);
			
			m_vVertexList[iVertexIndex].Col *= fDot;
			m_vVertexList[iVertexIndex].Col.w = 1.0f;
		}
	}
}

void CMap::InitFaceNormal()
{
	m_pFaceNormals = new TVector3[m_iNumFace];

	for (int i = 0; i < m_iNumFace; ++i)
	{

		m_pFaceNormals[i] = TVector3(0.0f, 0.0f, 0.0f);
	}

}
void CMap::GenNormalLookupTable()
{
	// We're going to create a table that lists, for each vertex, the
	// triangles which that vertex is a part of.

	if (m_pNormalLookupTable != NULL)
	{
		free(m_pNormalLookupTable);
		m_pNormalLookupTable = NULL;
	}

	// Each vertex may be a part of up to 6 triangles in the grid, so
	// create a buffer to hold a pointer to the normal of each neighbor.
	int buffersize = m_iRow * m_iCol * 6;

	m_pNormalLookupTable = (int*)malloc(sizeof(void*) * buffersize);
	for (int i = 0; i < buffersize; i++)
		m_pNormalLookupTable[i] = -1;

	// Now that the table is initialized, populate it with the triangle data.

	// For each triangle
	//   For each vertex in that triangle
	//     Append the triangle number to lookuptable[vertex]
	for (int i = 0; i < m_iNumFace; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			// Find the next empty slot in the vertex's lookup table "slot"
			for (int k = 0; k < 6; k++)
			{
				int vertex = m_vIndexList[i * 3 + j];
				if (m_pNormalLookupTable[vertex * 6 + k] == -1)
				{
					m_pNormalLookupTable[vertex * 6 + k] = i;
					break;
				}
			}
		}  // For each vertex that is part of the current triangle
	} // For each triangle
}

TVector3 CMap::ComputeFaceNormal(DWORD dwIndex0, DWORD dwIndex1, DWORD dwIndex2)
{
	TVector3 vNormal;
	TVector3 v0 = m_vVertexList[dwIndex1].Pos - m_vVertexList[dwIndex0].Pos;
	TVector3 v1 = m_vVertexList[dwIndex2].Pos - m_vVertexList[dwIndex0].Pos;

	D3DXVec3Cross(&vNormal,&v0,&v1);
	vNormal.Normalize();
	

	return vNormal;
}

void CMap::CalcFaceNormals()
{
	// Loop over how many faces there are
	int j = 0;
	for (int i = 0; i < m_iNumFace * 3; i += 3)
	{
		DWORD i0 = m_vIndexList[i];
		DWORD i1 = m_vIndexList[i + 1];
		DWORD i2 = m_vIndexList[i + 2];
		m_pFaceNormals[j] = ComputeFaceNormal(i0, i1, i2);
		j++;
	}
}

void CMap::CalcPerVertexNormalsFastLookup()
{
	// First, calculate the face normals for each triangle.
	CalcFaceNormals();

	// For each vertex, sum the normals indexed by the normal lookup table.
	int j = 0;
	for (int i = 0; i < m_iVertices; i++)
	{
		TVector3 avgNormal;
		avgNormal = TVector3(0.0f, 0.0f, 0.0f);

		// Find all the triangles that this vertex is a part of.
		for (j = 0; j < 6; j++)
		{
			int triIndex;
			triIndex = m_pNormalLookupTable[i * 6 + j];

			// If the triangle index is valid, get the normal and average it in.
			if (triIndex != -1)
			{
				avgNormal += m_pFaceNormals[triIndex];
			}
			else
				break;
		}

		// Complete the averaging step by dividing & normalizing.
		_ASSERT(j > 0);
		avgNormal.x /= (float)j;//.DivConst( (float)(j) );
		avgNormal.y /= (float)j;
		avgNormal.z /= (float)j;
		avgNormal.Normalize();

		// Set the vertex normal to this new normal.
		m_vVertexList[i].Nor.x = avgNormal.x;
		m_vVertexList[i].Nor.y = avgNormal.y;
		m_vVertexList[i].Nor.z = avgNormal.z;

	}  // For each vertex

	/////////////////////////////////////////////////////////////////
	// 페이스 노말 계산 및  이웃 페이스 인덱스 저장하여 정점 노말 계산
	/////////////////////////////////////////////////////////////////	
	if (m_bStaticLight) CalcVertexColor(m_vLightDir);
}

float CMap::GetHeightVertex(UINT index)
{
	return 0.0f;
}
TVector3 CMap::GetNormalVertex(UINT index)
{
	return TVector3(0.0f, 1.0f, 0.0f);
}
TVector4 CMap::GetColorVertex(UINT index)
{
	return TVector4(1.0f, 1.0f, 1.0f, 1.0f);
}
TVector2 GetTextureVertex(float offsetX, float offsetY)
{
	return TVector2(offsetX, offsetY);
}





bool CMap::CreateMap(CMapDesc& MapDesc, std::vector<float> heightlist)
{
	/*if (MapDesc.iCol > 1025 || MapDesc.iRow > 1025)
	{
		MapDesc.iCol = 1025;
		MapDesc.iRow = 1025;
	}*/
	
	m_iRow = MapDesc.iRow;
	m_iCol = MapDesc.iCol;
	m_iCellRow = MapDesc.iRow - 1;
	m_iCellCol = MapDesc.iCol - 1;
	m_iVertices = m_iRow * m_iCol;
	m_iNumFace = m_iCellRow * m_iCellCol * 2;
	m_fCellDistance = MapDesc.fCellDistance;
	m_MapDesc = MapDesc;
	Create(m_MapDesc.szTextureName.c_str(), m_MapDesc.szShaderName.c_str());




	return true;
}



bool CMap::CreateVertexData()
{
	m_vVertexList.resize(m_iVertices);

	//float fHalfCols = (m_iCol - 1) / 2.0f;
	//float fHalfRows = (m_iRow - 1) / 2.0f;
	//
	//
	//
	//
	//for (int Row = 0; Row < m_iRow; ++Row)
	//{
	//	for (int Col = 0; Col < m_iCol; ++Col)
	//	{
	//		int VertexIndex = Row * m_iCol + Col;
	//		m_vVertexList[VertexIndex].Pos.x = (Col - fHalfCols) * m_fCellDistance;
	//		m_vVertexList[VertexIndex].Pos.z = -((Row - fHalfRows) * m_fCellDistance);
	//		m_vVertexList[VertexIndex].Pos.y = GetHeightVertex(VertexIndex);
	//		
	//		m_vVertexList[VertexIndex].Nor= GetNormalVertex(VertexIndex);
	//		m_vVertexList[VertexIndex].Col= GetColorVertex(VertexIndex);
	//		m_vVertexList[VertexIndex].Tex = GetTextureVertex(fOffSetU*Col , fOffSetV*Row);
	//
	//	}
	//}
	float fOffSetU = 1.0f / (m_iCol - 1);
	float fOffSetV = 1.0f / (m_iRow - 1);
	float fHalfCols = (m_iCol - 1) * 0.5f;
	float fHalfRows = (m_iRow - 1) * 0.5f;
	m_iCellRow = (m_iRow - 1);
	m_iCellCol = (m_iCol - 1);
	m_iNumFace = m_iCellRow * m_iCellCol * 2;

	for (int iRow = 0; iRow < m_iRow; iRow++)
	{
		for (int iCol = 0; iCol < m_iCol; iCol++)
		{
			int  iVertexIndex = iRow * m_iCol + iCol;
			m_vVertexList[iVertexIndex].Pos.x = (iCol - fHalfCols) * m_fCellDistance;
			m_vVertexList[iVertexIndex].Pos.z = -((iRow - fHalfRows) * m_fCellDistance);
			m_vVertexList[iVertexIndex].Pos.y = GetHeightVertex(iVertexIndex)*1;
			m_vVertexList[iVertexIndex].Nor = GetNormalVertex(iVertexIndex);
			m_vVertexList[iVertexIndex].Col = GetColorVertex(iVertexIndex);
			m_vVertexList[iVertexIndex].Tex = GetTextureVertex(fOffSetU * iCol, fOffSetV * iRow);

		}
	}

	


	return true;
}

bool CMap::CreateIndexData()
{
	m_vIndexList.resize(m_iNumFace * 3);

	int CurIndex = 0;
	for (int Row = 0; Row < m_iCellRow; ++Row)
	{

		for (int Col = 0; Col < m_iCellCol; ++Col)
		{

			int NextRow = Row + 1;
			int NextCol = Col + 1;

			m_vIndexList[CurIndex + 0] = Row * m_iCol + Col;
			m_vIndexList[CurIndex + 1] = Row * m_iCol + NextCol;
			m_vIndexList[CurIndex + 2] = NextRow * m_iCol + Col;
			m_vIndexList[CurIndex + 3] = m_vIndexList[CurIndex + 2];
			m_vIndexList[CurIndex + 4] = m_vIndexList[CurIndex + 1];
			m_vIndexList[CurIndex + 5] = NextRow * m_iCol + NextCol;

			CurIndex += 6;

		}
	}
	
	GenerateVertexNormal();
	CreateTangentVector();
	return true;
}
bool CMap::Init()
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	return true;
}

bool CMap::Frame()
{
	return true;
}



bool CMap::Release()
{

	if (m_pNormalLookupTable)
	{
		free(m_pNormalLookupTable);
		m_pNormalLookupTable = NULL;
	}
	if (m_pFaceNormals != NULL)
	{
		delete[]  m_pFaceNormals;
		m_pFaceNormals = NULL;
	}
	if (m_pFaceNormal != NULL)
	{
		delete[]  m_pFaceNormal;
		m_pFaceNormal = NULL;
	}
	if (m_fLookup != NULL)
	{
		delete[] m_fLookup;
		m_fLookup = NULL;
	}
	


	return true;
}
CMap::CMap(void)
{
	m_iNumFace = 0;
	
	m_iCol = 0;
	m_iRow = 0;
	m_vLightDir = { 0,1,0 };
	m_pNormalLookupTable = nullptr;
	m_pFaceNormals = nullptr;
	
	
}
void CMap::SetMatrix(TMatrix* WolrdMatrix, TMatrix* ViewMatrix, TMatrix* ProjMatrix)
{
	if (WolrdMatrix != nullptr)
	{
		m_WolrdMatrix = *WolrdMatrix;
	}
	if (ViewMatrix != nullptr)
	{
		m_ViewMatrix = *ViewMatrix;
	}
	if (ProjMatrix != nullptr)
	{
		m_ProjMatrix = *ProjMatrix;
	}

	m_ConstantData.WolrdMatrix = m_WolrdMatrix.Transpose();

	m_ConstantData.ViewMatrix = m_ViewMatrix.Transpose();

	m_ConstantData.ProjMatrix = m_ProjMatrix.Transpose();



	CoreInterface::g_pImmediateContext->UpdateSubresource(m_pConstantBuffer.Get(), 0, nullptr, &m_ConstantData, 0, 0);
}


HRESULT CMap::CreateAlphaTexture(DWORD dwWidth, DWORD dwHeight)
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Height = dwHeight;
	texDesc.Width = dwWidth;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	
	m_fLookup = new BYTE[dwWidth * dwHeight * 4];
	
	for (UINT x = 0; x < dwHeight; ++x)
	{
		for (UINT y = 0; y < dwWidth; ++y)
		{
			BYTE* pixel = &m_fLookup[y * 4 + x * dwWidth * 4];
			pixel[0] = 0;
			pixel[1] = 0;
			pixel[2] = 0;
			pixel[3] = 0;
		}
	}

	D3D11_SUBRESOURCE_DATA initialData;
	initialData.pSysMem = m_fLookup;
	initialData.SysMemPitch = sizeof(BYTE) * 4 * dwWidth;
	initialData.SysMemSlicePitch = 0;

	hr = CoreInterface::g_pDevice->CreateTexture2D(&texDesc, &initialData, m_pRoughnessLookUpTex.GetAddressOf());
	if (FAILED(hr))
	{
		if (m_fLookup)
		{
			delete[] m_fLookup;
			m_fLookup = NULL;
		}
		
		return hr;
	}

	// Create a view onto the texture
	hr = CoreInterface::g_pDevice->CreateShaderResourceView(m_pRoughnessLookUpTex.Get(), NULL, m_pLookupRV.GetAddressOf());
	if (FAILED(hr))
	{
		if (m_fLookup)
		{
			delete[] m_fLookup;
			m_fLookup = NULL;
		}
		return hr;
	}
	

	return hr;
}

void CMap::CreateTangentVector()
{
	TVector3 vTangent, vBiNormal, vNormal;
	int iIndex = 0;
	int i0, i1, i2;

	m_vTangentList.resize(m_iVertices);

	for (int iIndex = 0; iIndex < m_vIndexList.size(); iIndex += 3)
	{
		i0 = m_vIndexList[iIndex + 0];
		i1 = m_vIndexList[iIndex + 1];
		i2 = m_vIndexList[iIndex + 2];

		m_NormalMap.CreateTangentSpaceVectors(&m_vVertexList[i0].Pos, &m_vVertexList[i1].Pos, &m_vVertexList[i2].Pos,
			m_vVertexList[i0].Tex, m_vVertexList[i1].Tex, m_vVertexList[i2].Tex,
			&vTangent, &vBiNormal, &m_vVertexList[i0].Nor);

		m_vTangentList[i0] += vTangent;

		i0 = m_vIndexList[iIndex + 1];
		i1 = m_vIndexList[iIndex + 2];
		i2 = m_vIndexList[iIndex + 0];

		m_NormalMap.CreateTangentSpaceVectors(&m_vVertexList[i0].Pos, &m_vVertexList[i1].Pos, &m_vVertexList[i2].Pos,
			m_vVertexList[i0].Tex, m_vVertexList[i1].Tex, m_vVertexList[i2].Tex,
			&vTangent, &vBiNormal, &m_vVertexList[i0].Nor);

		m_vTangentList[i0] += vTangent;

		i0 = m_vIndexList[iIndex + 2];
		i1 = m_vIndexList[iIndex + 0];
		i2 = m_vIndexList[iIndex + 1];

		m_NormalMap.CreateTangentSpaceVectors(&m_vVertexList[i0].Pos, &m_vVertexList[i1].Pos, &m_vVertexList[i2].Pos,
			m_vVertexList[i0].Tex, m_vVertexList[i1].Tex, m_vVertexList[i2].Tex,
			&vTangent, &vBiNormal, &m_vVertexList[i0].Nor);

		m_vTangentList[i0] += vTangent;

	}
	for (int i = 0; i < m_iRow * m_iCol; i++)
	{
		D3DXVec3Normalize(&m_vTangentList[i], &m_vTangentList[i]);
	}


	
	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = sizeof(TVector3) * m_vTangentList.size();
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA InitialData;
	InitialData.pSysMem = &m_vTangentList.at(0);
	CoreInterface::g_pDevice->CreateBuffer(
		&Desc,
		&InitialData,
		m_pTangentVB.GetAddressOf());

	UINT Stride = sizeof(TVector3);
	UINT offset = 0;
	
}

		