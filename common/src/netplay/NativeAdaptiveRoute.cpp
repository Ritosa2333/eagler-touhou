#include "NativeAdaptiveRoute.hpp"
#include <eagler/netplay/NativeRoomTransport.hpp>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <deque>
#include <limits>
#include <map>
#include <string_view>
#include <utility>

namespace Netplay
{
namespace
{
using Json = nlohmann::json;
using Millis = std::int64_t;
using Bytes = std::vector<std::uint8_t>;
constexpr std::string_view Prefix = "etr1:";
constexpr std::size_t MaxControls = 256, MaxBytes = 1024 * 1024;
constexpr Millis Window = 6000, ProbeTimeout = 1000, Cooldown = 60000;
constexpr Millis RttFreshness = 2000, StallTimeout = 3000;
constexpr std::size_t StartupSamples = 6;

Millis now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}

std::uint32_t number(const Json &m, const char *key)
{
    const auto value = m.find(key);
    if (value == m.end() || !value->is_number_unsigned()) return 0;
    const auto parsed = value->get<std::uint64_t>();
    return parsed <= UINT32_MAX ? static_cast<std::uint32_t>(parsed) : 0;
}

struct Sample { std::uint32_t id; Millis sent; Millis rtt = -1; };
struct Quality {
    std::size_t count = 0, received = 0;
    double p95 = std::numeric_limits<double>::infinity(), loss = 1;
    double score() const { return p95 + loss * 1000; }
};

Json report(const Quality &quality)
{
    return {{"count", quality.count}, {"received", quality.received},
            {"p95", quality.received ? Json(static_cast<Millis>(quality.p95)) : Json(nullptr)}};
}

Json surveyReport(const Quality &quality)
{
    return {{"sent", quality.count}, {"received", quality.received},
            {"p95", quality.received ? Json(static_cast<Millis>(quality.p95)) : Json(nullptr)}};
}

std::optional<Quality> readQuality(const Json &value)
{
    if (!value.is_object() || !value.contains("count") || !value["count"].is_number_unsigned() ||
        !value.contains("received") || !value["received"].is_number_unsigned() || !value.contains("p95"))
        return std::nullopt;
    const auto count = value["count"].get<std::uint64_t>(), received = value["received"].get<std::uint64_t>();
    if (count > 128 || received > count) return std::nullopt;
    Quality quality;
    quality.count = static_cast<std::size_t>(count);
    quality.received = static_cast<std::size_t>(received);
    if (count) quality.loss = double(count - received) / count;
    const auto &p95 = value["p95"];
    if (received) {
        if (!p95.is_number_unsigned() || p95.get<std::uint64_t>() > 10000) return std::nullopt;
        quality.p95 = p95.get<double>();
    } else if (!p95.is_null()) return std::nullopt;
    return quality;
}

Quality paired(Quality a, const Quality &b)
{
    a.count = (std::min)(a.count, b.count);
    a.received = (std::min)(a.received, b.received);
    a.p95 = (std::max)(a.p95, b.p95);
    a.loss = (std::max)(a.loss, b.loss);
    return a;
}

struct Path {
    NativePeerTransport *peer = nullptr;
    Millis lastProbe = 0;
    Millis lastRttAt = 0;
    Millis lastTrafficAt = 0;
    unsigned rttMs = 0;
    std::uint32_t probes = 0;
    bool relayConfirmed = false;
    bool direct = false;
    std::unique_ptr<NativePeerTransport> owned;
    std::uint32_t surveySerial = 0;
    std::string route;
    bool measuring = false, reported = false;
    Millis createdAt = 0;
    std::deque<Sample> samples;

    Quality Measure(Millis time, Millis since, std::size_t limit = 128) const
    {
        Quality result;
        std::vector<Millis> rtts;
        for (auto it = samples.rbegin(); it != samples.rend() && result.count < limit; ++it) {
            const auto &sample = *it;
            if (sample.sent < since || time - sample.sent < ProbeTimeout) continue;
            ++result.count;
            if (sample.rtt >= 0) rtts.push_back(sample.rtt);
        }
        if (!result.count) return result;
        result.received = rtts.size();
        result.loss = double(result.count - result.received) / result.count;
        if (!rtts.empty()) {
            std::sort(rtts.begin(), rtts.end());
            result.p95 = double(rtts[(rtts.size() * 95 + 99) / 100 - 1]);
        }
        return result;
    }
};
}

