#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>

class CFbxExporter
{

	FILE* m_Fp;


public:
	FILE* CreateFbxFile(const char* Filename, const char* Mode);
	void CloseFile();




};

