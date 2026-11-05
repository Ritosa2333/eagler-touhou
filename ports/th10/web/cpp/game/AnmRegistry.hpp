#pragma once
#include "AnmVm.hpp"
namespace th10 {
struct AnmFrameEnvironment;
// List heads at 0x72dad4 within the original animation manager. The allocator
// and its VM pool will be a separate owner; registry operations need only lists.
struct AnmRegistry {
    ListNode<AnmVm>* world_head;
    ListNode<AnmVm>* world_tail;
    ListNode<AnmVm>* ui_head;
    ListNode<AnmVm>* ui_tail;
    AnmVm* find(u32 id) const noexcept;
    AnmVm* find_and_clear(u32& id) const noexcept;
    u32 find_child(u32& id,i32 script) const noexcept;
    void interrupt(u32 id,std::int16_t label) const noexcept;
    i32 interrupt_and_update(u32 id,std::int16_t label,AnmFrameEnvironment& environment) const;
    void set_visibility(u32 id,bool visible) const noexcept;
    void request_delete(u32 id) const noexcept;
    void delete_and_clear(u32& id) const noexcept;
    void discard_file(AnmFile* file) const noexcept;
    void set_position(u32 id,const Vec3& position,bool playfield_coordinates) const noexcept;
};
}
