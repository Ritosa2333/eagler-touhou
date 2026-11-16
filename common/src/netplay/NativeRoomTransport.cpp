#include <eagler/netplay/NativeRoomTransport.hpp>
#include <eagler/netplay/NativePeerTransport.hpp>
#include "DesktopSwarmTurn.hpp"
#include "NativeAdaptiveRoute.hpp"
#include <nlohmann/json.hpp>
#include <rtc/websocket.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <deque>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <thread>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

namespace Netplay
{
namespace
{
using Json = nlohmann::json;
using Clock = std::chrono::steady_clock;

rtc::WebSocket::Configuration signalConfiguration(const std::string &url)
{
    rtc::WebSocket::Configuration config;
#ifdef _WIN32
    if (url.rfind("wss://", 0) == 0) {
        const auto roots = CertOpenSystemStoreW(0, L"ROOT");
        if (!roots) throw std::runtime_error("Cannot open the Windows root certificate store");
        std::string pem;
        for (PCCERT_CONTEXT certificate = nullptr; (certificate = CertEnumCertificatesInStore(roots, certificate));) {
            constexpr DWORD format = CRYPT_STRING_BASE64HEADER;
            DWORD size = 0;
            if (!CryptBinaryToStringA(certificate->pbCertEncoded, certificate->cbCertEncoded, format, nullptr, &size))
                continue;
            std::string encoded(size, '\0');
            if (CryptBinaryToStringA(certificate->pbCertEncoded, certificate->cbCertEncoded, format, encoded.data(), &size))
                pem.append(encoded.data(), size);
        }
        CertCloseStore(roots, 0);
        if (!pem.empty()) config.caCertificatePemFile = std::move(pem);
    }
#else
    (void)url;
#endif
    return config;
}

rtc::Configuration iceConfiguration(const Json &servers, bool preferTurn)
{
    rtc::Configuration config;
    if (preferTurn) config.iceTransportPolicy = rtc::TransportPolicy::Relay;
    for (const auto &server : servers) {
        const auto &urls = server.at("urls");
        const auto list = urls.is_string() ? Json::array({urls}) : urls;
        for (const auto &url : list) {
            rtc::IceServer entry(url.get<std::string>());
            if (entry.type == rtc::IceServer::Type::Turn) {
                entry.username = server.value("username", "");
                entry.password = server.value("credential", "");
            }
            config.iceServers.push_back(std::move(entry));
        }
    }
    return config;
}
}

std::unique_ptr<rtc::WebSocket> OpenRoomWebSocket(const std::string &url)
{
    auto config = signalConfiguration(url);
    config.connectionTimeout = std::chrono::seconds(10);
    auto socket = std::make_unique<rtc::WebSocket>(std::move(config));
    socket->open(url);
    return socket;
}

struct NativeRoomTransport::Impl
{
    struct IceTraffic {
        unsigned descriptions = 0, host = 0, reflexive = 0, relay = 0, observations = 0, other = 0;
        void candidate(rtc::Candidate::Type type)
        {
            using Type = rtc::Candidate::Type;
            if (type == Type::Host) ++host;
            else if (type == Type::ServerReflexive) ++reflexive;
            else if (type == Type::Relayed) ++relay;
            else ++other;
        }
    };
    bool open() const {
        return route && error.empty() && !replacing && Active() && Active()->IsOpen();
    }

    void pump(const std::atomic_bool* cancelled = nullptr);
    void close();

    std::string signalUrl;
    rtc::WebSocket::Configuration signalConfig;
    std::unique_ptr<rtc::WebSocket> socket;
#ifdef _WIN32
    DesktopSwarmTurn swarm;
#endif
    std::unique_ptr<NativePeerTransport> transport;
    std::unique_ptr<NativeAdaptiveRoute> adaptive;
    Json iceServers = Json::array();
#ifdef RTC_HAS_STUN_OBSERVATIONS
    bool observeStun = false;
#endif
    std::string tag;
    std::uint8_t player = 0;
    std::uint8_t count = 0;
    bool preferTurn = false;
    bool ready = false;
    bool localSwarm = false;
    bool swarmRoom = false;
    bool adaptiveSwarm = false;
    bool startupActive = false;
    bool startupPrimaryFailed = false;
    Json startupTurnIce = Json::array();
    std::string initialRelayRoute;
    NativeAdaptiveRoute::RetirePeer retirePeer;
    bool adaptiveActive = false;
    bool reportMatch = false;
    bool route = false;
    bool disconnected = false;
    bool replaceCapable = false;
    bool replacing = false;
    bool replacementRequested = false;
    unsigned generation = 0;
    std::string error;
    IceTraffic outgoingIce, incomingIce;
    Clock::time_point iceStarted{}, firstRemoteSdp{}, firstLocalSdp{};
    Clock::time_point deadline = Clock::now() + std::chrono::seconds(45);
    Clock::time_point nextSignalAttempt{};
    std::chrono::milliseconds signalRetry{500};

