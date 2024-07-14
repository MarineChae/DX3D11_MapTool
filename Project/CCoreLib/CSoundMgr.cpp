#include "CSoundMgr.h"

void CSound::SoundPlay(bool IsLoop)
{
    if (m_pChannel == nullptr)
    {
        if(IsLoop) m_pSound->setMode(FMOD_LOOP_NORMAL);
        else  m_pSound->setMode(FMOD_LOOP_OFF);
        m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
        m_pChannel->setVolume(0.05);
    }
    else
    {
        bool play;
        m_pChannel->isPlaying(&play);
        if (play)
        {
            return;
        }
        else
        {
            if (IsLoop) m_pSound->setMode(FMOD_LOOP_NORMAL);
            else  m_pSound->setMode(FMOD_LOOP_OFF);
            m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
            m_pChannel->setVolume(0.05);
        }

    }
  
}

void CSound::EffectSoundPlay()
{
    m_pSystem->playSound(m_pSound, nullptr, false, &m_pChannel);
    m_pChannel->setVolume(0.03);
}

void CSound::SoundStop()
{
    m_pChannel->stop();

}

void CSound::SooundPause()
{
    bool IsPause;

    m_pChannel->getPaused(&IsPause);
    m_pChannel->setPaused(!IsPause);

}

bool CSound::Load(std::wstring filename)
{
    std::string file = wtm(filename);
    m_pSystem->createSound(file.c_str(),FMOD_DEFAULT,0,&m_pSound);
    return true;
}

bool CSound::Init()
{
    return true;
}

bool CSound::Frame()
{
    return true;
}

bool CSound::Release()
{
    if (m_pSound)m_pSound->release();
    return true;
}

CSound* CSoundMgr::Load(std::wstring FileName)
{
    std::size_t found = FileName.find_last_of(L"/");
    std::wstring path = FileName.substr(0, found + 1);
    std::wstring Key = FileName.substr(found + 1);
    CSound* data = GetPtr(Key);

    if (data != nullptr)
    {
        return data;

    }
    std::unique_ptr<CSound> NewData = std::make_unique<CSound>();
    NewData->Set(m_pSystem);
    NewData->SetName(Key);

    if (NewData->Load(FileName))
    {
        m_List.insert(std::make_pair(Key, std::move(NewData)));
        return GetPtr(Key);
    }

    
    return nullptr;
}

 CSound* CSoundMgr::GetPtr(std::wstring FileName)
{
    auto iter = m_List.find(FileName);
    if (m_List.end() == iter)
    {
        return nullptr;
    }

    return iter->second.get();

}

bool CSoundMgr::Get(std::wstring FileName, CSound& texture)
{
    auto iter = m_List.find(FileName);
    if (m_List.end() == iter)
    {
        return false;
    }
    texture = *(iter->second);

    return true;

}

bool CSoundMgr::Frame()
{
    m_pSystem->update();
    for (auto& data : m_List)
    {
        data.second->Frame();
    }
    return true;
   
}

bool CSoundMgr::Release()
{
    for (auto& data : m_List)
    {
        data.second->Release();
    }
    m_List.clear();

    m_pSystem->close();
    m_pSystem->release();
    return true;
}


CSoundMgr::CSoundMgr()
{
    FMOD::System_Create(&m_pSystem);
    m_pSystem->init(64, FMOD_INIT_NORMAL, 0);

}

CSoundMgr::~CSoundMgr()
{
    
}
