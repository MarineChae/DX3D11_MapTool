#pragma once
#include "CDefine.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma comment( lib, "fmod_vc.lib")

class CSound
{
public:
	std::wstring m_FileName;
	std::wstring m_FilePath;

public:

	FMOD::System* m_pSystem;
	FMOD::Sound* m_pSound;
	FMOD::Channel* m_pChannel;
public:
	void Set(FMOD::System* pSystem)	{ m_pSystem = pSystem; }
	void SoundPlay(bool IsLoop);
	void EffectSoundPlay();
	void SoundStop();
	void SooundPause();
	void SetName(std::wstring Key){	m_FileName = Key;}

public:

	bool Load(std::wstring filename);
	bool Init();
	bool Frame();
	bool Release();
};
class CSoundMgr
{
public:
	FMOD::System* m_pSystem;
	std::map < std::wstring, std::unique_ptr<CSound>> m_List;
public:
	
	CSound* Load(std::wstring FileName);
	CSound* GetPtr(std::wstring FileName);
	bool Get(std::wstring FileName, CSound& texture);
	bool Frame();
	bool Release();
	static CSoundMgr& GetInstance()
	{
		static CSoundMgr sound;
		return sound;

	}



private:
	CSoundMgr();
public:
	~CSoundMgr();

};