    void openSignal(bool reconnect = false)
    {
        auto config = signalConfig;
        if (reconnect) config.connectionTimeout = std::chrono::milliseconds(2500);
        auto next = std::make_unique<rtc::WebSocket>(std::move(config));
        next->open(signalUrl);
        socket = std::move(next);
    }

    void send(const Json &message)
    {
        try {
            socket->send(message.dump());
        } catch (const std::runtime_error &) {
            // The receive thread can close signaling between isOpen and send.
            // An established route either reopens signaling or fails recovery.
            if (!route || !replaceCapable || socket->isOpen()) throw;
        }
    }

    bool sendSurvey(Json message)
    {
        if ((!route && !startupActive) || (!adaptiveSwarm && !startupActive) || replacing || !socket->isOpen()) return false;
        message["generation"] = generation;
        try { socket->send(message.dump()); return true; }
        catch (const std::exception &) { return false; }
    }

    void reconnectSignal()
    {
        const auto now = Clock::now();
        if (!socket->isClosed()) return;
        if (nextSignalAttempt == Clock::time_point{}) {
            nextSignalAttempt = now + signalRetry;
            return;
        }
        if (now < nextSignalAttempt) return;
        openSignal(true);
        nextSignalAttempt = now + signalRetry;
        signalRetry = (std::min)(signalRetry * 2, std::chrono::milliseconds(1500));
    }

    void requestReplacement()
    {
        if (socket->isOpen()) send({{"type", "rtc-replace-request"}, {"generation", generation}});
    }

    void createTransport(const Json &message, const std::atomic_bool* cancelled)
    {
        iceStarted = Clock::now();
        firstRemoteSdp = firstLocalSdp = {};
        outgoingIce = incomingIce = {};
        adaptive.reset();
        auto servers = iceServers;
        localSwarm = false;
#ifdef _WIN32
        const auto grant = message.find("swarmGrant");
        if (grant != message.end() && grant->is_object()) {
            auto url = swarm.Start(*grant, cancelled);
            if (cancelled && cancelled->load(std::memory_order_relaxed)) return;
            if (!url.empty()) {
                servers.push_back({{"urls", std::move(url)}, {"username", grant->at("username")},
                                   {"credential", grant->at("credential")}});
                localSwarm = true;
            } else std::fputs("Swarm local TURN failed to start\n", stderr);
        }
#endif
        auto config = iceConfiguration(servers, preferTurn);
#ifdef RTC_HAS_STUN_OBSERVATIONS
        config.enableStunObservations = observeStun && !preferTurn;
#endif
        transport = std::make_unique<NativePeerTransport>(tag, player, count, std::move(config));
        if (count == 2) {
            // Each desktop grant permits one allocation. A seat without
            // public TURN credentials uses the other seat's relay.
            auto standby = iceConfiguration(iceServers, false);
            if (std::any_of(standby.iceServers.begin(), standby.iceServers.end(),
                            [](const auto &server) { return server.type == rtc::IceServer::Type::Turn; }))
                standby.iceTransportPolicy = rtc::TransportPolicy::Relay;
            adaptive = std::make_unique<NativeAdaptiveRoute>(*transport, tag, player, std::move(standby), swarmRoom, !preferTurn);
            if (!preferTurn)
                adaptive->ConfigureSurvey(initialRelayRoute, message.value("surveyInitialSwarm", false),
                    [this](const Json &message) { return sendSurvey(message); }, retirePeer);
        }
    }