struct NativeAdaptiveRoute::State
{
    Path primary, standby;
    Path *active = &primary, *previous = nullptr;
    std::unique_ptr<NativePeerTransport> backup;
    std::unique_ptr<Path> planned, retained;
    Path *verificationTarget = nullptr;
    std::uint32_t standbySerial = 0;
    Millis nextSlowCheck = 0, outageReportedAt = 0;
    std::string initialRelayRoute;
    bool initialSurvey = false;
    SurveySignal surveySignal;
    RetirePeer retirePeer;
    std::string tag;
    std::uint8_t local, remote;
    rtc::Configuration config;
    bool swarm, allowSwitching, manualConfirmation, peerSwitching = true;
    bool running = false, hello = false, supported = false, directRetry = false, pairedQuality = false;
    bool startup = false;
    bool manualSwitch = false;
    bool disabled = false, ackPending = false, qualityReported = false;
    bool firstComparison = true, force = false;
    Millis lastPump = 0, quietUntil = 0, backupStarted = 0, retryAt = 0;
    Millis activeSince = 0;
    Millis previousUntil = 0, verification = 0, nextCheck = 0, verifyAfter = 0, switched = 0;
    Millis recovery = 0, lastNeed = 0;
    std::uint32_t generation = 0, epoch = 0, outgoing = 0, incoming = 0, resendFrom = 0, recoveryProbe = 0;
    std::uint32_t verificationTicket = 0, currentTicket = 0;
    struct Comparison { Quality present, candidate; };
    std::optional<Comparison> remoteQuality;
    struct Proposal { Path *path; std::uint32_t epoch; Millis at; Millis lastCommit = 0; };
    std::optional<Proposal> proposal;
    struct Offer { Path *path; std::uint32_t ticket; Millis at; int fromMs, toMs; bool confirmed = false; };
    std::optional<Offer> offered;
    std::map<std::uint32_t, Bytes> pending, reorder;
    std::size_t pendingBytes = 0, reorderBytes = 0, inboxBytes = 0;
    std::deque<Bytes> inbox;

    State(NativePeerTransport &peer, std::string name, std::uint8_t localPlayer, rtc::Configuration cfg,
          bool usingSwarm, bool switching, bool confirmation)
        : primary{&peer}, tag(std::move(name)), local(localPlayer), remote(1 - localPlayer),
          config(std::move(cfg)), swarm(usingSwarm), allowSwitching(switching), manualConfirmation(confirmation) {}

    bool Controller() const { return local == 0; }
    bool Switching() const { return allowSwitching && peerSwitching; }
    bool Eligible(Millis time) const { return running && time >= quietUntil; }
    Millis ProbeWaitSince() const { return (std::max)({activeSince, quietUntil, active->lastRttAt}); }
    bool Stalled(Millis time) const {
        return supported && running &&
            time - (std::max)(ProbeWaitSince(), active->lastTrafficAt) >= StallTimeout;
    }
    std::array<Path *, 4> Paths() { return {&primary, &standby, planned.get(), retained.get()}; }
    Path *Other()
    {
        for (auto *path : {planned.get(), &standby, retained.get(), &primary})
            if (path && path != active && !path->measuring && Ready(*path)) return path;
        return nullptr;
    }
    const char *Name(const Path &path) const {
        return &path == &primary ? "primary" : path.surveySerial ? path.route.c_str() : path.direct ? "direct" : "turn";
    }
    std::uint32_t WireGeneration(const Path &path) const { return path.surveySerial ? path.surveySerial : generation; }
    bool Measuring() const { return planned && planned->measuring && !planned->reported; }
    bool Ready(Path &path) const
    {
        if (!path.peer || !path.peer->IsOpen()) return false;
        if (&path == &primary) return true;
        if (path.direct) return !path.peer->UsesRelay();
        if (!path.relayConfirmed) path.relayConfirmed = path.peer->UsesRelay();
        return path.relayConfirmed;
    }
    bool Send(Path &path, const Json &message, bool control = true)
    {
        return path.peer && path.peer->SendText(remote, control, std::string(Prefix) + message.dump());
    }
    void Broadcast(const Json &message)
    {
        for (auto *path : Paths()) if (path && !path->measuring) Send(*path, message);
    }

    void ClearVerification() { verification = 0; verificationTarget = nullptr; }

    void Suspend(Millis time)
    {
        offered.reset();
        if (proposal && Controller() && !proposal->lastCommit) proposal.reset();
        else if (proposal) proposal->at = time;
        for (auto *path : Paths()) {
            if (!path) continue;
            path->samples.clear();
            path->lastRttAt = 0;
        }
        quietUntil = time + 2000;
        ClearVerification();
        if (recovery) {
            recovery = quietUntil + 7000;
            recoveryProbe = active->probes;
        }
    }

    void Discard(Millis time)
    {
        if (!backup || active == &standby || (proposal && proposal->path == &standby)) return;
        Send(primary, {{"kind", "retire"}, {"generation", generation}});
        if (previous == &standby) previous = nullptr;
        standby = {};
        backup.reset();
        if (verificationTarget == &standby) ClearVerification();
        if (offered && offered->path == &standby) offered.reset();
        retryAt = time + Cooldown;
    }

    void Retire(std::unique_ptr<Path> &path)
    {
        if (!path) return;
        if (previous == path.get()) previous = nullptr;
        if (verificationTarget == path.get()) ClearVerification();
        if (offered && offered->path == path.get()) offered.reset();
        if (retirePeer && path->owned) retirePeer(std::move(path->owned));
        path.reset();
    }

    void StandbyReport(Path &path, Json quality)
    {
        if (path.measuring && !path.reported && surveySignal)
            path.reported = surveySignal({{"type", startup ? "rtc-startup-quality" : "rtc-route-report"},
                {"serial", path.surveySerial}, {"quality", std::move(quality)}});
    }

