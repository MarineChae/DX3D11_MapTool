#include "CHeightMap.h"
#include"CoreInterface.h"


bool CHeightMap::CreateMap(CMapDesc& MapDesc, std::vector<float>HeightList)
{
	if (MapDesc.iCol > 1025 || MapDesc.iRow > 1025)
	{
		MapDesc.iCol = 1025;
		MapDesc.iRow = 1025;
	}
	
	if (!HeightList.empty())
		m_fHeightList = HeightList;

	m_iRow = MapDesc.iRow;
	m_iCol = MapDesc.iCol;
	m_iCellRow = MapDesc.iRow - 1;
	m_iCellCol = MapDesc.iCol - 1;
	m_iVertices = m_iRow * m_iCol;
	m_iNumFace = m_iCellRow * m_iCellCol * 2;
	m_fCellDistance = MapDesc.fCellDistance;
	m_MapDesc = MapDesc;
	

	CreateConstantBuffer();
	CreateVertexData();
	CreateIndexData();
	//CreateIndexBuffer();
	CreateVertexBuffer();
	m_pShader = CShaderMgr::GetInstance().Load(MapDesc.szShaderName);
	CreateInputLayout();
	m_pTexture = CTextureMgr::GetInstance().Load(MapDesc.szTextureName);
	UpdataMatrix();
	



	return true;
}
bool CHeightMap::CreateIndexBuffer(std::vector<DWORD> m_TreeIndexList)
{

	
	D3D11_BUFFER_DESC Desc;
	Desc.ByteWidth = sizeof(DWORD) * m_TreeIndexList.size();//������ ũ��
	Desc.Usage = D3D11_USAGE_DEFAULT;//���۸� ��𿡼� ����������
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//������ ���뵵
	Desc.CPUAccessFlags = 0;//cpu�� ���������� �ʿ������ null
	Desc.MiscFlags = 0;//�߰����� �ɼ�
	Desc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA pInitialData;
	pInitialData.pSysMem = &m_TreeIndexList.at(0);

	CoreInterface::g_pDevice->CreateBuffer(&Desc, &pInitialData, &m_pIndexBuffer);

	

	
	return true;

}
bool CHeightMap::CreateHeightMap(std::wstring HeightMapName,int size)
{
	if (!HeightMapName.empty())
	{
		HRESULT hr;
		size_t maxsize = 0;
		auto ImageObj = std::make_unique<DirectX::ScratchImage>();
		DirectX::TexMetadata mData;
		//���̸� �ؽ����� �����͸� �����´�
		hr = DirectX::GetMetadataFromWICFile(HeightMapName.c_str(), DirectX::WIC_FLAGS_NONE, mData);
		if (SUCCEEDED(hr))
		{
			hr = DirectX::LoadFromWICFile(HeightMapName.c_str(), DirectX::WIC_FLAGS_NONE, &mData, *ImageObj);
			if (SUCCEEDED(hr))
			{

			}
		}
		//���簢���� �ƴѰ�� ������ ���簢������ ������ش�.
		if (!CheckSquare(mData.width - 1))
		{
			mData.width = ResizeMap(mData.width);

		}
		if (!CheckSquare(mData.height - 1))
		{
			mData.height = ResizeMap(mData.height);

		}
		m_fHeightList.resize(mData.height * mData.width);
	// ������ ���̸��� �ȼ� �����͸� ������ ������ ���� ���� ����Ʈ�� �־��ش�.
		UCHAR* pTexels = (UCHAR*)ImageObj->GetImages()->pixels;
		PNCT_VERTEX	v;
		for (UINT row = 0; row < mData.height; row++)
		{
			UINT rowStart = row * ImageObj->GetImages()->rowPitch;
			for (UINT col = 0; col < mData.width; col++)
			{
				UINT colStart = col * 4;
				UINT uRed = pTexels[rowStart + colStart + 0];
				m_fHeightList[row * mData.width + col] = (float)uRed * 0.3;	/// DWORD�̹Ƿ� pitch/4	
			}
		}


		m_iRow = mData.height;
		m_iCol = mData.width;



	}
	else
	{
		//���̸��� ���°�� 0���� �ʱ�ȭ ���ش�
		if (!CheckSquare(size - 1))
		{
			size = ResizeMap(size);

		}
		if (!CheckSquare(size - 1))
		{
			size = ResizeMap(size);

		}
		m_fHeightList.resize(size * size);

		PNCT_VERTEX	v;
		for (UINT row = 0; row < size; row++)
		{
			for (UINT col = 0; col < size; col++)
			{
				m_fHeightList[row * size + col] = 0;	/// DWORD�̹Ƿ� pitch/4	
			}
		}


		m_iRow = size;
		m_iCol = size;



	}

	return true;
}
bool CHeightMap::CheckSquare(int n)
{

	return 	(n & (n - 1)) == 0;
}

int CHeightMap::ResizeMap(int n)
{
	if (n > 2 && n <= 4 )
	{
		return 5;
	}
	else if (n > 4 && n <= 8)
	{
		return 9;
	}
	else if (n > 8 && n <= 16)
	{
		return 17;
	}
	else if (n > 16 && n <= 32)
	{
		return 33;
	}
	else if (n > 32 && n <= 64)
	{
		return 65;
	}
	else if (n >64 && n <= 128)
	{
		return 129;
	}
	else if (n > 128 && n <= 256)
	{
		return 257;
	}
	else if (n > 256 && n <= 512)
	{
		return 513;
	}
	else if (n > 512 && n <= 1024)
	{
		return 1025;
	}
	return 1;

}
bool CHeightMap::Init(wstring HeightmapName,int size)
{
	
	CMap::Init();
	if (!HeightmapName.empty())
		CreateHeightMap(HeightmapName,0);
	else
		CreateHeightMap(L"", size);

	m_bStaticLight = true;
	CMapDesc MapDesc = { m_iCol,  m_iRow,1.0f,0.3f,L"../../Resource/000.jpg",L"../../Resource/MultiTex.hlsl" };

	Load(MapDesc);
	return true;
}
float CHeightMap::GetHeightVertex(UINT index)
{
	return m_fHeightList[index] * m_MapDesc.fScaleHeight;
}

TVector4 CHeightMap::GetColorVertex(UINT index)
{
	return TVector4(1,1,1,1);
}

float CHeightMap::GetHeightMap(int row, int col)
{
	return m_fHeightList[row*m_iRow+col] * m_MapDesc.fScaleHeight; 
}
