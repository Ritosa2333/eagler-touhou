#include "environment.hpp"
#include "../gameplay/enemy_entity.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../gameplay/player_state.hpp"
#include "../item_system/item.hpp"
#include "../bullet_system/bullet.hpp"
#include "../laser_system/laser.hpp"
#include "../startup_scene/startup.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../text_renderer/text.hpp"
#include <algorithm>
#include <cstring>
namespace th20::source::special_state {
namespace gp=gameplay;namespace ps=gp::player_state;namespace pe=program_entry;
namespace {
gp::EnemyController* enemies(){return static_cast<gp::EnemyController*>(game_session::context(0).objects_04[1]);}
gp::Enemy* find_enemy(unsigned handle){auto* owner=enemies();return owner?static_cast<gp::Enemy*>(gp::find_enemy_in_list(owner->enemies,handle)):nullptr;}
sprite::Vec3 position(unsigned handle){auto* value=find_enemy(handle);return value?gp::enemy_position(value):sprite::Vec3{0,0,0};} //constant57038c is zero vector
int clamp(game_session::Player& p,unsigned offset,int lo,int hi){const int value=std::clamp(ps::read<int>(p,offset),lo,hi);ps::write(p,offset,value);return value;}
void increment(game_session::Player& p,unsigned offset){ps::write(p,offset,std::clamp(recovered::signed_bits(ps::read<unsigned>(p,offset)+1),0,999999));}
int __cdecl follow_enemy(sprite::Animation* animation){auto& entry=*reinterpret_cast<Entry*>(animation->field_5c8);animation->vector_5bc=position(entry.enemy_handle);return 0;} //511ed0/511640
void initialize_entry(Entry& p,unsigned handle,int selected){
    p.enemy_handle=handle;recovered::timer_set(p.age,0);p.color=selected;
    auto& owner=*enemies();auto& sprites=*pe::sprite_controller;
    auto& file=*owner.services->existing_animation(owner,2);const sprite::Vec3 zero{0,0,0};
    sprite::spawn_named_animation(sprites,file,p.animation_handle,"enemy",selected+0x13f,&zero,0,-1,0);
    effects::Parameters effect;effects::construct_parameters(effect);effect.vector_00=position(p.enemy_handle);effect.value_18=0;
    const float spread=.06f,size=128.f;std::memcpy(&effect.value_1c,&spread,4);std::memcpy(&effect.value_24,&size,4);
    constexpr unsigned colors[]{0xffff0000,0xff0000ff,0xffffff00,0xff00ff00};effect.value_20=colors[selected];effects::controller(0)->spawn(11,&effect,nullptr,false);
    auto* animation=sprite::resolve_animation_handle(sprites,p.animation_handle);if(!animation)animation=&sprites.animation_dc;
    animation->field_5c8=reinterpret_cast<unsigned>(&p);animation->field_5dc=reinterpret_cast<unsigned>(&follow_enemy);
    auto* parent=find_enemy(handle);gp::SpawnParameters spawn;gp::construct_spawn_parameters(spawn);spawn.position=zero;if(parent)std::memcpy(spawn.variables,parent->state.fields_78,sizeof(spawn.variables));spawn.field_50=handle;
    auto& player=*game_session::context(0).current_player;int levels[4];for(unsigned i=0;i<4;++i)levels[i]=clamp(player,0x74+i*4,0,4);p.level=levels[selected];
    constexpr const char* scripts[]{"StoneAttackLevel_R1","StoneAttackLevel_R2","StoneAttackLevel_R3","StoneAttackLevel_R4","StoneAttackLevel_R5","StoneAttackLevel_B1","StoneAttackLevel_B2","StoneAttackLevel_B3","StoneAttackLevel_B4","StoneAttackLevel_B5","StoneAttackLevel_Y1","StoneAttackLevel_Y2","StoneAttackLevel_Y3","StoneAttackLevel_Y4","StoneAttackLevel_Y5","StoneAttackLevel_G1","StoneAttackLevel_G2","StoneAttackLevel_G3","StoneAttackLevel_G4","StoneAttackLevel_G5"};
    auto& attack=*gp::spawn_enemy(owner,scripts[selected*5+p.level],spawn,parent);auto& pattern=attack.state.pattern_1a8;
    const int sequence=clamp(player,0x94,0,999999)%3;pattern.fields_00[1]=sequence==2?4u:6u;
    const int index=p.level>=0&&p.level<=3?p.level:4;constexpr unsigned sprites_by_level[]{30,33,36,39,42};constexpr float sizes[]{64,80,88,96,112};
    pattern.fields_00[3]=sprites_by_level[index];pattern.fields_00[4]=sprites_by_level[index];pattern.field_a0=pattern.field_a4=sizes[index];
    if(selected>=0&&selected<4)increment(player,0x84+unsigned(selected)*4);increment(player,0x94);
    attack.callback=[&p](gp::Enemy* enemy){collect(p,enemy,environment());};
}
class GameEnvironment final:public Environment {
    text::Renderer& text(){return *text::renderer;}
public:
    game_session::Player& player() override{return *game_session::context(0).current_player;}
    int session_mode() override{return game_session::session.mode;}
    bool enemies_present() override{auto* owner=enemies();if(!owner)return false;for(auto handle:owner->data.handles_44)if(handle)return true;return false;}
    bool boss_collecting() override{return item::environment().boss_collecting();}
    bool special_blocked() override{return startup::unrecovered::owner_005c6114!=nullptr;}
    void spawn_enemy(const char* name,const gp::SpawnParameters& p) override{gp::spawn_enemy(*enemies(),name,p,nullptr);}
    bool enemy_exists(unsigned& handle) override{return find_enemy(handle)!=nullptr;}
    sprite::Vec3 enemy_position(unsigned& handle) override{return position(handle);}
    void effect(int type,const effects::Parameters& p) override{effects::controller(0)->spawn(type,&p,nullptr,false);}
    void delete_animation(unsigned& handle) override{sprite::request_animation_deletion(*pe::sprite_controller,handle);}
    void free_entry(Entry& p) override{std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(&p);}
    void initialize_entry(Entry& p,unsigned handle,int selected) override{special_state::initialize_entry(p,handle,selected);}
    void clear_bullets(const sprite::Vec3& p) override{bullet::cancel_circle(*bullet::controller(0),p,320,0,99999,0);}
    void clear_lasers(const sprite::Vec3& p) override{laser::controller(0)->cancel_circle(p,320,0,1);}
    sprite::Vec3 position_of(void* enemy) override{return gp::enemy_position(enemy);}
    void text_style(int x,int y) override{text().fields_1a1d4[8]=unsigned(x);text().fields_1a1d4[9]=unsigned(y);}
    void text_font(int value) override{text().fields_1a1d4[3]=unsigned(value);}
    void text_color(unsigned value) override{text().color=value;}
    void text_alpha(std::uint8_t value) override{text().color=(text().color&0xffffff)|(unsigned(value)<<24);}
    void text_scale(float x,float y) override{text().scale_x=x;text().scale_y=y;}
    void text_save(int value) override{text().fields_1a1d4[10]=unsigned(value);}
    void text_restore() override{ //4e67e0 exact ordered setters
        auto& p=text();p.color=0xffffffff;p.field_1a1c8=0xffffffff;p.shadow_color=0xff000000;p.fields_1a1d4[1]=0;p.font_width=9;p.scale_x=p.scale_y=1;p.fields_1a1d4[2]=0;p.fields_1a1d4[3]=0;p.fields_1a1d4[4]=2;p.fields_1a1d4[6]=0;p.fields_1a1d4[7]=0;p.fields_1a1d4[8]=p.fields_1a1d4[9]=1;p.rotation=0;p.fields_1a1d4[10]=0;
    }
    void text_line(const sprite::Vec3& p,const char* s) override{text().write_ascii(p,s);}
    void text_level(const sprite::Vec3& p,unsigned value) override{text().write_prefixed_integer(p,"Level ",recovered::signed_bits(value));}
};
}
Environment& environment(){static GameEnvironment host;return host;}
}