    void Standby(const Json &message, rtc::Configuration ice)
    {
        if (!surveySignal || !Switching() || disabled) return;
        const auto id = number(message, "serial");
        if (!id) return;
        if (message.value("type", "") == "rtc-standby-signal") {
            if (!planned || planned->surveySerial != id || !planned->owned) return;
            try {
                if (message.contains("description") && message["description"].is_object()) {
                    const auto &description = message.at("description");
                    const auto type = description.at("type").get<std::string>();
                    if (type == (Controller() ? "answer" : "offer"))
                        planned->peer->SetRemoteDescription(remote, rtc::Description(description.at("sdp").get<std::string>(), type));
                }
                if (message.contains("candidate") && message["candidate"].is_object()) {
                    const auto &candidate = message.at("candidate");
                    const auto mid = candidate.value("sdpMid", Json(nullptr));
                    planned->peer->AddRemoteCandidate(remote, rtc::Candidate(candidate.at("candidate").get<std::string>(),
                        mid.is_string() ? mid.get<std::string>() : "0"));
                }
            } catch (const std::exception &) { StandbyReport(*planned, nullptr); }
            return;
        }
        if (id <= standbySerial || (planned && proposal && proposal->path == planned.get())) return;
        const auto phase = message.value("phase", std::string{});
        const auto route = message.value("route", std::string{});
        if (phase != "idle" && ((phase != "measure" && phase != "select") ||
            (route != "direct" && route != "turn" && route.rfind("swarm-", 0) != 0))) return;
        standbySerial = id;
        initialSurvey = false;
        if (planned && active == planned.get()) {
            Retire(retained);
            retained = std::move(planned);
        } else Retire(planned);
        if (phase == "idle") return;
        planned = std::make_unique<Path>();
        planned->surveySerial = id;
        planned->createdAt = now();
        planned->route = route;
        planned->direct = route == "direct";
        planned->measuring = phase == "measure";
        try {
            planned->owned = std::make_unique<NativePeerTransport>(tag + "-route", local, 2, std::move(ice));
            planned->peer = planned->owned.get();
            planned->peer->AddPeer(remote);
        } catch (const std::exception &) {
            StandbyReport(*planned, nullptr);
            Retire(planned);
        }
    }

    void MakeBackup(std::uint32_t id, Millis time, bool direct)
    {
        auto servers = config;
        if (direct) {
            servers.iceTransportPolicy = rtc::TransportPolicy::All;
            auto &ice = servers.iceServers;
            ice.erase(std::remove_if(ice.begin(), ice.end(), [](const auto &server) {
                return server.type == rtc::IceServer::Type::Turn;
            }), ice.end());
        }
        auto peer = std::make_unique<NativePeerTransport>(tag + "-route", local, 2, std::move(servers));
        peer->AddPeer(remote);
        backup = std::move(peer);
        standby.peer = backup.get();
        standby.direct = direct;
        backupStarted = time;
        generation = id;
    }

    void ExchangeSignals(Millis time)
    {
        if (!backup) return;
        std::uint8_t seat;
        rtc::variant<rtc::Description, rtc::Candidate> value{rtc::Candidate{}};
        while (backup->PollSignal(seat, value)) {
            Json message{{"generation", generation}};
            if (auto *description = std::get_if<rtc::Description>(&value)) {
                message["kind"] = "description";
                message["description"] = {{"type", description->typeString()}, {"sdp", std::string(*description)}};
                if (standby.direct) message["route"] = "direct";
            } else {
                const auto &candidate = std::get<rtc::Candidate>(value);
                message["kind"] = "candidate";
                message["candidate"] = {{"candidate", candidate.candidate()}, {"sdpMid", candidate.mid()}};
            }
            if (!Send(primary, message)) { Discard(time); break; }
        }
    }

    bool OfferReady(Millis time) const
    {
        return offered && Eligible(time) && offered->path != active && Ready(*offered->path) &&
            time - offered->at <= 10000 && time - offered->path->lastRttAt <= 3000;
    }

    const char *RouteLabel(Path &path) const
    {
        if (path.surveySerial) return path.direct ? "直连" : path.route == "turn" ? "TURN" : "Swarm";
        if (path.direct || (&path == &primary && !path.peer->UsesRelay())) return "直连";
        return swarm ? "Swarm" : "TURN";
    }

    bool BeginHandoff(Path &path, Millis time, std::uint32_t offerTicket = 0)
    {
        if (proposal || &path == active || !Ready(path)) return false;
        const auto id = epoch + 1;
        Json message{{"kind", "switch"}, {"path", Name(path)}, {"epoch", id}, {"generation", WireGeneration(path)}};
        if (offerTicket) message["offerTicket"] = offerTicket;
        if (!Send(path, message)) return false;
        proposal = Proposal{&path, id, time};
        offered.reset();
        return true;
    }

    void Verify(Millis time, bool recover, bool announce = true, Path *target = nullptr)
    {
        if (!Switching() || !Eligible(time)) return;
        if (OfferReady(time)) {
            if (recover && Controller()) BeginHandoff(*offered->path, time);
            return;
        }
        if (!target) target = Other();
        if (!target || !Ready(*target) || (!recover && time < verifyAfter)) return;
        if (!verification) {
            verification = time;
            verificationTarget = target;
            force = recover;
            currentTicket = pairedQuality && Controller() ? ++verificationTicket : 0;
            remoteQuality.reset();
            qualityReported = false;
            if (announce) {
                Json message{{"kind", "verify"}};
                if (currentTicket) message["ticket"] = currentTicket;
                if (currentTicket && target->surveySerial) {
                    message["serial"] = target->surveySerial;
                    Send(*target, message);
                } else Broadcast(message);
            }
        } else if (recover) force = true;
    }