    NativePeerTransport *Active() const { return adaptive ? &adaptive->Active() : transport.get(); }

    void logIceTraffic() const
    {
        const auto elapsed = [this](Clock::time_point time) {
            return time == Clock::time_point{} ? -1LL :
                static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(time - iceStarted).count());
        };
        std::fprintf(stderr, "ICE signaling P%u: elapsed=%lldms sent-sdp=%u first=%lldms host=%u srflx=%u relay=%u obs=%u other=%u "
            "received-sdp=%u first=%lldms host=%u srflx=%u relay=%u other=%u\n",
            unsigned(player) + 1, elapsed(Clock::now()), outgoingIce.descriptions, elapsed(firstLocalSdp),
            outgoingIce.host, outgoingIce.reflexive, outgoingIce.relay, outgoingIce.observations, outgoingIce.other,
            incomingIce.descriptions, elapsed(firstRemoteSdp), incomingIce.host, incomingIce.reflexive,
            incomingIce.relay, incomingIce.other);
    }

    void receive(const Json &message, const std::atomic_bool* cancelled)
    {
        const auto type = message.value("type", "");
        if (type == "error" || type == "route-error") {
            error = message.value("error", "The room rejected the connection");
        } else if (type == "peers") {
            if (route && message.value("route", Json(nullptr)) != "rtc") {
                error = "The room no longer owns this WebRTC connection";
                return;
            }
            nextSignalAttempt = {};
            signalRetry = std::chrono::milliseconds(500);
            reportMatch = message.value("th09MatchReport", false);
            replaceCapable = replaceCapable || message.value("rtcReplace", false);
            if (!transport) {
                iceServers = message.value("iceServers", Json::array());
                swarmRoom = message.value("turnMode", "") == "swarm";
                adaptiveSwarm = message.value("adaptiveSwarm", false);
                initialRelayRoute = swarmRoom ? "swarm" : "turn";
                const auto worker = message.find("initialSwarmWorker");
                if (swarmRoom && worker != message.end() && worker->is_number_unsigned())
                    initialRelayRoute += "-" + worker->dump();
#ifdef RTC_HAS_STUN_OBSERVATIONS
                const auto observers = message.find("nat4Observers");
                observeStun = observers != message.end() && observers->is_array() && observers->size() == 3;
                if (observeStun) std::fputs("NAT4 STUN observers enabled\n", stderr);
#endif
                createTransport(message, cancelled);
                if (!transport) return;
            }
            for (const auto &peer : message.at("peers"))
                transport->AddPeer(peer.get<std::uint8_t>());
            if (message.value("route", Json(nullptr)) == "rtc") route = true;
            if (replacementRequested) requestReplacement();
        } else if (type == "route") {
            route = message.value("mode", "") == "rtc";
            if (!route) {
                error = message.value("reason", "The room did not select WebRTC");
                if (transport) {
                    std::fprintf(stderr, "ICE route rejected (local ready=%d)%s\n", ready, transport->ConnectionSummary().c_str());
                    logIceTraffic();
                }
            }
        } else if (type == "rtc-startup" && adaptive && !route && !preferTurn) {
            startupActive = true;
            adaptive->BeginStartup();
        } else if (type == "rtc-startup-select" && startupActive && adaptive) {
            const auto serial = message.value("serial", 2u);
            if (!adaptive->SelectStartup(serial)) return;
            startupActive = false;
            ready = true;
            if (serial == 1) {
                swarmRoom = localSwarm = false;
                initialRelayRoute = "turn";
                auto stun = iceServers;
                iceServers = std::move(startupTurnIce);
                for (const auto &entry : stun) {
                    const auto &urls = entry.at("urls");
                    const auto list = urls.is_string() ? Json::array({urls}) : urls;
                    if (std::all_of(list.begin(), list.end(), [](const auto &url) {
                        const auto &value = url.template get_ref<const std::string &>();
                        return value.rfind("stun:", 0) == 0 || value.rfind("stuns:", 0) == 0;
                    })) iceServers.push_back(entry);
                }
            }
            startupTurnIce = Json::array();
            send({{"type", "rtc-startup-selected"}, {"serial", serial}, {"rtcReplace", true},
                {"rtcRouteSurvey", adaptiveSwarm}});
        } else if ((type == "rtc-standby" || type == "rtc-standby-signal") && adaptive &&
                   (startupActive || (adaptiveSwarm && route)) && !preferTurn &&
                   !replacing && message.value("generation", 0u) == generation) {
            try {
                const auto servers = message.value("iceServers", Json::array());
                if (startupActive && type == "rtc-standby" && message.value("serial", 0u) == 1)
                    startupTurnIce = servers;
                adaptive->RelayStandby(message, iceConfiguration(servers, message.value("route", "") != "direct"));
            } catch (const std::exception &) {
                if (message.value("phase", "") == "measure")
                    sendSurvey({{"type", startupActive ? "rtc-startup-quality" : "rtc-route-report"},
                        {"serial", message.value("serial", 0u)}, {"quality", nullptr}});
            }
        } else if (type == "peer-disconnected") {
            error = "The remote player disconnected";
            disconnected = true;
        } else if (type == "rtc-replace-capable") {
            replaceCapable = true;
        } else if (type == "rtc-replacing" && route && replaceCapable && message.value("generation", 0u) == generation + 1) {
            ++generation;
            replacing = true;
            replacementRequested = false;
            ready = false;
            deadline = Clock::now() + std::chrono::seconds(17);
            std::fprintf(stderr, "Replacing WebRTC transport (generation %u)\n", generation);
            adaptive.reset();
            transport.reset();
        } else if (type == "rtc-replace" && replacing && !transport && message.value("generation", 0u) == generation) {
            createTransport(message, cancelled);
            if (transport) transport->AddPeer(1 - player);
        } else if (type == "rtc-replaced" && replacing && ready && message.value("generation", 0u) == generation) {
            replacing = false;
            std::fprintf(stderr, "WebRTC transport restored (generation %u)\n", generation);
        } else if (type == "rtc-replace-failed" && replacing && message.value("generation", 0u) == generation) {
            error = "WebRTC transport replacement failed";
        } else if (transport && type == "peer-join") {
            transport->AddPeer(message.at("player").get<std::uint8_t>());
        } else if (transport && type == "signal") {
            if (message.value("generation", 0u) != generation) return;
            const auto peer = message.at("from").get<std::uint8_t>();
            const auto description = message.value("description", Json(nullptr));
            if (!description.is_null()) {
                if (!incomingIce.descriptions++) firstRemoteSdp = Clock::now();
                transport->SetRemoteDescription(peer, rtc::Description(
                    description.at("sdp").get<std::string>(), description.at("type").get<std::string>()));
            }
            const auto candidate = message.value("candidate", Json(nullptr));
            if (!candidate.is_null()) {
                const auto mid = candidate.value("sdpMid", Json(nullptr));
                rtc::Candidate value(candidate.at("candidate").get<std::string>(), mid.is_string() ? mid.get<std::string>() : "0");
                incomingIce.candidate(value.type());
                transport->AddRemoteCandidate(peer, std::move(value));
            }
        }
    }
};

