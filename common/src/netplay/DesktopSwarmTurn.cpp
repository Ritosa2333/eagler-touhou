#include "DesktopSwarmTurn.hpp"

#ifdef _WIN32
#include <nlohmann/json.hpp>
#include <windows.h>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <filesystem>
#include <iterator>
#include <string_view>
#include <thread>

namespace Netplay
{
namespace
{
struct Handle
{
    HANDLE value = nullptr;
    Handle() = default;
    Handle(const Handle &) = delete;
    Handle &operator=(const Handle &) = delete;
    ~Handle() { Close(); }
    void Close()
    {
        if (value) CloseHandle(value);
        value = nullptr;
    }
};

bool pipe(Handle &input, Handle &output, Handle &privateEnd)
{
    SECURITY_ATTRIBUTES attributes{sizeof(attributes), nullptr, TRUE};
    return CreatePipe(&input.value, &output.value, &attributes, 4096)
        && SetHandleInformation(privateEnd.value, HANDLE_FLAG_INHERIT, 0);
}
}

struct DesktopSwarmTurn::Process
{
    Handle child;
    Handle input;
    Handle output;
};

DesktopSwarmTurn::DesktopSwarmTurn() = default;
DesktopSwarmTurn::~DesktopSwarmTurn() = default;

std::string DesktopSwarmTurn::Start(const nlohmann::json &grant, const std::atomic_bool* cancelled)
{
    wchar_t module[32768];
    const auto length = GetModuleFileNameW(nullptr, module, DWORD(std::size(module)));
    if (!length || length >= std::size(module)) return {};
    const auto helper = std::filesystem::path(module).replace_filename(L"eagler-swarm-turn.exe");

    auto process = std::make_unique<Process>();
    Handle childInput, childOutput;
    if (!pipe(childInput, process->input, process->input) || !pipe(process->output, childOutput, process->output)) return {};
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    startup.dwFlags = STARTF_USESTDHANDLES;
    startup.hStdInput = childInput.value;
    startup.hStdOutput = childOutput.value;
    startup.hStdError = childOutput.value;
    PROCESS_INFORMATION child{};
    wchar_t command[] = L"eagler-swarm-turn.exe --desktop";
    if (!CreateProcessW(helper.c_str(), command, nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                        nullptr, nullptr, &startup, &child)) return {};
    process->child.value = child.hProcess;
    CloseHandle(child.hThread);
    childInput.Close();
    childOutput.Close();

    const auto message = grant.dump() + '\n';
    DWORD written = 0;
    if (message.size() > 2048 || !WriteFile(process->input.value, message.data(), DWORD(message.size()), &written, nullptr)
        || written != message.size()) return {};

    using Clock = std::chrono::steady_clock;
    const auto deadline = Clock::now() + std::chrono::seconds(2);
    std::string ready;
    constexpr std::string_view prefix = "Swarm TURN listening on 127.0.0.1:";
    while (Clock::now() < deadline && ready.size() < 256) {
        if (cancelled && cancelled->load(std::memory_order_relaxed)) return {};
        DWORD available = 0;
        if (!PeekNamedPipe(process->output.value, nullptr, 0, nullptr, &available, nullptr)) return {};
        if (!available) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        char buffer[128];
        DWORD size = 0;
        if (!ReadFile(process->output.value, buffer, (std::min)(available, DWORD(sizeof(buffer))), &size, nullptr)) return {};
        ready.append(buffer, size);
        const auto end = ready.find_first_of("\r\n");
        if (end == std::string::npos) continue;
        if (ready.compare(0, prefix.size(), prefix) != 0 || end <= prefix.size()) return {};
        unsigned port = 0;
        const auto parsed = std::from_chars(ready.data() + prefix.size(), ready.data() + end, port);
        if (parsed.ec != std::errc() || parsed.ptr != ready.data() + end || port < 1 || port > 65535) return {};
        process_ = std::move(process);
        return "turn:127.0.0.1:" + std::to_string(port) + "?transport=udp";
    }
    return {};
}
}
#endif