    void Select(Path &path, std::uint32_t id, Millis time)
    {
        if (active != &path) {
            previous = active;
            previousUntil = time + 5000;
            active = &path;
            activeSince = time;
            outageReportedAt = 0;
            switched = time;
            if (active == &standby) {
                if (standby.direct) std::fputs("RTC switched to direct standby\n", stderr);
                else std::fputs(swarm ? "RTC switched to Swarm standby\n" : "RTC switched to TURN standby\n", stderr);
            }
            else if (path.surveySerial) std::fprintf(stderr, "RTC switched to %s (survey %u)\n", path.route.c_str(), path.surveySerial);
            else std::fputs("RTC switched to primary\n", stderr);
        }
        epoch = id;
        offered.reset();
        recovery = 0;
        resendFrom = pending.empty() ? 0 : pending.begin()->first;
    }

    bool Handoff(Path &path, const Json &m, std::string_view kind, Millis time)
    {
        const auto id = number(m, "epoch");
        if (!id || number(m, "generation") != WireGeneration(path)) return false;
        auto send = [&](std::string_view phase) {
            return Send(path, {{"kind", phase}, {"epoch", id}, {"generation", WireGeneration(path)}});
        };
        if (!Controller() && kind == "switch" && id == epoch + 1 && Eligible(time)) {
            const auto label = m.value("path", std::string{});
            if (label == Name(path)) {
                if (!Ready(path) || (proposal && proposal->path != &path)) return false;
                if (m.contains("offerTicket") && (!OfferReady(time) || offered->path != &path ||
                    number(m, "offerTicket") != offered->ticket)) return false;
                proposal = Proposal{&path, id, time};
                offered.reset();
                return send("accept");
            }
        }
        if (!Controller() && kind == "commit" && id == epoch && &path == active) return send("committed");
        if (!proposal || proposal->epoch != id || proposal->path != &path) return false;
        if (Controller() && kind == "accept" && Ready(path) && send("commit")) {
            Select(path, id, time);
            proposal->lastCommit = time;
            ClearVerification();
        } else if (!Controller() && kind == "commit" && Ready(path)) {
            Select(path, id, time);
            send("committed");
            proposal.reset();
            ClearVerification();
        } else if (Controller() && kind == "committed" && proposal->lastCommit) proposal.reset();
        return true;
    }

