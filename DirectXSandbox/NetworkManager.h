#pragma once

#include <string>

enum NetMode { MODE_CLIENT, MODE_SERVER };

enum NetCommand
{
    CMD_LOADSCENE,
    CMD_DELETEOBJ
};

namespace NetworkManager
{
    void Init(const std::string& address, uint16_t port, NetMode mode);
    void Update();
    void Shutdown();

    void ExecuteCommandMulticast(NetCommand cmd, const std::string& args);
};
