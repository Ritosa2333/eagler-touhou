#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace th08::multiplayer {
// Snapshot diagnostics before transport shutdown clears its last error. This
// does not participate in admission, timeouts, retransmission or simulation.
struct NetworkFailureContext {
    const char* reason="";
    const char* mode="";
    std::uint32_t generation=0,next=0,confirmed=0xffffffffu;
    std::uint32_t rollback=0xffffffffu,captured=0xffffffffu;
    std::uint32_t channel=0;
    std::uint64_t sent=0,received=0;
    std::size_t queued=0,inputQueued=0,controlQueued=0;
};
template<std::size_t N>
void FormatNetworkFailure(char (&out)[N],const char* operation,const NetworkFailureContext& c){
    static_assert(N>=768,"Keep room for the cause and every failure-time counter");
    const auto frame=[](std::uint32_t value)->long long{return value==0xffffffffu?-1:static_cast<long long>(value);};
    std::snprintf(out,N,
        "%.80s: %.240s [mode=%.16s generation=%u next=%u confirmed=%lld rollback=%lld captured=%lld channel=%u sent=%llu received=%llu queuedBytes=%zu inputBytes=%zu controlBytes=%zu]",
        operation&&*operation?operation:"network failure",c.reason&&*c.reason?c.reason:"no channel detail available",
        c.mode&&*c.mode?c.mode:"pending",c.generation,c.next,frame(c.confirmed),frame(c.rollback),frame(c.captured),
        c.channel,static_cast<unsigned long long>(c.sent),static_cast<unsigned long long>(c.received),c.queued,c.inputQueued,c.controlQueued);
}
}
