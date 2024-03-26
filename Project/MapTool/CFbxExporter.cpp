#include "CFbxExporter.h"

//파일이 없다면 생성해줌
FILE* CFbxExporter::CreateFbxFile(const char* Filename, const char* Mode)
{
	m_Fp = fopen(Filename, Mode);
	if (m_Fp != nullptr)
	{
		return m_Fp;
	}

	return nullptr;
}

void CFbxExporter::CloseFile()
{

	fclose(m_Fp);

}
