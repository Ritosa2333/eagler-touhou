#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {
using ecl_fixture::Bytes;
using ecl_fixture::floatBits;
using ecl_fixture::instruction;
using ecl_fixture::join;
using ecl_fixture::program;
using ecl_fixture::stringWords;

std::vector<unsigned char> read(const char* path) {
    std::ifstream file(path,std::ios::binary);
    if(!file){std::cerr<<"missing "<<path<<'\n';std::exit(2);}
    return {std::istreambuf_iterator<char>(file),{}};
}
void require(bool ok,const std::string& message) {
    if(!ok){std::cerr<<"FAIL "<<message<<'\n';std::exit(1);}
}
const th128::enemy::EnemyVisuals::Snapshot* findScript(
    const std::vector<th128::enemy::EnemyVisuals::Snapshot>& snapshots,int script) {
    const auto found=std::find_if(snapshots.begin(),snapshots.end(),
        [script](const auto& node){return node.state.script==script;});
    return found==snapshots.end()?nullptr:&*found;
}
}
int main(int argc,char** argv) {
    if(argc!=5){std::cerr<<"usage: ecl-effect-138 <bullet.anm> <enemy.anm> <ascii.anm> <profile8.bin>\n";return 2;}
    const auto bullet=read(argv[1]),enemy=read(argv[2]),ascii=read(argv[3]),profile=read(argv[4]);
    th128_world_reset();
    auto spawn=stringWords("boss");
    spawn.insert(spawn.end(),{floatBits(0),floatBits(144),100,0,0});
    const auto stage=program({
        {"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},1000)})},
        {"boss",join({instruction(263,{0,138}),instruction(0,{},1000)})}
    });
    require(th128_world_load("ecl-effect-138.ecl",std::strlen("ecl-effect-138.ecl"),stage.data(),unsigned(stage.size()))!=0,th128_world_error());
    require(th128_world_start("ecl-effect-138.ecl",std::strlen("ecl-effect-138.ecl"),1,0x1234)!=0,th128_world_error());
    require(th128_world_load_enemy_anm(1,enemy.data(),unsigned(enemy.size()))!=0,th128_world_error());
    require(th128_world_load_score_font(ascii.data(),unsigned(ascii.size()))!=0,th128_world_error());
    require(th128_world_enable_bullets(profile.data(),unsigned(profile.size()),bullet.data(),unsigned(bullet.size()),0,400)!=0,
        th128_world_error());
    const auto frame0=enemyVisuals->snapshots();
    const auto* root0=findScript(frame0,138);
    const auto* child1370=findScript(frame0,137);
    require(root0&&child1370,"ECL263 must create owner138/child137 at frame0");
    require(root0->parent==0&&root0->entity==1000000&&root0->slot==0&&root0->state.layer==9,
        "owner138 standalone identity/layer9");
    require(std::fabs(root0->world.x-320)<.001f&&std::fabs(root0->world.y-160)<.001f
        &&std::fabs(root0->world.z)<.001f,"owner138 native pose 320/160");
    require(child1370->parent==root0->node&&child1370->state.layer==16&&child1370->state.mode==16
        &&child1370->state.spriteBank==11&&child1370->state.sprite==313,
        "owner138 child137 ASCII313/layer16/mode16 bank="+std::to_string(child1370->state.spriteBank)
        +" sprite="+std::to_string(child1370->state.sprite));
    require(th128_world_tick()!=0,th128_world_error());
    const auto frame1=enemyVisuals->snapshots();
    const auto* root=findScript(frame1,138);
    const auto* child137=findScript(frame1,137);
    const auto* child136=findScript(frame1,136);
    require(root&&child137&&child136,"owner138 t1 must retain137 and birth136");
    require(child136->parent==root->node&&child136->state.layer==16
        &&child136->state.spriteBank==0&&child136->state.sprite==452,
        "owner138 child136 bank0 sprite452/layer16");
    int retirementFrame=-1;
    for(int tick=0;tick<256;++tick) {
        require(th128_world_tick()!=0,th128_world_error());
        const auto alive=enemyVisuals->snapshots();
        if(world->frame()==10)
            require(findScript(alive,138)!=nullptr,"owner138 must survive real cleanup through frame10");
        if(findScript(alive,138)==nullptr) {
            retirementFrame=int(world->frame());
            break;
        }
    }
    require(retirementFrame>10&&retirementFrame<256,"owner138 must retire after authored t62 loop, not a guessed early frame");
    require(!standaloneEffectOwners.contains(1000000),"retired owner138 registration must be pruned");
    std::string error;
    require(!standaloneEclEffect(1,138,{0,144,0},9,true,error),"unknown bank must reject script138");
    require(!standaloneEclEffect(0,139,{0,144,0},9,true,error),"unknown family139 must reject");
    require(!standaloneEclEffect(0,138,{0,144,0},8,true,error),"wrong layer must reject script138");
    require(!standaloneEclEffect(0,138,{0,144,0},9,false,error),"head=false must reject standalone138");
    require(standaloneEffectOwner==1000001,"invalid requests must not consume an owner identity");
    std::cout<<"PASS actual ECL263 138 routing, child137/136, native pose, guards, retirementFrame="
        <<retirementFrame<<"\n";
}
