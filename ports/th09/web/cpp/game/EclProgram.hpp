#pragma once
#include "Types.hpp"
#include <vector>
namespace th09 {
struct EclInstruction {i32 time;i16 opcode,size;u8 reserved,difficulties;u16 variable_mask;};
struct EclTimelineInstruction {i32 time;u16 opcode;u8 size,difficulties;};
static_assert(sizeof(EclInstruction)==12&&sizeof(EclTimelineInstruction)==8);
// TH09 bytecode has a variable-length timeline table, followed by the sub table.
// These are game scripts, not x86 instructions. Offsets stay in the resource.
class EclProgram {
    std::vector<u8> storage;
    std::vector<u32> subs,timelines,sub_lengths,timeline_lengths,instructions;
public:
    bool load(const u8* data,u32 size);
    void release();
    const u8* data() const noexcept{return storage.data();}
    u32 size() const noexcept{return storage.size();}
    u32 sub_count() const noexcept{return subs.size();}
    u32 timeline_count() const noexcept{return timelines.size();}
    EclInstruction* sub(i32 index) noexcept;
    EclTimelineInstruction* timeline(i32 index) noexcept;
    u32 sub_size(u32 index) const noexcept{return index<sub_lengths.size()?sub_lengths[index]:0;}
    u32 timeline_size(u32 index) const noexcept{return index<timeline_lengths.size()?timeline_lengths[index]:0;}
    bool has_instruction(const EclInstruction* instruction) const noexcept;
};
}
