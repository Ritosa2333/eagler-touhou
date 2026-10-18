#include "../th08_web/cpp/game/EclProgram.hpp"
#include <cassert>
#include <vector>
using namespace th08;
int main(){
    // One retail subroutine and one timeline; pointers must survive injection.
    std::vector<u8> retail(96);EclHeader header{};header.version=0x800;
    header.sub_count=header.timeline_count=1;header.timeline_offsets[0]=88;header.timeline_offsets[1]=96;
    std::memcpy(retail.data(),&header,sizeof(header));const u32 sub=76;std::memcpy(retail.data()+72,&sub,4);
    EclInstruction stop{};stop.opcode=-1;stop.size=12;std::memcpy(retail.data()+76,&stop,12);
    EclTimelineInstruction end{};end.time=-1;std::memcpy(retail.data()+88,&end,8);
    EclProgram program;assert(program.load(retail.data(),retail.size()));
    const auto* before=program.data();const auto* timeline=program.timeline(0);const auto* routine=program.sub(0);
    const u32 jump[]{16,0x00140004,0x0000ff00,10,0};u32 offset=0;
    // A malformed length is rejected without changing storage.
    u32 bad[]{16,0x00130004,0x0000ff00,10,0};
    assert(!program.append_practice_ecl(reinterpret_cast<const u8*>(bad),sizeof(bad),offset));
    assert(program.size()==retail.size()&&program.data()==before);
    assert(program.append_practice_ecl(reinterpret_cast<const u8*>(jump),sizeof(jump),offset));
    assert(offset==96&&program.data()==before&&program.timeline(0)==timeline&&program.sub(0)==routine);
    auto* injected=reinterpret_cast<const EclInstruction*>(program.data()+offset);
    assert(!program.has_instruction(injected));program.enable_practice_instructions();assert(program.has_instruction(injected));
    assert(!program.has_instruction(reinterpret_cast<const EclInstruction*>(program.data()+offset+1)));
    assert(!program.append_practice_ecl(reinterpret_cast<const u8*>(jump),sizeof(jump),offset));
    program.release();assert(program.size()==0);
    assert(program.load(retail.data(),retail.size()));assert(!program.has_instruction(program.sub(0)+1));
}
