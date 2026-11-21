#include "game/AppearanceBounds.hpp"
#include "animation/AnmLogic.hpp"
#include <cassert>
#include <bit>
#include <fstream>
#include <iterator>
#include <iostream>
#include <limits>
#include <vector>

int main(int argc,char** argv) {
    if(argc==1){
        std::uint32_t width=0,height=0,scaleX=0,scaleY=0;
        while(std::cin>>width>>height>>scaleX>>scaleY){
            const auto result=th128::enemy::scaledSpriteBounds(std::bit_cast<float>(width),std::bit_cast<float>(height),
                std::bit_cast<float>(scaleX),std::bit_cast<float>(scaleY));
            if(!result)std::cout<<"none\n";
            else std::cout<<std::bit_cast<std::uint32_t>(result->width)<<' '<<std::bit_cast<std::uint32_t>(result->height)<<'\n';
        }
        return 0;
    }
    assert(argc==3);
    std::ifstream stream(argv[1],std::ios::binary);
    const std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(stream)),{});
    th128::anm::Registry registry;
    std::string error;
    assert(registry.loadBank(1,bytes.data(),bytes.size(),error));
    const auto bank=registry.bank(1);
    const auto bounds=th128::enemy::constantSpriteBounds(*bank,0);
    assert(bounds&&bounds->width==32&&bounds->height==32);
    const auto rectangular=th128::enemy::constantSpriteBounds(*bank,20);
    assert(rectangular&&rectangular->width==32&&rectangular->height==48);
    std::ifstream bossStream(argv[2],std::ios::binary);const std::vector<unsigned char> bossBytes{std::istreambuf_iterator<char>(bossStream),{}};
    assert(registry.loadBank(3,bossBytes.data(),bossBytes.size(),error));const auto boss=registry.bank(3);
    for(int script=0;script<5;++script){const auto proven=th128::enemy::constantSpriteBounds(*boss,script);assert(proven&&proven->width==96&&proven->height==96);}
    auto held=*bank;held.scripts[0].commands.resize(3);held.scripts[0].commands[2].op=63;
    held.scripts[0].commands[2].words.clear();held.scripts[0].commands[2].time=0;held.scripts[0].commands[2].mask=0;
    const auto heldBounds=th128::enemy::constantSpriteBounds(held,0);assert(heldBounds&&heldBounds->width==32&&heldBounds->height==32);
    auto synthetic=*bank;
    synthetic.scripts[0].commands.front().mask=1;
    assert(!th128::enemy::constantSpriteBounds(synthetic,0));
    synthetic=*bank;
    synthetic.scripts[0].commands.front().op=50;
    assert(!th128::enemy::constantSpriteBounds(synthetic,0));
    synthetic=*bank;
    synthetic.sprites[1].width=16;
    assert(!th128::enemy::constantSpriteBounds(synthetic,0));
    synthetic=*bank;
    synthetic.sprites[0].width=std::numeric_limits<float>::quiet_NaN();
    assert(!th128::enemy::constantSpriteBounds(synthetic,0));
    synthetic=*bank;synthetic.sprites[0].height=std::numeric_limits<float>::quiet_NaN();
    assert(!th128::enemy::constantSpriteBounds(synthetic,0));
    synthetic=*bank;synthetic.sprites[0].height=8192;
    assert(!th128::enemy::constantSpriteBounds(synthetic,0));
    synthetic=*bank;
    synthetic.scripts[0].commands[synthetic.scripts[0].commands.size()-2].words[1]=64;
    assert(!th128::enemy::constantSpriteBounds(synthetic,0));
    assert(!th128::enemy::constantSpriteBounds(*bank,-1));
    synthetic=*bank;
    synthetic.scripts[0].commands[1].op=4;
    synthetic.scripts[0].commands[1].time=5;
    synthetic.scripts[0].commands[1].words={0,0};
    synthetic.scripts[0].commands[2].time=0;
    synthetic.scripts[0].commands.resize(3);
    assert(!th128::enemy::constantSpriteBounds(synthetic,0));
    return 0;
}
