#include "WebSocketTransport.hpp"

#include <limits>

#ifdef __EMSCRIPTEN__
#include <emscripten/websocket.h>
#endif

namespace Netplay
{
WebSocketTransport::~WebSocketTransport()
{
    Close();
}

#ifdef __EMSCRIPTEN__
bool WebSocketTransport::HandleOpen(int, const void *, void *userData)
{
    auto *self = static_cast<WebSocketTransport *>(userData);
    self->open_ = true;
    self->closing_ = false;
    self->failed_ = false;
    self->lastError_.clear();
    return false;
}

bool WebSocketTransport::HandleMessage(int, const void *event, void *userData)
{
    auto *self = static_cast<WebSocketTransport *>(userData);
    const auto *message = static_cast<const EmscriptenWebSocketMessageEvent *>(event);
    if (message->isText)
    {
        self->failed_ = true;
        self->lastError_ = "unexpected text WebSocket frame";
        return false;
    }
    self->received_.emplace_back(message->data, message->data + message->numBytes);
    return false;
}

bool WebSocketTransport::HandleError(int, const void *, void *userData)
{
    auto *self = static_cast<WebSocketTransport *>(userData);
    self->failed_ = true;
    self->lastError_ = "WebSocket error";
    return false;
}

bool WebSocketTransport::HandleClose(int, const void *event, void *userData)
{
    auto *self = static_cast<WebSocketTransport *>(userData);
    const auto *close = static_cast<const EmscriptenWebSocketCloseEvent *>(event);
    self->open_ = false;
    if (!self->closing_ && !self->failed_)
    {
        self->failed_ = true;
        self->lastError_ = close->reason[0]
                               ? close->reason
                               : (close->wasClean ? "remote WebSocket closed"
                                                  : "WebSocket closed unexpectedly");
    }
    return false;
}
#else
bool WebSocketTransport::HandleOpen(int, const void *, void *) { return false; }
bool WebSocketTransport::HandleMessage(int, const void *, void *) { return false; }
bool WebSocketTransport::HandleError(int, const void *, void *) { return false; }
bool WebSocketTransport::HandleClose(int, const void *, void *) { return false; }
#endif

bool WebSocketTransport::Connect(const char *url)
{
    Close();
    failed_ = false;
    closing_ = false;
    lastError_.clear();
    received_.clear();
#ifdef __EMSCRIPTEN__
    if (!url || !url[0] || !emscripten_websocket_is_supported())
    {
        failed_ = true;
        lastError_ = "WebSocket unsupported or URL missing";
        return false;
    }
    EmscriptenWebSocketCreateAttributes attributes;
    emscripten_websocket_init_create_attributes(&attributes);
    attributes.url = url;
    attributes.protocols = nullptr;
    attributes.createOnMainThread = true;
    socket_ = emscripten_websocket_new(&attributes);
    if (socket_ <= 0)
    {
        failed_ = true;
        lastError_ = "emscripten_websocket_new failed";
        socket_ = 0;
        return false;
    }
    emscripten_websocket_set_onopen_callback(
        socket_, this,
        reinterpret_cast<em_websocket_open_callback_func>(&WebSocketTransport::HandleOpen));
    emscripten_websocket_set_onmessage_callback(
        socket_, this,
        reinterpret_cast<em_websocket_message_callback_func>(&WebSocketTransport::HandleMessage));
    emscripten_websocket_set_onerror_callback(
        socket_, this,
        reinterpret_cast<em_websocket_error_callback_func>(&WebSocketTransport::HandleError));
    emscripten_websocket_set_onclose_callback(
        socket_, this,
        reinterpret_cast<em_websocket_close_callback_func>(&WebSocketTransport::HandleClose));
    return true;
#else
    (void)url;
    failed_ = true;
    lastError_ = "WebSocket transport is only implemented for Web/Emscripten";
    return false;
#endif
}

void WebSocketTransport::Close()
{
#ifdef __EMSCRIPTEN__
    if (socket_ > 0)
    {
        closing_ = true;
        unsigned short state = 0;
        if (emscripten_websocket_get_ready_state(socket_, &state) == EMSCRIPTEN_RESULT_SUCCESS &&
            state < 2)
            emscripten_websocket_close(socket_, 1000, "netplay transport close");
        emscripten_websocket_delete(socket_);
    }
#endif
    socket_ = 0;
    open_ = false;
    received_.clear();
}

bool WebSocketTransport::Send(const std::uint8_t *data, std::size_t size)
{
#ifdef __EMSCRIPTEN__
    if (!open_ || socket_ <= 0 || !data || size == 0 ||
        size > std::numeric_limits<std::uint32_t>::max())
        return false;
    return emscripten_websocket_send_binary(socket_, const_cast<std::uint8_t *>(data),
                                             static_cast<std::uint32_t>(size)) ==
           EMSCRIPTEN_RESULT_SUCCESS;
#else
    (void)data;
    (void)size;
    return false;
#endif
}

bool WebSocketTransport::Poll(std::vector<std::uint8_t> *packet)
{
    if (!packet || received_.empty())
        return false;
    *packet = std::move(received_.front());
    received_.pop_front();
    return true;
}

std::size_t WebSocketTransport::BufferedAmount() const
{
#ifdef __EMSCRIPTEN__
    std::size_t amount = 0;
    if (socket_ > 0)
        (void)emscripten_websocket_get_buffered_amount(socket_, &amount);
    return amount;
#else
    return 0;
#endif
}
} // namespace Netplay
