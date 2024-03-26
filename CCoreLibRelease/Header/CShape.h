#pragma once
#include"CObject.h"

class CShape : public CObject 
{

public:
	void CreateOBBBox(float fExtX = 1.0f, float fExtY = 1.0f, float fExtZ = 1.0f,
				   	TVector3 vCenter = TVector3(0.0f, 0.0f, 0.0f),
				   	TVector3 vDirX = TVector3(1.0f, 0.0f, 0.0f),
				   	TVector3 vDirY = TVector3(0.0f, 1.0f, 0.0f),
				   	TVector3 vDirZ = TVector3(0.0f, 0.0f, 1.0f));


};

class TLineShape : public CShape
{

};

class CBoxShape : public CShape
{
public:
	bool CreateVertexData();
	bool CreateIndexData();
	bool CreateResouce();
public:
	CBoxShape();
	~CBoxShape();

};

class CPlaneShape : public CShape
{


};