#pragma once
#include "core/stages/OmniverseTimelineView.hpp"
#include "core/stages/OmniverseTimelineArithmetic.hpp"
#include "core/stages/OmniverseTimelineEmitter.hpp"
#include "core/stages/Stage6BossTimelineColoredEffect.hpp"
namespace kaikeidou::omniverse_timeline_detail {
struct Engine{
 OmniverseTimelineState& s;Random& random;RandomPrecision precision;GameMath& math;OmniverseTimelineServices& services;
 PlayerShotFireInput& input;const std::array<std::uint32_t,9>& bindings;const PlayerShotFireContext& fire;std::uint32_t boss;Arithmetic m;
 StageTimeline& t(){return s.timeline;}std::uint32_t frame(){return t().frame;}std::int64_t signed_frame(){return signed_word(frame());}
 std::uint32_t& integer(unsigned i){return s.parameters.integers.at(i);}std::uint64_t& real(unsigned i){if(i<2)return s.parameters.reals[i];if(i<4)return s.boss.extra_reals[i-2];if(i==4)return s.extra.real_4;throw std::out_of_range("Omniverse real index");}
 std::uint32_t character(){return s.lifecycle.character;}View enemy_at(std::uint32_t i){return {s,0xa87fe8u+i*248u};}View enemy(){return enemy_at(boss);}View bullet(std::uint32_t i){return {s,0xa8e660u+i*240u};}View secondary(std::uint32_t i){return {s,0xa8ce60u+i*192u};}View item(std::uint32_t i){return {s,0xa758b0u+i*64u};}View effect_view(std::uint32_t i){return {s,0xa735c0u+i*112u};}
 Number q(unsigned o){return m.q(enemy().quad(o));}void put(unsigned o,Number n){enemy().quad(o,m.store(n));}
 Number cosine(std::uint64_t a){return omniverse_timeline_cosine(math,a);}Number sine(std::uint64_t a){return omniverse_timeline_sine(math,a);}
 Number rnd(std::uint32_t a){return random.real_number(constant(a),precision);}std::uint32_t random_integer(std::uint32_t n){return random.integer_bits(n,precision,true);}
 std::uint32_t read(unsigned i){return omniverse_timeline_input(bindings[i],fire,input);}
 void name(std::uint32_t a){copy_name(s.boss.spell_name,a);}void reset(std::uint32_t n){t().frame=n;s.parameters.game_frame=n;}
 void begin(std::uint32_t kind,std::uint32_t track,std::uint32_t target_y){if(frame()==1){if(s.player.word(0x24)==0)services.music(track,1);boss=static_cast<std::uint32_t>(services.allocate_boss(kind));enemy().word(8,14);enemy().quad(56,constant(0x6d1a60));enemy().quad(64,constant(0x6d1b58));enemy().quad(168,constant(0x6d1a60));enemy().quad(176,constant(target_y));}else if(frame()==10)services.begin_boss_effect();else if(frame()==60){t().subphase=1;reset(0xffffffffu);}}
 void effect(std::uint32_t kind,std::array<std::uint32_t,3> color){const auto y=enemy().quad(64),x=enemy().quad(56);const auto packed=services.color(color[0],color[1],color[2]);allocate_effect(s.effects,s.projectiles.sound_triggers,{kind,packed,x,y});}
 void effect_at(std::uint32_t kind,std::array<std::uint32_t,3> color,std::uint64_t x,std::uint64_t y){const auto packed=services.color(color[0],color[1],color[2]);allocate_effect(s.effects,s.projectiles.sound_triggers,{kind,packed,x,y});}
 std::int32_t allocate(std::uint32_t kind,std::int32_t group,std::array<std::uint32_t,3> color,std::uint32_t blend=0,std::uint32_t delayed=0){return allocate_projectile(s.projectiles,{kind,group,color,blend,255,delayed,41});}
 void emit(const ProjectileEmission& e){omniverse_timeline_emit(s.projectiles,e,s.player,s.parameters,random,precision,math);}
 void part0();void part1();void part2();void part3();void part4();void part5();void part5_reimu();void part5_marisa();void part5_sanae();void part6();void part6_reimu();void geometry_late_events();void geometry_update();void geometry_intersections();void part6_marisa();void part6_sanae();void part7();void part7_reimu();void part7_marisa();void part7_sanae();void part7_opacity();void closing();
};
}
