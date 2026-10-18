#pragma once
#include "Types.hpp"
#include <array>
#include <vector>
namespace th08 {
struct EclInstruction {i32 time;i16 opcode,size;u8 reserved,difficulties;u16 variable_mask;};
struct EclTimelineInstruction {i32 time;u16 opcode;u8 size,difficulties;};
struct EclHeader {u32 version;i16 sub_count,timeline_count;u32 timeline_offsets[16];};
static_assert(sizeof(EclInstruction)==12&&sizeof(EclTimelineInstruction)==8&&sizeof(EclHeader)==72);
// Owns original ECL bytecode, not machine code. Offset tables are decoded
// into ordinary object pointers without rewriting the portable input bytes.
class EclProgram {
    bool practice_instructions=false;
    std::vector<u8> storage;std::vector<EclInstruction*> subs;std::vector<u32> sub_lengths,instruction_offsets;
    std::array<EclTimelineInstruction*,16> timelines{};std::array<u32,16> timeline_lengths{};
public:
    bool load(const u8* data,u32 size);
    void release(){practice_instructions=false;storage.clear();subs.clear();sub_lengths.clear();instruction_offsets.clear();timelines.fill(nullptr);timeline_lengths.fill(0);}
    u8* mutable_data()noexcept{return storage.data();}
    void enable_practice_instructions()noexcept{practice_instructions=true;}
    const u8* data()const noexcept{return storage.data();}
    u32 size()const noexcept{return storage.size();}
    u32 sub_count()const noexcept{return subs.size();}
    u32 timeline_count()const noexcept{return storage.empty()?0:reinterpret_cast<const EclHeader*>(storage.data())->timeline_count;}
    EclInstruction* sub(i32 index)const noexcept{return index>=0&&u32(index)<subs.size()?subs[index]:nullptr;}
    EclTimelineInstruction* timeline(i32 index)const noexcept{return index>=0&&u32(index)<timeline_count()?timelines[index]:nullptr;}
    u32 sub_size(u32 index)const noexcept{return index<sub_lengths.size()?sub_lengths[index]:0;}
    u32 timeline_size(u32 index)const noexcept{return index<timeline_count()?timeline_lengths[index]:0;}
    bool has_instruction(const EclInstruction* instruction)const noexcept;
};
}
