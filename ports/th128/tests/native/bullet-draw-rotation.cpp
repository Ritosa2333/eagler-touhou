#include "game/BulletRules.hpp"
#include "animation/AnmLogic.hpp"
#include <bit>
#include <cassert>
#include <iostream>
#include <fstream>
#include <iterator>

int main(int argc,char** argv){
    assert(argc==2);std::ifstream file(argv[1],std::ios::binary);assert(file);
    const std::vector<unsigned char> bytes{std::istreambuf_iterator<char>(file),{}};
    th128::anm::Registry registry;std::string error;assert(registry.loadBank(0,bytes.data(),bytes.size(),error));
    th128::anm::VM vm;th128::anm::Runtime runtime;runtime.remapSprite=[](int index){return index;};assert(vm.bind(registry,0,74,runtime));
    assert((vm.state().flags&0x40000000u)&&!(vm.state().flags&0x20000000u));
    float prior=0;std::uint32_t angleBits=0,rotationBits=0,flags=0;
    while(std::cin>>angleBits>>rotationBits>>flags){
        const auto result=th128::bullet::directionalDrawAngle(std::bit_cast<float>(angleBits),std::bit_cast<float>(rotationBits),flags);
        assert(result);prior=*result;std::cout<<std::bit_cast<std::uint32_t>(prior)<<'\n';
    }
}
