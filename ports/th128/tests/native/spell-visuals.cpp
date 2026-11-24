#include "game/SpellVisuals.hpp"
#include "../ecl-fixture.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
using namespace ecl_fixture;
int main(int count,char** arguments){try{
 require(count==7,"actual bullet/boss/text/ascii/card/player ANMs required");th128::spell::Visuals visuals;std::string error;const int banks[]={0,3,6,11,12,7};
 for(int index=0;index<6;++index){std::ifstream file(arguments[index+1],std::ios::binary);const Bytes bytes{std::istreambuf_iterator<char>(file),{}};require(visuals.load(banks[index],bytes,error),error);}
 require(visuals.start({0,128,0},error),error);
 for(int frame=0;frame<180;++frame)require(visuals.tick({0,128,0},true,error),error);
 const auto nodes=visuals.snapshots();require(std::any_of(nodes.begin(),nodes.end(),[](const auto& node){return node.state.bank==12&&node.state.script==3;}),"original moving card backdrop");
 require(std::count_if(nodes.begin(),nodes.end(),[](const auto& node){return node.state.bank==0&&(node.state.script==156||node.state.script==157)&&node.state.mode==9;})==2,"original pair of spell rings");
 require(visuals.titlePose().has_value(),"authored spell title position must be retained");require(visuals.finish(error),error);
 for(int frame=0;frame<40;++frame)require(visuals.tick({0,128,0},false,error),error);
 require(visuals.snapshots().empty(),"all original spell UI owners must retire");
 require(visuals.levelUp(1,{80,300,0},error),error);const auto upgrade=visuals.snapshots();require(std::any_of(upgrade.begin(),upgrade.end(),[](const auto& node){return node.state.mode==17&&node.state.bank==7;}),"real power-up ring child");
 for(int tick=0;tick<150;++tick)require(visuals.tick({0,0,0},false,error),error);
 const auto notice=visuals.snapshots();require(notice.size()==1&&notice[0].state.bank==7&&notice[0].state.script==29,"native persistent level notice must replace its prior owner");
 std::cout<<"actual spell backdrop, two rings, portrait, title and original exit lifetimes passed\n";
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
