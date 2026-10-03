#include "../th10_web/cpp/multiplayer/RollbackPool.hpp"
#include <cassert>
#include <cstdint>

int main(){
    th10::multiplayer::RollbackPool<64,3> pool;
    auto* a=static_cast<std::uint8_t*>(pool.allocate(16,true));
    auto* b=static_cast<std::uint8_t*>(pool.allocate(64,true));
    auto* c=static_cast<std::uint8_t*>(pool.allocate(1,false));
    assert(a&&b&&c&&a!=b&&b!=c&&pool.allocate(1)==nullptr);
    a[0]=0x5a;assert(pool.owns(a)&&pool.release(a));
    auto* reused=static_cast<std::uint8_t*>(pool.allocate(16,false));
    assert(reused==a&&reused[0]==0x5a);
    assert(!pool.release(reinterpret_cast<void*>(1)));
    assert(pool.release(reused)&&pool.release(b)&&pool.release(c));
}
