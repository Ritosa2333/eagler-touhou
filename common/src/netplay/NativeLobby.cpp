#include <eagler/netplay/NativeLobby.hpp>
#include <eagler/netplay/NativeRoomTransport.hpp>

#include <rtc/websocket.hpp>
#include <httplib.h>
#include <charconv>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <future>
#include <mutex>
#include <random>
#include <thread>
#include <utility>

namespace Netplay {
namespace {
using Json = nlohmann::json;
using Clock = std::chrono::steady_clock;
using namespace std::chrono_literals;

std::string withQuery(const std::string& base, const std::string& parameters) {
    return base + (base.find('?') == std::string::npos ? "?" : "&") + parameters;
}

bool trustedRelay(const NativeLobby::Config& config) {
    const auto site = config.site.substr(0, config.site.find('/', config.site.find("://") + 3));
    const auto relay = config.relay.substr(0, config.relay.find('/', config.relay.find("://") + 3));
    if (site == "https://touhouonline.com") return relay == "wss://relay.touhouonline.com" || relay == "wss://touhouonline.com";
    if (site == "https://test.touhouonline.com") return relay == "wss://relay.test.touhouonline.com" || relay == "wss://test.touhouonline.com";
    return site.rfind("http://127.0.0.1:", 0) == 0 && relay == "ws://" + site.substr(7);
}

Json accountRequest(const std::string& endpoint, const char* command, Json args) {
    constexpr std::string_view local = "http://127.0.0.1:";
    if (endpoint.rfind(local, 0) != 0) return {};
    const auto separator = endpoint.find('/', local.size());
    if (separator == std::string::npos) return {};
    int port = 0;
    const auto result = std::from_chars(endpoint.data() + local.size(), endpoint.data() + separator, port);
    if (result.ec != std::errc() || result.ptr != endpoint.data() + separator || port <= 0 || port > 65535) return {};
    httplib::Client client("127.0.0.1", port);
    client.set_connection_timeout(0, 400000);
    client.set_max_timeout(6500);
    const auto response = client.Post(endpoint.substr(separator), {{"Origin", "eagler-overlay://panel"}},
        Json{{"command", command}, {"args", std::move(args)}}.dump(), "application/json");
    if (!response || response->status != 200 || response->body.size() > 40 * 1024) return {};
    return Json::parse(response->body, nullptr, false);
}

bool validCode(const std::string& code) {
    return code.size() >= 4 && code.size() <= 8 &&
        code.find_first_not_of("0123456789") == std::string::npos;
}
} // namespace

struct NativeLobby::State {
    using Socket = std::unique_ptr<rtc::WebSocket>;

