#include "NetworkManager.h"

#include <future>
#include <vector>
#include <fmt/format.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

#include "Engine.h"
#include "Gui.h"
#include "Logger.h"
#include "ObjectManager.h"

NetMode currentMode;
std::string currentAddress;
uint16_t currentPort;
TCPsocket serverSocket;
std::future<void> listenerTask;

std::vector<struct ClientID> clients;

bool didInit = false;
bool doNetworkUpdate = false;

struct ClientID
{
    uint8_t id;
    TCPsocket client;

    ClientID(uint8_t _id, TCPsocket _client)
    {
        id = _id;
        client = _client;
    }

    ClientID(TCPsocket _socket)
    {
        id = 0;
        client = _socket;
    }

    ClientID()
    {
        id = 0;
        client = TCPsocket();
    }
};

struct Command
{
    NetCommand type;
    char args[100];

    Command(NetCommand _type, const std::string& _args)
    {
        type = _type;
        strcpy(args, _args.c_str());
    }

    Command() {}
};

ClientID thisClient;

void InitModeServer();
void InitModeClient();

void UpdateServer();
void UpdateClient();

void ListenServer();
void ListenClient();

void ShutdownServer();
void ShutdownClient();

void NetworkManager::Init(const std::string& address, uint16_t port, NetMode mode)
{
    currentMode = mode;
    currentAddress = address;
    currentPort = port;

    if (SDLNet_Init() < 0)
    {
        LOG_ERROR("Failed to initialize network.");
        return;
    }

    if (mode == MODE_SERVER) InitModeServer();
    if (mode == MODE_CLIENT) InitModeClient();
}

void InitModeServer()
{
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, NULL, currentPort) < 0)
    {
        LOG_ERROR("Failed to resolve host.");
        return;
    }
    
    LOG_INFO(fmt::format("Starting up server on {}:{}...", currentAddress, currentPort));
    
    serverSocket = SDLNet_TCP_Open(&ip);
    if (!serverSocket)
    {
        LOG_ERROR("Failed to start server on " + currentAddress + ":" + std::to_string(currentPort) + ". " + SDLNet_GetError());
        return;
    }

    LOG_INFO("Server listening on " + currentAddress + ":" + std::to_string(currentPort) + "");
    didInit = true;

    listenerTask = std::async(std::launch::async, &ListenServer);
}

void InitModeClient()
{
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, currentAddress.c_str(), currentPort) < 0)
    {
        LOG_ERROR("Failed to resolve host.");
        return;
    }
    
    LOG_INFO(fmt::format("Connecting to {}:{}...", currentAddress, currentPort));
    
    thisClient.client = SDLNet_TCP_Open(&ip);
    if (!thisClient.client)
    {
        LOG_ERROR("Failed to connect to " + currentAddress + ":" + std::to_string(currentPort) + ". " + SDLNet_GetError());
        return;
    }

    LOG_INFO("Connected to " + currentAddress + ":" + std::to_string(currentPort) + "");

    uint8_t *wdata = NULL;
    wdata = (uint8_t*)calloc(5, sizeof(uint8_t));
    for (int j =0;j<5;j++)
    {
        wdata[j] = rand()%256;
    }

    thisClient.id = *wdata;
    SDLNet_TCP_Send(thisClient.client, &thisClient.id, sizeof(thisClient.id));

    LOG_INFO("Created client. (" + std::to_string(thisClient.id) + ")");
    
    didInit = true;

    listenerTask = std::async(std::launch::async, &ListenClient);
}

void NetworkManager::Update()
{
    if (!didInit) return;

    if (currentMode == MODE_SERVER) UpdateServer();
    if (currentMode == MODE_CLIENT) UpdateClient();
}

void UpdateServer()
{
    TCPsocket client = SDLNet_TCP_Accept(serverSocket);
    if (client)
    {
        ClientID newClient(std::move(client));
        SDLNet_TCP_Recv(client, &newClient.id, sizeof(uint8_t));
        clients.push_back(std::move(client));
        
        LOG_INFO("Client connected. (" + std::to_string(newClient.id) + ")");
    }
}

void UpdateClient()
{
    doNetworkUpdate = true;
}

void ListenServer()
{
    
}

void ListenClient()
{
    bool run = true;
    while (run)
    {
        if (doNetworkUpdate)
        {
            Command netCommand;
            SDLNet_TCP_Recv(thisClient.client, &netCommand, sizeof(netCommand));

            LOG_INFO("Received network command");

            if (netCommand.type == CMD_LOADSCENE)
            {
                Engine::LoadScene(netCommand.args);
            }

            if (netCommand.type == CMD_DELETEOBJ)
            {
                Engine::GetObjectManager()->DeleteObjectByName(netCommand.args);
                Gui::SetSelectedObject("", nullptr);
            }
            
            doNetworkUpdate = true;
        }
    }
    
    LOG_WARN("Exiting...");
}

void NetworkManager::Shutdown()
{
    if (!didInit) return;

    if (currentMode == MODE_SERVER) ShutdownServer();
    if (currentMode == MODE_CLIENT) ShutdownClient();
}

void NetworkManager::ExecuteCommandMulticast(NetCommand cmd, const std::string& args)
{
    Command netCommand(cmd, args);

    for (int i = 0; i < clients.size(); ++i)
    {
        SDLNet_TCP_Send(clients[i].client, &netCommand, sizeof(netCommand));
    }
}

void ShutdownServer()
{
    for (int i = 0; i < clients.size(); ++i)
    {
        SDLNet_TCP_Close(clients[i].client);
    }
    SDLNet_TCP_Close(serverSocket);
}

void ShutdownClient()
{
    SDLNet_TCP_Close(thisClient.client);
}