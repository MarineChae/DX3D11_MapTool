#include "SaveLoader.h"

bool SaveLoader::SaveMap(shared_ptr<CQuadTree> tree, std::string filename, std::map<UINT, shared_ptr<InstanceObject>>list)
{
	FILE* fpWrite = nullptr;
	if (fopen_s(&fpWrite, filename.c_str(), "w") == 0)
	{
		bool bRet = true;
		//사이즈 저장
		std::string header = "#MapSize";
		bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
		bRet = fprintf_s(fpWrite, "%d\t", tree->m_pMap->m_iCol);
		bRet = fprintf_s(fpWrite, "%d\n", tree->m_pMap->m_iRow);

		//높이값 저장
		header = "#Mapheight";
		bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
		bRet = fprintf_s(fpWrite, "%d\n", static_cast<int>(tree->m_pMap->m_vVertexList.size()));
		tree->UpdateMapVertexList(); // 노드의 높이를 맵 높이와 같게 만듬
		for (auto& vertex : tree->m_pMap->m_vVertexList)
		{
			bRet = fprintf_s(fpWrite, "%f\n", vertex.Pos.y);//버텍스 높이 
		}

		
		for (auto& inst : list)
		{
			header = "#MapObject";
			bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
			bRet = fprintf_s(fpWrite, "%d\n", static_cast<int>(inst.second->m_InstanceList.first.size()));//오브젝트 개수
			for (auto& object : inst.second->m_InstanceList.first)
			{
				bRet = fprintf_s(fpWrite, "%s\t", wtm(object->m_MapObjName).c_str());
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Scale._11);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Scale._22);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Scale._33);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Rotation.x);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Rotation.y);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Rotation.z);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Transform._41);
				bRet = fprintf_s(fpWrite, "%f\t", object->m_Transform._42);
				bRet = fprintf_s(fpWrite, "%f\n", object->m_Transform._43);
			}
		}
		





		header = "#MapTexture";
		bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
		bRet = fprintf_s(fpWrite, "%s\t", wtm(L"../../Resource/" +tree->m_pTextureList[1]->m_csName).c_str());
		bRet = fprintf_s(fpWrite, "%s\t", wtm(L"../../Resource/" + tree->m_pTextureList[2]->m_csName).c_str());
		bRet = fprintf_s(fpWrite, "%s\t", wtm(L"../../Resource/" + tree->m_pTextureList[3]->m_csName).c_str());
		bRet = fprintf_s(fpWrite, "%s\n", wtm(L"../../Resource/" + tree->m_pTextureList[4]->m_csName).c_str());
		header = "#MapAlpha";
		bRet = fprintf_s(fpWrite, "%s\n", header.c_str());
		std::string alphafile = wtm(L"../../Resource/" + tree->m_pTextureList[0]->m_csName);
		bRet = fprintf_s(fpWrite, "%s\n", alphafile.c_str());




		fclose(fpWrite);
	}

	return true;
}

bool SaveLoader::LoadMap(std::string filename, std::map<UINT, shared_ptr<InstanceObject>>list)
{
	FILE* fpRead = nullptr;

	if (fopen_s(&fpRead, filename.c_str(), "rt") == 0)
	{
		TCHAR buffer[256] = { 0, };

		while (_fgetts(buffer, _countof(buffer), fpRead) != 0)
		{
			TCHAR type[36] = { 0, };

			_stscanf_s(buffer, _T("%s"), type, (unsigned int)_countof(type));
			
			if (_tcscmp(type, L"#MapSize") == 0)
			{
				_fgetts(buffer, _countof(buffer), fpRead);
				_stscanf_s(buffer, _T("\n %d %d "),
					&m_Mapsize[0], &m_Mapsize[1]);
			}
			else if (_tcscmp(type, L"#Mapheight") == 0)
			{
				_fgetts(buffer, _countof(buffer), fpRead);
				int vertexSize = 0;
				_stscanf_s(buffer, _T("%d"), &vertexSize);

				for (int i = 0; i < vertexSize; i++)
				{
					_fgetts(buffer, _countof(buffer), fpRead);
					float fheight = 0.0f;
					_stscanf_s(buffer, _T("%f"), &fheight);
					m_MapHeight.push_back(fheight);
				}
			}
			else if (_tcscmp(type, L"#MapTexture") == 0)
			{
				TCHAR tex1[36] = { 0, };
				TCHAR tex2[36] = { 0, };
				TCHAR tex3[36] = { 0, };
				TCHAR tex4[36] = { 0, };

				_fgetts(buffer, _countof(buffer), fpRead);
				_stscanf_s(buffer, _T("%s %s %s %s \n"),
					tex1, (unsigned int)_countof(tex1),
					tex2, (unsigned int)_countof(tex2),
					tex3, (unsigned int)_countof(tex3),
					tex4, (unsigned int)_countof(tex4));
				m_SubTexture.push_back(tex1);
				m_SubTexture.push_back(tex2);
				m_SubTexture.push_back(tex3);
				m_SubTexture.push_back(tex4);
			}
			else if (_tcscmp(type, L"#MapAlpha") == 0)
			{
				TCHAR tex[36] = { 0, };
				_fgetts(buffer, _countof(buffer), fpRead);
				_stscanf_s(buffer, _T("%s\n"), tex, (unsigned int)_countof(tex));
				m_AlphaTexture = tex;
			}
			else if (_tcscmp(type, L"#MapObject") == 0)
			{
				TCHAR value[36] = { 0, };
				_fgetts(buffer, _countof(buffer), fpRead);
				int objectAmount = 0;
				_stscanf_s(buffer, _T("%d"), &objectAmount);//몇개인지
				for (int i = 0; i < objectAmount; i++)
				{
					TMatrix mat;
					_fgetts(buffer, _countof(buffer), fpRead);
					_stscanf_s(buffer, _T("%s %f %f %f %f %f %f %f %f %f \n"), value, (unsigned int)_countof(value),
						&mat._11, &mat._12, &mat._13,
						&mat._21, &mat._22, &mat._23,
						&mat._31, &mat._32, &mat._33);
						
					
					m_MapObject.insert(std::make_pair(value, mat));
				}
			}
		}
		fclose(fpRead);
	}

    return true;
}



SaveLoader::SaveLoader()
{
}

SaveLoader::~SaveLoader()
{
}
