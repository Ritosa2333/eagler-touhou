#pragma once
#include "overlay.hpp"
namespace th20::source::overlay {
class Environment {
public:
    virtual ~Environment()=default;
    virtual Weapon* create_weapon(int character,int stone)=0; //18-entry factory table5b0ab0
    virtual void destroy_weapon(Weapon*)=0;                  //532740
    virtual void destroy_mesh(sprite::RenderMesh*)=0;        //4a2800
    virtual void unload_animation(int)=0;                  //44c430
    virtual void preserve_animation(int,bool)=0;            //4863f0
    virtual void select_view(int)=0;                        //4776a0
    virtual sprite::AnimationFile* load_animation(int,const char*)=0; //44ee50
    virtual void load_error()=0;                            //454150
    virtual bool restarting()=0;                           //488830
    virtual std::uint8_t replay_inherited(int)=0;            //5c60fc+1c→record+ec
    virtual int stage_id()=0;                               //selected_stage->id
    virtual void delete_animation(std::uint32_t&)=0;         //44fcd0
};
// Remaining integration has no default behavior. Restored owner source uses
// these named dependencies until its replay and game-entry chain is completed.
namespace unrecovered {std::uint8_t replay_inherited_stone(int slot);}
}
