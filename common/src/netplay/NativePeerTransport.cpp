#include <eagler/netplay/NativePeerTransport.hpp>
#include <rtc/datachannel.hpp>

#include <cstring>
#include <chrono>
#include <cstdio>
#include <deque>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace Netplay
{
namespace
{
using Signal = rtc::variant<rtc::Description, rtc::Candidate>;
using Channel = std::shared_ptr<rtc::DataChannel>;
constexpr std::size_t BufferLimit = 2 * 1024 * 1024;

bool send(const Channel &channel, const std::uint8_t *data, std::size_t size)
{
    if (!channel || !channel->isOpen() || !data || !size || size > channel->maxMessageSize()
        || size > BufferLimit || channel->bufferedAmount() > BufferLimit - size)
        return false;
    try {
        // libdatachannel returns false for a queued (accepted) send.
        channel->send(reinterpret_cast<const rtc::byte *>(data), size);
        return true;
    } catch (const std::exception &) {
        return false;
    }
}
}

struct NativePeerTransport::Impl
{
    struct Peer
    {
        explicit Peer(rtc::Configuration config)
            : connection(std::make_shared<rtc::PeerConnection>(std::move(config))) {}
        std::shared_ptr<rtc::PeerConnection> connection;
        mutable std::mutex mutex;
        Channel input;
        Channel control;
        std::vector<rtc::Candidate> pendingCandidates;

        std::pair<Channel, Channel> channels() const
        {
            std::lock_guard lock(mutex);
            return {input, control};
        }
    };
    struct Signals
    {
        std::mutex mutex;
        std::deque<std::pair<std::uint8_t, Signal>> pending;
        bool overflow = false;

        void push(std::uint8_t player, Signal signal)
        {
            std::lock_guard lock(mutex);
            if (pending.size() >= 512) overflow = true;
            else pending.emplace_back(player, std::move(signal));
        }
    };

    std::string inputLabel;
    std::string controlLabel;
    std::uint8_t localPlayer;
    rtc::Configuration config;
    std::vector<std::shared_ptr<Peer>> peers;
    std::shared_ptr<Signals> signals = std::make_shared<Signals>();
    bool opened = false;
    std::size_t nextChannel = 0;
};

NativePeerTransport::NativePeerTransport(std::string tag, std::uint8_t localPlayer,
                                         std::uint8_t playerCount, rtc::Configuration config)
    : impl_(std::make_unique<Impl>())
{
    if (playerCount < 2 || localPlayer >= playerCount || tag.empty() || tag.size() > 32
        || tag.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_") != std::string::npos)
        throw std::invalid_argument("Invalid peer transport configuration");
    impl_->inputLabel = tag + "-input";
    impl_->controlLabel = tag + "-control";
    impl_->localPlayer = localPlayer;
    impl_->config = std::move(config);
    impl_->config.disableAutoNegotiation = true;
    impl_->peers.resize(playerCount);
}

NativePeerTransport::~NativePeerTransport()
{
    for (const auto &peer : impl_->peers) {
        if (!peer) continue;
        peer->connection->resetCallbacks();
        peer->connection->close();
    }
}

void NativePeerTransport::AddPeer(std::uint8_t player)
{
    auto &state = *impl_;
    if (player >= state.peers.size() || player == state.localPlayer)
        throw std::invalid_argument("Invalid peer index");
    if (state.peers[player]) return;
    auto peer = std::make_shared<Impl::Peer>(state.config);
    const auto &connection = peer->connection;
    connection->onLocalDescription([signals = state.signals, player](rtc::Description description) {
        signals->push(player, std::move(description));
    });
    connection->onLocalCandidate([signals = state.signals, player](rtc::Candidate candidate) {
        signals->push(player, std::move(candidate));
    });
    connection->onDataChannel([weak = std::weak_ptr(peer), inputLabel = state.inputLabel,
                                controlLabel = state.controlLabel](Channel channel) {
        auto peer = weak.lock();
        if (!peer) return;
        std::lock_guard lock(peer->mutex);
        if (channel->label() == inputLabel && !peer->input) peer->input = std::move(channel);
        else if (channel->label() == controlLabel && !peer->control) peer->control = std::move(channel);
    });
    if (state.localPlayer < player) {
        rtc::DataChannelInit input;
        input.reliability.unordered = true;
        input.reliability.maxRetransmits = 0;
        peer->control = connection->createDataChannel(state.controlLabel);
        peer->input = connection->createDataChannel(state.inputLabel, input);
        connection->setLocalDescription(rtc::Description::Type::Offer);
    }
    state.peers[player] = std::move(peer);
}

void NativePeerTransport::SetRemoteDescription(std::uint8_t player, rtc::Description description)
{
    const auto started = std::chrono::steady_clock::now();
    AddPeer(player);
    auto &peer = *impl_->peers[player];
    const bool offer = description.type() == rtc::Description::Type::Offer;
    peer.connection->setRemoteDescription(std::move(description));
    const auto applied = std::chrono::steady_clock::now();
    for (auto &candidate : std::exchange(peer.pendingCandidates, {}))
        AddRemoteCandidate(player, std::move(candidate));
    if (offer) peer.connection->setLocalDescription(rtc::Description::Type::Answer);
    const auto ms = [](auto interval) {
        return static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(interval).count());
    };
    const auto checked = std::chrono::steady_clock::now();
    if (const auto total = ms(checked - started); total >= 250)
        std::fprintf(stderr, "RTC SDP processing: remote=%lldms local=%lldms\n", ms(applied - started), ms(checked - applied));
}

void NativePeerTransport::AddRemoteCandidate(std::uint8_t player, rtc::Candidate candidate)
{
    AddPeer(player);
    auto &peer = *impl_->peers[player];
    const auto unavailable = [&] {
        const auto state = peer.connection->state();
        return state == rtc::PeerConnection::State::Disconnected || state == rtc::PeerConnection::State::Failed
            || state == rtc::PeerConnection::State::Closed;
    };
    if (unavailable()) return;
    if (!peer.connection->remoteDescription()) {
        if (peer.pendingCandidates.size() < 512) peer.pendingCandidates.push_back(std::move(candidate));
        return;
    }
    const auto appliedAt = std::chrono::steady_clock::now();
    const auto type = candidate.type();
    try {
        peer.connection->addRemoteCandidate(std::move(candidate));
    } catch (const std::logic_error &) {
        // libdatachannel can finish closing ICE while a trickled candidate is being applied.
        if (!unavailable()) throw;
    }
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - appliedAt).count();
    if (elapsed >= 250)
        std::fprintf(stderr, "RTC remote candidate processing: type=%d elapsed=%lldms\n",
            static_cast<int>(type), static_cast<long long>(elapsed));
}

