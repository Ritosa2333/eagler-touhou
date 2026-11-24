#include "game/FocusIndicator.hpp"
#include <cassert>
#include <fstream>
#include <iterator>
int main(int count,char** arguments){
    assert(count==2);std::ifstream input(arguments[1],std::ios::binary);assert(input);
    const std::vector<unsigned char> bytes{std::istreambuf_iterator<char>(input),{}};
    th128::anm::Registry registry;std::string error;assert(registry.loadBank(0,bytes.data(),bytes.size(),error));
    th128::player::FocusIndicator marker;assert(marker.configure(registry,error));assert(marker.draws().empty());
    assert(marker.update(true,0,400,0,error));const auto born=marker.draws();assert(born.size()==2&&marker.focused());
    for(const auto& draw:born){assert(draw.state.script==79||draw.state.script==80);assert(draw.state.sprite==444);assert(draw.state.layer==16);assert(draw.x==320&&draw.y==416);}
    assert(marker.update(true,-100,200,.1f,error));const auto moved=marker.draws();assert(moved.size()==2);
    for(const auto& draw:moved)assert(draw.x==220&&draw.y==216&&draw.z==.1f);
    for(int frame=0;frame<60;++frame)assert(marker.tick(error));assert(marker.draws().size()==2);
    const auto before=marker.draws();for(int frame=0;frame<10;++frame)assert(marker.draws()[0].state.ticks==before[0].state.ticks);
    assert(marker.update(false,-100,200,.1f,error));assert(!marker.focused());
    for(int frame=0;frame<10;++frame)assert(marker.tick(error));assert(marker.draws().empty());
    assert(marker.update(true,10,100,0,error));assert(marker.draws().size()==2);assert(!marker.configure(registry,error));
}
