#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <vector>
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
static std::uint32_t rng=0x6b72e15f;
static std::uint32_t next(){rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng;}
static std::size_t checks=0;
static std::size_t verify(void* space,bool miss){
    const auto before=eagler_sim_test_used_scans();
    const auto expected=eagler_sim_test_used_uncached(space);
    assert(eagler_sim_used(space)==expected);
    assert(eagler_sim_test_used_scans()==before+unsigned(miss));
    for(unsigned i=0;i<128;++i)assert(eagler_sim_used(space)==expected);
    assert(eagler_sim_test_used_scans()==before+unsigned(miss));
    ++checks;return expected;
}
int main(){
    constexpr std::size_t Capacity=2*1024*1024;
    auto* base=std::aligned_alloc(4096,Capacity);
    auto* other=std::aligned_alloc(4096,Capacity);
    assert(base&&other);
    auto* a=eagler_sim_create(base,Capacity);assert(a);
    const auto initial=verify(a,true);
    // Small requests deliberately differ from both actual chunk size and
    // usable size; accounting by either estimate must fail the oracle here.
    for(std::size_t n: {0u,1u,15u,16u,17u,31u,32u,33u,255u,256u,257u,4095u,4096u}){
        auto* p=eagler_sim_malloc(a,n);assert(p);
        assert(verify(a,false)>initial);
        eagler_sim_free(a,p);assert(verify(a,false)==initial);
    }
    for(std::size_t align: {8u,16u,24u,32u,48u,64u,256u,1024u,4096u,65536u})
      for(std::size_t n: {0u,1u,17u,129u,4097u,70000u}){
        auto* p=eagler_sim_memalign(a,align,n);assert(p);
        assert(verify(a,false)>initial);
        eagler_sim_free(a,p);assert(verify(a,false)==initial);
      }
    auto* host=std::malloc(8192);assert(host);
    std::memset(host,0x57,8192);std::free(host);
    assert(verify(a,false)==initial);
    std::array<void*,512> slots{};
    for(unsigned step=0;step<20000;++step){
        const auto index=next()%slots.size();
        if(slots[index]){eagler_sim_free(a,slots[index]);slots[index]=nullptr;verify(a,false);}
        if(next()%5){
            const auto bytes=next()%8193;
            slots[index]=(step%3)?eagler_sim_malloc(a,bytes):eagler_sim_memalign(a,std::size_t(16)<<(next()%9),bytes);
            verify(a,!slots[index]);
        }
        if(step%251==0){
            assert(!eagler_sim_malloc(a,std::numeric_limits<std::size_t>::max()));verify(a,true);
            assert(!eagler_sim_memalign(a,4096,Capacity*2));verify(a,true);
        }
    }
    for(auto* p:slots)if(p){eagler_sim_free(a,p);verify(a,false);}
    assert(verify(a,false)==initial);
    eagler_sim_free(a,nullptr);assert(verify(a,false)==initial);
    auto* keep=eagler_sim_malloc(a,513);assert(keep);
    const auto savedUsed=verify(a,false);
    std::vector<unsigned char> saved(Capacity);
    std::memcpy(saved.data(),base,Capacity);
    auto* transient=eagler_sim_memalign(a,4096,16385);assert(transient);verify(a,false);
    eagler_sim_free(a,keep);verify(a,false);
    std::memcpy(base,saved.data(),Capacity);eagler_sim_invalidate_used();
    assert(verify(a,true)==savedUsed);
    eagler_sim_free(a,keep);assert(verify(a,false)==initial);
    auto* b=eagler_sim_create(other,Capacity);assert(b);
    assert(verify(a,true)==initial);
    const auto initialB=verify(b,true);
    auto* p=eagler_sim_malloc(a,12345);assert(p);verify(a,true);
    verify(b,true);eagler_sim_free(a,p);assert(verify(a,true)==initial);
    auto* q=eagler_sim_memalign(b,4096,1031);assert(q);verify(b,true);
    eagler_sim_free(b,q);assert(verify(b,false)==initialB);
    q=eagler_sim_malloc(b,23145);assert(q);verify(b,false);
    assert(eagler_sim_create(other,Capacity)==b);assert(verify(b,true)==initialB);
    assert(!eagler_sim_create(other,16));assert(verify(b,true)==initialB);
    // A differently aligned external segment has different constant overhead;
    // one initial mallinfo scan establishes it, then exact deltas still apply.
    auto* unaligned=eagler_sim_create(static_cast<unsigned char*>(other)+7,Capacity-7);assert(unaligned);
    const auto emptyUnaligned=verify(unaligned,true);
    q=eagler_sim_memalign(unaligned,4096,32001);assert(q);verify(unaligned,false);
    eagler_sim_free(unaligned,q);assert(verify(unaligned,false)==emptyUnaligned);
    eagler_sim_invalidate_used();std::free(base);std::free(other);
    std::printf("simulation heap cache: PASS %zu uncached comparisons, 128 repeated hits each; 20000 deterministic operations\n",checks);
}
