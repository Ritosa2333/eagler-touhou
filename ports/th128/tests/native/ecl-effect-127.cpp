#include "../../src/runtime/WorldExports.cpp"
#include "../ecl-fixture.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <cstring>
#include <vector>

namespace {
using ecl_fixture::Bytes;
using ecl_fixture::floatBits;
using ecl_fixture::instruction;
using ecl_fixture::join;
using ecl_fixture::program;
using ecl_fixture::stringWords;

std::vector<unsigned char> read(const char* path){
    std::ifstream file(path,std::ios::binary);
    if(!file){std::cerr<<"missing "<<path<<'\n';std::exit(2);}
    return {std::istreambuf_iterator<char>(file),{}};
}
void require(bool ok,const std::string& message){
    if(!ok){std::cerr<<"FAIL "<<message<<'\n';std::exit(1);}
}
const th128::enemy::EnemyVisuals::Snapshot* findScript(
    const std::vector<th128::enemy::EnemyVisuals::Snapshot>& snapshots,int script){
    const auto found=std::find_if(snapshots.begin(),snapshots.end(),
        [script](const auto& node){return node.state.script==script;});
    return found==snapshots.end()?nullptr:&*found;
}
}
int main(int argc,char** argv){
    if(argc!=5){std::cerr<<"usage: ecl-effect-127 <bullet.anm> <enemy.anm> <ascii.anm> <profile8.bin>\n";return 2;}
    const auto bullet=read(argv[1]),enemy=read(argv[2]),ascii=read(argv[3]),profile=read(argv[4]);
    th128_world_reset();
    auto spawn=stringWords("boss");
    spawn.insert(spawn.end(),{floatBits(0),floatBits(128),100,0,0});
    const auto stage=program({
        {"main",join({instruction(257,spawn,0,0,255,6),instruction(0,{},1000)})},
        {"boss",join({instruction(263,{0,127}),instruction(0,{},1000)})}
    });
    require(th128_world_load("ecl-effect-127.ecl",std::strlen("ecl-effect-127.ecl"),stage.data(),unsigned(stage.size()))!=0,th128_world_error());
    require(th128_world_start("ecl-effect-127.ecl",std::strlen("ecl-effect-127.ecl"),1,0x1234)!=0,th128_world_error());
    require(th128_world_load_enemy_anm(1,enemy.data(),unsigned(enemy.size()))!=0,th128_world_error());
    require(th128_world_load_score_font(ascii.data(),unsigned(ascii.size()))!=0,th128_world_error());
    require(th128_world_enable_bullets(profile.data(),unsigned(profile.size()),bullet.data(),unsigned(bullet.size()),0,400)!=0,
        th128_world_error());
    const auto frame0=enemyVisuals->snapshots();
    const auto* root0=findScript(frame0,127);
    const auto* child1260=findScript(frame0,126);
    require(root0&&child1260,"enable_bullets local pass must execute ECL263 and create owner127/child126 at frame0");
    require(root0->parent==0&&root0->entity==1000000&&root0->slot==0&&root0->state.layer==9,
        "ECL263 owner127 must keep standalone owner identity/layer9");
    require(child1260->parent==root0->node&&child1260->state.layer==16
        &&child1260->state.spriteBank==11&&child1260->state.sprite==313,
        "owner127 child126 must use original ASCII313");
    require(std::fabs(child1260->world.x-320)<.001f&&std::fabs(child1260->world.y-144)<.001f
        &&std::fabs(child1260->world.z)<.001f,"owner127 pose must be ECL263 position plus 320/16");
    require(th128_world_tick()!=0,th128_world_error());
    const auto frame1=enemyVisuals->snapshots();
    const auto* root=findScript(frame1,127);
    const auto* child126=findScript(frame1,126);
    const auto* child121=findScript(frame1,121);
    require(root&&child126&&child121,"owner127 authored t1 must retain child126 and birth child121");
    require(child121->parent==root->node&&child121->state.layer==16
        &&child121->state.spriteBank==0&&child121->state.sprite==452,
        "owner127 child121 must use original bank0 sprite452");
    int retirementFrame=-1;
    for(int tick=0;tick<256;++tick) {
        require(th128_world_tick()!=0,th128_world_error());
        const auto alive=enemyVisuals->snapshots();
        if(world->frame()==10)
            require(findScript(alive,127)!=nullptr,"owner127 must survive real world cleanup through frame10");
        if(findScript(alive,127)==nullptr) {
            retirementFrame=int(world->frame());
            break;
        }
    }
    require(retirementFrame>10,"owner127 must retire after its authored loop, not at a hardcoded early frame");
    require(!standaloneEffectOwners.contains(1000000),"retired standalone owner registration must be pruned");
    std::string error;
    require(!standaloneEclEffect(1,127,{0,128,0},9,true,error),"unknown bank must reject script127");
    require(!standaloneEclEffect(0,125,{0,128,0},9,true,error),"unknown script must reject");
    require(!standaloneEclEffect(0,127,{0,128,0},8,true,error),"wrong layer must reject script127");
    require(!standaloneEclEffect(0,127,{0,128,0},9,false,error),"head=false must reject standalone127");
    require(standaloneEffectOwner==1000001,"invalid requests must not consume an owner identity");
    std::cout<<"PASS actual ECL263 127 routing, ASCII313, pose, bank/script/layer/head guards, retirementFrame="
        <<retirementFrame<<"\n";
}
