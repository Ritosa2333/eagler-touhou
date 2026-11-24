#include "game/ItemHost.hpp"
#include <cassert>
#include <fstream>
#include <iterator>
#include <string>
int main(int argc,char** argv){
    assert(argc==2);std::ifstream input(argv[1],std::ios::binary);
    const std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(input),{}};
    th128::item::ItemHost host;std::string error;
    assert(host.load(bytes,error));
    host.setMissHost([](std::string&){return true;});
    assert(host.spawn(2,{0,100,.1f},-1.5707963705062866f,2.2f,error));
    auto before=host.snapshots();assert(before.size()==1&&before[0].script==234&&before[0].sprite.width>0);
    assert(host.tick({180,400,0},1,false,error));
    auto after=host.snapshots();assert(after.size()==1&&after[0].age==1&&after[0].position.y<before[0].position.y);
    assert(after[0].state.ticks==before[0].state.ticks+1);
    for(int pass=0;pass<400&&!host.snapshots().empty();++pass)assert(host.tick({180,400,0},1,false,error));
    assert(host.snapshots().empty());
    assert(host.spawn(2,{0,100,.1f},-1.5707963705062866f,2.2f,error));
    assert(host.snapshots()[0].slot==0);
    th128::item::ItemHost collected;assert(collected.load(bytes,error));
    unsigned rewards=0;collected.setCollectHost([&](int kind,th128::item::Position,int state,std::string&){assert(kind==2&&state==4);++rewards;return true;});
    assert(collected.spawn(2,{0,360,.1f},-1.5707963705062866f,2.2f,error));
    for(unsigned pass=0;pass<100&&!collected.snapshots().empty();++pass)assert(collected.tick({0,400,0},1,false,error));
    assert(collected.snapshots().empty()&&rewards==1);
    th128::item::ItemHost ice;assert(ice.load(bytes,error));assert(ice.enableIceItems(6,error));
    unsigned iceRewards=0;ice.setCollectHost([&](int kind,th128::item::Position,int state,std::string&){assert(kind==9&&state==3);++iceRewards;return true;});
    ice.setMissHost([](std::string&){return true;});
    for(int index=0;index<100;++index)assert(ice.spawn(9,{0,300,.1f},0,0,error));
    assert(ice.iceState().pending==100&&ice.iceState().cursor==0&&ice.iceState().births==100);
    assert(ice.spawn(9,{0,300,.1f},0,0,error));assert(ice.iceState().births==101&&ice.iceState().pending==100);
    for(int pass=0;pass<60;++pass)assert(ice.tick({0,400,0},1,false,error));
    assert(iceRewards==100&&ice.iceState().pending==0&&ice.snapshots().empty());
}
