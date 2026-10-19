#include "item.hpp"
#include "collect.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/dispatch.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../effect_system/effect.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../gameplay/enemy_frame.hpp"
#include <cstring>
namespace th20::source::item {
namespace {
template<class T>T read(const void* object,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(object)+offset,sizeof(value));return value;}
sprite::Controller& sprites(){return *program_entry::sprite_controller;}
class GameEnvironment final:public Environment {
public:
    void bind_animation(ItemInf& owner,sprite::Animation& animation,int script) override {auto* file=read<sprite::AnimationFile*>(owner.context->primary_owner,0x286d8c);sprite::bind_animation_script(*file,animation,script,nullptr);}
    void bind_special_animation(sprite::Animation& animation,int script) override {auto* file=read<sprite::AnimationFile*>(unrecovered::special_owner_0051b960(),0x10);sprite::bind_animation_script(*file,animation,script,nullptr);}
    void spawn_effect(Item& item) override{spawn_item_effect(item);}
    void bonus_notification() override{auto* target=read<std::uint8_t*>(game_session::overlay_owner(0),0x34);target[0x35]=1;}
    void select_view(ItemInf& owner) override{sprites().field_6c4=owner.view_index;}
    void configure_layer(ItemInf& owner,int layer) override{sprite::configure_animation_layer(sprites(),layer,owner.view_index);}
    bool boss_collecting() override{return read<int>(gameplay::unrecovered::boss_hud_005c06a4(),0x1bc)!=0;}
    void activate_special(Item& item) override{activate_special_item(item);}
    void collect(Item& item) override{collect_item(item);}
    void collect_sound(const Item& item) override{program_entry::thread_registry.request_effect_at(37,item.position.x);}
    void update_animation(sprite::Animation& animation) override{sprite::execute_animation(animation);}
    void draw_animation(sprite::Animation& animation) override{sprite::draw_animation(sprites(),animation);}
    void move_attachment(std::uint32_t& handle,const sprite::Vec3& position) override{if(auto* animation=sprite::find_animation(sprites(),handle))animation->vector_5bc=position;}
    void retire_attachment(std::uint32_t& handle) override{sprite::request_animation_deletion(sprites(),handle);}
};
}
Environment& environment(){static GameEnvironment value;return value;}
void spawn_item_effect(Item& item){
    if(item.type==4||item.type==6||item.type==14||item.type==5||item.type==7){
        auto& effects=*static_cast<effects::Controller*>(item.context->objects_04[7]);std::uint32_t handle;
        sprite::spawn_named_animation(sprites(),*effects.files[0],handle,"effect",94,&item.position,0,-1,0);
        program_entry::thread_registry.request_effect(item.type==4||item.type==5?74:48,0);
    } else if(item.type==1||item.type==2){
        auto& effects=*static_cast<effects::Controller*>(item.context->objects_04[7]);std::uint32_t handle;
        sprite::spawn_named_animation(sprites(),*effects.files[0],handle,"effect",94,&item.position,0,-1,0);
        if(item.sound>=0)program_entry::thread_registry.request_effect(item.sound,0);
    }
}
void activate_special_item(Item& item){activate_special_item(item,environment());}
}
