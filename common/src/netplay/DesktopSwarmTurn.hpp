#pragma once

#ifdef _WIN32
#include <nlohmann/json_fwd.hpp>

#include <atomic>
#include <memory>
#include <string>

namespace Netplay
{
class DesktopSwarmTurn
{
public:
    DesktopSwarmTurn();
    ~DesktopSwarmTurn();
    std::string Start(const nlohmann::json &grant, const std::atomic_bool* cancelled);

private:
    struct Process;
    std::unique_ptr<Process> process_;
};
}
#endif
