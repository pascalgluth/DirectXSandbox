#include "GameplayStatics.h"
#include "Logger.h"
#include <Audio.h>

#include "Engine.h"

struct Sound3DInstance
{
    std::unique_ptr<DirectX::SoundEffectInstance> effectInstance;
    DirectX::AudioListener listener;
    DirectX::AudioEmitter emitter;

    Sound3DInstance() {}
};

std::unique_ptr<DirectX::AudioEngine> audioEngine;
std::vector<std::unique_ptr<DirectX::SoundEffect>*> effects;
std::vector<Sound3DInstance*> sound3dInstances;

bool GameplayStatics::Init()
{
    DirectX::AUDIO_ENGINE_FLAGS eflags = AudioEngine_EnvironmentalReverb | AudioEngine_ReverbUseFilters;
#ifdef _DEBUG
    eflags |= DirectX::AudioEngine_Debug;
#endif

    audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);
    if (!audioEngine || audioEngine->IsCriticalError())
    {
        LOG_ERROR("Failed to initialize audio.");
        return false;
    }
    audioEngine->SetReverb(DirectX::Reverb_ConcertHall);

    return true;
}

void GameplayStatics::Update(float dt)
{
    audioEngine->Update();

    for (auto*& sound : sound3dInstances)
    {
        sound->listener.SetPosition(Engine::GetGFX()->GetCamPos());
        sound->effectInstance->Apply3D(sound->listener, sound->emitter);
    }
}



GameplayStatics::Sound::Sound(const std::string& file)
{
    try
    {
        std::wstring wideFileName(file.begin(), file.end());
        effect = std::make_unique<DirectX::SoundEffect>(audioEngine.get(), wideFileName.c_str());
        if (effect) loaded = true;
        else loaded = false;
    }
    catch (...)
    {
        LOG_ERROR("Failed to load sound: " + file);
        loaded = false;
    }
}

void GameplayStatics::PlaySound3D(const Sound* sound, DirectX::XMFLOAT3 location)
{
    if (sound && sound->loaded)
    {
        Sound3DInstance inst;
        inst.effectInstance = sound->effect->CreateInstance(DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters);
        inst.emitter.SetPosition(location);
        inst.listener.SetPosition(Engine::GetGFX()->GetCamPos());
        inst.effectInstance->Play();
        sound3dInstances.push_back(&inst);
    }
}

void GameplayStatics::PlaySound2D(const Sound* sound, float volume)
{
    if (sound && sound->loaded)
    {
        float oldVolume = audioEngine->GetMasterVolume();
        if (volume != oldVolume) audioEngine->SetMasterVolume(volume);

        sound->effect->Play();

        audioEngine->SetMasterVolume(oldVolume);
    }
}
