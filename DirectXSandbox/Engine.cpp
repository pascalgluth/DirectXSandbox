#include "Engine.h"
#include <Keyboard.h>
#include <Mouse.h>
#include <json/json_struct.h>

#include "DirectoryHelperMacros.h"
#include "GameplayStatics.h"
#include "Logger.h"
#include "ObjectManager.h"
#include "SerializedTypes.h"
#include "NetworkManager.h"

namespace Engine
{
    Graphics* gfx;
    ObjectManager* objManager;
    HWND hWnd;
    int newWidth;
    int newHeight;
    bool resizeQueued = false;
    bool networkUpdate = false;

    std::unique_ptr<DirectX::Keyboard> keyboard;
    std::unique_ptr<DirectX::Mouse> mouse;

    GameplayStatics::Sound* mySound;
    
    void Initialize(HWND _hWnd, int width, int height)
    {
        srand(time(0));
        
        resizeQueued = false;
        hWnd = _hWnd;

        keyboard = std::make_unique<DirectX::Keyboard>();
        mouse = std::make_unique<DirectX::Mouse>();
        mouse->SetWindow(hWnd);

        gfx = new Graphics();
        gfx->Initialize(hWnd, width, height);

        NewScene();

        GameplayStatics::Init();

        mySound = new GameplayStatics::Sound(FILE_OTHER("dog_sound.wav"));
        //GameplayStatics::PlaySound2D(mySound);
        //GameplayStatics::PlaySound3D(mySound, gfx->GetCamPos());

        
#ifdef _SERVER
        NetworkManager::Init("localhost", 6666, MODE_SERVER);
#else
        NetworkManager::Init("localhost", 6666, MODE_CLIENT);
#endif
        
    }

    void QueueResize(int width, int height)
    {
        newWidth = width;
        newHeight = height;
        resizeQueued = true;
    }

    void Shutdown()
    {
        delete objManager;
        delete gfx;
        NetworkManager::Shutdown();
    }

    void Update()
    {
        // todo: use the correct delta time....

        NetworkManager::Update();
        
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

    void LoadScene(const std::string& file)
    {
        LOG_INFO("Loading scene...");

        std::ifstream ifs(file);
        std::stringstream ifbuffer;

        ifbuffer << ifs.rdbuf();
        std::string json = ifbuffer.str();

        for (int i = 0; i < json.length(); ++i)
        {
            if (json[i] == '\n') json[i] = ' ';
        }
        
        SerializedScene scene;
        JS::ParseContext context(json);
        context.parseTo(scene);

        delete objManager;
        objManager = new ObjectManager(gfx->GetDevice(), gfx->GetDeviceContext());

        for (int i = 0; i < scene.objects.size(); ++i)
        {
            GameObject* newObj;
            if (scene.objects[i].isVisibleObject)
                newObj =
                    objManager->CreateVisibleObject<VisibleGameObject>(scene.objects[i].name, scene.objects[i].model);
            else
                newObj = objManager->CreateObject<GameObject>(scene.objects[i].name);

            newObj->SetPosition(scene.objects[i].position.DXF3());
            newObj->SetRotation(scene.objects[i].rotation.DXF3());
            newObj->SetScale(scene.objects[i].scale.x, scene.objects[i].scale.y, scene.objects[i].scale.z);
        }
        
        LOG_INFO("Scene Loaded.");
    }

    void SaveScene(const std::string& file)
    {
        LOG_INFO("Saving scene...");
        
        SerializedScene scene(objManager);
        std::string jsonScene = JS::serializeStruct(scene);

        std::ofstream ofs;
        ofs.open(file, std::ofstream::out | std::ofstream::trunc);

        ofs << jsonScene;
        ofs.close();

        LOG_INFO("Saved Scene.");
    }

    void NewScene()
    {
        if (objManager) delete objManager;
        objManager = new ObjectManager(gfx->GetDevice(), gfx->GetDeviceContext());

        LOG_INFO("Created new scene. Don't forget to save it!");
    }
}