    void Text(Path &path, bool control, const std::string &message, Millis time)
    {
        if (message.size() > 24576 || message.compare(0, Prefix.size(), Prefix) != 0) return;
        auto m = Json::parse(message.begin() + Prefix.size(), message.end(), nullptr, false);
        if (!m.is_object() || !m.contains("kind") || !m["kind"].is_string()) return;
        const auto &kind = m["kind"].get_ref<const std::string &>();
        if (startup && !control) {
            const auto id = number(m, "id");
            if (id && kind == "startup-ping") {
                Send(path, {{"kind", "startup-pong"}, {"id", id}}, false);
                return;
            }
            if (id && kind == "startup-pong") {
                for (auto &sample : path.samples)
                    if (sample.id == id && sample.rtt < 0 && time - sample.sent <= ProbeTimeout) sample.rtt = time - sample.sent;
                return;
            }
        }
        if (kind == "hello" && (&path == &primary || &path == active || (startup && &path == planned.get())) && control) {
            supported = m.contains("enabled") && m["enabled"] == true;
            peerSwitching = !m.contains("switching") || m["switching"] != false;
            directRetry = supported && m.contains("directRetry") && m["directRetry"] == true;
            pairedQuality = supported && m.contains("pairedQuality") && m["pairedQuality"] == true;
            manualSwitch = pairedQuality && manualConfirmation && m.contains("manualSwitch") && m["manualSwitch"] == true;
            return;
        }
        if (!supported || disabled) return;
        if (!control) {
            const auto id = number(m, "id");
            if (kind == "ping" && id) {
                if (running) Send(path, {{"kind", "pong"}, {"id", id}}, false);
                else if (!path.measuring) Send(path, {{"kind", "pause"}});
            } else if (kind == "pong" && id) {
                for (auto &sample : path.samples) {
                    if (sample.id != id || sample.rtt >= 0) continue;
                    const auto rtt = time - sample.sent;
                    if (rtt > ProbeTimeout) break;
                    sample.rtt = rtt;
                    path.rttMs = static_cast<unsigned>(rtt);
                    path.lastRttAt = time;
                    if (recovery && &path == active && id > recoveryProbe &&
                        path.peer->IsOpen() && pending.empty() && !proposal) {
                        recovery = 0;
                        force = false;
                    }
                    break;
                }
            }
            return;
        }
        if (path.measuring) return;
        if (kind == "pause") Suspend(time);
        else if (kind == "route-offer" && manualSwitch && !Controller() && &path != active && Ready(path) &&
                 number(m, "ticket") == verificationTicket && verificationTicket &&
                 number(m, "epoch") == epoch && number(m, "generation") == WireGeneration(path)) {
            const auto from = m.value("fromMs", Json(nullptr)), to = m.value("toMs", Json(nullptr));
            if (to.is_number_unsigned() && to.get<std::uint64_t>() <= 10000 &&
                (from.is_null() || (from.is_number_unsigned() && from.get<std::uint64_t>() <= 10000))) {
                const auto ticket = verificationTicket;
                offered = Offer{&path, ticket, time, from.is_null() ? -1 : from.get<int>(), to.get<int>()};
                ClearVerification();
            }
        }
        else if (kind == "route-confirm" && manualSwitch && Controller() && OfferReady(time) && &path == offered->path &&
                 number(m, "ticket") == offered->ticket && number(m, "epoch") == epoch &&
                 number(m, "generation") == WireGeneration(path)) BeginHandoff(path, time, offered->ticket);
        else if (kind == "need" && Controller()) Verify(time, true, pairedQuality);
        else if (kind == "verify") {
            if (!pairedQuality) Verify(time, false, false);
            else if (Controller() && !m.contains("ticket")) Verify(time, false);
            else if (!Controller() && Switching() && Eligible(time)) {
                const auto ticket = number(m, "ticket");
                if (ticket > verificationTicket) {
                    const auto serial = number(m, "serial");
                    auto *target = serial ? &path : Other();
                    if (!target || (serial && (&path != planned.get() || path.surveySerial != serial))) return;
                    verificationTicket = currentTicket = ticket;
                    verification = time;
                    verificationTarget = target;
                    qualityReported = false;
                }
            }
        } else if (kind == "quality" && pairedQuality && Controller() && verification && currentTicket &&
                   &path == verificationTarget && number(m, "ticket") == currentTicket &&
                   number(m, "generation") == WireGeneration(path) && number(m, "epoch") == epoch) {
            const auto present = readQuality(m.value("present", Json{})), candidate = readQuality(m.value("candidate", Json{}));
            if (present && candidate && candidate->count >= 12) remoteQuality = Comparison{*present, *candidate};
        }
        else if (kind == "retire" && &path == &primary && number(m, "generation") == generation) Discard(time);
        else if (kind == "ack") {
            const auto sequence = number(m, "sequence");
            if (sequence <= outgoing) {
                auto end = pending.upper_bound(sequence);
                for (auto entry = pending.begin(); entry != end; ++entry) pendingBytes -= entry->second.size();
                pending.erase(pending.begin(), end);
            }
        } else if (Switching() && &path == &primary && (kind == "description" || kind == "candidate")) {
            const auto id = number(m, "generation");
            if (!id || id > 1000000) return;
            try {
                if (kind == "description") {
                    const auto &description = m.at("description");
                    const auto sdp = description.at("sdp").get<std::string>();
                    const auto type = description.at("type").get<std::string>();
                    if (sdp.size() > 16384 || type != (Controller() ? "answer" : "offer")) return;
                    const auto route = m.value("route", std::string{});
                    const bool direct = route == "direct";
                    if ((!route.empty() && !direct) || (direct && !directRetry)) return;
                    if (!Controller() && id > generation && !backup) MakeBackup(id, time, direct);
                    if (backup && id == generation && standby.direct == direct)
                        backup->SetRemoteDescription(remote, rtc::Description(sdp, type));
                } else if (backup && id == generation) {
                    const auto &candidate = m.at("candidate");
                    const auto line = candidate.at("candidate").get<std::string>();
                    if (line.size() <= 2048) backup->AddRemoteCandidate(remote, rtc::Candidate(line, candidate.value("sdpMid", "0")));
                }
            } catch (const std::exception &) { Discard(time); }
        } else if (kind == "switch" || kind == "accept" || kind == "commit" || kind == "committed") {
            if (Switching() && (!m.contains("path") || m["path"].is_string())) Handoff(path, m, kind, time);
        }
    }

    void Enqueue(const rtc::binary &binary, std::size_t offset = 0)
    {
        if (binary.size() <= offset || inboxBytes + binary.size() - offset > MaxBytes || inbox.size() >= MaxControls) return;
        inbox.emplace_back(binary.size() - offset);
        std::memcpy(inbox.back().data(), binary.data() + offset, inbox.back().size());
        inboxBytes += inbox.back().size();
    }

    bool DeliverOrdered()
    {
        bool advanced = false;
        while (inbox.size() < MaxControls) {
            auto item = reorder.find(incoming + 1);
            if (item == reorder.end() || inboxBytes + item->second.size() > MaxBytes) break;
            inboxBytes += item->second.size();
            reorderBytes -= item->second.size() + 8;
            inbox.push_back(std::move(item->second));
            reorder.erase(item);
            ++incoming;
            advanced = true;
        }
        return advanced;
    }

    void Binary(Path &path, bool control, const rtc::binary &bytes, Millis time)
    {
        if (path.measuring) return;
        if (!bytes.empty()) path.lastTrafficAt = time;
        if (supported && control && bytes.size() >= 8 && std::memcmp(bytes.data(), "ETR1", 4) == 0) {
            std::uint32_t id = 0;
            for (unsigned byte = 0; byte < 4; ++byte)
                id |= std::to_integer<std::uint32_t>(bytes[byte + 4]) << (byte * 8);
            if (!id || std::uint64_t(id) > std::uint64_t(incoming) + MaxControls || bytes.size() <= 8 || bytes.size() > MaxBytes) return;
            if (id > incoming && !reorder.count(id) && reorderBytes + bytes.size() <= MaxBytes) {
                Bytes payload(bytes.size() - 8);
                std::memcpy(payload.data(), bytes.data() + 8, payload.size());
                reorderBytes += bytes.size();
                reorder.emplace(id, std::move(payload));
            }
            DeliverOrdered();
            ackPending = !Send(path, {{"kind", "ack"}, {"sequence", incoming}});
        } else if (&path == active || (&path == previous && time <= previousUntil)) Enqueue(bytes);
    }