void NativeRoomTransport::Impl::close()
{
    if (!socket) return;
    if (route && !disconnected && socket->isOpen()) {
        try { socket->send(Json{{"type", "peer-disconnected"}}.dump()); }
        catch (const std::exception &) {}
    }
    try { socket->forceClose(); }
    catch (const std::exception &) {}
}

struct NativeRoomTransport::ConnectionWorker
{
    ConnectionWorker() : thread([this] { run(); }) {}
    ~ConnectionWorker()
    {
        {
            std::lock_guard lock(mutex);
            stop = true;
            cancel();
        }
        wake.notify_one();
        thread.join();
    }

    void begin(std::unique_ptr<Impl> state)
    {
        auto token = std::make_shared<std::atomic_bool>(false);
        {
            std::lock_guard lock(mutex);
            active = token;
            pending = {std::move(state), std::move(token)};
        }
        wake.notify_one();
    }

    void retire(std::unique_ptr<Impl> state)
    {
        {
            std::lock_guard lock(mutex);
            cancel();
            if (state) retired.push_back(std::move(state));
        }
        wake.notify_one();
    }

    void retirePeer(std::unique_ptr<NativePeerTransport> peer)
    {
        {
            std::lock_guard lock(mutex);
            retiredPeers.push_back(std::move(peer));
        }
        wake.notify_one();
    }

