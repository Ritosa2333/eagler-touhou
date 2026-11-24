#include "game/Geometry526Host.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>

int main(int count,char** arguments){
    assert(count==2);std::ifstream input(arguments[1],std::ios::binary);
    std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(input),{}};assert(!bytes.empty());
    th128::geometry526::Host host;std::string error;
    assert(!host.create(75,80,0x00800080u,error));assert(host.load(bytes,error));
    const auto main=host.mainSnapshot(error);assert(main);
    assert(main->state.bank==6&&main->state.script==82&&main->state.sprite==83&&main->state.mode==0);
    assert(main->state.x==128&&main->state.y==16&&main->state.layer==29&&main->state.blend==3&&main->state.held);
    assert(main->sprite.textureEntry==2&&main->sprite.width==384&&main->sprite.height==448);
    assert(host.create(75,80,0x00800080u,error));assert(host.effectCount()==1&&host.nodeCount()==16);
    assert(!host.snapshots(error));assert(!error.empty());
    assert(host.update(75,{0,100,0},1,error));
    const auto born=host.snapshots(error);assert(born&&born->size()==16);
    for(unsigned row=0;row<born->size();++row){
        const auto& mesh=born->at(row);assert(mesh.owner==75&&mesh.column==row&&mesh.vertices.size()==34);
        assert(mesh.state.bank==6&&mesh.state.spriteBank==6&&mesh.state.script==82&&mesh.state.sprite==83);
        assert(mesh.state.layer==29&&mesh.state.blend==0&&mesh.state.mode==12&&mesh.state.held);
        assert(((mesh.state.flags>>24)&31u)==12&&((mesh.state.flags>>5)&7u)==0);
        assert(((mesh.state.flags>>20)&3u)==1&&((mesh.state.flags>>22)&3u)==1);
        assert(mesh.state.ticks==1&&mesh.sprite.textureEntry==2&&mesh.sprite.width==384&&mesh.sprite.height==448);
        if(row)assert(mesh.id>born->at(row-1).id);
    }
    const auto original=born->at(8).vertices;
    assert(host.snapshots(error)->at(8).vertices[0].x==original[0].x);
    assert(host.snapshots(error)->at(8).state.ticks==1);
    assert(host.tickPrimary(error));assert(host.snapshots(error)->at(8).state.ticks==2);
    assert(host.update(75,{0,100,0},1,error));assert(host.effectState(75)->scale==20);
    assert(host.create(76,120,0xffffffffu,error));assert(host.update(76,{100,120,0},1,error));
    assert(host.effectCount()==2&&host.nodeCount()==32);
    assert(!host.create(77,std::numeric_limits<float>::quiet_NaN(),0,error));
    assert(host.effectCount()==2&&host.nodeCount()==32);
    assert(!host.update(75,{std::numeric_limits<float>::quiet_NaN(),0,0},1,error));
    assert(host.effectState(75)->scale==20);
    assert(!host.load({bytes.data(),8},error));
    assert(host.create(75,80,0x11223344u,error));assert(host.update(75,{0,100,0},1,error));
    const auto replaced=host.snapshots(error);assert(replaced&&replaced->size()==32);
    for(const auto& mesh:*replaced)if(mesh.owner==75)assert(mesh.id>born->back().id&&mesh.state.ticks==1);
    assert(host.create(75,0,0,error));assert(host.effectCount()==1&&host.nodeCount()==16);
    assert(!host.effectState(75));assert(!host.update(75,{0,0,0},1,error));
    assert(host.removeOwner(76,error));assert(host.effectCount()==0&&host.nodeCount()==0);
    assert(host.snapshots(error)->empty());assert(host.tickPrimary(error));
    assert(host.create(78,-10,0,error));assert(host.effectCount()==0);
    std::cout<<"actual text82 owns16 ordered strips; updates, snapshots, replacement and retirement passed\n";
}
