#include "worker.hpp"
#include "runtime_core.hpp"

namespace th20::source::runtime {
void join_worker(Worker& w) {
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(6));
    w.close_requested.store(true,std::memory_order_seq_cst); // 40baa0/40c600 XCHG
    if(w.thread.joinable()) w.thread.join(); // 40c340 -> 40c360
}
void detach_worker(Worker& w) {
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(6));
    w.close_requested.store(true,std::memory_order_seq_cst);
    // Original 4ba8b0 calls 40bc60, reacquiring the same recursive mutex.
    std::lock_guard<std::recursive_mutex> nested(shared_locks().slot(6));
    if(w.thread.joinable()) w.thread.detach();
}
void sync_close_worker(Worker& w) {
    // Original 4d9e30 really detaches before its join-if-joinable operation.
    detach_worker(w);join_worker(w);
}
Worker::~Worker() {join_worker(*this);}
}