    std::unique_ptr<Impl> take()
    {
        std::unique_lock lock(mutex, std::try_to_lock);
        return lock ? std::move(result) : nullptr;
    }

private:
    struct Attempt {
        std::unique_ptr<Impl> state;
        std::shared_ptr<std::atomic_bool> cancelled;
    };

    void cancel()
    {
        if (active) active->store(true, std::memory_order_relaxed);
        active.reset();
        pending = {};
        if (result) retired.push_back(std::move(result));
    }

    void connect(Attempt attempt)
    {
        auto &state = *attempt.state;
        try {
            state.signalConfig = signalConfiguration(state.signalUrl);
            if (!attempt.cancelled->load(std::memory_order_relaxed)) state.openSignal();
            while (!attempt.cancelled->load(std::memory_order_relaxed)) {
                state.pump(attempt.cancelled.get());
                if (!state.error.empty() || state.open()) break;
                std::unique_lock lock(mutex);
                wake.wait_for(lock, std::chrono::milliseconds(4), [&] {
                    return attempt.cancelled->load(std::memory_order_relaxed);
                });
            }
        } catch (const std::exception &error) {
            state.error = error.what();
        }
        {
            std::lock_guard lock(mutex);
            if (!attempt.cancelled->load(std::memory_order_relaxed)) {
                result = std::move(attempt.state);
                return;
            }
        }
        state.close();
    }

    void run()
    {
        for (;;) {
            Attempt attempt;
            std::deque<std::unique_ptr<Impl>> closing;
            std::deque<std::unique_ptr<NativePeerTransport>> closingPeers;
            {
                std::unique_lock lock(mutex);
                wake.wait(lock, [&] { return stop || pending.state || !retired.empty() || !retiredPeers.empty(); });
                if (stop && retired.empty() && retiredPeers.empty()) return;
                closing.swap(retired);
                closingPeers.swap(retiredPeers);
                attempt = std::move(pending);
            }
            for (auto &state : closing) state->close();
            closing.clear();
            closingPeers.clear();
            if (attempt.state) connect(std::move(attempt));
        }
    }

    std::mutex mutex;
    std::condition_variable wake;
    Attempt pending;
    std::shared_ptr<std::atomic_bool> active;
    std::unique_ptr<Impl> result;
    std::deque<std::unique_ptr<Impl>> retired;
    std::deque<std::unique_ptr<NativePeerTransport>> retiredPeers;
    bool stop = false;
    std::thread thread;
};

NativeRoomTransport::NativeRoomTransport() = default;
NativeRoomTransport::~NativeRoomTransport() { Close(); }

void NativeRoomTransport::Connect(const std::string &signalUrl, std::string tag,
                                  std::uint8_t localPlayer, std::uint8_t playerCount, bool preferTurn)
{
    if (localPlayer >= playerCount || playerCount < 2
        || (signalUrl.rfind("ws://", 0) != 0 && signalUrl.rfind("wss://", 0) != 0))
        throw std::invalid_argument("Invalid native room configuration");
    Close();
    if (!worker_) worker_ = std::make_unique<ConnectionWorker>();
    auto next = std::make_unique<Impl>();
    next->signalUrl = signalUrl;
    if (playerCount == 2 && !preferTurn)
        next->signalUrl += signalUrl.find('?') == std::string::npos ? "?rtcStartup=1" : "&rtcStartup=1";
    next->tag = std::move(tag);
    next->player = localPlayer;
    next->count = playerCount;
    next->preferTurn = preferTurn;
    next->retirePeer = [worker = worker_.get()](std::unique_ptr<NativePeerTransport> peer) { worker->retirePeer(std::move(peer)); };
    worker_->begin(std::move(next));
    opening_ = true;
}

void NativeRoomTransport::Close()
{
    opening_ = false;
    if (worker_) worker_->retire(std::move(impl_));
}

bool NativeRoomTransport::ReportMatch(std::uint32_t start, std::uint32_t frames, unsigned winner, unsigned first, unsigned second)
{
    if (!impl_ || !impl_->reportMatch || !impl_->socket->isOpen()) return false;
    try {
        impl_->socket->send(Json{{"type", "th09-match"}, {"start", start}, {"frames", frames},
            {"winner", winner}, {"characters", Json::array({first, second})}}.dump());
        return true;
    } catch (const std::exception &) { return false; }
}