    void Drain(Path &path, Millis time)
    {
        for (unsigned count = 0; path.peer && inbox.size() < MaxControls && count < 128; ++count) {
            bool control;
            auto packet = path.peer->PollMessage(control);
            if (!packet) break;
            if (const auto *message = std::get_if<std::string>(&*packet)) Text(path, control, *message, time);
            else Binary(path, control, std::get<rtc::binary>(*packet), time);
        }
    }

    void Probe(Path &path, Millis time)
    {
        if (!Ready(path) || (path.measuring && path.reported)) return;
        while (!path.samples.empty() && time - path.samples.front().sent > Window) path.samples.pop_front();
        const auto interval = path.measuring || verification ? 50 :
            (&path == active || (offered && offered->path == &path) ? 250 : 10000);
        if (time - path.lastProbe < interval) return;
        // A probe refused by a full input channel is also a failed sample.
        Send(path, {{"kind", "ping"}, {"id", ++path.probes}}, false);
        path.samples.push_back({path.probes, time});
        path.lastProbe = time;
    }

    void Survey(Millis time)
    {
        if (!surveySignal || !pairedQuality || !Switching()) return;
        const bool outage = Stalled(time);
        if (!outage) outageReportedAt = 0;
        if (outage ? !outageReportedAt : time >= nextSlowCheck) {
            nextSlowCheck = time + 250;
            const auto current = active->Measure(time, outage ? ProbeWaitSince() : time - Window, 40);
            const auto route = active->surveySerial ? active->route :
                active->peer->UsesRelay() ? initialRelayRoute : "direct";
            if (outage ? current.count >= 5 : !offered && current.count >= 12 &&
                (current.p95 > 150 || current.loss > .05 || (initialSurvey && route != "direct"))) {
                if (surveySignal({{"type", "rtc-route-slow"}, {"route", route}, {"quality", surveyReport(current)}})) {
                    nextSlowCheck = time + 5000;
                    if (outage) outageReportedAt = time;
                }
            }
        }
        if (!PollPlannedSignals(time)) return;
        if (Measuring()) {
            const auto result = planned->Measure(time, planned->createdAt, 40);
            if (result.count >= 12) StandbyReport(*planned, surveyReport(result));
        } else if (!planned->measuring && Controller() && !proposal && !verification) Verify(time, false, true, planned.get());
    }

    bool PollPlannedSignals(Millis time)
    {
        if (!planned || planned.get() == active) return false;
        if (planned->peer->Failed() || time - planned->createdAt > 15000) {
            StandbyReport(*planned, nullptr);
            if (!proposal || proposal->path != planned.get()) Retire(planned);
            return false;
        }
        std::uint8_t seat;
        rtc::variant<rtc::Description, rtc::Candidate> value{rtc::Candidate{}};
        while (planned->peer->PollSignal(seat, value)) {
            Json message{{"type", "rtc-standby-signal"}, {"serial", planned->surveySerial}};
            if (auto *description = std::get_if<rtc::Description>(&value)) {
                message["description"] = {{"type", description->typeString()}, {"sdp", std::string(*description)}};
            } else {
                const auto &candidate = std::get<rtc::Candidate>(value);
                message["candidate"] = {{"candidate", candidate.candidate()}, {"sdpMid", candidate.mid()}};
            }
            surveySignal(message);
        }
        return Ready(*planned);
    }

    void ProbeStartup(Path &path, Millis time)
    {
        if (!Ready(path) || path.reported) return;
        if (path.samples.size() < StartupSamples && time - path.lastProbe >= 50) {
            path.samples.push_back({++path.probes, time});
            path.lastProbe = time;
            Send(path, {{"kind", "startup-ping"}, {"id", path.probes}}, false);
        }
        if (path.samples.size() != StartupSamples) return;
        Quality quality;
        for (const auto &sample : path.samples) {
            if (sample.rtt < 0 && time - sample.sent < ProbeTimeout) return;
            ++quality.count;
            if (sample.rtt >= 0) {
                quality.p95 = quality.received ? (std::max)(quality.p95, double(sample.rtt)) : double(sample.rtt);
                ++quality.received;
            }
        }
        path.reported = surveySignal({{"type", "rtc-startup-quality"}, {"serial", path.surveySerial},
            {"direct", &path == &primary && !path.peer->UsesRelay()}, {"quality", surveyReport(quality)}});
    }