bool NativePeerTransport::PollSignal(std::uint8_t &player, Signal &signal)
{
    auto &signals = *impl_->signals;
    std::lock_guard lock(signals.mutex);
    if (signals.pending.empty()) return false;
    auto &front = signals.pending.front();
    player = front.first;
    signal = std::move(front.second);
    signals.pending.pop_front();
    return true;
}

bool NativePeerTransport::IsOpen() const
{
    for (std::size_t index = 0; index < impl_->peers.size(); ++index) {
        if (index == impl_->localPlayer) continue;
        const auto &peer = impl_->peers[index];
        if (!peer) return false;
        const auto [input, control] = peer->channels();
        if (!input || !input->isOpen() || !control || !control->isOpen()) return false;
    }
    impl_->opened = true;
    return true;
}

bool NativePeerTransport::Failed() const
{
    {
        std::lock_guard lock(impl_->signals->mutex);
        if (impl_->signals->overflow) return true;
    }
    for (const auto &peer : impl_->peers)
        if (peer && peer->connection->state() == rtc::PeerConnection::State::Failed) return true;
    return false;
}

NativePeerTransport::Link NativePeerTransport::LinkStatus(bool readRtt) const
{
    for (const auto &peer : impl_->peers) {
        rtc::Candidate local, remote;
        if (!peer || !peer->connection->getSelectedCandidatePair(&local,&remote)) continue;
        const bool relayed = local.type() == rtc::Candidate::Type::Relayed || remote.type() == rtc::Candidate::Type::Relayed;
        const auto marker = [](const rtc::Candidate &candidate) {
            const auto line = candidate.candidate();
            constexpr std::string_view prefix = "candidate:";
            if (line.rfind(prefix, 0) != 0) return false;
            auto foundation = std::string_view(line).substr(prefix.size());
            foundation = foundation.substr(0, foundation.find(' '));
            return candidate.type() == rtc::Candidate::Type::ServerReflexive && candidate.transportType() == rtc::Candidate::TransportType::Udp &&
                foundation.size() > 4 && foundation.size() <= 32 && foundation.compare(foundation.size()-4,4,"next") == 0 &&
                foundation.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/") == std::string::npos;
        };
        Link result;
        result.route = relayed ? Route::Relay : (marker(local) || marker(remote)) ? Route::Nat4 : Route::Direct;
        if (readRtt) {
            if (auto rtt = peer->connection->rtt(); rtt && rtt->count() >= 0)
                result.rttMs = static_cast<unsigned>(rtt->count());
        }
        return result;
    }
    return {};
}

