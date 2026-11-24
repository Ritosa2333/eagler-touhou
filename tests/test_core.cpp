#include "core_contract.hpp"
#include <iostream>
#include <limits>
unsigned checks=0;
void check(bool ok){++checks;if(!ok)throw std::runtime_error("Core contract test failed");}
// Instruction-level arithmetic oracle, split into 16-bit MUL products and carry.
std::uint16_t asm_irand(std::uint32_t &seed) {
    const auto lo=std::uint16_t(seed),hi=std::uint16_t(seed>>16);
    const auto cx=std::uint16_t(std::uint32_t(0x4e35)*hi+std::uint32_t(0x015a)*lo);
    const auto product=std::uint32_t(0x4e35)*lo;
    const auto ax=std::uint16_t(product);
    const auto low=std::uint16_t(ax+1);
    const auto high=std::uint16_t((product>>16)+cx+(ax==0xffff));
    seed=std::uint32_t(low)|(std::uint32_t(high)<<16);
    return high&0x7fff;
}
int main(){try {
    soew::IRand vector(1);
    for(auto expected:{346,130,10982,1090,11656,7117,17595,6415})check(vector.next()==expected);
    for(auto seed:{0u,1u,0xffffu,0x10000u,0x12345678u,0xffffffffu}) {
        soew::IRand rng(seed);auto oracle=seed;
        for(int i=0;i<10000;++i){check(rng.next()==asm_irand(oracle));check(rng.state==oracle);}
    }
    soew::RandomRing ring;soew::IRand rng(1),source(1);ring.cursor=17;ring.fill(rng);
    check(ring.cursor==17);for(auto b:ring.bytes)check(b==std::uint8_t(source.next()));check(rng.state==source.state);
    for(unsigned i=0;i<256;++i)ring.bytes[i]=std::uint8_t(i);
    ring.cursor=254;check(ring.next16()==0xfffe && ring.cursor==255);
    check(ring.next16()==0x00ff && ring.cursor==0);
    check(ring.next16()==0x0100 && ring.cursor==1);check(ring.next8()==1 && ring.cursor==2);
    ring.cursor=255;check(ring.next8()==255&&ring.cursor==0);
    for(unsigned mask=0;mask<256;++mask){ring.cursor=255;check(ring.masked(mask)==mask&&ring.cursor==0);}
    // Byte-addressed oracle preserves the INC-before-word-load alias at255.
    std::array<std::uint8_t,257> memory{};
    for(unsigned start=0;start<256;++start) {
        ring.cursor=std::uint8_t(start);memory[256]=std::uint8_t(start);
        for(unsigned i=0;i<256;++i)memory[i]=ring.bytes[i];
        const auto bx=memory[256];memory[256]=std::uint8_t(memory[256]+1);
        const auto word=std::uint16_t(memory[bx]|(std::uint16_t(memory[bx+1])<<8));
        check(ring.next16()==word);check(ring.cursor==memory[256]);
    }
    soew::Keys k;check(soew::sense(k)==0);k.n7=true;check(soew::sense(k)==soew::UpLeft);
    k.up=true;k.left=true;check(soew::sense(k)==(soew::UpLeft|soew::Up|soew::Left));
    k=soew::Keys{};k.z=true;k.space=true;check(soew::sense(k)==soew::Shot);
    k=soew::Keys{};k.x=true;k.enter=true;k.escape=true;k.q=true;check(soew::sense(k)==0x01e0);
    k=soew::Keys{};k.up=k.down=k.left=k.right=true;check(soew::sense(k)==15);
    k=soew::Keys{};k.n8=k.n2=k.n4=k.n6=true;check(soew::sense(k)==15);
    std::cout<<"core_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<" after "<<checks<<" checks\n";return 1;}}