    void Assess(Millis time)
    {
        if (time < nextCheck) return;
        nextCheck = time + 50;
        if (offered) {
            const auto current = active->Measure(time, time - Window);
            if (!OfferReady(time) || (!offered->confirmed && current.count >= 12 && current.score() <= offered->toMs * 1.1 + 5))
                offered.reset();
        }
        if (recovery && time > recovery) { disabled = true; return; }
        if ((!planned || !planned->measuring) && !verification && (!switched || time - switched >= Cooldown) && Other()) {
            const auto current = active->Measure(time, time - Window);
            if (current.count >= 12 && (firstComparison || current.p95 >= 60 || current.loss >= .1)) {
                Verify(time, false);
                if (verification) firstComparison = false;
            }
        }
        if (verification && time - verification >= ProbeTimeout + 600) {
            if (auto *target = verificationTarget; !proposal && target && Ready(*target)) {
                auto &next = *target;
                auto candidate = next.Measure(time, verification);
                auto current = active->Measure(time, verification);
                if (!Controller() && pairedQuality && currentTicket && !qualityReported && candidate.count >= 12 &&
                    (!Ready(*active) || current.count >= 12)) {
                    qualityReported = Send(next, {{"kind", "quality"}, {"ticket", currentTicket}, {"epoch", epoch},
                        {"generation", WireGeneration(next)}, {"present", report(current)}, {"candidate", report(candidate)}});
                } else if (Controller() && (!pairedQuality || remoteQuality)) {
                    if (remoteQuality && pairedQuality) {
                        candidate = paired(candidate, remoteQuality->candidate);
                        current = paired(current, remoteQuality->present);
                    }
                    if (candidate.received >= 12 && candidate.loss <= .05 &&
                        ((force && !Ready(*active)) || candidate.score() * 1.1 + 5 < current.score())) {
                        if (!manualSwitch || force) BeginHandoff(next, time);
                        else {
                            const auto from = std::isfinite(current.p95) ? static_cast<int>(current.p95) : -1;
                            const auto to = static_cast<int>(candidate.p95);
                            if (Send(next, {{"kind", "route-offer"}, {"ticket", currentTicket}, {"epoch", epoch},
                                    {"generation", WireGeneration(next)}, {"fromMs", from < 0 ? Json(nullptr) : Json(from)}, {"toMs", to}})) {
                                offered = Offer{&next, currentTicket, time, from, to};
                                ClearVerification();
                            }
                        }
                    }
                }
            }
            if (verification && time - verification > 4000) { ClearVerification(); verifyAfter = time + 10000; }
        }
        if (Controller() && proposal) {
            if (proposal->lastCommit) {
                if (time - proposal->at > 7000) disabled = true;
                else if (time - proposal->lastCommit >= 250) {
                    Send(*proposal->path, {{"kind", "commit"}, {"epoch", proposal->epoch}, {"generation", WireGeneration(*proposal->path)}});
                    proposal->lastCommit = time;
                }
            } else if (time - proposal->at > 3000) { proposal.reset(); ClearVerification(); verifyAfter = time + 10000; }
        }
    }

    void Replay()
    {
        if (!resendFrom || !active->peer->IsOpen()) return;
        auto it = pending.lower_bound(resendFrom);
        for (; it != pending.end(); ++it) {
            if (active->peer->BufferedAmount() > 32768 || !active->peer->SendControl(it->second.data(), it->second.size())) break;
        }
        resendFrom = it == pending.end() ? 0 : it->first;
    }
};

NativeAdaptiveRoute::NativeAdaptiveRoute(NativePeerTransport &primary, std::string tag, std::uint8_t local,
                                         rtc::Configuration standbyConfig, bool swarm, bool allowSwitching, bool manualConfirmation)
    : state_(std::make_unique<State>(primary, std::move(tag), local, std::move(standbyConfig), swarm, allowSwitching, manualConfirmation)) {}
NativeAdaptiveRoute::~NativeAdaptiveRoute() = default;

void NativeAdaptiveRoute::Pump(bool running)
{
    auto &s = *state_;
    const auto time = now();
    if (running && !s.running) s.activeSince = time;
    s.running = running;
    if (s.lastPump && time - s.lastPump > 750) {
        s.Broadcast({{"kind", "pause"}});
        s.Suspend(time);
    }
    s.lastPump = time;
    if (!s.hello) s.hello = s.Send(*s.active, {{"kind", "hello"}, {"enabled", true}, {"switching", s.allowSwitching},
        {"directRetry", true}, {"pairedQuality", true}, {"manualSwitch", s.manualConfirmation}});
    s.Drain(s.primary, time);
    s.Drain(s.standby, time);
    if (s.planned) s.Drain(*s.planned, time);
    if (s.retained) s.Drain(*s.retained, time);
    if (s.startup) {
        s.ProbeStartup(s.primary, time);
        if (s.PollPlannedSignals(time)) s.ProbeStartup(*s.planned, time);
        return;
    }
    if (!s.supported || s.disabled) return;
    if (s.Switching() && s.Controller() && !s.backup && !s.planned && s.primary.peer->IsOpen() && time >= s.retryAt) {
        const bool direct = s.primary.peer->UsesRelay();
        if (!direct || s.directRetry) {
            try { s.MakeBackup(s.generation + 1, time, direct); }
            catch (const std::exception &) { s.retryAt = time + Cooldown; }
        }
    }
    if (s.backup && !s.Ready(s.standby) && (s.backup->Failed() || time - s.backupStarted > 15000)) s.Discard(time);
    s.ExchangeSignals(time);
    if (s.ackPending) s.ackPending = !s.Send(*s.active, {{"kind", "ack"}, {"sequence", s.incoming}});
    s.Replay();
    if (s.Eligible(time)) {
        for (auto *path : s.Paths()) if (path) s.Probe(*path, time);
        s.Survey(time);
        s.Assess(time);
    }
}

bool NativeAdaptiveRoute::Recover()
{
    auto &s = *state_;
    const auto time = now();
    if (!s.supported || s.disabled || !s.Switching() || !s.running) return false;
    const bool waitingForSurvey = (s.planned && s.planned.get() != s.active) ||
        (s.outageReportedAt && time - s.outageReportedAt <= 1500);
    if (!s.Other() && !waitingForSurvey) return false;
    if (!s.recovery) {
        s.recovery = (std::max)(time, s.quietUntil) + (waitingForSurvey ? 16000 : 7000);
        s.recoveryProbe = s.active->probes;
        if (s.Stalled(time)) std::fputs("RTC path stopped responding; checking standby\n", stderr);
    }
    if (time > s.recovery) return false;
    s.Verify(time, true);
    if (!s.Controller() && time - s.lastNeed >= 1000) {
        s.Broadcast({{"kind", "need"}});
        s.lastNeed = time;
    }
    return true;
}