void NativeRoomTransport::Pump()
{
    if (opening_) {
        impl_ = worker_->take();
        opening_ = !impl_;
    }
    if (impl_) impl_->pump();
}

void NativeRoomTransport::Impl::pump(const std::atomic_bool* cancelled)
{
    if (!error.empty()) return;
    auto &state = *this;
    try {
        for (unsigned i = 0; i < 64; ++i) {
            const auto message = state.socket->receive();
            if (!message) break;
            if (auto *body = std::get_if<std::string>(&*message)) state.receive(Json::parse(*body), cancelled);
            if (!state.error.empty() || (cancelled && cancelled->load(std::memory_order_relaxed))) return;
        }
        if (state.route && state.replaceCapable && (!state.replacing || state.replacementRequested))
            state.reconnectSignal();
        if (state.transport && state.socket->isOpen()) {
            std::uint8_t player;
            rtc::variant<rtc::Description, rtc::Candidate> signal{rtc::Candidate{}};
            while (state.transport->PollSignal(player, signal)) {
                Json message{{"type", "signal"}, {"to", player}};
                if (state.generation) message["generation"] = state.generation;
                if (auto *description = std::get_if<rtc::Description>(&signal)) {
                    if (!state.outgoingIce.descriptions++) state.firstLocalSdp = Clock::now();
                    message["description"] = {{"type", description->typeString()}, {"sdp", std::string(*description)}};
                } else {
                    const auto &candidate = std::get<rtc::Candidate>(signal);
                    auto line = candidate.candidate();
                    if (line.rfind("candidate:natobs ", 0) == 0) {
                        const auto marker = line.rfind(" observer ");
                        if (marker == std::string::npos) continue;
                        ++state.outgoingIce.observations;
                        message["observer"] = "stun:" + line.substr(marker + 10);
                        line.resize(marker);
                    } else state.outgoingIce.candidate(candidate.type());
                    message["candidate"] = {{"candidate", std::move(line)}, {"sdpMid", candidate.mid()}};
                }
                state.send(message);
            }
            if (!state.ready && state.transport->IsOpen()) {
                const auto swarmSelected = state.localSwarm && state.transport->UsesRelay(true);
                Json ready{{"type", "rtc-ready"}};
                if (state.generation) ready["generation"] = state.generation;
                else {
                    ready["rtcReplace"] = state.count == 2;
                    if (state.adaptiveSwarm && state.adaptive && !state.preferTurn) ready["rtcRouteSurvey"] = true;
                }
                if (state.localSwarm) {
                    ready["swarmDirect"] = swarmSelected;
                }
                std::fprintf(stderr, "ICE ready (P%u, desktop Swarm=%d)%s\n", unsigned(state.player) + 1,
                             swarmSelected, state.transport->ConnectionSummary().c_str());
                state.logIceTraffic();
                state.send(ready);
                state.ready = true;
            }
        }
        if ((state.route || state.startupActive) && state.adaptive && !state.replacing)
            state.adaptive->Pump(state.adaptiveActive);
        const auto *active = state.Active();
        const bool adaptiveFailed = state.adaptive && state.adaptive->Failed();
        const bool silentStall = state.adaptive && state.adaptive->Stalled();
        const bool unhealthy = active && (active->Failed() || active->Disconnected() || adaptiveFailed || silentStall);
        if (state.startupActive && !state.socket->isClosed() && Clock::now() < state.deadline) {
            if (state.transport && state.transport->Failed() && !state.startupPrimaryFailed) {
                state.send({{"type", "rtc-startup-quality"}, {"serial", 0}, {"quality", nullptr}});
                state.startupPrimaryFailed = true;
            }
        } else if (state.replacing) {
            if ((!state.replacementRequested && (state.socket->isClosed() || unhealthy)) || Clock::now() >= state.deadline) {
                if (state.socket->isOpen()) state.send({{"type", "rtc-replace-failed"}, {"generation", state.generation}});
                state.error = "WebRTC transport replacement failed";
            }
        } else if (state.route && unhealthy && state.adaptive && state.adaptive->Recover()) {
            return;
        } else if (state.route && unhealthy && state.replaceCapable) {
            if (silentStall) std::fputs("RTC path stopped responding; requesting replacement\n", stderr);
            state.requestReplacement();
            state.replacing = state.replacementRequested = true;
            state.deadline = Clock::now() + std::chrono::seconds(17);
        } else if (active && (active->Failed() || adaptiveFailed)) {
            if (!state.route && state.socket->isOpen()) state.send({{"type", "rtc-failed"}});
            if (!state.route) std::fprintf(stderr, "ICE failed%s\n", active->ConnectionSummary().c_str());
            state.error = "WebRTC connection failed";
        } else if (!state.route && (state.socket->isClosed() || Clock::now() >= state.deadline)) {
            state.error = "The room did not establish a WebRTC connection";
        }
        if (state.route && !state.replacing && active && active->Disconnected() && !state.disconnected) {
            state.disconnected = true;
            if (state.socket->isOpen()) state.send({{"type", "peer-disconnected"}});
        }
    } catch (const std::exception &error) {
        state.error = error.what();
    }
    if (!state.error.empty()) std::fprintf(stderr, "%s\n", state.error.c_str());
}

