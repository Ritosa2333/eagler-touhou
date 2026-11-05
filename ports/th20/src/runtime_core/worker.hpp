#pragma once
#include <atomic>
#include <thread>
#include <cstddef>
#ifdef TH_SDL3
#include <utility>
#endif

namespace th20::source::runtime {
#ifdef TH_SDL3
// The browser build has no worker threads (wasm32 without pthreads) while the
// workloads are synchronous MEMFS file reads, so worker bodies run inline at
// the assignment site. The interface mirrors the std::jthread subset the call
// sites use; on Windows worker_thread remains the real std::jthread. The
// storage member keeps the object layout byte-identical to a real jthread so
// downstream structs embedding Worker keep their original offsets.
class inline_thread {
public:
    inline_thread() noexcept = default;
    template<typename F, typename... A> explicit inline_thread(const char* tag, F&& f, A&&... a) {
        (void)tag; // the browser runtime has no thread to name; kept for call-site parity
        std::forward<F>(f)(std::forward<A>(a)...);
    }
    template<typename F, typename... A> explicit inline_thread(F&& f, A&&... a) {
        std::forward<F>(f)(std::forward<A>(a)...);
    }
    inline_thread(inline_thread&&) noexcept = default;
    inline_thread& operator=(inline_thread&&) noexcept = default;
    bool joinable() const noexcept { return false; }
    void join() noexcept {}
    void detach() noexcept {}
private:
    std::jthread storage; // never started; layout placeholder only
};
using worker_thread = inline_thread;
// Tagged construction keeps the Windows call sites byte-compatible while the
// browser build can name which inline worker hangs.
#define TH20_WORKER_THREAD(tag, ...) ::th20::source::runtime::worker_thread(tag, __VA_ARGS__)
#else
using worker_thread = std::jthread;
#define TH20_WORKER_THREAD(tag, ...) ::th20::source::runtime::worker_thread(__VA_ARGS__)
#endif
// Original constructor 0x0040b780: std::jthread default construction at+0,
// then atomic<bool>(false) at+12. MSVC's x86 jthread is thread8+stop_source4.
// On Windows this type owns a real C++ thread; it contains no original
// instruction bytes. TH_SDL3 substitutes the inline worker above.
struct Worker {
    worker_thread thread;
    std::atomic<bool> close_requested{false};
    // User-provided construction prevents value-initialization from clearing
    // tail padding. Original40b780 initializes12 thread bytes and bool+12 only.
    Worker() noexcept : thread(), close_requested(false) {}
    Worker(const Worker&)=delete;
    Worker& operator=(const Worker&)=delete;
    ~Worker(); // 40b980: close_and_join, then normal jthread destruction
};
// MSVC x86 jthread is thread8+stop_source4=12 bytes; libc++/wasm32 packs it
// differently. Binding on the Windows build only.
#ifndef TH_SDL3
static_assert(sizeof(std::jthread)==12 && offsetof(Worker,close_requested)==12 && sizeof(Worker)==16);
#endif
void join_worker(Worker&);                  // 40bcf0
void detach_worker(Worker&);                // 4ba8b0 -> 40bc60
void sync_close_worker(Worker&);            // 4d9e30 composition
}
