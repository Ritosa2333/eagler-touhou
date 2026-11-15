#include "game/PerfectHud.hpp"
#include <cassert>
#include <fstream>
#include <iterator>
#include <iostream>

int main(int argc,char** argv){
    assert(argc==2);std::ifstream file(argv[1],std::ios::binary);assert(file);
    const std::vector<unsigned char> bytes{std::istreambuf_iterator<char>(file),{}};
    th128::hud::PerfectHud hud;std::string error;assert(hud.load(bytes,error));
    assert(hud.boot(0,error));const auto born=hud.snapshots();assert(born.size()==14);
    for(const auto& node:born)if(node.state.script>=12&&node.state.script<=14)assert(!(node.state.flags&2u));
    for(const int value:{10000,20000,30000,0}){
        assert(hud.charge(value,error));unsigned visible=0;
        for(const auto& node:hud.snapshots())if(node.state.script>=12&&node.state.script<=14){
            assert(bool(node.state.flags&2u)==(value>=(node.state.script-11)*10000));visible+=bool(node.state.flags&2u);
        }
        assert(visible==unsigned(value/10000));
    }
    assert(!hud.charge(30001,error));assert(hud.snapshots().size()==14);
    assert(hud.notice(error));assert(hud.snapshots().size()==15);
    for(int tick=0;tick<60;++tick)assert(hud.tick(error));
    auto snapshot=hud.snapshots();const auto rootNotice=std::find_if(snapshot.begin(),snapshot.end(),[](const auto& node){return node.state.script==47;});
    assert(rootNotice!=snapshot.end()&&rootNotice->state.sprite==46&&rootNotice->state.ticks==61&&rootNotice->state.sx==1&&rootNotice->state.sy==1);
    assert(hud.notice(error));snapshot=hud.snapshots();
    assert(std::count_if(snapshot.begin(),snapshot.end(),[](const auto& node){return node.state.script==47;})==1);
    for(int tick=0;tick<120;++tick)assert(hud.tick(error));
    assert(hud.snapshots().size()==14);
    const auto retained=hud.snapshots();assert(!hud.load({bytes.data(),8},error));assert(hud.snapshots().size()==retained.size());
    for(const auto& node:hud.snapshots())if(node.state.script>=2&&node.state.script<=14){
        assert(node.state.flags&0x80000000u);assert(!(node.state.flags&0x40000000u));
    }
    std::cout<<"actual front0/47 lifecycle and three native charge children passed\n";
}
