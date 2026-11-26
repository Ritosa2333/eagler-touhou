// 已有逻辑服务的同Refs组装；全部游戏算法仍直接调用冻结恢复模块。
#pragma once
#include "core/runtime/GameplayFrame.hpp"
#include "core/runtime/GameplayFrameWorldView.hpp"
#include "core/runtime/GameplayHostPlatform.hpp"
#include "core/frontend/MenuSceneRuntimeContext.hpp"
#include "core/frontend/MenuTitleReturn.hpp"
namespace kaikeidou {
class GameplayHostServices:public BossPhaseServices,public Stage6BossTimelineServices,public BossPresentationServices,
 public DialogueServices,public SaveIO,public StageBackgroundServices,public MusicPresentationServices,public LifecycleServices {
public:
 using U=std::uint32_t;using Q=std::uint64_t;
 GameStartEntryRefs game;GameplayFrameWorldView& state;StageTimeline& timeline;GameplayHostPlatform& platform;
 GameplayHostPresentation& drawing;GameMath& math;RandomPrecision precision;MenuSceneRuntimeContext& menu;
 GameplayHostServices(GameStartEntryRefs r,GameplayFrameWorldView& world,StageTimeline& time,GameplayHostPlatform& api,GameplayHostPresentation& gfx,GameMath& functions,RandomPrecision bits,MenuSceneRuntimeContext& scene):game(r),state(world),timeline(time),platform(api),drawing(gfx),math(functions),precision(bits),menu(scene){}
 void synchronize(){auto& life=game.lifecycle;auto& d=game.death;d.stage=life.stage;d.part=timeline.part;d.subphase=timeline.subphase;d.character=life.character;d.boss_state=life.boss_state;game.boss_phase.hit_flag=life.hit_flag;game.boss_phase.weapon=game.options_context.weapon;game.patterns.game_frame=timeline.frame;}
 void publish(){auto& d=game.death;auto& l=game.lifecycle;timeline.part=d.part;timeline.subphase=d.subphase;timeline.frame=game.patterns.game_frame;l.stage=d.stage;l.part=d.part;l.subphase=d.subphase;l.boss_state=d.boss_state;l.hit_flag=game.boss_phase.hit_flag;}
 U color(U r,U g,U b)override{return drawing.color(r,g,b);}
 void resolve_enemy_death(U index)override{synchronize();kaikeidou::resolve_enemy_death(game.enemies,index,game.items,game.effects,game.shake,game.projectiles,game.player,game.patterns,game.death,game.random,math,precision,*this);publish();}
 void advance_phase(U mode)override{advance_boss_phase(mode,game.enemies,game.player,game.items,game.effects,game.shake,game.projectiles,game.savedata,game.patterns,game.death,game.boss_phase,game.random,math,precision,*this);publish();}
 std::int32_t allocate_boss(U kind)override{synchronize();auto index=kaikeidou::allocate_boss(game.enemies,game.boss_allocation,game.boss_phase,game.death,game.protection,kind);publish();return index;}
 void configure_phase(U spell,U seconds,U hp,U index)override{synchronize();configure_boss_phase(game.enemies,game.boss_allocation,game.boss_phase,game.death,game.protection,game.boss_presentation,game.patterns,*this,precision,spell,seconds,hp,index);publish();}
 void begin_boss_effect()override{kaikeidou::begin_boss_effect(game.boss_presentation);}
 void retire_boss(U mode)override{kaikeidou::retire_boss(mode,game.enemies,game.items,game.effects,game.shake,game.projectiles.sound_triggers,game.boss_allocation,game.boss_phase,game.boss_presentation,*this);}
 void sync_background(){game.resources.stage=game.lifecycle.stage;game.resources.part=timeline.part;game.resources.character=game.lifecycle.character;}
 void restore_stage_music(U mode)override{sync_background();control_stage_movies(mode,game.resources,*this);}
 void pause_background()override{sync_background();pause_stage_background(game.resources,*this);}
 void prepare_spell_music()override{pause_background();}
 void persist_spell_records(const SaveData& data)override{store_spell_records(data,*this);}
 void dialog(U mode)override{initialize_dialogue(game.dialogue,mode);state.fire_lock=game.dialogue.fields.word(8);}
 void award_stage_clear()override{kaikeidou::award_stage_clear(game.player,game.savedata,{game.lifecycle.stage,game.lifecycle.difficulty,game.lifecycle.character,game.options_context.weapon,game.lifecycle.practice},*this);}
 void change_stage_resources(U stage)override{game.lifecycle.stage=game.death.stage;sync_background();kaikeidou::change_stage_resources(stage,game.resources,game.meshes,game.player,game.random,precision,math,*this);}
 std::int32_t delete_graph(U graph,U flag)override{return static_cast<std::int32_t>(platform.call(0x403e20,{graph,flag}).eax);}
 U load_graph(std::string_view path,U flag)override{return platform.call(0x43c090,{flag},{path}).eax;}
 U platform_random(U maximum)override{return platform.call(0x466760,{maximum}).eax;}
 std::int32_t movie_seek(U handle,U position)override{return static_cast<std::int32_t>(platform.call(0x406770,{handle,position}).eax);}
 std::int32_t movie_play(U handle,U mode,U flag)override{return static_cast<std::int32_t>(platform.call(0x406690,{handle,mode,flag}).eax);}
 std::int32_t movie_pause(U handle,U flag)override{return static_cast<std::int32_t>(platform.call(0x406700,{handle,flag}).eax);}
 std::int32_t set_viewport_matrix(const std::array<U,16>& m)override{return static_cast<std::int32_t>(platform.call(0x40a9a0,std::vector<U>(m.begin(),m.end())).eax);}
 bool open(std::string_view path,bool writing)override{platform.memory.reconcile();auto result=platform.platform.savedata_io().open(path,writing);platform.memory.reconcile();return result;}
 std::size_t read(std::uint8_t* data,std::size_t size)override{platform.memory.reconcile();auto result=platform.platform.savedata_io().read(data,size);platform.memory.reconcile();return result;}
 void write(const std::uint8_t* data,std::size_t size)override{platform.memory.reconcile();platform.platform.savedata_io().write(data,size);platform.memory.reconcile();}
 void close()override{platform.memory.reconcile();platform.platform.savedata_io().close();platform.memory.reconcile();}
 void brightness(U r,U g,U b)override{drawing.brightness(r,g,b);}void blend(U m,U a)override{drawing.blend(m,a);}
 void sprite(Q x,Q y,Q s,Q a,U g,U t,U f)override{drawing.sprite(x,y,s,a,g,t,f);}
 void text(U x,U y,std::string_view value,U ink,U font,U edge,U vertical)override{drawing.draw_font_text(static_cast<std::int32_t>(x),static_cast<std::int32_t>(y),value,ink,font,edge,vertical);}
 std::int32_t set_create_sound_data_type(U type)override{return static_cast<std::int32_t>(platform.call(0x43e0d0,{type}).eax);}
 std::int32_t stop_sound(U handle)override{return static_cast<std::int32_t>(platform.call(0x440240,{handle}).eax);}
 std::int32_t delete_sound(U handle,U flag)override{return static_cast<std::int32_t>(platform.call(0x43e010,{handle,flag}).eax);}
 std::int32_t load_sound(std::string_view path,U buffers,U union_handle)override{return static_cast<std::int32_t>(platform.call(0x4434d0,{buffers,union_handle},{path}).eax);}
 std::int32_t set_volume(U volume,U handle)override{return static_cast<std::int32_t>(platform.call(0x43fd40,{volume,handle}).eax);}
 std::int32_t play_sound(U handle,U type,U top)override{return static_cast<std::int32_t>(platform.call(0x4425f0,{handle,type,top}).eax);}
 void music(U track,U mode)override{switch_music(track,mode,game.music,game.runtime.outer_scene,game.lifecycle.scene,game.savedata.options[1],static_cast<unsigned>(precision),*this);}
 U read_counter(U binding)override{return platform.physical(binding);}
 void drop_items()override{drop_death_items(game.items,game.player,game.lifecycle.character,math,precision);}
 void effect(U kind,U ink,Q x,Q y)override{allocate_effect(game.effects,game.projectiles.sound_triggers,{kind,ink,x,y});}
 void reset_options(U first,U last)override{reset_player_options(game.options,game.player,game.options_context,first,last);}
 void reset_speed(Q scale)override{reset_player_bomb_speed(game.player,game.lifecycle.character,scale,static_cast<unsigned>(precision));}
 void original_action(U track,U mode)override{music(track,mode);}
};
class GameplayHostTitleReturn final:public MenuTitleReturnServices {
 GameplayHostServices& h_;
public:
 using U=std::uint32_t;explicit GameplayHostTitleReturn(GameplayHostServices& h):h_(h){}
 std::int32_t delete_graph(U h,U f)override{return h_.delete_graph(h,f);}U load_graph(std::string_view p,U f)override{return h_.load_graph(p,f);}U platform_random(U m)override{return h_.platform_random(m);}
 std::int32_t movie_seek(U h,U p)override{return h_.movie_seek(h,p);}std::int32_t movie_play(U h,U m,U f)override{return h_.movie_play(h,m,f);}
 std::int32_t set_create_sound_data_type(U t)override{return h_.set_create_sound_data_type(t);}std::int32_t stop_sound(U h)override{return h_.stop_sound(h);}std::int32_t delete_sound(U h,U f)override{return h_.delete_sound(h,f);}
 std::int32_t load_sound(std::string_view p,U n,U h)override{return h_.load_sound(p,n,h);}std::int32_t set_volume(U v,U h)override{return h_.set_volume(v,h);}std::int32_t play_sound(U h,U t,U f)override{return h_.play_sound(h,t,f);}
};
class GameplayHostLateTimeline final:public ExBossLateTimelineServices {
 GameplayHostServices& h_;GameplayHostTitleReturn& title_;
public:
 using U=std::uint32_t;GameplayHostLateTimeline(GameplayHostServices& h,GameplayHostTitleReturn& title):h_(h),title_(title){}
 std::int32_t allocate_boss(U k)override{return h_.allocate_boss(k);}void configure_phase(U s,U t,U hp,U i)override{h_.configure_phase(s,t,hp,i);}void begin_boss_effect()override{h_.begin_boss_effect();}void retire_boss(U m)override{h_.retire_boss(m);}U color(U r,U g,U b)override{return h_.color(r,g,b);}
 void dialog(U m)override{h_.dialog(m);}void award_stage_clear()override{h_.award_stage_clear();}void pause_background()override{h_.pause_background();}void stop_music()override{h_.stop_sound(h_.game.music.audio_handle);}
 void return_to_title(bool replay)override{return_menu_title(h_.game.menu,h_.menu.title,h_.game.resources,title_,replay);h_.platform.memory.reconcile();}
 void initialize_title_visuals()override{initialize_menu_title(h_.menu.title,h_.game.menu,h_.game.music,h_.game.lifecycle.scene,h_.game.savedata,static_cast<unsigned>(h_.precision),title_);h_.platform.memory.reconcile();}
 void draw_line(U x1,U y1,U x2,U y2,U ink,U width)override{h_.platform.call(0x42d190,{x1,y1,x2,y2,ink,width});}
};
class GameplayHostEnemies final:public PlayerBombHitServices,public EnemyDamageServices {
 GameplayHostServices& h_;
public:
 explicit GameplayHostEnemies(GameplayHostServices& h):h_(h){}
 std::uint32_t overflow_row_word()override{throw std::runtime_error("Marisa C row7 requires a registered original stack boundary");}
 void damage(std::uint32_t slot,std::uint32_t amount)override{apply_enemy_damage(h_.game.enemies,h_.game.player,h_.game.projectiles.sound_triggers,h_.game.protection,slot,amount,*this);}
 void check_enemy_death(std::uint32_t slot)override{kaikeidou::resolve_enemy_death(h_.game.enemies,slot,h_.game.items,h_.game.effects,h_.game.shake,h_.game.projectiles,h_.game.player,h_.game.patterns,h_.game.death,h_.game.random,h_.math,h_.precision,h_);}
 void synchronize(const LifecycleContext& life,const StageTimeline& time){auto& d=h_.game.death;d.stage=life.stage;d.part=time.part;d.subphase=time.subphase;d.character=life.character;d.boss_state=life.boss_state;}
};
class GameplayHostFirst final:public ExFirstServices,public ExBossEnemyServices {
 GameplayHostServices& h_;
public:
 explicit GameplayHostFirst(GameplayHostServices& h):h_(h){}
 void kill_enemy(std::uint32_t index)override{h_.resolve_enemy_death(index);}void music(std::uint32_t track,std::uint32_t mode)override{h_.music(track,mode);}
 void title()override{start_stage_title(h_.game.title,h_.game.lifecycle.stage,h_);}void clear(std::int32_t mode)override{clear_projectiles(h_.game.projectiles,mode);}binary_float::Number cosine(std::uint64_t value)override{return h_.math.cosine(value);}
};
}
