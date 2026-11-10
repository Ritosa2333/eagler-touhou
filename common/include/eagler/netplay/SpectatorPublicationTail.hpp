#pragma once
#include <eagler/netplay/SpectatorStagePublisher.hpp>
#include <deque>

namespace Netplay {
// Own the final confirmed replay prefix before the title releases its world or
// input ring. Network backpressure can then drain independently of retirement.
class SpectatorPublicationTail {
public:
    static constexpr std::size_t MaxBytes = 16 * 1024 * 1024;
    enum class Result { Idle, Waiting, Complete, Continued, Failed };
    template<class Inputs>
    bool Freeze(SpectatorStagePublisher& publisher, std::uint32_t confirmed,
                std::uint32_t simulated, Inputs inputs, std::uint64_t now,
                const std::vector<std::uint8_t>& prefix = {}, bool finish = true) {
        Clear();
        const auto retain = [&](const std::vector<std::uint8_t>& wire) {
            if (wire.size() > MaxBytes - bytes_) return false;
            packets_.push_back(wire); bytes_ += wire.size(); return true;
        };
        if ((!prefix.empty() && !retain(prefix)) || !publisher.Publish(confirmed, simulated, inputs, retain) ||
            (finish && !publisher.Finish(publisher.Published(), retain))) {Clear(); return false;}
        finishes_ = finish;
        deadline_ = now + 15'000; return true;
    }
    template<class Send, class Clock>
    Result Drain(std::uint64_t now, bool failed, Send send, Clock clock) {
        if (!deadline_) return Result::Idle;
        if (failed || now >= deadline_) {Clear(); return Result::Failed;}
        const auto began = clock();
        for (unsigned sent = 0; !packets_.empty() && sent < 32; ++sent) {
            if (sent && clock() - began >= 2000) break;
            if (!send(packets_.front())) return Result::Waiting;
            bytes_ -= packets_.front().size(); packets_.pop_front();
        }
        if (!packets_.empty()) return Result::Waiting;
        const bool finish = finishes_; Clear(); return finish ? Result::Complete : Result::Continued;
    }
    void Clear() {packets_.clear(); bytes_ = 0; deadline_ = 0;}
    bool Pending() const {return deadline_ != 0;}
    std::size_t Size() const {return packets_.size();}
private:
    std::deque<std::vector<std::uint8_t>> packets_;
    std::size_t bytes_ = 0;
    std::uint64_t deadline_ = 0;
    bool finishes_ = true;
};
}