const std::string &NativeRoomTransport::LastError() const
{
    static const std::string empty;
    return impl_ ? impl_->error : empty;
}

NativeRoomTransport::Link NativeRoomTransport::LinkStatus() const
{
    if (Recovering()) return {Route::Reconnecting,{}};
    if (!IsOpen()) return {};
    const bool live = impl_->adaptive && impl_->adaptive->HasLatencyProbes();
    const auto status = impl_->Active()->LinkStatus(!live);
    using Path = NativePeerTransport::Route;
    const bool swarm = impl_->adaptive ? impl_->adaptive->ActiveUsesSwarm() : impl_->swarmRoom;
    const Route route = status.route == Path::Relay ? (swarm ? Route::Swarm : Route::Turn) :
        status.route == Path::Nat4 ? Route::Nat4 : status.route == Path::Direct ? Route::Direct : Route::Connecting;
    const auto epoch = impl_->adaptive ? impl_->adaptive->ActivePathEpoch() : 0;
    return {route, live ? impl_->adaptive->ActiveRtt() : status.rttMs, (std::uint64_t(impl_->generation) << 32) | epoch,
            live && impl_->adaptive->ProbesUnanswered()};
}

bool NativeRoomTransport::IsOpen() const
{
    return impl_ && impl_->route && !Failed() && !Recovering() && impl_->Active() && impl_->Active()->IsOpen();
}

bool NativeRoomTransport::Failed() const { return !LastError().empty(); }

bool NativeRoomTransport::Recovering() const
{
    return impl_ && !Failed() && (impl_->replacing || (impl_->adaptive && impl_->adaptive->Recovering()));
}

void NativeRoomTransport::SetAdaptiveRouteActive(bool active)
{
    if (impl_) impl_->adaptiveActive = active;
}

bool NativeRoomTransport::Disconnected() const
{
    return impl_ && impl_->route && (Failed() || impl_->disconnected);
}

bool NativeRoomTransport::SendTo(std::uint8_t player, const std::uint8_t *data, std::size_t size)
{
    return IsOpen() && impl_->Active()->SendTo(player, data, size);
}

bool NativeRoomTransport::SendRepairTo(std::uint8_t player, const std::uint8_t *data, std::size_t size)
{
    return IsOpen() && impl_->Active()->SendRepairTo(player, data, size);
}

bool NativeRoomTransport::SendControl(const std::uint8_t *data, std::size_t size)
{
    return IsOpen() && (impl_->adaptive ? impl_->adaptive->SendControl(data, size) : impl_->Active()->SendControl(data, size));
}

bool NativeRoomTransport::Poll(std::vector<std::uint8_t> *packet)
{
    return IsOpen() && (impl_->adaptive ? impl_->adaptive->Poll(packet) : impl_->Active()->Poll(packet));
}

std::size_t NativeRoomTransport::BufferedAmount() const
{
    return impl_ && impl_->Active() ? impl_->Active()->BufferedAmount() : 0;
}
}
