#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

extern "C" {
void* eagler_sim_create(void*,std::size_t);
void* eagler_sim_malloc(void*,std::size_t);
void* eagler_sim_memalign(void*,std::size_t,std::size_t);
void eagler_sim_free(void*,void*);
std::size_t eagler_sim_used(void*);
void eagler_sim_invalidate_used();
std::size_t eagler_sim_test_used_uncached(void*);
std::size_t eagler_sim_test_used_scans();
}

static unsigned checks=0;
static std::size_t check(void* space,bool miss) {
    const auto scans=eagler_sim_test_used_scans();
    const auto expected=eagler_sim_test_used_uncached(space);
    assert(eagler_sim_used(space)==expected);
    assert(eagler_sim_test_used_scans()==scans+unsigned(miss));
    for(unsigned i=0;i<128;++i) assert(eagler_sim_used(space)==expected);
    assert(eagler_sim_test_used_scans()==scans+unsigned(miss));
    ++checks;
    return expected;
}

int main() {
    constexpr std::size_t Capacity=2*1024*1024;
    auto* baseA=std::aligned_alloc(4096,Capacity);
    auto* baseB=std::aligned_alloc(4096,Capacity);
    assert(baseA&&baseB);
    auto* a=eagler_sim_create(baseA,Capacity);
    assert(a);
    const auto emptyA=check(a,true);
    auto* small=eagler_sim_malloc(a,1025);
    assert(small&&check(a,true)>emptyA);
    std::memset(small,0x57,1025);
    check(a,false); // User bytes do not change allocator accounting.
    eagler_sim_free(a,small);
    assert(check(a,true)==emptyA);

    auto* aligned=eagler_sim_memalign(a,4096,17001);
    assert(aligned&&reinterpret_cast<std::uintptr_t>(aligned)%4096==0);
    assert(check(a,true)>emptyA);
    eagler_sim_free(a,aligned);
    assert(check(a,true)==emptyA);

    // Failed/no-op mutators still invalidate; no estimate of requested sizes
    // or alignment padding is allowed to replace the allocator's own result.
    assert(!eagler_sim_malloc(a,std::numeric_limits<std::size_t>::max()));
    assert(check(a,true)==emptyA);
    assert(!eagler_sim_memalign(a,4096,Capacity*2));
    assert(check(a,true)==emptyA);
    eagler_sim_free(a,nullptr);
    assert(check(a,true)==emptyA);
    auto* zero=eagler_sim_malloc(a,0);
    assert(zero&&check(a,true)>emptyA);
    eagler_sim_free(a,zero);
    assert(check(a,true)==emptyA);

    std::array<void*,2048> blocks{};
    for(std::size_t i=0;i<blocks.size();++i) {
        blocks[i]=eagler_sim_malloc(a,17+(i*67)%511);
        assert(blocks[i]);
        if(i%37==0) check(a,true);
    }
    check(a,true);
    for(std::size_t i=0;i<blocks.size();i+=2) eagler_sim_free(a,blocks[i]);
    check(a,true);
    for(std::size_t i=1;i<blocks.size();i+=2) eagler_sim_free(a,blocks[i]);
    assert(check(a,true)==emptyA);

    auto* b=eagler_sim_create(baseB,Capacity);
    assert(b&&b!=a);
    assert(check(a,true)==emptyA); // Creation conservatively invalidates all.
    const auto emptyB=check(b,true);
    check(b,false);
    check(a,true); // A different mspace replaces the single cache slot.
    auto* other=eagler_sim_malloc(b,23456);
    assert(other&&check(a,true)==emptyA);
    assert(check(b,true)>emptyB);
    eagler_sim_free(b,other);
    assert(check(b,true)==emptyB);
    assert(!eagler_sim_create(baseB,16));
    assert(check(b,true)==emptyB);

    // Reusing a fixed region can reuse the exact mspace pointer. Abandon this
    // allocation and reconstruct the allocator in its caller-owned memory.
    other=eagler_sim_malloc(b,34567);
    assert(other&&check(b,true)>emptyB);
    assert(eagler_sim_create(baseB,Capacity)==b);
    assert(check(b,true)==emptyB);

    auto* host=std::malloc(8192);
    assert(host);
    std::memset(host,0x7c,8192);
    std::free(host);
    assert(check(b,false)==emptyB);
    eagler_sim_invalidate_used();
    assert(check(b,true)==emptyB);
    eagler_sim_invalidate_used();
    std::free(baseA);std::free(baseB);
    std::printf("simulation heap cache: PASS %u uncached comparisons, 128 repeated hits each\n",checks);
}
