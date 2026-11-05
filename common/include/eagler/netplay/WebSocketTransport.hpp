#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

namespace Netplay
{
class WebSocketTransport
{
public:
    WebSocketTransport() = default;
    ~WebSocketTransport();

    WebSocketTransport(const WebSocketTransport &) = delete;
    WebSocketTransport &operator=(const WebSocketTransport &) = delete;

    bool Connect(const char *url);
    void Close();
    bool IsOpen() const { return open_; }
    bool Failed() const { return failed_; }
    bool Send(const std::uint8_t *data, std::size_t size);
    bool Poll(std::vector<std::uint8_t> *packet);
    std::size_t BufferedAmount() const;
    const std::string &LastError() const { return lastError_; }

private:
    static bool HandleOpen(int eventType, const void *event, void *userData);
    static bool HandleMessage(int eventType, const void *event, void *userData);
    static bool HandleError(int eventType, const void *event, void *userData);
    static bool HandleClose(int eventType, const void *event, void *userData);

    int socket_ = 0;
    bool open_ = false;
    bool failed_ = false;
    bool closing_ = false;
    std::string lastError_;
    std::deque<std::vector<std::uint8_t>> received_;
};
} // namespace Netplay
