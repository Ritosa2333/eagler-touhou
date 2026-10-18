#pragma once
#include "sprite.hpp"
namespace th20::source::sprite {
// Vtable at Animation+568: deleting destructor, update, draw, retire.
// Each concrete callback must implement its actual engine behavior.
struct AnimationCallback {
    virtual ~AnimationCallback() = default;
    virtual std::int32_t update() = 0;
    virtual void draw() = 0;
    virtual void retire() = 0;
    virtual void interrupt(std::int32_t event) = 0; // vtable+0x10, 4388f0
};
using AnimationList = Controller::AnimationList;
void initialize_animation_list(AnimationList&) noexcept;       // 44a640
void initialize_animation_link(AnimationLink&,Animation*) noexcept; //44a670
void prepend_animation_link(AnimationList&,AnimationLink&) noexcept; //44a6b0
Animation* create_heap_animation();                            //447800
Animation* allocate_animation(Controller&);                    //44c9b0
void release_animation_geometry(Animation&) noexcept;          //44c5f0
void destroy_animation_contents(Animation&);                   //44c240 /449370
void destroy_heap_animation(Animation*);                       //447410 /449790
bool is_pooled_animation(const Controller&,const Animation*) noexcept; //44d970
std::int32_t retire_animation(Controller&,Animation&);          //44c2f0
std::uint32_t assign_animation_handle(Controller&,Animation&) noexcept; //44c8f0
Animation* find_animation(Controller&,std::uint32_t) noexcept;   //44cd00
Animation* resolve_animation_handle(Controller&,std::uint32_t&) noexcept; //44ced0
std::uint32_t register_animation(Controller&,AnimationList* group,Animation&,bool secondary,bool front) noexcept; //44b7f0/44b840/44b890/44b8e0
void retire_animation_group(Controller&,AnimationList* group); //44a3a0
void queue_animation_retirement(Animation&,AnimationList&); //44f840, descendants before parent
void update_animation_group(Controller&,AnimationList* group,bool secondary); //449870/449b20
std::int32_t update_animations(Controller&,bool secondary); //4497d0/449a70, returns1
void mark_file_animations(Controller&,AnimationFile*,bool preserve_flagged) noexcept; //44fd30
void set_animation_color(Animation&,std::uint32_t) noexcept;    //44fa10
Vec3 detached_animation_position(Animation&);                  //44cb80
std::uint32_t spawn_child_animation(Controller&,AnimationFile&,Animation&,std::int32_t script,std::uint32_t flags); //451050
std::uint32_t spawn_detached_animation(Controller&,AnimationFile&,Animation&,std::int32_t script,std::uint32_t flags); //4512b0
}
