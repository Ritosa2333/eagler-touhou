#include "game/MessageVisuals.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
using namespace ecl_fixture;
int main(int count,char** arguments){try{
    require(count==4,"text/player/boss ANM required");const auto read=[](const char* name){std::ifstream file(name,std::ios::binary);require(bool(file),"actual ANM missing");return Bytes{std::istreambuf_iterator<char>(file),{}};};
    const auto text=read(arguments[1]),player=read(arguments[2]),boss=read(arguments[3]);std::string error;th128::message::Visuals visuals;
    require(visuals.load(6,text,error)&&visuals.load(7,player,error)&&visuals.load(3,boss,error),error);
    th128::message::Event event;event.opcode=-1;require(visuals.event(event,error),error);require(visuals.snapshots().size()==4,"actual constructor binds four dynamic text lines");
    event.opcode=1;event.integers[0]=0;require(visuals.event(event,error),error);
    event.opcode=13;event.integers[0]=5;require(visuals.event(event,error),error);
    auto snapshots=visuals.snapshots();unsigned expressions=0;for(const auto& node:snapshots)if(node.state.bank==7&&(node.state.script==18||node.state.script==19)){
        require(node.state.sprite==(node.state.script==18?50:59),"original player expression tables must select sprite45/54 + expression");++expressions;
    }
    require(expressions==2,"both original portrait halves must exist");
    event.opcode=2;event.integers[0]=0;require(visuals.event(event,error),error);
    event.opcode=14;event.integers[0]=7;require(visuals.event(event,error),error);
    snapshots=visuals.snapshots();expressions=0;for(const auto& node:snapshots)if(node.state.bank==3&&node.state.script>=6&&node.state.script<=8){
        require(node.state.sprite==(node.state.script==6?18:node.state.script==7?27:36),"actual stage1 Boss expression uses11/20/29 + expression");++expressions;
    }
    require(expressions==3,"all three actual Boss portrait strips must exist");
    event.opcode=7;require(visuals.event(event,error),error);for(int tick=0;tick<16;++tick)require(visuals.tick(error),error);
    snapshots=visuals.snapshots();require(std::any_of(snapshots.begin(),snapshots.end(),[](const auto& draw){return draw.state.bank==7&&draw.state.sprite==50&&draw.state.primary>>24;}),"player face must animate into view with real IRQ2");
    event.opcode=4;require(visuals.event(event,error),error);event.opcode=5;require(visuals.event(event,error),error);
    for(int tick=0;tick<35;++tick)require(visuals.tick(error),error);
    snapshots=visuals.snapshots();require(std::none_of(snapshots.begin(),snapshots.end(),[](const auto& draw){return draw.state.bank==7||draw.state.bank==3;}),"IRQ1 must retire real portraits");
    event.opcode=20;require(visuals.event(event,error),error);for(int tick=0;tick<20;++tick)require(visuals.tick(error),error);
    snapshots=visuals.snapshots();require(std::any_of(snapshots.begin(),snapshots.end(),[](const auto& draw){return draw.state.bank==3&&draw.state.script==10&&draw.state.sprite==39&&draw.state.sx==1&&draw.x==544&&draw.y==422;}),"MSG20 must create the original first-stage Boss introduction");
    for(int tick=0;tick<230;++tick)require(visuals.tick(error),error);
    snapshots=visuals.snapshots();require(std::none_of(snapshots.begin(),snapshots.end(),[](const auto& draw){return draw.state.bank==3;}),"Boss introduction must retire through original ANM script");
    std::cout<<"original message text roots, player halves, Boss strips, expressions and IRQ ownership passed\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