NativePeerTransport &NativeAdaptiveRoute::Active() const { return *state_->active->peer; }
bool NativeAdaptiveRoute::HasLatencyProbes() const { return state_->supported && state_->running; }
bool NativeAdaptiveRoute::ProbesUnanswered() const
{
    return HasLatencyProbes() && now() - state_->ProbeWaitSince() > RttFreshness;
}

bool NativeAdaptiveRoute::Stalled() const
{
    return state_->Stalled(now());
}

std::optional<unsigned> NativeAdaptiveRoute::ActiveRtt() const
{
    const auto &path = *state_->active;
    if (!path.lastRttAt || now() - path.lastRttAt > RttFreshness) return std::nullopt;
    return path.rttMs;
}
bool NativeAdaptiveRoute::ActiveUsesSwarm() const
{
    const auto &path = *state_->active;
    return path.surveySerial ? path.route.rfind("swarm-", 0) == 0 : state_->swarm;
}
std::uint32_t NativeAdaptiveRoute::ActivePathEpoch() const { return state_->epoch; }
RouteSwitchStatus NativeAdaptiveRoute::SwitchStatus() const
{
    auto &s = *state_;
    if (!s.manualSwitch) return {};
    if (s.proposal || (s.offered && s.offered->confirmed && s.OfferReady(now()))) return {nullptr, nullptr, -1, -1, true};
    if (!s.OfferReady(now())) return {};
    return {s.RouteLabel(*s.active), s.RouteLabel(*s.offered->path), s.offered->fromMs, s.offered->toMs, false};
}

bool NativeAdaptiveRoute::ConfirmOffer()
{
    auto &s = *state_;
    const auto time = now();
    if (!s.OfferReady(time) || s.proposal || s.offered->confirmed) return false;
    if (s.Controller()) return s.BeginHandoff(*s.offered->path, time, s.offered->ticket);
    auto &offer = *s.offered;
    offer.confirmed = s.Send(*offer.path, {{"kind", "route-confirm"}, {"ticket", offer.ticket},
        {"epoch", s.epoch}, {"generation", s.WireGeneration(*offer.path)}});
    return offer.confirmed;
}
void NativeAdaptiveRoute::ConfigureSurvey(std::string initialRoute, bool initialSurvey, SurveySignal send, RetirePeer retire)
{
    state_->initialRelayRoute = std::move(initialRoute);
    state_->initialSurvey = initialSurvey;
    state_->surveySignal = std::move(send);
    state_->retirePeer = std::move(retire);
}
void NativeAdaptiveRoute::BeginStartup() { state_->startup = true; }
bool NativeAdaptiveRoute::SelectStartup(std::uint32_t serial)
{
    auto &s = *state_;
    auto *path = serial ? s.planned.get() : &s.primary;
    if (!s.startup || !path || path->surveySerial != serial || !s.Ready(*path)) return false;
    s.Select(*path, 0, now());
    s.startup = false;
    if (serial) {
        path->measuring = false;
        s.initialSurvey = false;
        s.initialRelayRoute = "turn";
        s.hello = false;
    } else s.Retire(s.planned);
    path->samples.clear();
    return true;
}
void NativeAdaptiveRoute::RelayStandby(const Json &message, rtc::Configuration ice)
{
    state_->Standby(message, std::move(ice));
}
bool NativeAdaptiveRoute::Recovering() const { return state_->recovery != 0; }
bool NativeAdaptiveRoute::Failed() const { return state_->disabled; }

bool NativeAdaptiveRoute::SendControl(const std::uint8_t *data, std::size_t size)
{
    auto &s = *state_;
    if (!s.supported) return s.active->peer->SendControl(data, size);
    if (!data || !size || size > MaxBytes - 8 || s.pending.size() >= MaxControls || s.pendingBytes + size + 8 > MaxBytes ||
        s.resendFrom || s.outgoing == UINT32_MAX || s.active->peer->BufferedAmount() > 32768) return false;
    const auto id = ++s.outgoing;
    Bytes packet(size + 8);
    std::memcpy(packet.data(), "ETR1", 4);
    for (unsigned byte = 0; byte < 4; ++byte) packet[byte + 4] = static_cast<std::uint8_t>(id >> (byte * 8));
    std::memcpy(packet.data() + 8, data, size);
    s.pendingBytes += packet.size();
    auto &stored = s.pending.emplace(id, std::move(packet)).first->second;
    if (!s.active->peer->SendControl(stored.data(), stored.size())) s.resendFrom = id;
    return true;
}

bool NativeAdaptiveRoute::Poll(std::vector<std::uint8_t> *packet)
{
    auto &s = *state_;
    if (!packet || s.inbox.empty()) return false;
    *packet = std::move(s.inbox.front());
    s.inbox.pop_front();
    s.inboxBytes -= packet->size();
    if (!s.reorder.empty() && s.DeliverOrdered()) s.ackPending = true;
    return true;
}
}
