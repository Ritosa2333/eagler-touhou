#include "shot_hit.hpp"
#include "../damage_regions/hit_callbacks.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_state/state.hpp"
#include "../effect_system/effect.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/pool.hpp"
#include <cstring>
#include <stdexcept>
namespace th20::source::player_entity {
namespace {
class GameShotHits final:public ShotHitServices {
public:
    ShotCallbackEnvironment& callbacks() override{return shot_callback_environment();}
    int default_hit(Shot& shot) override{return damage::default_shot_hit(&shot,*program_entry::sprite_controller);}
    float random_signed(unsigned stream) override{return state::signed_unit(state::random_streams[stream]);}
    std::uint32_t random_bounded(unsigned stream,std::uint32_t maximum) override{return state::bounded(state::random_streams[stream],maximum);}
    sprite::AnimationFile& effect_file(game_session::Context& context) override{return *static_cast<effects::Controller*>(context.objects_04[7])->files[0];}
    std::uint32_t spawn(sprite::AnimationFile& file,const char* name,int script,const sprite::Vec3& position,float angle) override{std::uint32_t handle;sprite::spawn_named_animation(*program_entry::sprite_controller,file,handle,name,script,&position,angle,-1,0);return handle;}
    damage::Region* create_circle(game_session::Context& context,const sprite::Vec3& position,float radius,float growth,int frames,int damage_value) override{auto& owner=*static_cast<damage::HitCtrlInf*>(context.object_28);const auto handle=damage::create_circle(owner,position,radius,growth,frames,damage_value);return owner.find(handle);}
    void remember_effect(game_session::Context& context,std::uint32_t handle) override{remember_shot_effect(*static_cast<effects::Controller*>(context.objects_04[7]),handle,callbacks());}
};
template<unsigned Index>int __fastcall hit(Shot* shot,void*,const sprite::Vec3* position,const sprite::Vec2* size,float angle,float radius){return hit_shot_callback(*shot,Index,*position,size,angle,radius,shot_hit_services());}
const std::uintptr_t hit_callbacks[]{0,reinterpret_cast<std::uintptr_t>(&hit<1>),reinterpret_cast<std::uintptr_t>(&hit<2>),reinterpret_cast<std::uintptr_t>(&hit<3>),reinterpret_cast<std::uintptr_t>(&hit<4>),reinterpret_cast<std::uintptr_t>(&hit<5>),reinterpret_cast<std::uintptr_t>(&hit<6>),reinterpret_cast<std::uintptr_t>(&hit<7>),reinterpret_cast<std::uintptr_t>(&hit<8>),reinterpret_cast<std::uintptr_t>(&hit<9>),reinterpret_cast<std::uintptr_t>(&hit<10>),0};
}
ShotHitServices& shot_hit_services(){static GameShotHits result;return result;}
namespace unrecovered {std::uintptr_t shot_hit_callback(unsigned index){if(index>=std::size(hit_callbacks))throw std::out_of_range("Invalid SHT hit callback index");return hit_callbacks[index];}}
}