std::string NativePeerTransport::ConnectionSummary() const
{
    std::ostringstream summary;
    for (std::size_t index = 0; index < impl_->peers.size(); ++index) {
        if (index == impl_->localPlayer) continue;
        summary << " P" << index + 1;
        const auto &peer = impl_->peers[index];
        if (!peer) {
            summary << " signaling";
            continue;
        }
        const auto [input, control] = peer->channels();
        summary << " rtc=" << peer->connection->state() << " ice=" << peer->connection->iceState()
            << " gathering=" << peer->connection->gatheringState()
            << " signaling=" << peer->connection->signalingState()
            << " input=" << bool(input && input->isOpen()) << " control=" << bool(control && control->isOpen());
        rtc::Candidate local, remote;
        if (peer->connection->getSelectedCandidatePair(&local, &remote))
            summary << " pair=" << local.type() << '/' << remote.type();
    }
    return summary.str();
}

bool NativePeerTransport::UsesRelay(bool localOnly) const
{
    for (const auto &peer : impl_->peers) {
        rtc::Candidate local, remote;
        if (peer && peer->connection->getSelectedCandidatePair(&local, &remote) &&
            (local.type() == rtc::Candidate::Type::Relayed || (!localOnly && remote.type() == rtc::Candidate::Type::Relayed)))
            return true;
    }
    return false;
}

bool NativePeerTransport::SendText(std::uint8_t player, bool control, std::string message)
{
    if (player >= impl_->peers.size() || !impl_->peers[player]) return false;
    const auto [input, reliable] = impl_->peers[player]->channels();
    const auto &channel = control ? reliable : input;
    if (!channel || !channel->isOpen() || message.size() > channel->maxMessageSize() ||
        channel->bufferedAmount() > (control ? 32768 : 4096)) return false;
    try { channel->send(std::move(message)); return true; }
    catch (const std::exception &) { return false; }
}

bool NativePeerTransport::Disconnected() const { return impl_->opened && !IsOpen(); }

bool NativePeerTransport::SendTo(std::uint8_t player, const std::uint8_t *data, std::size_t size)
{
    if (player >= impl_->peers.size() || !impl_->peers[player]) return false;
    return send(impl_->peers[player]->channels().first, data, size);
}

bool NativePeerTransport::SendRepairTo(std::uint8_t player, const std::uint8_t *data, std::size_t size)
{
    if (player >= impl_->peers.size() || !impl_->peers[player]) return false;
    return send(impl_->peers[player]->channels().second, data, size);
}

bool NativePeerTransport::SendControl(const std::uint8_t *data, std::size_t size)
{
    if (!IsOpen()) return false;
    bool sent = true;
    for (const auto &peer : impl_->peers)
        if (peer) sent = send(peer->channels().second, data, size) && sent;
    return sent;
}

std::optional<rtc::message_variant> NativePeerTransport::PollMessage(bool &controlLane)
{
    const auto count = impl_->peers.size() * 2;
    for (std::size_t tried = 0; tried < count; ++tried) {
        const auto slot = impl_->nextChannel;
        impl_->nextChannel = (slot + 1) % count;
        const auto &peer = impl_->peers[slot / 2];
        if (!peer) continue;
        const auto [input, control] = peer->channels();
        const auto &channel = slot % 2 ? input : control;
        if (!channel) continue;
        if (auto message = channel->receive()) {
            // The slot identifies the channel before advancing to the next lane.
            controlLane = slot % 2 == 0;
            return message;
        }
    }
    return std::nullopt;
}

bool NativePeerTransport::Poll(std::vector<std::uint8_t> *packet)
{
    if (!packet) return false;
    bool control;
    for (unsigned attempt = 0; attempt < 64; ++attempt) {
        const auto message = PollMessage(control);
        if (!message) break;
        if (const auto *bytes = std::get_if<rtc::binary>(&*message); bytes && !bytes->empty()) {
            packet->resize(bytes->size());
            std::memcpy(packet->data(), bytes->data(), bytes->size());
            return true;
        }
    }
    return false;
}

std::size_t NativePeerTransport::BufferedAmount() const
{
    std::size_t bytes = 0;
    for (const auto &peer : impl_->peers) {
        if (!peer) continue;
        const auto [input, control] = peer->channels();
        if (input) bytes += input->bufferedAmount();
        if (control) bytes += control->bufferedAmount();
    }
    return bytes;
}
}