    explicit State(const Config& settings) : config(settings), thread([this] { run(); }) {}
    ~State() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stopping = true;
        }
        wake.notify_one();
        thread.join();
    }

    void publish() {
        std::lock_guard<std::mutex> lock(mutex);
        snapshot = view;
        updated.store(true, std::memory_order_release);
    }

    void clearRoom() {
        if (room) room->close();
        room.reset();
        ++roomRevision;
        code.clear();
        view["code"] = "";
        view["room"] = nullptr;
        view["chat"] = Json::array();
        view.erase("start");
        view["registered"] = false;
        challenge.clear();
        nextIdentityCheck = {};
        view.erase("profileAssets");
        profileKey = Json::array();
    }

    void enter(const Json& action, const std::string& type) {
        const bool create = type == "create";
        std::string next = action.value("code", std::string{});
        if (create) {
            std::random_device entropy;
            next = std::to_string(std::uniform_int_distribution<unsigned>(10000000, 99999999)(entropy));
        }
        if (!validCode(next)) throw std::runtime_error("房间号应为 4 至 8 位数字");
        clearRoom();
        code = next;
        view["code"] = code;
        view["status"] = "正在进入房间…";
        autoPlayer = create;
        autoWatch = action.value("spectator", false);
        auto query = "room=" + config.product + "-" + code + "&lobby=" + config.client +
            "&member=" + config.member + "&intent=" + (create ? "create" : "join");
        if (create) query += "&visibility=public&players=" + std::to_string(config.players);
        room = OpenRoomWebSocket(withQuery(config.relay, query));
    }

    static void send(const Socket& socket, const Json& message) {
        if (socket && socket->isOpen()) socket->send(message.dump());
    }

    void apply(Json action) {
        const auto type = action.value("type", std::string{});
        if (type == "create" || type == "join") {
            if (config.relay.empty()) throw std::runtime_error("尚未配置联机服务器");
            enter(action, type);
        } else if (type == "leave") {
            if (room && room->isOpen()) {
                // libdatachannel supplies the WebSocket close handshake.
                room->close();
            }
            clearRoom();
            view["status"] = "";
        } else if (type == "directory") {
            send(directory, {{"type", "refresh"}, {"product", config.product}});
        } else if (type == "open-account") {
            if (!accountRequest(config.accountEndpoint, "open_account", Json::object()).is_object())
                throw std::runtime_error("无法打开登录窗口");
            refreshingAccount = true;
            accountDeadline = Clock::now() + 2min;
            nextIdentityCheck = {};
        } else if (type == "ack-start") {
            view.erase("start");
        } else if (room && room->isOpen()) {
            send(room, action);
        } else throw std::runtime_error("房间尚未连接");
        publish();
    }

    void read(const Socket& socket, bool lobby) {
        for (unsigned i = 0; i < 32; ++i) {
            auto packet = socket->receive();
            if (!packet) return;
            const auto* text = std::get_if<std::string>(&*packet);
            if (!text || text->size() > 256 * 1024) continue;
            auto message = Json::parse(*text, nullptr, false);
            if (!message.is_object() || !message.contains("type") || !message["type"].is_string()) continue;
            const auto type = message["type"].get<std::string>();
            if (!lobby) {
                if (type != "directory" || !message.contains("rooms") || !message["rooms"].is_array()) continue;
                view["rooms"] = Json::array();
                for (auto& item : message["rooms"]) {
                    if (!item.is_object() || item.value("product", std::string{}) != config.product) continue;
                    view["rooms"].push_back(std::move(item));
                }
                view["directoryStatus"] = "";
            } else if (type == "error") {
                view["status"] = message.value("error", std::string("操作失败"));
            } else if (type == "state" || type == "start" || type == "spectator-start") {
                if (type == "state" && challenge.empty() && trustedRelay(config) && !config.accountEndpoint.empty()) {
                    const auto key = message.value("accountChallenge", std::string{});
                    if (!key.empty() && key.size() <= 1024) challenge = key;
                }
                if (message.contains("room") && message["room"].is_object()) {
                    view["room"] = std::move(message["room"]);
                    view["status"] = "";
                }
                if (type == "state" && (autoPlayer || autoWatch)) {
                    if (autoPlayer) send(room, {{"type", "take-seat"}, {"seat", 0}, {"loadout", 0}, {"nativeRuntime", true}});
                    else send(room, {{"type", "spectate"}});
                    autoPlayer = autoWatch = false;
                }
                if ((type == "start" || type == "spectator-start") && message.contains("serial")) {
                    view["start"] = {{"serial", message["serial"]}, {"spectator", type == "spectator-start"}};
                } else if (type == "state" && view["room"].is_object() && view["room"].value("phase", std::string{}) == "starting") {
                    view["start"] = {{"serial", view["room"].value("startSerial", 0u)}, {"spectator", false}};
                }
            } else if (type == "chat-status") {
                view["registered"] = message.value("registered", false);
                if (view["registered"].get<bool>()) refreshingAccount = false;
            } else if (type == "quick-chat" || type == "free-chat" || type == "th09-rating") {
                if (type == "th09-rating") profileKey = Json::array();
                auto& chat = view["chat"];
                if (chat.size() >= 24) chat.erase(chat.begin());
                message["receivedAt"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                    Clock::now().time_since_epoch()).count();
                chat.push_back(std::move(message));
            } else continue;
            publish();
        }
    }

    void profiles() {
        if (config.accountEndpoint.empty()) return;
        if (profileRequest.valid()) {
            if (profileRequest.wait_for(0s) != std::future_status::ready) return;
            auto assets = profileRequest.get();
            if (assets.is_object() && requestedRoom == code) {
                view["profileAssets"] = std::move(assets);
                publish();
            }
        }
        const auto seats = view["room"].is_object() ? view["room"].value("seats",Json::array()) : Json::array();
        if (!seats.is_array()) return;
        Json players = Json::array();
        for (const auto& seat : seats) {
            if (!seat.is_object()) continue;
            const auto profile = seat.value("profile",Json{});
            if (profile.is_object() && profile.contains("username") && profile["username"].is_string())
                players.push_back({{"username",profile["username"]},{"avatar",profile.value("avatar",std::string{})}});
        }
        if (players.empty() || players.size() > 2 || players == profileKey) return;
        profileKey = players;
        requestedRoom = code;
        // Public assets use a separate request so a slow profile server cannot delay room signaling.
        std::packaged_task<Json()> request([endpoint = config.accountEndpoint, players = std::move(players)] {
            return accountRequest(endpoint,"room_profiles",{{"players",players}});
        });
        profileRequest = request.get_future();
        std::thread(std::move(request)).detach();
    }

    void pollRoomIdentity(Clock::time_point now) {
        if (identityRequest.valid()) {
            if (identityRequest.wait_for(0s) != std::future_status::ready) return;
            const auto identity = identityRequest.get();
            if (identityRoomRevision == roomRevision && identity.is_object()) {
                const auto ticket = identity.value("ticket", std::string{});
                if (!ticket.empty() && ticket.size() <= 4096) {
                    send(room, {{"type","account-nickname"},{"ticket",ticket}});
                    refreshingAccount = false;
                }
            }
        }
        if (challenge.empty() || now < nextIdentityCheck ||
            (nextIdentityCheck != Clock::time_point{} && (!refreshingAccount || now >= accountDeadline))) return;

        nextIdentityCheck = now + 2500ms;
        identityRoomRevision = roomRevision;
        // Leaving the room and closing the game must be able to finish while
        // the account bridge is waiting on its network request.
        std::packaged_task<Json()> request([endpoint = config.accountEndpoint, key = challenge] {
            return accountRequest(endpoint, "room_identity", {{"challenge", key}});
        });
        identityRequest = request.get_future();
        std::thread(std::move(request)).detach();
    }

    void pump() {
        if (config.relay.empty()) return;
        profiles();
        const auto now = Clock::now();
        if (directory && directory->isClosed()) {
            directory.reset();
            directoryOpen = false;
            view["directoryStatus"] = "大厅正在重连…";
            directoryRetry = now + 1s;
            publish();
        }
        if (!directory && now >= directoryRetry) {
            directoryRetry = now + 2s;
            directory = OpenRoomWebSocket(withQuery(config.relay, "directory=1&member=" + config.member));
        }
        if (directory) {
            if (!directoryOpen && directory->isOpen()) {
                directoryOpen = true;
                send(directory, {{"type", "refresh"}, {"product", config.product}});
            }
            read(directory, false);
        }
        if (room) {
            read(room, true);
            if (room->isClosed()) {
                clearRoom();
                if (view["status"].get<std::string>().empty()) view["status"] = "已离开房间";
                publish();
            } else if (room->isOpen()) pollRoomIdentity(now);
        }
    }

    void run() {
        if (config.relay.empty()) view["directoryStatus"] = "尚未配置联机服务器";
        publish();
        for (;;) {
            std::deque<Json> actions;
            {
                std::unique_lock<std::mutex> lock(mutex);
                wake.wait_for(lock, 15ms, [&] { return stopping || !outbound.empty(); });
                if (stopping) break;
                actions.swap(outbound);
            }
            for (auto& action : actions) {
                try { apply(std::move(action)); }
                catch (const std::exception& error) {
                    view["status"] = error.what();
                    publish();
                }
            }
            try { pump(); }
            catch (const std::exception& error) {
                view["status"] = error.what();
                publish();
            }
        }
    }

    const Config& config;
    std::mutex mutex;
    std::condition_variable wake;
    bool stopping = false;
    std::deque<Json> outbound;
    Json snapshot;
    std::atomic_bool updated{false};
    Json view = {{"rooms", Json::array()}, {"room", nullptr}, {"chat", Json::array()},
                 {"status", ""}, {"directoryStatus", "正在连接大厅…"}, {"code", ""}};
    Socket directory, room;
    std::string code, challenge, requestedRoom;
    Json profileKey = Json::array();
    std::future<Json> profileRequest;
    std::future<Json> identityRequest;
    std::uint64_t roomRevision = 0, identityRoomRevision = 0;
    bool refreshingAccount = false;
    Clock::time_point accountDeadline{}, nextIdentityCheck{};
    bool directoryOpen = false;
    bool autoPlayer = false, autoWatch = false;
    Clock::time_point directoryRetry{};
    std::thread thread;
};

NativeLobby::NativeLobby(Config config) : config_(std::move(config)), state_(std::make_unique<State>(config_)) {}
NativeLobby::~NativeLobby() = default;

void NativeLobby::Send(Json action) {
    {
        std::lock_guard<std::mutex> lock(state_->mutex);
        if (state_->outbound.size() >= 32) return;
        state_->outbound.push_back(std::move(action));
    }
    state_->wake.notify_one();
}

Json NativeLobby::TakeSnapshot() {
    if (!state_->updated.load(std::memory_order_acquire)) return {};
    std::unique_lock<std::mutex> lock(state_->mutex, std::try_to_lock);
    if (!lock) return {};
    state_->updated.store(false, std::memory_order_relaxed);
    return std::exchange(state_->snapshot, nullptr);
}

} // namespace Netplay
