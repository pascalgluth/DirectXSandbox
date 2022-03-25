#pragma once

#include <Audio.h>
#include <DirectXMath.h>
#include <memory>
#include <string>

namespace GameplayStatics
{
    bool Init();
    void Update(float dt);

    struct Sound
    {
        Sound(const std::string& file);
        std::unique_ptr<DirectX::SoundEffect> effect;
        bool loaded = false;
    };
    void PlaySound3D(const Sound* sound, DirectX::XMFLOAT3 location);
    void PlaySound2D(const Sound* sound, float volume = 1.f);
}
