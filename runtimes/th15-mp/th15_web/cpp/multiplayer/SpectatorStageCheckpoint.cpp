#include "SpectatorStageCheckpoint.hpp"
#include "../sdl/ApplicationState.hpp"
#include "../game/AnmFile.hpp"
#include "../game/MultiplayerHud.hpp"
#include <algorithm>
#include <unordered_set>

namespace th15::multiplayer {
namespace {
using Writer=Netplay::SpectatorScalars::Writer;
using Reader=Netplay::SpectatorScalars::Reader;
constexpr u32 Magic=0x32564353,RawLimit=8*1024*1024;
void word(std::vector<u8>& out,u32 value){for(unsigned n=0;n<4;n++)out.push_back(u8(value>>(n*8)));}
u32 word(const u8* p){u32 value=0;for(unsigned n=0;n<4;n++)value|=u32(p[n])<<(n*8);return value;}
// Native file codecs have large fixed, mostly zero-filled records. Remove
// zeros with one 64-bit presence mask per block before the existing LZSS
// codec, keeping three-player stage capsules within the relay's 64 KiB frame.
std::vector<u8> sparse_pack(const std::vector<u8>& raw){
 std::vector<u8> packed;packed.reserve(raw.size());
 for(size_t at=0;at<raw.size();at+=64){
  const size_t mask=packed.size(),count=std::min(size_t(64),raw.size()-at);packed.resize(mask+8,0);
  for(size_t n=0;n<count;n++)if(raw[at+n]){packed[mask+n/8]|=u8(1u<<(n%8));packed.push_back(raw[at+n]);}
 }
 return packed;
}
bool sparse_unpack(const std::vector<u8>& packed,u32 size,std::vector<u8>& raw){
 std::vector<u8> next(size,0);size_t read=0;
 for(size_t at=0;at<size;at+=64){
  if(packed.size()-read<8)return false;const size_t mask=read,count=std::min(size_t(64),size-at);read+=8;
  for(size_t n=0;n<64;n++)if(packed[mask+n/8]&(1u<<(n%8))){
   if(n>=count||read==packed.size()||!packed[read])return false;next[at+n]=packed[read++];
  }
 }
 if(read!=packed.size())return false;raw=std::move(next);return true;
}
bool part(std::vector<u8>& out,const std::vector<u8>& bytes){if(bytes.size()>RawLimit||out.size()+4+bytes.size()>RawLimit)return false;word(out,u32(bytes.size()));out.insert(out.end(),bytes.begin(),bytes.end());return true;}
struct Parts {
 const std::vector<u8>& bytes;size_t at=0;
 bool next(std::vector<u8>& out){if(bytes.size()-at<4)return false;const u32 size=word(bytes.data()+at);at+=4;if(size>bytes.size()-at)return false;out.assign(bytes.begin()+at,bytes.begin()+at+size);at+=size;return true;}
 bool done()const{return at==bytes.size();}
};
template<class C> bool timer(C& c,Timer& v){return c.Values(v.previous,v.current,v.fractional,v.rate_index,v.flags);}
template<class C> bool input(C& c,GameInput& v){return c.Values(v.held,v.previous,v.repeated,v.pressed,v.released,v.long_held)&&c.Array(v.repeat_age)&&c.Array(v.duration);}
template<class C> bool hud_scalars(C& c,GameHud& h){
 if(!c.Values(h.flags,h.last_countdown,h.displayed_score,h.score_increment,h.clear_bonus,h.result_grazes,h.result_deaths,h.boss_state,h.spell_state,h.tutorial_state,h.collection_state)||!timer(c,h.intro_age)||!timer(c,h.frame_age)||!c.Values(h.result.remaining,h.result.initial,h.result.total,h.result.current,h.result.increment,h.result.limit,h.boss.displayed_segments))return false;
 for(auto& track:h.boss.health){if(!c.Value(track.reserved))return false;for(auto& s:track.segments)if(!c.Values(s.fraction,s.color))return false;}
 for(auto& b:h.boss_animations)if(!c.Values(b.created,b.dimmed))return false;
 return true;
}
template<class C> bool spell_scalars(C& c,GameBattle& b){
 auto& s=b.spell_card;auto& clock=s.clock;
 return c.Values(b.spell.flags,b.spell.frame,b.spell.bonus,s.identifier,s.maximum_bonus,s.duration,s.active_frames,s.replay,s.anchor.x,s.anchor.y,s.anchor.z,clock.sequence,clock.completed_frames,clock.encoded,clock.origin)&&timer(c,s.age);
}
template<class F> bool hud_handles(GameHud& h,F f){
 for(auto& handle:h.life_icons)if(!f(handle))return false;for(auto& handle:h.bomb_icons)if(!f(handle))return false;for(auto& handle:h.boss_icons)if(!f(handle))return false;
 for(auto* handle:{&h.root,&h.pointdevice,&h.background_notice,&h.mode_notice,&h.chapter_notice,&h.difficulty_notice,&h.difficulty_label,&h.retry_intro,&h.boss_banner,&h.result_notice})if(!f(*handle))return false;
 for(auto& handle:h.bonus_digits)if(!f(handle))return false;for(auto& handle:h.result_banners)if(!f(handle))return false;for(auto& b:h.boss_animations)for(auto& handle:b.handles)if(!f(handle))return false;for(auto& handle:h.boss_stars)if(!f(handle))return false;return true;
}
std::vector<u32> retained_stage_widgets(sdl::ApplicationState& app,const std::vector<u32>& widgets){
 auto& registry=app.animations.registry;std::unordered_set<const AnmVm*> owned,children;
 auto mark=[&](auto&& self,const AnmVm* vm)->void{if(!vm||!owned.insert(vm).second)return;for(auto* child:registry.children(*vm))self(self,child);};
 hud_handles(app.scene()->hud,[&](u32& handle){mark(mark,registry.find(handle));return true;});
 for(auto handle:widgets)mark(mark,registry.find(handle));
 for(bool alternate:{false,true})for(u32 n=0;n<registry.ordered_count(alternate);n++)if(auto* vm=registry.find(registry.ordered_handle(alternate,n)))for(auto* child:registry.children(*vm))children.insert(child);
 auto* root=registry.find(app.scene()->hud.root);std::vector<u32> result;if(!root)return result;
 const auto* logo=app.animations.resource(app.assets()->logo);
 for(bool alternate:{false,true})for(u32 n=0;n<registry.ordered_count(alternate);n++){
  const u32 handle=registry.ordered_handle(alternate,n);auto* vm=registry.find(handle);
  if(vm&&(vm->resource==root->resource||vm->resource==logo)&&!owned.count(vm)&&!children.count(vm)&&(vm->visual.render_flags>>5&3)!=1)result.push_back(handle);
 }
 return result;
}
// Only neutral object checkpoint owners are used. ChapterCheckpoint would
// apply retry penalties and mutate score/progress on both capture and restore.
struct Objects {
 StageGameplay& scene;AnmManager& animations;AnmCheckpoint pool;
 EnemyCheckpoint enemies;StageCheckpoint background;BulletCheckpoint bullets;ItemCheckpoint items;EffectCheckpoint effects;
 std::array<std::unique_ptr<PlayerCheckpoint>,3> players;
 Objects(sdl::ApplicationState& app):scene(*app.scene()),animations(app.animations),enemies(*scene.battle.enemies,animations,pool,app.assets()->enemy_banks),background(scene.background),bullets(*scene.battle.bullet_scene,animations,pool,app.assets()->bullet),items(*scene.battle.items),effects(scene.battle.effects,animations.registry,pool){for(unsigned n=0;n<scene.battle.mp_options.seat_count;n++)players[n]=std::make_unique<PlayerCheckpoint>(*scene.battle.pilots[n]->player,animations.registry,pool);}
};
bool root_write(const AnmVm& vm,AnmManager& manager,std::vector<u8>& out,std::string& error){
 AnmFile codec;AnmFile::Block block;if(!codec.write(vm,manager,block,true,true)){error=codec.error;return false;}out.assign(block.begin(),block.end());if(!codec.write_geometry(vm,manager,out,{})){error=codec.error;return false;}return true;
}
bool root_read(AnmVm& vm,AnmManager& manager,const std::vector<u8>& bytes,std::string& error){
 AnmFile codec;AnmVm next;u32 used=0;if(bytes.size()<0x608||!codec.read(bytes.data(),u32(bytes.size()),next,manager,true,false)||!codec.read_geometry(bytes.data(),bytes.data()+0x608,u32(bytes.size()-0x608),next,manager,used,{})||used+0x608!=bytes.size()){error=codec.error.empty()?"Invalid spectator player animation":codec.error;return false;}vm.copy_checkpoint(next);return true;
}
bool bank_write(AnmManager& manager,std::vector<u8>& out){
 out.clear();std::vector<std::pair<i32,std::string>> names(manager.resource_names.begin(),manager.resource_names.end());std::sort(names.begin(),names.end());word(out,u32(names.size()));for(const auto& entry:names){if(entry.second.empty()||entry.second.size()>1024)return false;word(out,u32(entry.first));word(out,u32(entry.second.size()));out.insert(out.end(),entry.second.begin(),entry.second.end());}return true;
}
bool bank_read(AnmManager& manager,const std::vector<u8>& in,std::unordered_map<i32,i32>& mapping){
 if(in.size()<4)return false;const u32 count=word(in.data());if(count>4096)return false;size_t at=4;
 for(u32 n=0;n<count;n++){if(in.size()-at<8)return false;const i32 id=signed_bits(word(in.data()+at));const u32 length=word(in.data()+at+4);at+=8;if(id<0||!length||length>1024||length>in.size()-at||mapping.count(id))return false;const std::string name(reinterpret_cast<const char*>(in.data()+at),length);at+=length;if(name.find(char(0))!=std::string::npos)return false;i32 target=-1;for(const auto& entry:manager.resource_names)if(entry.second==name){target=entry.first;break;}mapping.emplace(id,target);}
 return at==in.size();
}
}

bool SpectatorStageCheckpoint::Boundary(const sdl::ApplicationState& app){
 const auto* scene=app.scene();if(!scene||!scene->spectator_ready()||!app.session||!app.session->driver()||!app.display||!app.display->game||!app.run||app.run->previous_scene()||app.loading_waiting()||app.pending_load||app.pause&&app.pause->state.screen!=PauseScreen::Inactive)return false;
 const auto& battle=scene->battle;
 // These owners are deliberately reset by native stage activation. A Boss
 // checkpoint or a general arbitrary-frame snapshot is outside this recipe.
 if(!battle.mp_enabled||battle.laser_scene->manager.count()||(battle.spell.flags&1)||battle.mp_bomb_active()||scene->messages.active())return false;
 return app.progress.stage_frame>0&&app.progress.stage_frame<=4;
}
bool SpectatorStageCheckpoint::Capture(sdl::ApplicationState& app,const SessionSetup& setup,const GameInput& menu,u32 frame,std::vector<u8>& out,std::string& error){
 error.clear();out.clear();if(!Boundary(app)){error="TH15 native stage checkpoint boundary is not ready";return false;}
 auto& scene=*app.scene();auto& battle=scene.battle;SpectatorStageState state;state.setup=setup;state.progress=app.progress;state.score=battle.score;state.enemy=battle.enemy_world;state.game_random=app.environment.game_rng;state.visual_random=app.visual;state.menu=menu;state.frame=frame;state.application_frames=app.frames;state.fragments=battle.mp_fragments;
 for(unsigned n=0;n<setup.player_count;n++)CaptureSpectatorPilot(*battle.pilots[n],state.pilots[n]);
 std::vector<u8> raw,bytes;if(!EncodeSpectatorStageState(state,bytes)||!part(raw,bytes)||!bank_write(app.animations,bytes)||!part(raw,bytes)){error="TH15 spectator scalar/resource codec failed";return false;}
 Objects objects(app);
 auto save=[&](auto& owner,auto write){if(!owner.capture()||!write()){error=owner.error;return false;}return part(raw,bytes);};
 if(!save(objects.background,[&]{return objects.background.write_file(bytes);})||!save(objects.enemies,[&]{return objects.enemies.write_file(bytes);})||!save(objects.bullets,[&]{return objects.bullets.write_file(bytes);})||!save(objects.items,[&]{return objects.items.write_file(bytes,app.animations);})||!save(objects.effects,[&]{return objects.effects.write_file(bytes,app.animations);}))return false;
 for(unsigned n=0;n<setup.player_count;n++){if(!save(*objects.players[n],[&]{return objects.players[n]->write_file(bytes,app.animations);})||!root_write(battle.pilots[n]->player->visuals.root,app.animations,bytes,error)||!part(raw,bytes))return false;}
 // HUD handles include the active native ANM trees. They are reattached to
 // the receiving HUD; file bank numbers resolve through resource names.
 Writer hud;auto& runtime=app.session->driver()->runtime;auto& background=scene.background;auto& draw=app.display->game->background;
 auto session_input=app.session->driver()->controls();
 if(!hud_scalars(hud,scene.hud)||!timer(hud,runtime.age)||!hud.Values(runtime.ending_frames,runtime.requested_chapter,runtime.frame_skip,runtime.sampling_mode,app.current_destination,background.color,background.frames,background.draw_objects,background.deforming,background.rate,draw.state.flags,draw.state.tint_color)||!timer(hud,draw.state.transition)||!hud.Values(draw.rate,battle.effects.cursor,battle.items->requests,battle.items->cancel_density,battle.items->cancel_stamp,battle.items->alternating_pieces,battle.items->active_count,battle.items->motion_scale)||!input(hud,session_input)){error="TH15 spectator scene scalars exceed bounds";return false;}
 if(!hud.Value(u32(background.object_flags.size())))return false;for(auto v:background.object_flags)if(!hud.Value(v))return false;if(!hud.Value(u32(background.instance_flags.size())))return false;for(auto v:background.instance_flags)if(!hud.Value(v))return false;
 if(!spell_scalars(hud,battle)||!part(raw,hud.Bytes()))return false;
 Writer damage;for(unsigned n=0;n<setup.player_count;n++)if(!damage.Values(battle.pilots[n]->player->damage.maximum_damage,battle.pilots[n]->player->damage.score))return false;
 if(!part(raw,damage.Bytes()))return false;
 bytes.clear();
 if(!hud_handles(scene.hud,[&](u32& handle){std::vector<u8> tree;if(auto* vm=app.animations.registry.find(handle)){const auto saved=objects.pool.capture(vm,app.animations.registry);if(!saved||!objects.pool.write_tree(saved,app.animations,tree)){error=objects.pool.error;return false;}}return part(bytes,tree);})||!part(raw,bytes))return false;
 // The additional seats own live native HUD animation widgets created by
 // GameDraw. Their timers and aliases survive a restore just like retail HUD.
 auto* mp_hud=app.display->game->multiplayer_hud.get();
 if(!mp_hud||mp_hud->widgets.size()>128){error="TH15 multiplayer HUD checkpoint is unavailable";return false;}
 Writer mp_values;if(!mp_values.Value(u32(mp_hud->widgets.size())))return false;
 for(auto& seat:mp_hud->displayed)if(!mp_values.Array(seat))return false;
 for(auto* group:{&mp_hud->lives,&mp_hud->bombs})for(auto& seat:*group)for(auto handle:seat){
  const auto at=std::find(mp_hud->widgets.begin(),mp_hud->widgets.end(),handle);
  const u32 index=handle&&at!=mp_hud->widgets.end()?u32(at-mp_hud->widgets.begin()+1):0;
  if(!mp_values.Value(index))return false;
 }
 if(!part(raw,mp_values.Bytes()))return false;bytes.clear();
 for(auto handle:mp_hud->widgets){std::vector<u8> tree;const auto saved=objects.pool.capture(handle,app.animations.registry);if(!saved||!objects.pool.write_tree(saved,app.animations,tree)||!part(bytes,tree)){error=objects.pool.error;return false;}}
 if(!part(raw,bytes)||!root_write(mp_hud->transfer_power_icon,app.animations,bytes,error)||!part(raw,bytes))return false;
 if(battle.spell_card.name.size()>1024)return false;bytes.assign(battle.spell_card.name.begin(),battle.spell_card.name.end());if(!part(raw,bytes))return false;
 // Stage logos and replaced HUD widgets run without a current HUD handle.
 // Retain their exact native timers, including the first logo tick already
 // executed before the safe checkpoint, rather than recreating them at age 0.
 const auto retained=retained_stage_widgets(app,mp_hud->widgets);if(retained.size()>128)return false;
 bytes.clear();word(bytes,u32(retained.size()));for(auto handle:retained){std::vector<u8> tree;const auto saved=objects.pool.capture(handle,app.animations.registry);if(!saved||!objects.pool.write_tree(saved,app.animations,tree)||!part(bytes,tree))return false;}if(!part(raw,bytes))return false;
 const auto sparse=sparse_pack(raw);auto codec=std::make_unique<Lzss>();auto compressed=codec->encode(sparse.data(),u32(sparse.size()));
 if(compressed.empty()||compressed.size()+12>Netplay::SpectatorStageMaxStateBytes){error="TH15 stage initializer exceeds spectator wire bound: "+std::to_string(compressed.size()+12);return false;}
 word(out,Magic);word(out,u32(raw.size()));word(out,u32(sparse.size()));out.insert(out.end(),compressed.begin(),compressed.end());return true;
}

bool SpectatorStageCheckpoint::Restore(sdl::ApplicationState& app,const SessionSetup& setup,GameInput& menu,u32 frame,const std::vector<u8>& capsule,std::string& error){
 error.clear();if(capsule.size()<13||capsule.size()>Netplay::SpectatorStageMaxStateBytes||word(capsule.data())!=Magic){error="Invalid TH15 spectator stage header";return false;}
 const u32 raw_size=word(capsule.data()+4),sparse_size=word(capsule.data()+8),masks=((raw_size+63)/64)*8;
 if(!raw_size||raw_size>RawLimit||sparse_size<masks||sparse_size>raw_size+masks){error="Invalid TH15 spectator stage expansion";return false;}
 std::vector<u8> raw,sparse(sparse_size),bytes;auto decoder=std::make_unique<Lzss>();u32 expanded=0;
 if(!decoder->decode(capsule.data()+12,u32(capsule.size()-12),sparse.data(),sparse_size,expanded)||expanded!=sparse_size||!sparse_unpack(sparse,raw_size,raw)){error="Invalid TH15 spectator stage compression";return false;}
 Parts parts{raw};SpectatorStageState state;if(!parts.next(bytes)||!DecodeSpectatorStageState(bytes,state)||state.frame!=frame||state.setup.session_id!=setup.session_id||GameplayAbi(state.setup)!=GameplayAbi(setup)||state.setup.seed!=setup.seed){error="TH15 spectator checkpoint identity differs";return false;}
 const auto& saved=state.progress;
 app.selected_stage=saved.stage;app.progress=saved;app.progress.new_run=true;app.selection_player=state.pilots[0].resources;app.selection_score=state.score;app.environment.game_rng=state.game_random;app.visual=state.visual_random;
 app.mp_options.stage=saved.stage;app.pending_load=0;app.pending_destination=-1;app.current_destination=12;app.exiting=false;app.failure.clear();app.files.read_only=true;
 if(!app.begin_run()||!app.scene()||!app.session||!app.session->driver()){error=app.failure.empty()?"TH15 spectator stage construction failed":app.failure;return false;}
 auto& scene=*app.scene();auto& battle=scene.battle;
 if(!scene.initialize_background(app.camera)||!scene.reset_for_entry()||!scene.start_for_entry(12)){error=scene.error;return false;}
 app.pending_load=0;app.pending_destination=-1;app.run->release_previous();app.display->release_previous();
 std::unordered_map<i32,i32> mapping;if(!parts.next(bytes)||!bank_read(app.animations,bytes,mapping)){error="TH15 spectator animation resources differ";return false;}
 struct BankGuard{AnmManager& manager;const std::unordered_map<i32,i32>* previous;~BankGuard(){manager.checkpoint_banks=previous;}} bank_guard{app.animations,app.animations.checkpoint_banks};app.animations.checkpoint_banks=&mapping;
 Objects objects(app);u32 used=0;
 auto read=[&](auto& owner,auto parse){if(!parts.next(bytes)||!parse()||used!=bytes.size()){error=owner.error.empty()?"Invalid TH15 spectator object section":owner.error;return false;}return true;};
 if(!read(objects.background,[&]{return objects.background.read_file(bytes.data(),u32(bytes.size()),used);})||!read(objects.enemies,[&]{return objects.enemies.read_file(bytes.data(),u32(bytes.size()),used);})||!read(objects.bullets,[&]{return objects.bullets.read_file(bytes.data(),u32(bytes.size()),used);})||!read(objects.items,[&]{return objects.items.read_file(bytes.data(),u32(bytes.size()),used,app.animations);})||!read(objects.effects,[&]{return objects.effects.read_file(bytes.data(),u32(bytes.size()),used,app.animations);}))return false;
 // Install resources first so the native option restore chooses the saved
 // power/loadout rather than constructing a second set of option callbacks.
 for(unsigned n=0;n<setup.player_count;n++)battle.pilots[n]->session=state.pilots[n].resources;
 for(unsigned n=0;n<setup.player_count;n++){if(!read(*objects.players[n],[&]{return objects.players[n]->read_file(bytes.data(),u32(bytes.size()),used,app.animations);})||!parts.next(bytes)||!root_read(battle.pilots[n]->player->visuals.root,app.animations,bytes,error))return false;}
 if(!objects.background.restore()||!objects.enemies.restore()||!objects.bullets.restore()||!objects.items.restore()||!objects.effects.restore()){error="TH15 spectator native object restoration failed";return false;}
 for(unsigned n=0;n<setup.player_count;n++)if(!objects.players[n]->restore()||!RestoreSpectatorPilot(state.pilots[n],*battle.pilots[n])){error=objects.players[n]->error;return false;}
 if(!parts.next(bytes)){error="Missing TH15 spectator scene scalars";return false;}Reader hud(bytes);auto& runtime=app.session->driver()->runtime;auto& background=scene.background;auto& draw=app.display->game->background;GameInput session_input;
 if(!hud_scalars(hud,scene.hud)||!timer(hud,runtime.age)||!hud.Values(runtime.ending_frames,runtime.requested_chapter,runtime.frame_skip,runtime.sampling_mode,app.current_destination,background.color,background.frames,background.draw_objects,background.deforming,background.rate,draw.state.flags,draw.state.tint_color)||!timer(hud,draw.state.transition)||!hud.Values(draw.rate,battle.effects.cursor,battle.items->requests,battle.items->cancel_density,battle.items->cancel_stamp,battle.items->alternating_pieces,battle.items->active_count,battle.items->motion_scale)||!input(hud,session_input)){error="Invalid TH15 spectator scene scalars";return false;}
 u32 count=0;if(!hud.Value(count)||count!=background.object_flags.size())return false;for(auto& v:background.object_flags)if(!hud.Value(v))return false;if(!hud.Value(count)||count!=background.instance_flags.size())return false;for(auto& v:background.instance_flags)if(!hud.Value(v))return false;if(!spell_scalars(hud,battle)||!hud.Done())return false;
 app.session->driver()->spectator_input(session_input);
 if(!parts.next(bytes))return false;Reader damage(bytes);for(unsigned n=0;n<setup.player_count;n++)if(!damage.Values(battle.pilots[n]->player->damage.maximum_damage,battle.pilots[n]->player->damage.score))return false;if(!damage.Done())return false;
 if(!parts.next(bytes))return false;Parts trees{bytes};std::vector<u8> tree;
 if(!hud_handles(scene.hud,[&](u32& handle){if(!trees.next(tree))return false;if(auto* vm=app.animations.registry.find(handle))if(!app.animations.registry.destroy_tree(*vm))return false;handle=0;if(tree.empty())return true;u32 consumed=0;const auto saved=objects.pool.read_tree(tree.data(),u32(tree.size()),consumed,app.animations);if(!saved||consumed!=tree.size())return false;handle=objects.pool.restore(saved,app.animations.registry);return handle!=0;})||!trees.done()){error="Invalid TH15 spectator HUD animation trees";return false;}
 if(!app.display->game->prepare_multiplayer_hud()||!app.display->game->multiplayer_hud||!parts.next(bytes)){error="TH15 multiplayer HUD construction failed";return false;}
 auto& mp_hud=*app.display->game->multiplayer_hud;Reader mp_values(bytes);u32 widget_count=0;
 if(!mp_values.Value(widget_count)||widget_count>128||widget_count!=mp_hud.widgets.size())return false;
 for(auto& seat:mp_hud.displayed)if(!mp_values.Array(seat))return false;
 std::array<std::array<std::array<u32,8>,3>,2> aliases{};
 for(auto& group:aliases)for(auto& seat:group)for(auto& index:seat)if(!mp_values.Value(index)||index>widget_count)return false;
 if(!mp_values.Done()||!parts.next(bytes))return false;Parts widgets{bytes};
 for(auto& handle:mp_hud.widgets){
  if(!widgets.next(tree)||tree.empty())return false;
  if(auto* vm=app.animations.registry.find(handle))if(!app.animations.registry.destroy_tree(*vm))return false;
  u32 consumed=0;const auto saved=objects.pool.read_tree(tree.data(),u32(tree.size()),consumed,app.animations);
  if(!saved||consumed!=tree.size()||!(handle=objects.pool.restore(saved,app.animations.registry)))return false;
 }
 if(!widgets.done())return false;
 for(unsigned kind=0;kind<2;kind++)for(unsigned seat=0;seat<3;seat++)for(unsigned n=0;n<8;n++){
  const auto index=aliases[kind][seat][n];(kind?mp_hud.bombs:mp_hud.lives)[seat][n]=index?mp_hud.widgets[index-1]:0;
 }
 if(!parts.next(bytes)||!root_read(mp_hud.transfer_power_icon,app.animations,bytes,error)||!parts.next(bytes)||bytes.size()>1024){error="Invalid TH15 multiplayer HUD checkpoint";return false;}
 battle.spell_card.name.assign(bytes.begin(),bytes.end());
 for(auto handle:retained_stage_widgets(app,mp_hud.widgets))if(auto* vm=app.animations.registry.find(handle))if(!app.animations.registry.destroy_tree(*vm))return false;
 if(!parts.next(bytes)||bytes.size()<4||word(bytes.data())>128)return false;
 const u32 retained_count=word(bytes.data());Parts retained{bytes,4};
 for(u32 n=0;n<retained_count;n++){if(!retained.next(tree)||tree.empty())return false;u32 consumed=0;const auto saved=objects.pool.read_tree(tree.data(),u32(tree.size()),consumed,app.animations);if(!saved||consumed!=tree.size()||!objects.pool.restore(saved,app.animations.registry))return false;}
 if(!retained.done()||!parts.done())return false;
 app.progress=saved;battle.score=state.score;battle.mp_fragments=state.fragments;menu=state.menu;app.frames=state.application_frames;app.environment.game_rng=state.game_random;app.visual=state.visual_random;
 // Unloading the previous run marks its pooled animations for retirement.
 // Complete that disposal without ticking ANM, the scheduler, or either RNG.
 for(bool alternate:{false,true}){
  std::vector<u32> retired;auto& registry=app.animations.registry;
  for(u32 n=0;n<registry.ordered_count(alternate);n++){const u32 handle=registry.ordered_handle(alternate,n);if(auto* vm=registry.find(handle))if((vm->visual.render_flags>>5&3)==1)retired.push_back(handle);}
  for(auto handle:retired)if(auto* vm=registry.find(handle))if(!registry.destroy_tree(*vm)){error=registry.error;return false;}
 }app.animations.collect_resources();
 // The decoded EnemyWorldState contains scalar fields only. Keep the new
 // manager's vectors, callbacks and chapter owner when applying those values.
 return RestoreSpectatorEnemyScalars(state.enemy,battle.enemy_world);
}
}
