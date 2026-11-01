#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "sdl/AudioDevice.hpp"
using namespace th13;
namespace {
struct Provider:EclResourceProvider{std::vector<u8> bytes;std::vector<std::string>* names=nullptr;Provider(){std::vector<u8>b;th13test::emit(b,0,23,0,255,0,{10000});th13test::emit(b,0,10,0,255,0,{});bytes=th13test::build_scpt({{"main",b}},{"enemy.anm","st01enm.anm"});}bool read(const std::string&,std::vector<u8>&b)override{b=bytes;return true;}bool animation(u32,const std::string&n)override{if(names)names->push_back(n);return true;}};
struct Host:StageBattleServices{struct E{i32 id;float x;bool positional;};std::vector<E> requests;sdl::AudioDevice audio;bool life_hud(i32,i32)override{return true;}bool bomb_hud(i32,i32)override{return true;}bool request_game_over()override{return true;}bool hud_sound(i32 id)override{requests.push_back({id,0,false});return audio.enqueue(id);}bool hud_sound(i32 id,float x)override{requests.push_back({id,x,true});return audio.enqueue(id,x);}};
struct Rig{Provider p;Host h;SessionState s;HudPlayerState player;HudScoreState score;StageBattle b;GameInput in;bool init(){p.names=&b.sim.anim_list;if(!h.audio.initialize_effects_for_test("artifacts/reference/raw")||!b.load(p,"synthetic.ecl",1,h,s,player,score))return false;b.sim.enemies.clear();b.set_player_pos(0,200);h.requests.clear();return true;}};
}
TEST(spirit_birth_flag_and_pan){Rig r;CHECK(r.init());r.b.item_manager.spawn_value_item(1,96,200,0,2,true);CHECK_EQ(r.h.requests.size(),size_t(1));CHECK_EQ(r.h.requests[0].id,60);CHECK(r.h.requests[0].positional);r.h.audio.update();CHECK_EQ(r.h.audio.voice_pan(60),500);r.h.requests.clear();r.b.item_manager.spawn_value_item(3,-96,200,0,2,false);CHECK(r.h.requests.empty());}
TEST(spirit_collect_cross_segment_order){Rig r;CHECK(r.init());r.s.ending_points=198;r.b.item_manager.spawn_value_item(1,0,200,0,.5f,false);r.h.requests.clear();CHECK(r.b.step(r.in));CHECK_EQ(r.h.requests.size(),size_t(2));CHECK_EQ(r.h.requests[0].id,63);CHECK(r.h.requests[0].positional);CHECK_EQ(r.h.requests[1].id,68);CHECK(!r.h.requests[1].positional);}
TEST(big_power_full_plays_level_sound_before_pickup){for(int type:{1,3,8}){Rig r;CHECK(r.init());r.player.power=400;r.b.item_manager.spawn_item(0,200,0,type,0,0);r.h.requests.clear();CHECK(r.b.step(r.in));CHECK_EQ(r.h.requests.size(),size_t(type==1?1:2));if(type!=1)CHECK_EQ(r.h.requests[0].id,13);CHECK_EQ(r.h.requests.back().id,37);}}
int main(){return th13test::run_all("audio feedback production");}
