#pragma once
#include "CDefine.h"

class CParticle
{
public:
	bool m_bValid;
	TVector3 m_vPos;
	TVector3 m_vScale;
	TVector3 m_vDir;
	double m_AccumulatedTime = 0.0;
	double m_HitDelayTime;
	double m_SustainmentTime;
	double m_fDelay = 0.0;
	int m_iIndex = 0;
	int iID;
	int m_iMaxFrame = 0;
	CRect m_RT;
	void Frame()
	{

		m_AccumulatedTime += g_fSecondPerFrame;

		if (m_AccumulatedTime >= m_fDelay)
		{
			m_iIndex++;
			if (m_iIndex >= m_iMaxFrame)
			{

				m_iIndex = 0;
				m_bValid = false;
			}
			m_AccumulatedTime -= m_fDelay;
		}



	}
};

