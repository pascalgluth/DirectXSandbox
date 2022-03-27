#include "Engine.h"
#include <Keyboard.h>
#include <Mouse.h>

#include "DirectoryHelperMacros.h"
#include "GameplayStatics.h"
#include "ObjectManager.h"

namespace Engine
{
    Graphics* gfx;
    ObjectManager* objManager;
    HWND hWnd;
    int newWidth;
    int newHeight;
    bool resizeQueued = false;

    std::unique_ptr<DirectX::Keyboard> keyboard;
    std::unique_ptr<DirectX::Mouse> mouse;

    GameplayStatics::Sound* mySound;
    
    void Initialize(HWND _hWnd, int width, int height)
    {
        resizeQueued = false;
        hWnd = _hWnd;

        keyboard = std::make_unique<DirectX::Keyboard>();
        mouse = std::make_unique<DirectX::Mouse>();
        mouse->SetWindow(hWnd);

        gfx = new Graphics();
        gfx->Initialize(hWnd, width, height);

        objManager = new ObjectManager(gfx->GetDevice(), gfx->GetDeviceContext());

        GameplayStatics::Init();

        mySound = new GameplayStatics::Sound(FILE_OTHER("dog_sound.wav"));
        //GameplayStatics::PlaySound2D(mySound);
        //GameplayStatics::PlaySound3D(mySound, gfx->GetCamPos());

        VisibleGameObject* block1 = objManager->CreateVisibleObject<VisibleGameObject>("block1", FILE_MODEL("Lowpoly_City_Free_Pack.fbx"), FILE_TEXTURE("Palette.jpg"));
        VisibleGameObject* block2 = objManager->CreateVisibleObject<VisibleGameObject>("block2", FILE_MODEL("Lowpoly_City_Free_Pack.fbx"), FILE_TEXTURE("Palette.jpg"));
        if (block2)
        {
            block2->SetPosition(block1->GetPosition());
            block2->AddOffset(2150.f, 0.f, 0.f);
        }
        VisibleGameObject* block3 = objManager->CreateVisibleObject<VisibleGameObject>("block3", FILE_MODEL("Lowpoly_City_Free_Pack.fbx"), FILE_TEXTURE("Palette.jpg"));
        if (block3)
        {
            block3->SetPosition(block1->GetPosition());
            block3->AddOffset(2150.f, 0.f, -2150.f);
        }
        VisibleGameObject* block4 = objManager->CreateVisibleObject<VisibleGameObject>("block4", FILE_MODEL("Lowpoly_City_Free_Pack.fbx"), FILE_TEXTURE("Palette.jpg"));
        if (block4)
        {
            block4->SetPosition(block1->GetPosition());
            block4->AddOffset(0.f, 0.f, -2150.f);
        }
    }

    void QueueResize(int width, int height)
    {
        newWidth = width;
        newHeight = height;
        resizeQueued = true;
    }

    void Shutdown()
    {
        delete gfx;
    }

    void Update()
    {
        objManager->Update(0);
        gfx->UpdateScene(0);
    }

    void Render()
    {
        gfx->RenderFrame(objManager);
    }

    bool ResizeQueued()
    {
        return resizeQueued;
    }

    WindowSize GetNewWindowSize()
    {
        WindowSize newSize;
        newSize.width = newWidth;
        newSize.height = newHeight;
        return newSize;
    }

    void GFXResize()
    {
        gfx->Resize(newWidth, newHeight);
    }

    void SetResizeComplete()
    {
        resizeQueued = false;
    }

    Graphics* GetGFX()
    {
        return gfx;
    }

    ObjectManager* GetObjectManager()
    {
        return objManager;
    }
}
