#pragma once
#include"MyFbxImport.h"



class CModelMgr
{
public:
	MyFbxImport m_pFbxImpoter;
	std::map<std::wstring, std::shared_ptr<CFbxObj>> m_FbxObjList;
public:
	static CModelMgr& GetInstance()
	{
		static CModelMgr mgr;
		return mgr;
	}
public:
	std::shared_ptr<CFbxObj> Load(std::wstring FileName);
	std::shared_ptr<CFbxObj> GetPtr(std::wstring Key);
public:
	bool Release();

private:
	

public:
	CModelMgr();
	virtual ~CModelMgr();




};

