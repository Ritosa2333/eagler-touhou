#include "../../cpp/game/Archive.hpp"
#include "../../cpp/game/Replay.hpp"
#include "../../cpp/game/ReplayRecorder.hpp"
#include "../../cpp/game/GameInput.hpp"
#include "../../cpp/game/ScoreFile.hpp"
#include "../../cpp/game/MenuCursor.hpp"
#include "../../cpp/game/TitleMenu.hpp"
#include "../../cpp/game/PauseMenu.hpp"
#include "../../cpp/game/Ending.hpp"
#include "../../cpp/game/ResourceCrypt.hpp"
#include "../../cpp/game/Rng.hpp"
#include "../../cpp/game/AnmResource.hpp"
#include "../../cpp/game/Interpolation.hpp"
#include "../../cpp/game/AnmVm.hpp"
#include "../../cpp/game/AnmGeometry.hpp"
#include "../../cpp/game/AnmManager.hpp"
#include "render-fixture.hpp"
#include <cstdlib>
#include "../../cpp/game/EclResource.hpp"
#include "../../cpp/game/GameResources.hpp"
#include "../../cpp/game/GameSessionResources.hpp"
#include "../../cpp/game/GameSession.hpp"
#include "ecl-fixture.hpp"
#include "../../cpp/game/Movement.hpp"
#include "enemy-fixture.hpp"
#include "../../cpp/game/EnemyCommands.hpp"
#include "../../cpp/game/BulletLaunch.hpp"
#include "bullet-fixture.hpp"
#include "bullet-manager-fixture.hpp"
#include "../../cpp/game/PlayerCollision.hpp"
#include "enemy-animation-fixture.hpp"
#include "enemy-frame-fixture.hpp"
#include "enemy-manager-fixture.hpp"
#include "item-manager-fixture.hpp"
#include "../../cpp/game/ShtResource.hpp"
#include "../../cpp/game/ShotSchedule.hpp"
#include "shot-fixture.hpp"
#include "player-motion-fixture.hpp"
#include "player-frame-fixture.hpp"
#include "cancellation-fixture.hpp"
#include "laser-manager-fixture.hpp"
#include "spell-fixture.hpp"
#include "bomb-fixture.hpp"
#include "enemy-callback-fixture.hpp"
#include "../../cpp/game/Stage.hpp"
#include "../../cpp/game/ScreenFade.hpp"
#include "stage-completion-fixture.hpp"
#include "../../cpp/game/FrameStatistics.hpp"
using namespace th11;
#define API(name) extern "C" __attribute__((export_name(#name)))
API(frame_stats_create) FrameStatistics* frame_stats_create(){return new FrameStatistics;}
API(frame_stats_delete) void frame_stats_delete(FrameStatistics* p){delete p;}
API(frame_stats_sample) void frame_stats_sample(FrameStatistics* p,double time,u32 active,u32 ticks){p->sample(time,active,ticks);}
API(frame_stats_values) const double* frame_stats_values(FrameStatistics* p){static double v[7];v[0]=p->window_start;v[1]=p->actual;v[2]=p->nominal;v[3]=p->frames;v[4]=p->fast_windows;v[5]=p->fps;v[6]=p->slowdown();return v;}
API(frame_stats_label) const char* frame_stats_label(FrameStatistics* p,u32* out){static std::string text;auto label=p->label();text=label.text;out[0]=label.style.color;out[1]=label.style.font;out[2]=u32(label.position.x);out[3]=u32(label.position.y);return text.c_str();}
API(replay_create) Replay* replay_create(){return new Replay;}
API(replay_metadata) void replay_metadata(Replay* p){p->retain_metadata();}
API(title_create) TitleMenu* title_create(AnmManager* a,AnmResource* t,AnmResource* v,AnmResource* ascii,ScoreFile* s){return new TitleMenu(*a,*t,*v,*ascii,*s);}
API(title_delete) void title_delete(TitleMenu* p){delete p;}
API(pause_create) PauseMenu* pause_create(AnmManager* a,AnmResource* t,AnmResource* f,ScoreFile* s){return new PauseMenu(*a,*t,*f,*s);}
API(pause_delete) void pause_delete(PauseMenu* p){delete p;}
API(pause_begin) u32 pause_begin(PauseMenu* p,u32 replay){return p->begin(replay);}
API(pause_end) u32 pause_end(PauseMenu* p,u32 practice,u32 complete,i32 score){p->result_score=score;p->timestamp=1600000000;return p->begin_end(practice,complete);}
API(pause_end_value) i32 pause_end_value(PauseMenu* p,u32 n){return n==0?p->unranked:n==1?p->save_requested:p->scan_requested;}
API(pause_update) u32 pause_update(PauseMenu* p,u32 pressed,u32 repeat){return p->update(pressed,repeat);}
API(pause_data) void* pause_data(PauseMenu* p,u32 n){switch(n){case 0:return &p->state;case 1:return &p->timer;case 2:return &p->elapsed;case 3:return &p->cursor;case 4:return &p->menu_animation;case 5:return p->sounds.data();case 6:return &p->forced_exit;default:return nullptr;}}
API(pause_value) i32 pause_value(PauseMenu* p,u32 n){return n==0?p->sounds.size():n==1?i32(p->action):p->capture_requested;}
API(pause_name) void* pause_name(PauseMenu* p,u32 n){return n==0?static_cast<void*>(&p->name_cursor):n==1?static_cast<void*>(p->entered_name.data()):&p->name_length;}
API(ending_create) Ending* ending_create(AnmManager* a,AnmResource* t,ScoreFile* s){return new Ending(*a,*t,*s);}
API(ending_delete) void ending_delete(Ending* p){delete p;}
API(ending_begin) u32 ending_begin(Ending* p,GameResources* r,i32 selection,i32 difficulty,i32 continues){return p->begin(*r,selection,difficulty,continues);}
API(ending_update) u32 ending_update(Ending* p,u32 held,u32 pressed){return p->update(held,pressed);}
API(ending_tick) u32 ending_tick(Ending* p,u32 held,u32 pressed){return p->tick(held,pressed);}
API(ending_value) i32 ending_value(Ending* p,u32 kind){switch(kind){case 0:return p->active;case 1:return p->flags;case 2:return p->line;case 3:return p->color;case 4:return p->instruction-p->messages[p->message].data();case 5:return p->text_requests.size();case 6:return p->music_request;case 7:return p->music_fade;default:return p->sounds.size();}}
API(ending_data) const void* ending_data(Ending* p,u32 k,u32 i){switch(k){case 0:return &p->time;case 1:return &p->wait;case 2:return p->text_lines.data();case 3:return p->images.data();case 4:return &p->text_requests[i];case 5:return p->text_requests[i].bytes.c_str();case 6:return p->error.c_str();default:return p->sounds.data();}}
API(title_error) const char* title_error(TitleMenu* p){return p->error.c_str();}
API(title_update) u32 title_update(TitleMenu* p,u32 pressed,u32 repeat){return p->update(pressed,repeat);}
API(title_data) void* title_data(TitleMenu* p,u32 kind){switch(kind){case 0:return &p->cursor;case 1:return &p->timer;case 2:return &p->selection;case 3:return p->handles.data();case 4:return p->sounds.data();case 5:return p->config.bytes.data();default:return nullptr;}}
API(config_reset) void config_reset(GameConfig* p,const u8* bindings){p->reset(bindings);}
API(config_open) u32 config_open(GameConfig* p,const u8* data,u32 size){return p->open(data,size);}
API(title_value) i32 title_value(TitleMenu* p,u32 kind){switch(kind){case 0:return i32(p->screen);case 1:return p->substate;case 2:return p->flags;case 3:return p->sounds.size();case 4:return p->start_requested;case 5:return p->fade_requested;default:return 0;}}
API(title_state) void title_state(TitleMenu* p,i32 screen,i32 substate,u32 flags){p->change(TitleScreen(screen));p->substate=substate;p->flags=flags;}
API(title_music_load) u32 title_music_load(TitleMenu* p,AnmResource* text,const u8* data,u32 size){p->text_resource=text;return p->load_music_comments(data,size);}
API(title_music_state) void* title_music_state(TitleMenu* p){return &p->music_state;}
API(title_music_request) i32 title_music_request(TitleMenu* p){return p->music_pause?-2:p->music_request;}
API(title_buttons) void title_buttons(TitleMenu* p,u32 buttons){p->controller_buttons=buttons;}
API(title_replay_load) u32 title_replay_load(TitleMenu* p,u32 index,const u8* b,u32 size){if(index>=75)return 0;auto r=std::make_shared<TitleMenu::ReplayEntry>();if(!r->replay.open(b,size))return 0;r->file.assign(b,b+size);p->replay_files[index]=r;return 1;}
API(title_scan_done) void title_scan_done(TitleMenu* p){p->replay_scan_complete();}
API(title_replay_path) void title_replay_path(TitleMenu* p,u32 index,const char* path){if(index<75&&p->replay_files[index])p->replay_files[index]->path=path;}
API(title_page) void* title_page(TitleMenu* p){return &p->page;}
API(title_secondary) void* title_secondary(TitleMenu* p){return &p->secondary;}
API(title_key_edges) void* title_key_edges(TitleMenu* p){return p->key_edges.data();}
API(title_record_value) i32 title_record_value(TitleMenu* p,i32 n){return n==0?p->record_rows:n==1?p->unlock_progress:p->unlock_timeout;}
API(title_result_set) void title_result_set(TitleMenu* p,i32 score,u32 timestamp){p->result_score=score;p->result_timestamp=timestamp;}
API(title_name_data) const void* title_name_data(TitleMenu* p,i32 n){return n==0?static_cast<const void*>(&p->name_cursor):n==1?static_cast<const void*>(p->entered_name.data()):&p->name_length;}
API(title_replay_value) i32 title_replay_value(TitleMenu* p,i32 kind){switch(kind){case 0:return p->last_replay;case 1:return p->replay_file;case 2:return p->replay_stage;case 3:return p->replay_scan_requested;default:return p->replay_start_requested;}}
API(title_text_count) u32 title_text_count(TitleMenu* p){return p->text_requests.size();}
API(title_text_data) const void* title_text_data(TitleMenu* p,u32 i,u32 bytes){if(i>=p->text_requests.size())return nullptr;return bytes?static_cast<const void*>(p->text_requests[i].bytes.c_str()):&p->text_requests[i];}
API(menu_cursor_create) MenuCursor* menu_cursor_create(){return new MenuCursor;}
API(menu_cursor_delete) void menu_cursor_delete(MenuCursor* p){delete p;}
API(menu_cursor_select) i32 menu_cursor_select(MenuCursor* p,i32 value){return p->select(value);}
API(menu_cursor_move) i32 menu_cursor_move(MenuCursor* p,i32 value){return p->move(value);}
API(menu_cursor_push) void menu_cursor_push(MenuCursor* p){p->push();}
API(menu_cursor_pop) void menu_cursor_pop(MenuCursor* p){p->pop();}
API(score_file_create) ScoreFile* score_file_create(){return new ScoreFile;}
API(score_file_delete) void score_file_delete(ScoreFile* p){delete p;}
API(score_file_initialize) void score_file_initialize(ScoreFile* p,Rng* rng){p->initialize(*rng);}
API(score_file_data) u8* score_file_data(ScoreFile* p,u32 index){return index<7?p->characters[index].data():p->settings.data();}
API(score_file_open) u32 score_file_open(ScoreFile* p,const u8* data,u32 n){return p->open(data,n);}
API(score_file_save) u32 score_file_save(ScoreFile* p,u8* out,u32 capacity){std::vector<u8> bytes;if(!p->save(bytes)||bytes.size()>capacity)return 0;std::memcpy(out,bytes.data(),bytes.size());return bytes.size();}
API(score_insert) i32 score_insert(ScoreFile* p,u32 selection,u32 difficulty,i32 score,i32 stage,i32 continues,u32 timestamp,float slowdown){return p->insert_score(selection,difficulty,score,stage,continues,timestamp,slowdown);}
API(game_input_create) GameInput* game_input_create(){return new GameInput;}
API(game_input_delete) void game_input_delete(GameInput* p){delete p;}
API(game_input_tick) void game_input_tick(GameInput* p,u32 keys,u32 auto_focus){p->update(keys,auto_focus);}
API(controller_input) u32 controller_input(u32 held,const u8* buttons,u32 count,i32 x,i32 y,const u8* config){return controller_keys(held,buttons,count,x,y,config);}
API(keyboard_input) u32 keyboard_input(const bool* keys){return keyboard_keys(keys);}
API(recorder_create) ReplayRecorder* recorder_create(){return new ReplayRecorder;}
API(recorder_delete) void recorder_delete(ReplayRecorder* p){delete p;}
API(recorder_begin) u32 recorder_begin(ReplayRecorder* p,i32 character,i32 subtype,i32 difficulty,u32 practice,u32 timestamp){return p->begin(character,subtype,difficulty,practice,timestamp);}
API(recorder_stage) u32 recorder_stage(ReplayRecorder* p,u32 stage,const u8* header,u32 initial){ReplayStageState state;state.read(header);return p->start_stage(stage,state,initial);}
API(recorder_tick) u32 recorder_tick(ReplayRecorder* p,u32 held,u32 pressed,u32 released,float fps){return p->tick(held,pressed,released,fps);}
API(recorder_finish) u32 recorder_finish(ReplayRecorder* p,u32 terminal){return p->finish_stage(terminal);}
API(recorder_header) u8* recorder_header(ReplayRecorder* p,u32 stage){return stage?p->stages[stage].header.data():p->header.data();}
API(recorder_save) u32 recorder_save(ReplayRecorder* p,const char* name,i32 score,i32 reached,i32 continues,float slowdown,u8* out,u32 capacity){std::vector<u8> bytes;if(!p->save(name,score,reached,continues,slowdown,bytes)||bytes.size()>capacity)return 0;std::memcpy(out,bytes.data(),bytes.size());return bytes.size();}
API(replay_delete) void replay_delete(Replay* p){delete p;}
API(replay_open) int replay_open(Replay* p,const u8* data,u32 n){return p->open(data,n);}
API(replay_data) const u8* replay_data(Replay* p){return p->decoded().data();}
API(replay_size) u32 replay_size(Replay* p){return p->decoded().size();}
API(replay_error) const char* replay_error(Replay* p){return p->error().c_str();}
API(replay_stage) const ReplayStage* replay_stage(Replay* p,u32 s){return p->stage(s);}
API(replay_select) int replay_select(Replay* p,u32 s){return p->select(s);}
API(replay_tick) void replay_tick(Replay* p,ReplayInput* out,u32 active){*out=p->tick(active);}
API(replay_restore) void replay_restore(const u8* p,GameEconomy* e,Rng* rng,PlayerMotionState* motion,u32 first){ReplayStageState state;state.read(p);state.restore(*e,*rng,first);state.restore_position(*motion);}
API(codec_encode) u32 codec_encode(const u8* source,u32 size,u8* out,u32 capacity){Lzss codec;auto bytes=codec.encode(source,size);if(bytes.size()>capacity)return 0;std::memcpy(out,bytes.data(),bytes.size());return bytes.size();}
struct PopupFixture{ScorePopups popups;float rate=1;std::vector<ScoreGlyph> glyphs;};
API(popup_create) PopupFixture* popup_create(){return new PopupFixture;}
API(popup_delete) void popup_delete(PopupFixture* p){delete p;}
API(popup_add) void popup_add(PopupFixture* p,i32 value,float x,float y,u32 color){p->popups.add({x,y,0},value,color,&p->rate);}
API(popup_tick) void popup_tick(PopupFixture* p,float rate){p->rate=rate;p->popups.update(rate);}
API(popup_data) ScorePopup* popup_data(PopupFixture* p){return p->popups.entries.data();}
API(popup_glyphs) u32 popup_glyphs(PopupFixture* p,float x,float y){p->glyphs=p->popups.glyphs({x,y,0});return p->glyphs.size();}
API(popup_glyph_data) ScoreGlyph* popup_glyph_data(PopupFixture* p){return p->glyphs.data();}
API(completion_create) test::StageCompletionFixture* completion_create(){return new test::StageCompletionFixture;}
API(completion_delete) void completion_delete(test::StageCompletionFixture* p){delete p;}
API(completion_configure) void completion_configure(test::StageCompletionFixture* p,i32 stage,i32 selection,i32 difficulty,i32 control,i32 replay,u32 flags){p->economy.difficulty=difficulty;p->completion.mode={stage,selection,control,replay,bool(flags&1),bool(flags&2),bool(flags&4)};}
API(completion_data) void* completion_data(test::StageCompletionFixture* p,u32 k){switch(k){case 0:return &p->economy;case 1:return &p->completion.state;case 2:return &p->records;case 3:return &p->rate;default:return p->events.data();}}
API(completion_finish) int completion_finish(test::StageCompletionFixture* p){return p->completion.complete();}
API(completion_tick) void completion_tick(test::StageCompletionFixture* p){p->completion.update();}
API(completion_events) u32 completion_events(test::StageCompletionFixture* p){return p->events.size();}
API(fog_sample) void fog_sample(FogInterpolator* p,SceneFog* out,const float* rate){*out=sample(*p,rate);}
struct StageScriptFixture final:StageScriptWorld {
    StageState state{};StageResource resource;float rate=1;u32 color=0;
    std::vector<i32> events;
    bool stage_animation(AnmVm& vm,i32 script)override{events.push_back(14);events.push_back(i32(&vm-state.script_animations));events.push_back(script);return true;}
    bool stage_deformation(i32 mode)override{events.push_back(17);events.push_back(mode);return true;}
    void stage_color(u32 value)override{color=value;}
};
API(std_create) StageScriptFixture* std_create(){return new StageScriptFixture;}
API(std_delete) void std_delete(StageScriptFixture* f){delete f;}
API(std_load) int std_load(StageScriptFixture* f,const u8* p,u32 n){return f->resource.open(p,n);}
API(std_data) void* std_data(StageScriptFixture* f,u32 k){switch(k){case 0:return &f->state;case 1:return &f->rate;case 2:return &f->color;case 3:return f->events.data();default:return const_cast<char*>(f->resource.error.c_str());}}
API(std_count) u32 std_count(StageScriptFixture* f,u32 k){switch(k){case 0:return f->resource.objects.size();case 1:return f->resource.instances.size();case 2:return f->resource.animation_count;case 3:return f->resource.instructions.size();default:return f->events.size();}}
API(std_tick) int std_tick(StageScriptFixture* f){f->events.clear();return stage_script(f->state,f->resource,&f->rate,*f);}
API(std_interrupt) int std_interrupt(StageScriptFixture* f,i32 id){return stage_interrupt(f->state,f->resource,id,&f->rate);}
struct StageFixture {AnmResource animations,text;AnmManager manager;u32 color=0;Stage stage{manager,animations,text,color,0};test::RecordingGraphics graphics;AnmRenderer renderer{graphics};SceneCamera render_camera;ScreenFades fades;};
API(stage_create) StageFixture* stage_create(){return new StageFixture;}
API(stage_delete) void stage_delete(StageFixture* f){delete f;}
API(stage_data) void* stage_data(StageFixture* f,u32 k){switch(k){case 0:return &f->stage.state;case 1:return &f->manager;case 2:return &f->animations;case 3:return &f->text;case 4:return &f->color;case 5:return f->stage.object_animations.data();default:return const_cast<char*>(f->stage.error.c_str());}}
API(stage_load) int stage_load(StageFixture* f,const u8* b,u32 n,i32 number){StageResource file;if(!file.open(b,n))return 0;f->stage.active_camera=&f->render_camera;return f->stage.initialize(file,number,{});}
API(stage_tick) int stage_tick(StageFixture* f){return f->stage.update();}
API(stage_label) int stage_label(StageFixture* f,i32 id){return f->stage.interrupt(id);}
API(stage_object_flag) u32 stage_object_flag(StageFixture* f,u32 i){return f->stage.resource.objects[i].flags;}
API(stage_mesh_data) void* stage_mesh_data(StageFixture* f,u32 k){auto* mesh=f->stage.mesh.get();if(!mesh)return nullptr;switch(k){case 0:return &mesh->columns;case 1:return mesh->vertices.data();case 2:return mesh->positions.data();default:return mesh->animations.data();}}
API(stage_mesh_copy) void stage_mesh_copy(StageFixture* f){if(f->stage.mesh)f->stage.mesh->copy_strips();}
API(stage_visible_test) int stage_visible_test(const Vec3* object,const Vec3* size,const Vec3* instance,const SceneCamera* camera,float maximum){StageObject value;value.position=*object;value.size=*size;return stage_visible(value,*instance,*camera,maximum);}
API(stage_layer) int stage_layer(StageFixture* f,u32 layer){f->render_camera=f->stage.state.camera;const bool ok=f->stage.draw_layer(f->renderer,f->render_camera,layer);f->renderer.flush();return ok;}
API(stage_render_reset) void stage_render_reset(StageFixture* f){f->renderer.invalidate();f->graphics.data.clear();f->graphics.submissions.clear();f->graphics.snapshots.clear();f->graphics.target_events.clear();f->graphics.initialize_game_pipeline();}
API(stage_render_data) void* stage_render_data(StageFixture* f,u32 k){switch(k){case 0:return f->graphics.data.data();case 1:return f->graphics.submissions.data();case 2:return f->graphics.snapshots.data();case 3:return &f->render_camera;default:return f->graphics.target_events.data();}}
API(stage_render_size) u32 stage_render_size(StageFixture* f,u32 k){return k==0?f->graphics.data.size():k==1?f->graphics.submissions.size():k==2?f->graphics.snapshots.size():f->graphics.target_events.size();}
API(stage_instance_flag) u32 stage_instance_flag(StageFixture* f,u32 i){return f->stage.resource.instances[i].flags;}
API(stage_pass) int stage_pass(StageFixture* f,u32 foreground){const bool ok=foreground?f->stage.draw_foreground(f->renderer,f->render_camera):f->stage.draw_background(f->renderer,f->render_camera,f->fades);f->renderer.flush();return ok;}
API(stage_transition) void stage_transition(StageFixture* f,u32 resume){if(resume)f->stage.resume_background();else f->stage.fade_out(f->fades);}
API(stage_render_value) u32 stage_render_value(StageFixture* f,u32 k){switch(k){case 0:return f->renderer.tint;case 1:return f->renderer.tint_enabled;case 2:return f->graphics.state.depthWrite;case 3:return u32(f->graphics.state.depthCompare);case 4:return f->graphics.state.fog;default:return f->fades.effects.size();}}
struct FadeFixture {float rate=1;ScreenFade fade;test::RecordingGraphics graphics;AnmRenderer renderer{graphics};};
API(fade_create) FadeFixture* fade_create(){return new FadeFixture;}
API(fade_delete) void fade_delete(FadeFixture* f){delete f;}
API(fade_start) void fade_start(FadeFixture* f,i32 type,i32 duration,u32 color,float rate){f->rate=rate;f->fade={};f->fade.type=type;f->fade.duration=duration;f->fade.color=color;f->fade.alpha=type==3?255:0;f->fade.timer.set(0,&f->rate);f->graphics.initialize_game_pipeline();}
API(fade_tick) i32 fade_tick(FadeFixture* f,i32 stopped,i32 frozen){return f->fade.update(stopped,frozen);}
API(fade_data) void* fade_data(FadeFixture* f,u32 k){return k==0?static_cast<void*>(&f->fade.timer):k==1?static_cast<void*>(&f->fade.alpha):f->graphics.data.data();}
API(fade_draw) u32 fade_draw(FadeFixture* f){f->graphics.data.clear();f->fade.draw(f->renderer);return f->graphics.data.size();}
API(bomb_create) test::BombFixture* bomb_create(ShtResource* s,AnmResource* p,AnmResource* b,AnmManager* a){return new test::BombFixture(*s,*p,*b,*a);}
API(bomb_delete) void bomb_delete(test::BombFixture* p){delete p;}
API(bomb_player) test::PlayerFrameFixture* bomb_player(test::BombFixture* p){return &p->frame;}
API(bomb_state) BombState* bomb_state(test::BombFixture* p){return &p->controller.state;}
API(bomb_select) void bomb_select(test::BombFixture* p,i32 selection){p->controller.selection=selection;}
API(bomb_action) i32 bomb_action(test::BombFixture* p,u32 action){p->events.clear();return action?p->controller.update():p->controller.start();}
API(bomb_events) void* bomb_events(test::BombFixture* p){return p->events.data();}
API(bomb_value) i32 bomb_value(test::BombFixture* p,u32 kind){return kind==0?p->events.size():kind==1?p->count:kind==3?p->background_color:p->controller.last_error;}
API(bomb_damage) i32 bomb_damage(test::BombFixture* p,const Vec3* pos){i32 damage;return p->controller.damage(*pos,{},damage)?damage:-2;}
API(bomb_damage_box) i32 bomb_damage_box(test::BombFixture* p,const Vec3* pos,const Vec2* size){i32 damage;return p->controller.damage(*pos,*size,damage)?damage:-2;}
API(bomb_text) AnmResource* bomb_text(test::BombFixture* p){return &p->text;}
API(bomb_mesh) const void* bomb_mesh(test::BombFixture* p,u32 k){const auto* mesh=p->controller.distortion();if(!mesh)return nullptr;switch(k){case 0:return &mesh->columns;case 1:return mesh->vertices.data();case 2:return mesh->positions.data();default:return mesh->animations.data();}}
API(bomb_mesh_copy) void bomb_mesh_copy(test::BombFixture* p){if(auto* mesh=const_cast<ScreenDeformation*>(p->controller.distortion()))mesh->copy_strips();}
struct CompositorFixture {AnmResource text;AnmManager manager;SceneCompositor compositor{manager};test::RecordingGraphics graphics;AnmRenderer renderer{graphics};};
API(comp_create) CompositorFixture* comp_create(){return new CompositorFixture;}
API(comp_delete) void comp_delete(CompositorFixture* f){delete f;}
API(comp_load) int comp_load(CompositorFixture* f,const u8* b,u32 n){return f->text.open(b,n)&&f->compositor.initialize(f->text);}
API(comp_data) void* comp_data(CompositorFixture* f,u32 k){switch(k){case 0:return f->compositor.copies;case 1:return f->graphics.target_events.data();case 2:return f->graphics.data.data();case 3:return f->graphics.submissions.data();case 4:return &f->compositor.clear_color;case 5:return &f->compositor.region;default:return &f->manager;}}
API(comp_size) u32 comp_size(CompositorFixture* f,u32 k){return k==0?f->graphics.target_events.size():k==1?f->graphics.data.size():f->graphics.submissions.size();}
API(comp_stage) int comp_stage(CompositorFixture* f){return f->compositor.stage_start();}
API(comp_reset) void comp_reset(CompositorFixture* f){f->renderer.invalidate();f->graphics.data.clear();f->graphics.target_events.clear();f->graphics.submissions.clear();f->graphics.target=0;}
API(comp_pass) int comp_pass(CompositorFixture* f,u32 kind){return f->compositor.draw(f->renderer,SceneDrawKind(kind));}
API(scene_camera_update) void scene_camera_update(SceneCamera* p,u32 screen){if(screen)p->screen();else p->perspective();}
API(comp_camera) SceneCamera* comp_camera(CompositorFixture* f,u32 index){return index==0?&f->compositor.play_camera:index==1?&f->compositor.full_camera:&f->compositor.world_camera;}
API(comp_cameras_reset) void comp_cameras_reset(CompositorFixture* f,u32 restricted){f->compositor.reset_cameras(restricted);}
#include "../../cpp/game/ScreenDeformation.hpp"
struct DeformationFixture {AnmResource text;AnmManager manager;EnemyState enemy{};ScreenDeformation mesh{manager};};
API(deform_create) DeformationFixture* deform_create(){return new DeformationFixture;}
API(deform_delete) void deform_delete(DeformationFixture* f){delete f;}
API(deform_data) void* deform_data(DeformationFixture* f,u32 n){switch(n){case 0:return &f->enemy;case 1:return f->mesh.vertices.data();case 2:return f->mesh.positions.data();case 3:return f->mesh.animations.data();case 4:return &f->manager;default:return &f->text;}}
API(deform_initialize) bool deform_initialize(DeformationFixture* f){return f->mesh.initialize(f->text);}
API(deform_update) void deform_update(DeformationFixture* f,i32 spell){f->mesh.update(f->enemy,spell);}
API(deform_copy) void deform_copy(DeformationFixture* f){f->mesh.copy_strips();}
#include "dialogue-fixture.hpp"
API(dialog_create) test::DialogueFixture* dialog_create(){return new test::DialogueFixture;}
API(dialog_delete) void dialog_delete(test::DialogueFixture* p){delete p;}
API(dialog_resource) AnmResource* dialog_resource(test::DialogueFixture* p,u32 n){return &p->resources[n];}
API(dialog_manager) AnmManager* dialog_manager(test::DialogueFixture* p){return &p->manager;}
API(dialog_data) void* dialog_data(test::DialogueFixture* p,u32 n){if(n==0)return &p->dialogue.state;if(n==1)return p->message.data();if(n==2)return p->events.data();return const_cast<char*>(p->dialogue.error.c_str());}
API(dialog_load) void dialog_load(test::DialogueFixture* p,const u8* b,u32 n){p->message.assign(b,b+n);}
API(dialog_begin) u32 dialog_begin(test::DialogueFixture* p,i32 id,i32 character,i32 stage){p->events.clear();p->dialogue.character=character;p->dialogue.stage=stage;return p->dialogue.begin(p->message,id);}
API(dialog_tick) i32 dialog_tick(test::DialogueFixture* p,u32 held,u32 pressed){p->events.clear();return p->dialogue.update(held,pressed);}
API(dialog_events) u32 dialog_events(test::DialogueFixture* p){return p->events.size();}
API(allocate) void* allocate(u32 n){return std::malloc(n);}
API(release) void release(void* p){std::free(p);}
struct ShakeFixture {float rate=1;Rng rng;ScreenShakes shakes;};
API(shake_create) ShakeFixture* shake_create(){return new ShakeFixture;}
API(shake_delete) void shake_delete(ShakeFixture* p){delete p;}
API(shake_data) void* shake_data(ShakeFixture* p,u32 k){switch(k){case 0:return &p->rate;case 1:return &p->rng;default:return &p->shakes.offset;}}
API(shake_start) void shake_start(ShakeFixture* p,i32 duration,i32 from,i32 to){p->shakes.start(duration,from,to,&p->rate);}
API(shake_tick) void shake_tick(ShakeFixture* p,u32 stopped){p->shakes.update(p->rng,stopped);}
API(shake_count) u32 shake_count(ShakeFixture* p){return p->shakes.effects.size();}
API(shake_timer) Timer* shake_timer(ShakeFixture* p,u32 index){return &p->shakes.effects[index].timer;}
API(spell_create) test::SpellFixture* spell_create(){return new test::SpellFixture;}
API(spell_delete) void spell_delete(test::SpellFixture* p){delete p;}
API(spell_data) void* spell_data(test::SpellFixture* p,u32 k){switch(k){case 0:return &p->rate;case 1:return &p->controller.timer;case 2:return &p->spell_flags;case 3:return &p->spell_bonus;case 4:return &p->controller.initial_bonus;case 5:return &p->controller.circle_position;case 6:return &p->economy;case 7:return p->controller.name.data();default:return &p->stage_flags;}}
API(spell_select) void spell_select(test::SpellFixture* p,i32 shot,i32 stage,i32 difficulty,u32 replay){p->controller.selection=shot;p->controller.stage=stage;p->economy.difficulty=difficulty;p->controller.replay=replay;}
API(spell_begin) int spell_begin(test::SpellFixture* p,i32 id,i32 timeout,const char* name,u32 bomb,float x,float y,float z){p->events.clear();return p->controller.begin(id,timeout,name,{x,y,z},bomb);}
API(spell_tick) int spell_tick(test::SpellFixture* p,float y,float x,float by,float z,u32 bomb){p->events.clear();return p->controller.update(y,{x,by,z},bomb);}
API(spell_end) int spell_end(test::SpellFixture* p){p->events.clear();return p->controller.end();}
API(spell_hide_circle) void spell_hide_circle(test::SpellFixture* p){p->controller.hide_circle();}
API(spell_timing_create) SpellTiming* spell_timing_create(){return new SpellTiming;}
API(spell_timing_delete) void spell_timing_delete(SpellTiming* p){delete p;}
API(spell_timing_update) u32 spell_timing_update(SpellTiming* p,u32 flags,i32 frames,double now,u32 replay){return p->update(flags,frames,now,replay)?flags:0xffffffff;}
API(spell_timing_data) void* spell_timing_data(SpellTiming* p,u32 k){switch(k){case 0:return &p->start;case 1:return &p->last_frames;case 2:return &p->encoded;case 3:return &p->index;default:return p->records.data();}}
API(spell_value) i32 spell_value(test::SpellFixture* p,u32 k){switch(k){case 0:return p->spell_id;case 1:return p->spell_elapsed;case 2:return p->controller.timeout;case 3:return p->controller.frame_count;case 4:return p->events.size();default:return 0;}}
API(spell_record) SpellRecord* spell_record(test::SpellFixture* p,u32 shot,u32 id){return &p->records.entries[shot][id];}
API(spell_event) i32 spell_event(test::SpellFixture* p,u32 index){return p->events[index];}
API(resources_create) GameResources* resources_create(){return new GameResources;}
API(resources_delete) void resources_delete(GameResources* p){delete p;}
API(resources_open) int resources_open(GameResources* p,const u8* bytes,u32 size){return p->open_archive(bytes,size);}
API(resources_count) u32 resources_count(GameResources* p){return p->resource_count();}
API(resources_has) int resources_has(GameResources* p,const char* name){return p->has(name);}
API(resources_read) u32 resources_read(GameResources* p,const char* name,u8* output,u32 capacity){std::vector<u8> data;if(!p->read(name,data)||data.size()>capacity)return 0;std::memcpy(output,data.data(),data.size());return u32(data.size());}
API(resources_anm) int resources_anm(GameResources* p,const char* name,AnmResource* out){return p->open_anm(name,*out);}
API(resources_sht) int resources_sht(GameResources* p,const char* name,ShtResource* out){return p->open_sht(name,*out);}
API(resources_ecl) int resources_ecl(GameResources* p,const char* name,EclProgram* out){return p->load_ecl(name,*out);}
API(resources_anm_create) AnmResource* resources_anm_create(){return new AnmResource;}
API(resources_anm_delete) void resources_anm_delete(AnmResource* p){delete p;}
API(resources_sht_create) ShtResource* resources_sht_create(){return new ShtResource;}
API(resources_sht_delete) void resources_sht_delete(ShtResource* p){delete p;}
API(resources_ecl_create) EclProgram* resources_ecl_create(){return new EclProgram;}
API(resources_ecl_delete) void resources_ecl_delete(EclProgram* p){delete p;}
API(resources_anm_count) u32 resources_anm_count(AnmResource* p,u32 kind){return kind==0?p->textures.size():kind==1?p->sprites.size():p->scripts.size();}
API(resources_sht_count) u32 resources_sht_count(ShtResource* p){return p->groups.size();}
API(resources_ecl_count) u32 resources_ecl_count(EclProgram* p){return p->definitions.size();}
API(stage_resources_create) StageResources* stage_resources_create(){return new StageResources;}
API(stage_resources_delete) void stage_resources_delete(StageResources* p){delete p;}
API(resources_stage) int resources_stage(GameResources* p,u32 stage,StageResources* out){return p->load_stage(stage,*out);}
API(stage_value) u32 stage_value(StageResources* p,u32 kind){switch(kind){case 0:return p->background.scripts.size();case 1:return p->logo.scripts.size();case 2:return p->enemies.scripts.size();case 3:return p->timeline.definitions.size();case 4:return p->boss_programs.size();default:return p->messages.size();}}
API(stage_message_size) u32 stage_message_size(StageResources* p,u32 index){return index<p->messages.size()?p->messages[index].size():0;}
API(stage_timeline_name) const char* stage_timeline_name(StageResources* p,u32 index){return index<p->timeline.definitions.size()?p->timeline.subroutine(index).name.c_str():"";}
API(session_resources_create) GameSessionResources* session_resources_create(){return new GameSessionResources;}
API(session_resources_delete) void session_resources_delete(GameSessionResources* p){delete p;}
API(session_resources_core) int session_resources_core(GameSessionResources* p,GameResources* source){return p->load_core(*source);}
API(session_resources_stage) int session_resources_stage(GameSessionResources* p,GameResources* source,u32 number){return p->load_stage(*source,number);}
API(session_resources_value) u32 session_resources_value(GameSessionResources* p,u32 kind){switch(kind){case 0:return p->core.ascii.scripts.size();case 1:return p->core.bullet.scripts.size();case 2:return p->core.players[0].scripts.size();case 3:return p->core.players[1].scripts.size();case 4:return p->core.shots[0].groups.size()+p->core.shots[1].groups.size()+p->core.shots[2].groups.size()+p->core.shots[3].groups.size()+p->core.shots[4].groups.size()+p->core.shots[5].groups.size();case 5:return p->core.defaults.definitions.size();default:return p->stage_number;}}
API(game_session_create) GameSession* game_session_create(){return new GameSession;}
API(game_session_delete) void game_session_delete(GameSession* p){delete p;}
API(game_session_begin) int game_session_begin(GameSession* p,GameResources* source,u32 stage,i32 character,i32 subtype,i32 difficulty,u32 demo){return p->begin(*source,stage,character,subtype,difficulty,demo);}
API(game_session_replay) int game_session_replay(GameSession* p,GameResources* r,const u8* data,u32 size,u32 stage,u32 demo){return p->begin_replay(*r,data,size,stage,demo);}
API(game_session_replay_frame) u32 game_session_replay_frame(GameSession* p){return p->playback.frame();}
API(game_session_save_replay) u32 game_session_save_replay(GameSession* p,const char* name,u8* output,u32 capacity){std::vector<u8> data;if(!p->save_replay(name,data)||data.size()>capacity)return 0;std::memcpy(output,data.data(),data.size());return data.size();}
API(game_session_touch_motion) void game_session_touch_motion(GameSession* p,i32 mode,float x,float y){if(!p->battle)return;auto& in=p->battle->player_input.movement;in.touch_mode=mode;in.touch_x=x;in.touch_y=y;}
API(game_session_animations) AnmManager* game_session_animations(GameSession* p){return &p->animations;}
API(game_session_error) const char* game_session_error(GameSession* p){return p->error.c_str();}
API(game_session_update) int game_session_update(GameSession* p,u32 held,u32 pressed,u32 released,u32 pause,u32 confirm,u32 cancel){return p->update({held,pressed,released,bool(pause),bool(confirm),bool(cancel)});}
API(game_session_pause) int game_session_pause(GameSession* p){return p->pause();}
API(game_session_resume) int game_session_resume(GameSession* p){return p->resume();}
API(game_session_title) int game_session_title(GameSession* p){return p->return_to_title();}
API(game_session_end_probe) void game_session_end_probe(GameSession* p,i32 continues){p->battle->hud.score.continues=continues;p->economy.score_units=1234567;p->battle->completion.state.exit=StageExit::Ending;}
API(game_session_loss_probe) void game_session_loss_probe(GameSession* p){p->economy.lives=-1;p->battle->game_over_requested=true;}
API(game_session_title_menu) TitleMenu* game_session_title_menu(GameSession* p){return p->title.get();}
API(game_session_pause_menu) PauseMenu* game_session_pause_menu(GameSession* p){return p->pause_menu.get();}
API(game_session_pause_state) i32 game_session_pause_state(GameSession* p){return p->pause_menu?p->pause_menu->state:-1;}
API(game_session_stage_data) void* game_session_stage_data(GameSession* p,u32 k){switch(k){case 0:return p->battle&&p->battle->stage?&p->battle->stage->state:nullptr;case 1:return &p->compositor.play_camera;case 2:return &p->compositor.world_camera;case 3:return p->animations.reference_positions;case 4:return &p->animations.camera_delta;case 5:return p->battle?&p->battle->camera_position:nullptr;case 6:return p->battle?&p->battle->enemy_environment.camera_delta:nullptr;default:return &p->compositor.clear_color;}}
API(game_session_stage_action) int game_session_stage_action(GameSession* p,u32 action,i32 value){if(!p->battle)return 0;auto& b=*p->battle;switch(action){case 0:b.spell_background(value);return 1;case 1:return b.bomb_background_color(u32(value));case 2:return b.stage->interrupt(value);case 3:b.stage->fade_out(b.fades);return 1;case 4:b.stage->resume_background();return 1;default:return 0;}}
API(game_session_render) int game_session_render(GameSession* p,CompositorFixture* f){f->graphics.data.clear();f->graphics.target_events.clear();f->graphics.submissions.clear();f->graphics.snapshots.clear();return p->draw(f->renderer);}
API(game_session_start_stage) int game_session_start_stage(GameSession* p,GameResources* r,u32 number){return p->start_stage(*r,number);}
API(game_session_complete) int game_session_complete(GameSession* p,u32 flags){if(!p->battle)return 0;auto& b=*p->battle;b.completion.mode.practice=flags&1;b.completion.mode.force_title=flags&2;return b.dialogue_stage_complete();}
API(game_session_transition_value) u32 game_session_transition_value(GameSession* p,u32 k){if(!p->battle)return 0;auto& b=*p->battle;switch(k){case 0:return b.transitioning;case 1:return b.transition_started;case 2:return b.stage_active;case 3:return bool(b.outgoing_stage);case 4:return b.frame;case 5:return b.completion.state.result_timer.current;case 6:return b.completion.state.hud_flags;case 7:return b.transition_timer.current;case 8:return bool(p->resources.previous_stage);case 9:return b.result_animation;case 10:return u32(b.completion.state.exit);default:return b.completion.state.displayed_bonus;}}
API(game_session_transition_data) void* game_session_transition_data(GameSession* p,u32 k){if(!p->battle)return nullptr;auto& b=*p->battle;switch(k){case 0:return b.outgoing_stage?&b.outgoing_stage->state:nullptr;case 1:return &b.completion.state;case 2:return &p->clear_records;case 3:return &b.transition_timer;case 4:return b.player.get();case 5:return &b.completion.mode;default:return &p->animations.rate;}}
API(game_session_ecl_program) EclProgram* game_session_ecl_program(GameSession* p){return &p->resources.stage->timeline;}
API(game_session_enemy_state) EnemyState* game_session_enemy_state(GameSession* p,u32 last){auto* node=last?p->battle->enemies.last:p->battle->enemies.first;return node?&node->value->state:nullptr;}
API(game_session_ecl_command) i32 game_session_ecl_command(GameSession* p,EclContext* context){auto& b=*p->battle;auto& e=*b.enemies.first->value;context->owner=&e.script;EnemyScriptServices service(e.state,b.enemy_environment,b.enemy_commands);return service.command(*context);}
API(scene_draw_count) u32 scene_draw_count(){return sizeof(scene_draw_passes)/sizeof(scene_draw_passes[0]);}
API(scene_draw_value) u32 scene_draw_value(u32 index,u32 kind){const auto& p=scene_draw_passes[index];return kind==0?p.priority:kind==1?u32(p.kind):p.layer;}
static std::vector<u32> drawn_layers;
API(game_session_layer_probe) int game_session_layer_probe(GameSession* p,u32 invalid){
    p->return_to_title();p->animations.clear();drawn_layers.clear();std::vector<AnmVm*> vms;
    for(u32 layer=0;layer<31;++layer){auto* v=p->animations.create(p->resources.core.title,0,0,layer,layer>=29);if(!v)return -1;vms.push_back(v);}
    for(u32 layer=0;layer<vms.size();++layer){auto* v=vms[layer];v->layer=layer;v->flags=0x20000;v->draw_callback=[](AnmVm& vm){drawn_layers.push_back(vm.layer);};}
    if(!p->animations.update(false)||!p->animations.update(true))return -2;
    if(invalid){vms[0]->flags=3;vms[0]->color=0xffffffff;vms[0]->sprite=nullptr;}
    test::RenderFixture render;return p->draw(render.renderer);
}
API(game_session_drawn_layers) const u32* game_session_drawn_layers(){return drawn_layers.data();}
API(game_session_drawn_count) u32 game_session_drawn_count(){return drawn_layers.size();}
API(battle_data) void* battle_data(GameSession* p,u32 kind){if(!p->battle)return nullptr;auto& b=*p->battle;switch(kind){case 0:return &b.economy;case 1:return &b.player->state;case 2:return &b.player->motion.state;case 3:return &b.player->input;case 4:return &b.enemy_environment.player_position;case 5:return &b.bullets.player;case 6:return &b.LaserWorld::player;case 7:return &b.enemy_environment.shared_integers;default:return nullptr;}}
API(battle_items) ItemState* battle_items(GameSession* p){return &p->battle->items.at(0);}
API(battle_damage_protection) int battle_damage_protection(GameSession* p){p->battle->synchronize_player();return p->battle->special_ending;}
API(battle_item_player) ItemPlayer* battle_item_player(GameSession* p){return &p->battle->items.player;}
API(battle_collision) i32 battle_collision(GameSession* p,u32 shape,float x,float y,float a,float size){auto& b=*p->battle;if(shape>=2)return shape==2?b.collision({x,y,0},0,a,size):b.warning_collision({x,y,0},0,a,size);BulletState bullet{};bullet.flags=shape?0x10:0;bullet.position={x,y,0};bullet.hitbox={a,size};return b.collision(bullet);}
API(battle_item) i32 battle_item(GameSession* p,i32 type){auto& b=*p->battle;const auto q=b.player->motion.state.position;if(b.items.spawn(type,q,0xffffffff,0,0)<0)return 0;b.player->copy_item_state(b.items.player);return b.items.update();}
API(battle_value) i32 battle_value(GameSession* p,u32 k){auto& b=*p->battle;switch(k){case 0:return b.player->shots.player.character;case 1:return b.player->shots.player.subtype;case 2:return b.player->input.movement.pressed;case 3:return b.items.at(0).vm.resource==&b.resources.core.bullet;case 4:return b.events.size();case 5:return b.enemy_animations.resources[0].file==&b.resources.core.bullet&&b.enemy_animations.resources[1].file==&b.resources.core.enemy&&b.enemy_animations.resources[2].file==&b.resources.stage->enemies;case 6:return b.enemies.script_error.opcode;case 7:return b.player->motion.state.option_count;case 8:return b.enemies.first?b.enemies.first->value->state.flags:0;default:return b.frame;}}
API(battle_event) i32 battle_event(GameSession* p,u32 index,u32 field){auto& events=p->battle->events;if(index>=events.size())return -1;const auto& e=events[index];return field==0?i32(e.kind):field==1?e.value:e.extra;}
API(battle_graze) int battle_graze(GameSession* p,u32 type){auto& b=*p->battle;return b.register_graze()&&(type?b.graze_reward():b.reward_graze());}
API(battle_spell_setup) AnmManager* battle_spell_setup(GameSession* p,i32 section){auto& b=*p->battle;p->animations.clear();b.enemy_commands.stage_section=section;b.enemy_commands.bosses[0]=&b.enemies.first->value->state;b.enemy_commands.bosses[0]->current.position={-33.25f,47.125f,12.5f};return &p->animations;}
API(battle_spell_begin) int battle_spell_begin(GameSession* p,i32 id,i32 timeout,const char* name){auto& b=*p->battle;return b.spells.begin(id,timeout,name,b.enemy_commands.bosses[0]->current.position,false);}
API(battle_spell_tick) int battle_spell_tick(GameSession* p,float y){auto& b=*p->battle;return b.spells.update(y,b.enemy_commands.bosses[0]->current.position,false)&&p->animations.update(false);}
API(battle_spell_end) int battle_spell_end(GameSession* p){return p->battle->spells.end();}
API(battle_spell_data) void* battle_spell_data(GameSession* p,u32 k){auto& b=*p->battle;switch(k){case 0:return &b.spell_backgrounds[0];case 1:return &b.spell_backgrounds[1];case 2:return b.spell_titles;case 3:return &b.spell_circle;case 4:return &b.spell_flags;case 5:return &b.spell_elapsed;case 6:return &b.spell_bonus;default:return &b.spells.timer;}}
API(battle_laser_cancel) int battle_laser_cancel(GameSession* p){auto& b=*p->battle;LaserLineParameters args{};args.position={0,150,0};args.angle=0;args.growth_limit=120;args.initial_length=80;args.end_distance=500;args.width=12;args.speed=2;args.sprite=0;args.color=0;args.start_sound=-1;args.transform_sound=-1;if(b.lasers.spawn(args)<0)return 0;return b.lasers.cancel_circle(args.position,100,1,false)>=0;}
API(battle_converted_cut) int battle_converted_cut(GameSession* p,u32 infinite,u32 count){
    auto& b=*p->battle;b.lasers.clear();b.target=b.enemies.first->value;
    b.target->state.current.position={50,100,0};b.target->state.flags=0;
    for(u32 i=0;i<count;++i){
        if(infinite){LaserInfiniteParameters a{};a.position={0,150,0};a.initial_length=80;a.max_length=120;a.width=12;a.start_sound=a.transform_sound=-1;if(!b.lasers.spawn(a))return -1;}
        else{LaserLineParameters a{};a.position={0,150,0};a.initial_length=80;a.growth_limit=120;a.end_distance=500;a.width=12;a.start_sound=a.transform_sound=-1;if(!b.lasers.spawn(a))return -1;}
    }
    return b.lasers.cancel_circle({0,150,0},100,3,false);
}
API(battle_converted_count) u32 battle_converted_count(GameSession* p,u32 kind){auto& b=*p->battle;u32 count=0;for(u32 i=0;i<256;++i){auto& s=b.player->shots.at(i);if(s.state&&s.spec&&s.spec->spawn==ShotSpawn::ConvertedLaser){if(kind==1&&s.target!=b.target)continue;if(kind==2){auto* a=b.animations.find(s.animation);if(!a||a->resource!=&b.resources.core.players[b.enemy_environment.character])continue;}++count;}}return count;}
API(battle_damage) int battle_damage(GameSession* p){auto& b=*p->battle;auto& s=*b.player;const auto& spec=b.resources.core.shots[b.enemy_environment.character*3+b.enemy_environment.subtype].groups[0].shots[0];s.motion.copy_shot_state(s.shots.player);if(s.shots.spawn(spec,0,{0,200,0})<0)return -1;EnemyState e{};e.current.position=s.shots.at(0).movement.position;e.hitbox={48,48};e.lifetime.set(0,&b.animations.rate);i32 value=0;return b.shot_damage(e,value)?value:-2;}
API(battle_damage_gate) int battle_damage_gate(GameSession* p,i32 callback,i32 player_advanced,i32 enemy_advanced){auto& b=*p->battle;auto& s=*b.player;const auto saved=s.state.state_timer;s.state.state_timer.previous=0;s.state.state_timer.current=player_advanced;EnemyState e{};e.current.position={0,200,0};e.hitbox={48,48};e.lifetime.previous=0;e.lifetime.current=enemy_advanced;s.shots.damage_areas.circle(e.current.position,128,0,61,17,&b.animations.rate);i32 value=-1;const bool ok=callback?b.callback_damage(e.current.position,e.hitbox,value):b.shot_damage(e,value);s.state.state_timer=saved;return ok?value:-2;}
API(battle_enemy_death) int battle_enemy_death(GameSession* p){return p->battle->enemy_death();}
struct CommunicationFixture {GameEconomy economy;float rate=1;Communication gauge;CommunicationFixture(){gauge.reset(&rate);}};
API(communication_create) CommunicationFixture* communication_create(){return new CommunicationFixture;}
API(communication_delete) void communication_delete(CommunicationFixture* p){delete p;}
API(communication_data) void* communication_data(CommunicationFixture* p,u32 k){return k==0?static_cast<void*>(&p->economy):k==1?static_cast<void*>(&p->rate):static_cast<void*>(&p->gauge);}
API(communication_reward) void communication_reward(CommunicationFixture* p,i32 amount){p->gauge.reward(p->economy,amount,&p->rate);}
API(communication_update) void communication_update(CommunicationFixture* p,float y){p->gauge.update(p->economy,y);}
API(game_session_value) i32 game_session_value(GameSession* p,u32 kind){switch(kind){case 0:return i32(p->state.phase);case 1:return i32(p->state.frame);case 2:return i32(p->state.stage);case 3:return i32(p->animations.active_count());case 4:return p->battle_callbacks_attached();case 5:return p->battle?p->battle->last_error:-999;case 6:return p->battle&&p->battle->player?p->battle->player->last_error:-999;case 7:return p->battle?i32(p->battle->enemies.count):-999;case 8:return p->battle?i32(p->battle->bullets.active_count):-999;default:return p->ready();}}
API(lm_create) test::LaserManagerFixture* lm_create(){return new test::LaserManagerFixture;}
API(lm_delete) void lm_delete(test::LaserManagerFixture* p){delete p;}
API(lm_spawn) i32 lm_spawn(test::LaserManagerFixture* p,const void* args,u32 type){return type?p->manager.spawn(*static_cast<const LaserInfiniteParameters*>(args)):p->manager.spawn(*static_cast<const LaserLineParameters*>(args));}
API(lm_update) u32 lm_update(test::LaserManagerFixture* p,u32 paused,u32 frozen){return p->manager.update(paused,frozen);}
API(lm_data) void* lm_data(test::LaserManagerFixture* p,u32 k){switch(k){case 0:return p->manager.first();case 1:return p->manager.last();case 2:return &p->manager.next_id;case 3:return &p->manager.last_center;case 4:return &p->manager.last_size;default:return &p->manager.rate;}}
API(lm_count) u32 lm_count(test::LaserManagerFixture* p){return p->manager.active_count;}
API(lm_mark) void lm_mark(test::LaserManagerFixture* p){p->manager.mark_all();}
API(lm_warning) u32 lm_warning(test::LaserManagerFixture* p,float delta){return p->manager.update_warning(delta);}
API(lm_find) LaserState* lm_find(test::LaserManagerFixture* p,i32 id){return p->manager.find(id);}
API(lm_cancel_id) u32 lm_cancel_id(test::LaserManagerFixture* p,i32 id){return p->manager.cancel_id(id);}
API(lm_enemy_command) i32 lm_enemy_command(test::LaserManagerFixture* p,test::EnemyFixture* e,EclContext* c){e->commands.lasers=&p->manager;return enemy_laser_command(e->enemy.state,*c,e->globals,e->commands);}
API(lm_clear) void lm_clear(test::LaserManagerFixture* p){p->manager.clear();}
API(lm_cancel) i32 lm_cancel(test::LaserManagerFixture* p,u32 kind,const Vec3* center,const Vec3* size,float radius,u32 rewards,u32 skip){return kind==0?p->manager.cancel_all(rewards,skip):kind==1?p->manager.cancel_rectangle(*center,*size,rewards):p->manager.cancel_circle(*center,radius,rewards,skip);}
API(lm_draw) u32 lm_draw(test::LaserManagerFixture* p,u32 paused){p->render.graphics.data.clear();p->render.renderer.invalidate();if(!p->manager.draw(p->render.renderer,paused))return 0;p->render.renderer.flush();return 1;}
API(cx_create) test::CancellationFixture* cx_create(){return new test::CancellationFixture;}
API(cx_delete) void cx_delete(test::CancellationFixture* p){delete p;}
API(cx_load) int cx_load(test::CancellationFixture* p,const u8* b,u32 n){return p->load(b,n);}
API(cx_bullet) BulletState* cx_bullet(test::CancellationFixture* p,u32 i){return &p->bullets.at(i);}
API(cx_enemy) Enemy* cx_enemy(test::CancellationFixture* p,u32 i){return &p->targets[i];}
API(cx_item) ItemState* cx_item(test::CancellationFixture* p,u32 i){return &p->items.at(i);}
API(cx_player) ItemPlayer* cx_player(test::CancellationFixture* p){return &p->items.player;}
API(cx_anm) AnmManager* cx_anm(test::CancellationFixture* p){return &p->animations;}
API(cx_cancel) int cx_cancel(test::CancellationFixture* p,const Vec3* center,float radius,int reward,int skip,u32 flags,i32 id){return center?p->bullets.cancel_circle(*center,radius,reward,skip,{flags,id}):p->bullets.cancel_all(skip,{flags,id});}
API(cx_step) int cx_step(test::CancellationFixture* p){return p->items.update()&&p->animations.update(false);}
API(cx_cancel_rectangle) int cx_cancel_rectangle(test::CancellationFixture* p,int reward,u32 flags,i32 id){return p->bullets.cancel_rectangle(reward,{flags,id});}
API(cx_rectangle) Vec2* cx_rectangle(test::CancellationFixture* p){return &p->bullets.cancel_center;}
API(cx_bullet_sprite) int cx_bullet_sprite(test::CancellationFixture* p,u32 i,i32 sprite){if(sprite<0){p->bullets.at(i).vm.sprite=nullptr;return 1;}if(u32(sprite)>=p->resource.sprites.size())return 0;p->bullets.at(i).vm.sprite=&p->resource.sprites[sprite];return 1;}
API(cx_value) u32 cx_value(test::CancellationFixture* p,u32 k){return k==0?p->items.cancel_cursor:k==1?p->items.cancel_spawn_count:k==2?p->items.active_count:p->animations.active_count();}
API(cx_laser_spawn) i32 cx_laser_spawn(test::CancellationFixture* p,const void* args,u32 type){p->laser_world.rate=p->animations.rate;return type?p->lasers.spawn(*static_cast<const LaserInfiniteParameters*>(args)):p->lasers.spawn(*static_cast<const LaserLineParameters*>(args));}
API(cx_laser_first) LaserState* cx_laser_first(test::CancellationFixture* p){return p->lasers.first();}
API(cx_laser_update) u32 cx_laser_update(test::CancellationFixture* p){p->laser_world.rate=p->animations.rate;return p->lasers.update();}
API(cx_laser_cancel) i32 cx_laser_cancel(test::CancellationFixture* p,const Vec3* center,float radius,u32 rewards,u32 skip){p->laser_world.rate=p->animations.rate;return center?p->lasers.cancel_circle(*center,radius,rewards,skip):p->lasers.cancel_all(rewards,skip);}
API(cx_enemy_command) i32 cx_enemy_command(test::CancellationFixture* p,test::EnemyFixture* e,EclContext* c,u32 flags,i32 id){const BulletCancelContext context{flags,id};e->commands.bullets=&p->bullets;e->commands.lasers=&p->lasers;e->commands.cancellation=&context;p->laser_world.rate=p->animations.rate;const auto result=e->services.command(*c);e->commands.cancellation=nullptr;return result;}
API(pf_create) test::PlayerFrameFixture* pf_create(ShtResource* s,AnmResource* p,AnmResource* b,AnmManager* a){return new test::PlayerFrameFixture(*s,*p,*b,*a);}
API(pf_delete) void pf_delete(test::PlayerFrameFixture* p){delete p;}
API(pf_pointer) void* pf_pointer(test::PlayerFrameFixture* p,u32 k){switch(k){case 0:return &p->player.motion.state;case 1:return &p->player.state;case 2:return &p->player.motion.body;case 3:return &p->player.spell;case 4:return &p->economy;case 5:return &p->player.shots.schedule.timer;case 6:return p->player.shots.damage_areas.areas.data();case 7:return p->player.shots.laser_active.data();default:return p->events.data();}}
API(pf_option) PlayerOption* pf_option(test::PlayerFrameFixture* p,u32 i){return &p->player.motion.options[i];}
API(pf_shot) ShotState* pf_shot(test::PlayerFrameFixture* p,u32 i){return &p->player.shots.at(i);}
API(pf_input) void pf_input(test::PlayerFrameFixture* p,u32 held,u32 pressed,i32 combination,u32 flags){auto& in=p->player.input;in.movement={held,pressed,combination/3,combination%3,bool(flags&1),bool(flags&2),bool(flags&4)};in.special_available=!(flags&8);in.special_active=flags&16;in.shooting_blocked=flags&32;in.replay=flags&64;in.hit_sound=!(flags&128);p->events.clear();}
API(pf_action) i32 pf_action(test::PlayerFrameFixture* p,u32 action){if(action==4)return p->player.start_stage();if(action==5){p->player.motion.recall_options(true);return 1;}return action==0?p->player.initialize():action==1?p->player.update():action==2?p->player.hit():p->player.die();}
API(pf_prepare) i32 pf_prepare(test::PlayerFrameFixture* p,ShtResource* s){p->player.motion.set_speeds(s->header);return p->player.motion.reset_body()&&p->player.motion.rebuild_options(s->header,p->economy,p->player.input.movement.character*3+p->player.input.movement.subtype);}
API(pf_collision) void pf_collision(test::PlayerFrameFixture* p,PlayerCollision* result){*result=p->player.collision();}
API(pf_items) void pf_items(test::PlayerFrameFixture* p,ItemPlayer* result){p->player.copy_item_state(*result);}
API(pf_draw) int pf_draw(test::PlayerFrameFixture* p,test::RenderFixture* r){return p->player.draw(r->renderer);}
API(pf_value) u32 pf_value(test::PlayerFrameFixture* p,u32 k){switch(k){case 0:return p->events.size();case 1:return p->deaths;case 2:return p->player.last_error;case 3:return p->player.motion.last_error;case 4:return p->player.shots.last_error;case 5:return p->player.motion.focus_animation;default:return p->player.input.special_active;}}
API(pm_create) test::PlayerMotionFixture* pm_create(AnmResource* p,AnmResource* b,AnmManager* a){return new test::PlayerMotionFixture(*p,*b,*a);}
API(pm_delete) void pm_delete(test::PlayerMotionFixture* p){delete p;}
API(pm_state) PlayerMotionState* pm_state(test::PlayerMotionFixture* p){return &p->motion.state;}
API(pm_body) AnmVm* pm_body(test::PlayerMotionFixture* p){return &p->motion.body;}
API(pm_option) PlayerOption* pm_option(test::PlayerMotionFixture* p,u32 i){return &p->motion.options[i];}
API(pm_speeds) void pm_speeds(test::PlayerMotionFixture* p,ShtResource* s){p->motion.set_speeds(s->header);}
API(pm_rebuild) i32 pm_rebuild(test::PlayerMotionFixture* p,ShtResource* s,i32 power,i32 step,i32 maximum,i32 combination){GameEconomy e;e.power=power;e.power_step=step;e.max_power=maximum;return p->motion.rebuild_options(s->header,e,combination);}
API(pm_input) void pm_input(test::PlayerMotionFixture* p,u32 held,u32 pressed,i32 combination,u32 manager,u32 enemies,u32 bomb){p->input={held,pressed,combination/3,combination%3,bool(manager),bool(enemies),bool(bomb)};p->events.clear();}
API(pm_update) i32 pm_update(test::PlayerMotionFixture* p,u32 warp){return warp?p->motion.update_warp(p->input):p->motion.update(p->input);}
API(pm_value) u32 pm_value(test::PlayerMotionFixture* p,u32 k){return k==0?p->motion.focus_animation:k==1?p->motion.last_error:p->events.size();}
API(pm_events) const i32* pm_events(test::PlayerMotionFixture* p){return p->events.data();}
API(sm_create) test::ShotFixture* sm_create(ShtResource* s,AnmResource* a,AnmManager* m){return new test::ShotFixture(*s,*a,*m);}
API(sm_delete) void sm_delete(test::ShotFixture* p){delete p;}
API(sm_player) ShotPlayer* sm_player(test::ShotFixture* p){return &p->manager.player;}
API(sm_shot) ShotState* sm_shot(test::ShotFixture* p,u32 i){return &p->manager.at(i);}
API(sm_spawn) i32 sm_spawn(test::ShotFixture* p,const ShotSpec* s,i32 frame,const Vec3* origin){return p->manager.spawn(*s,frame,*origin);}
API(sm_converted) i32 sm_converted(test::ShotFixture* p,const Vec3* origin,float angle,u32 target){p->manager.locked_target=target?&p->enemy:nullptr;return p->manager.spawn_converted_laser(*origin,angle);}
API(sm_update) int sm_update(test::ShotFixture* p){return p->manager.update();}
API(sm_fire) int sm_fire(test::ShotFixture* p){return p->manager.fire();}
API(sm_timer) Timer* sm_timer(test::ShotFixture* p){return &p->manager.schedule.timer;}
API(sm_lasers) const u32* sm_lasers(test::ShotFixture* p){return p->manager.laser_active.data();}
API(sm_error) i32 sm_error(test::ShotFixture* p){return p->manager.last_error;}
API(sm_sound_count) u32 sm_sound_count(test::ShotFixture* p){return p->sounds.size();}
API(sm_sounds) const void* sm_sounds(test::ShotFixture* p){return p->sounds.data();}
API(sm_enemy) Enemy* sm_enemy(test::ShotFixture* p,u32 enabled){p->enemies=enabled?&p->link:nullptr;return &p->enemy;}
API(sm_invalidate) void sm_invalidate(test::ShotFixture* p){p->manager.invalidate_target(&p->enemy);}
API(sm_damage) i32 sm_damage(test::ShotFixture* p,const Vec3* v,const Vec2* size,u32 advanced){i32 result=0;return p->manager.damage(*v,*size,advanced,p->economy,result)?result:INT32_MIN;}
API(sm_economy) GameEconomy* sm_economy(test::ShotFixture* p){return &p->economy;}
API(sm_areas) DamageArea* sm_areas(test::ShotFixture* p){return p->manager.damage_areas.areas.data();}
API(sm_area_circle) DamageArea* sm_area_circle(test::ShotFixture* p,const Vec3* v,float r,float growth,i32 life,i32 damage,const float* rate){return p->manager.damage_areas.circle(*v,r,growth,life,damage,rate);}
API(sm_areas_update) void sm_areas_update(test::ShotFixture* p){p->manager.damage_areas.update();}
API(sht_create) ShtResource* sht_create(){return new ShtResource;}
API(sht_delete) void sht_delete(ShtResource* p){delete p;}
API(sht_open) int sht_open(ShtResource* p,const u8* b,u32 n){return p->open(b,n);}
API(sht_header) ShtHeader* sht_header(ShtResource* p){return &p->header;}
API(sht_count) u32 sht_count(ShtResource* p,u32 i){return i<p->groups.size()?p->groups[i].shots.size():0;}
API(sht_shots) const ShotSpec* sht_shots(ShtResource* p,u32 i){return i<p->groups.size()?p->groups[i].shots.data():nullptr;}
API(sht_group) i32 sht_group(ShtResource* p,i32 power,i32 step,i32 character,i32 subtype,u32 focus,i32 mode){return p->group_index(power,step,character,subtype,focus,mode);}
API(sht_due) u32 sht_due(const ShotSpec* p,i32 frame){return p->due(frame);}
API(shot_schedule) i32 shot_schedule(ShotSchedule* p,i32 state,i32 warp,u32 held,const float* rate){return p->update(state,warp,held,rate);}
API(im_create) test::ItemManagerFixture* im_create(){return new test::ItemManagerFixture;}
API(im_delete) void im_delete(test::ItemManagerFixture* p){delete p;}
API(im_economy) GameEconomy* im_economy(test::ItemManagerFixture* p){p->real_rewards=true;return &p->economy;}
API(im_rewards) int im_rewards(test::ItemManagerFixture* p,i32 type,const Vec3* position){p->events.clear();ItemState item{};item.type=type;item.position=*position;bool convert=false;return p->rewards.collect(item,convert)?1+int(convert):-1;}
API(im_draw) int im_draw(test::ItemManagerFixture* p){p->renderer.invalidate();p->graphics.data.clear();p->graphics.calls=0;const bool ok=p->manager->draw(p->renderer);p->renderer.flush();return ok;}
API(im_vertices) const void* im_vertices(test::ItemManagerFixture* p){return p->graphics.data.data();}
API(im_vertex_bytes) u32 im_vertex_bytes(test::ItemManagerFixture* p){return p->graphics.data.size();}
API(im_load) int im_load(test::ItemManagerFixture* p,const u8* b,u32 n){return p->load(b,n);}
API(im_item) ItemState* im_item(test::ItemManagerFixture* p,u32 n){return &p->manager->at(n);}
API(im_data) void* im_data(test::ItemManagerFixture* p,u32 kind){switch(kind){case 0:return &p->manager->player;case 1:return &p->animations.script_rng;case 2:return &p->animations.visual_rng;case 3:return &p->animations.rate;default:return p->events.data();}}
API(im_spawn) i32 im_spawn(test::ItemManagerFixture* p,i32 type,const Vec3* v,u32 color,float angle,float speed){return p->manager->spawn(type,*v,color,angle,speed);}
API(im_update) int im_update(test::ItemManagerFixture* p){p->events.clear();return p->manager->update();}
API(im_scatter) int im_scatter(test::ItemManagerFixture* p,EnemyDrop* drops,const Vec3* v){return p->manager->scatter(*drops,*v);}
API(im_attract) void im_attract(test::ItemManagerFixture* p){p->manager->attract_all();}
API(im_convert) int im_convert(test::ItemManagerFixture* p){return p->manager->convert_power_items();}
API(im_value) u32 im_value(test::ItemManagerFixture* p,u32 kind){switch(kind){case 0:return p->manager->active_count;case 1:return p->manager->cancel_cursor;case 2:return p->manager->cancel_spawn_count;case 3:return u32(p->manager->last_error);default:return p->events.size();}}
API(em_create) test::EnemyManagerFixture* em_create(EclProgram* p){return new test::EnemyManagerFixture(*p);}
API(em_delete) void em_delete(test::EnemyManagerFixture* p){delete p;}
API(em_spawn) Enemy* em_spawn(test::EnemyManagerFixture* p,const char* name,const EnemySpawn* value){return p->enemies.spawn(name,*value);}
API(em_update) int em_update(test::EnemyManagerFixture* p){return p->enemies.update();}
API(em_clear) int em_clear(test::EnemyManagerFixture* p){return p->enemies.erase_stage_enemies();}
API(em_cancel_first) void em_cancel_first(test::EnemyManagerFixture* p,EnemyLink* first){p->enemies.first=first;}
API(em_cancel) int em_cancel(test::EnemyManagerFixture* p,const Vec3* center,float radius,int reward){p->frame_world.cancel_rewards.clear();return p->enemies.cancel_circle(*center,radius,reward);}
API(em_cancel_beam) int em_cancel_beam(test::EnemyManagerFixture* p,const Vec3* center,float width,int reward){p->frame_world.cancel_rewards.clear();return p->enemies.cancel_beam(*center,width,reward);}
API(em_cancel_rewards) void* em_cancel_rewards(test::EnemyManagerFixture* p){return p->frame_world.cancel_rewards.data();}
API(em_cancel_reward_count) u32 em_cancel_reward_count(test::EnemyManagerFixture* p){return p->frame_world.cancel_rewards.size();}
API(em_first) EnemyLink* em_first(test::EnemyManagerFixture* p){return p->enemies.first;}
API(em_value) u32 em_value(test::EnemyManagerFixture* p,u32 kind){switch(kind){case 0:return p->enemies.count;case 1:return p->enemies.total_created;case 2:return u32(p->enemies.last_error);default:return p->enemies.timer.current;}}
API(em_setup) void em_setup(test::EnemyManagerFixture* p,i32 difficulty,float rate){p->enemy.world.difficulty=difficulty;p->enemy.world.rate=rate;p->manager.rate=rate;p->enemy.world.boss=nullptr;p->animations.resources[1]=p->animations.resources[2]=p->animations.resources[0];}
API(em_error_opcode) u32 em_error_opcode(test::EnemyManagerFixture* p){return p->enemies.script_error.opcode;}
API(ef_create) test::EnemyFrameFixture* ef_create(){return new test::EnemyFrameFixture;}
API(ef_delete) void ef_delete(test::EnemyFrameFixture* p){p->enemy.enemy.script.release_threads(p->enemy.services);delete p;}
API(ef_setting) void ef_setting(test::EnemyFrameFixture* p,u32 k,i32 v){p->setting(k,v);}
API(ef_update) i32 ef_update(test::EnemyFrameFixture* p){p->frame_world.events.clear();return enemy_frame(p->enemy.services,p->frame_world);}
API(ef_events) const void* ef_events(test::EnemyFrameFixture* p){return p->frame_world.events.data();}
API(ef_event_count) u32 ef_event_count(test::EnemyFrameFixture* p){return p->frame_world.events.size();}
API(ef_value) i32 ef_value(test::EnemyFrameFixture* p,u32 k){auto& w=p->frame_world;switch(k){case 0:return w.target==&p->enemy.enemy;case 1:return w.target_locked;case 2:return w.countdown_seconds;case 3:return w.countdown_hundredths;case 4:return w.spell_flags;case 5:return w.spell_bonus;case 6:return w.shared_spell_state;default:return w.bomb_animation_flags;}}
API(ea_create) test::EnemyAnimationFixture* ea_create(){return new test::EnemyAnimationFixture;}
API(ea_delete) void ea_delete(test::EnemyAnimationFixture* p){delete p;}
API(ea_load) int ea_load(test::EnemyAnimationFixture* p,const u8* b,u32 n){return p->load(b,n);}
API(ea_enemy) test::EnemyFixture* ea_enemy(test::EnemyAnimationFixture* p){return &p->enemy;}
API(ea_manager) AnmManager* ea_manager(test::EnemyAnimationFixture* p){return &p->manager;}
API(ea_command) i32 ea_command(test::EnemyAnimationFixture* p,EclContext* c){return p->animations.command(p->enemy.enemy.state,*c,p->enemy.globals);}
API(ea_position) void ea_position(test::EnemyAnimationFixture* p,u32 id,float x,float y,float z,u32 playfield){p->animations.position(id,{x,y,z},playfield);}
API(player_collision) i32 player_collision(const PlayerCollision* p,const Vec2* position,const Vec2* size,u32 circular){const auto result=circular?p->circle(*position,size->x):p->rectangle(*position,*size);return i32(result.kind)|(result.trigger_hit?4:0);}
API(player_laser_collision) i32 player_laser_collision(const PlayerCollision* p,const Vec2* origin,const Vec2* half,float angle,float width,float length,int probe){const auto result=p->laser(*origin,*half,angle,width,length,probe);return i32(result.kind)|(result.trigger_hit?4:0);}
API(bm_create) test::BulletManagerFixture* bm_create(){return new test::BulletManagerFixture;}
API(ecb_create) test::EnemyCallbackFixture* ecb_create(){return new test::EnemyCallbackFixture;}
API(ecb_delete) void ecb_delete(test::EnemyCallbackFixture* p){delete p;}
API(ecb_base) test::BulletManagerFixture* ecb_base(test::EnemyCallbackFixture* p){return p;}
API(ecb_data) void* ecb_data(test::EnemyCallbackFixture* p,u32 kind,u32 index){switch(kind){case 0:return &p->enemy.enemy.state;case 1:return &p->linked[index%8].state;case 2:return &p->arc;case 3:return p->calls.data();case 4:return p->spawns.data();case 5:return &p->enemy.world.player_position;default:return nullptr;}}
API(ecb_count) u32 ecb_count(test::EnemyCallbackFixture* p,u32 kind){return kind?p->spawns.size():p->calls.size();}
API(ecb_run) i32 ecb_run(test::EnemyCallbackFixture* p,u32 kind,i32 damage){p->calls.clear();p->spawns.clear();p->damage_value=damage;i32 out=0;const bool ok=kind==13?p->callbacks.damage_arc(p->enemy.enemy.state,out):kind==14?p->callbacks.collision_arc(p->enemy.enemy.state):p->callbacks.invoke_tick(p->enemy.enemy.state,EnemyTick(kind),out);return ok?out:-2;}
API(bm_enemy) test::EnemyFixture* bm_enemy(test::BulletManagerFixture* p){return &p->enemy;}
API(bm_enemy_fire) i32 bm_enemy_fire(test::BulletManagerFixture* p,EclContext* c){return enemy_fire_command(p->enemy.enemy.state,*c,p->enemy.globals,p->enemy.commands);}
API(bm_draw) int bm_draw(test::BulletManagerFixture* p,u32 group){p->renderer.invalidate();p->graphics.data.clear();p->graphics.calls=0;const bool result=p->manager->draw_group(group,p->renderer);p->renderer.flush();return result;}
API(bm_vertices) const void* bm_vertices(test::BulletManagerFixture* p){return p->graphics.data.data();}
API(bm_vertex_bytes) u32 bm_vertex_bytes(test::BulletManagerFixture* p){return p->graphics.data.size();}
API(bm_delete) void bm_delete(test::BulletManagerFixture* p){delete p;}
API(bm_load) int bm_load(test::BulletManagerFixture* p,const u8* b,u32 n){return p->load(b,n);}
API(bm_data) void* bm_data(test::BulletManagerFixture* p,u32 kind){switch(kind){case 0:return &p->animations.rate;case 1:return &p->animations.script_rng;case 2:return &p->animations.visual_rng;case 3:return &p->manager->player;default:return p->events.data();}}
API(bm_bullet) BulletState* bm_bullet(test::BulletManagerFixture* p,u32 i){return &p->manager->at(i);}
API(bm_spawn) i32 bm_spawn(test::BulletManagerFixture* p,const BulletEmitter* e,i32 i,i32 layer,float aim){return p->manager->spawn(*e,i,layer,aim);}
API(bm_fire) int bm_fire(test::BulletManagerFixture* p,const BulletEmitter* e){return p->manager->fire(*e);}
API(bm_update) int bm_update(test::BulletManagerFixture* p,u32 pause){return p->manager->update(pause);}
API(bm_one) i32 bm_one(test::BulletManagerFixture* p,u32 i){return p->manager->update_one(p->manager->at(i));}
API(bm_cancel) int bm_cancel(test::BulletManagerFixture* p,u32 i){return p->manager->cancel(p->manager->at(i));}
API(bm_cancel_area) int bm_cancel_area(test::BulletManagerFixture* p,const Vec3* center,float radius,int reward,int skip,u32 spell_flags,i32 spell_id){p->cancellation_events.clear();const BulletCancelContext context{spell_flags,spell_id};return center?p->manager->cancel_circle(*center,radius,reward,skip,context):p->manager->cancel_all(skip,context);}
API(bm_cancel_beam) int bm_cancel_beam(test::BulletManagerFixture* p,const Vec3* center,float width,int reward,u32 spell_flags,i32 spell_id){p->cancellation_events.clear();return p->manager->cancel_beam(*center,width,reward,{spell_flags,spell_id});}
API(bm_convert_area) int bm_convert_area(test::BulletManagerFixture* p,const Vec3* center,float radius,int reward,u32 flags,i32 id){p->cancellation_events.clear();return p->manager->convert_circle(*center,radius,reward,{flags,id});}
API(bm_cancel_events) void* bm_cancel_events(test::BulletManagerFixture* p){return p->cancellation_events.data();}
API(bm_cancel_event_count) u32 bm_cancel_event_count(test::BulletManagerFixture* p){return p->cancellation_events.size();}
API(bm_appearance) int bm_appearance(test::BulletManagerFixture* p,u32 i,i32 type,i32 color){return p->manager->change_appearance(p->manager->at(i),type,color,true);}
API(bm_value) u32 bm_value(test::BulletManagerFixture* p,u32 k){switch(k){case 0:return p->manager->cursor;case 1:return p->manager->active_count;case 2:return p->manager->last_error;default:return p->events.size();}}
API(bm_configure) void bm_configure(test::BulletManagerFixture* p,i32 hit,u32 cursor,float exclusion){p->collision_result=hit;p->manager->cursor=cursor;p->manager->exclusion_squared=exclusion;p->manager->rate=p->animations.rate;p->events.clear();}
API(bullet_fixture_create) test::BulletFixture* bullet_fixture_create(){return new test::BulletFixture;}
API(bullet_fixture_delete) void bullet_fixture_delete(test::BulletFixture* p){delete p;}
API(bullet_fixture_data) void* bullet_fixture_data(test::BulletFixture* p,u32 kind){switch(kind){case 0:return &p->bullet;case 1:return &p->rate;case 2:return &p->player;case 3:return &p->sprite;case 4:return p->events.data();case 5:return p->emissions.data();default:return &p->default_tangent;}}
API(bullet_fixture_events) u32 bullet_fixture_events(test::BulletFixture* p,u32 kind){if(kind==2){p->events.clear();p->emissions.clear();return 0;}return kind?p->emissions.size():p->events.size();}
API(bullet_motion) i32 bullet_motion_api(test::BulletFixture* p,u32 kind){return bullet_motion(p->bullet,kind,*p);}
API(bullet_transform) i32 bullet_transform_api(test::BulletFixture* p){return bullet_start_transforms(p->bullet,*p);}
API(bullet_launch) i32 bullet_launch_api(const BulletEmitter* e,i32 index,i32 layer,float aim,Rng* rng,const Vec3* player,float exclusion,BulletLaunch* out){return bullet_launch(*e,index,layer,aim,*rng,*player,exclusion,*out)?0:-1;}
API(enemy_fixture_create) test::EnemyFixture* enemy_fixture_create(){return new test::EnemyFixture;}
API(enemy_fixture_delete) void enemy_fixture_delete(test::EnemyFixture* p){delete p;}
API(enemy_fixture_data) void* enemy_fixture_data(test::EnemyFixture* p,u32 k){switch(k){case 0:return &p->enemy;case 1:return &p->boss;case 2:return &p->world;case 4:return &p->commands;case 5:return &p->visibility_count;case 6:return &p->commands.stage_section;default:return &p->sprite;}}
API(enemy_variable) double enemy_variable(test::EnemyFixture* p,i32 id,u32 kind){switch(kind){case 0:return p->globals.integer(id);case 1:return p->globals.floating(id);case 2:return uintptr_t(p->globals.integer_reference(id));default:return uintptr_t(p->globals.float_reference(id));}}
API(enemy_initialize) void enemy_initialize(test::EnemyFixture* p){p->enemy.state.initialize(&p->enemy,&p->world.rate);}
API(enemy_movement) i32 enemy_movement(test::EnemyFixture* p,u32 combine){if(combine){p->enemy.state.combine_movement();return 0;}return p->enemy.state.update_movement(&p->world.rate,p->world.camera_delta);}
API(enemy_damage) i32 enemy_damage(EnemyState* p,i32 amount){return p->take_damage(amount);}
API(enemy_interrupt) const char* enemy_interrupt(EnemyState* p,EnemyPhaseState* w){return p->check_interrupts(*w);}
API(enemy_command) i32 enemy_command(test::EnemyFixture* p,EclContext* c){return enemy_movement_command(p->enemy.state,*c,p->globals)||enemy_emitter_command(p->enemy.state,*c,p->globals)?0:enemy_state_command(p->enemy.state,*c,p->globals,p->commands);}
API(enemy_float_argument) double enemy_float_argument(EclContext* c,test::EnemyFixture* p,u32 index,u32 resolved){return resolved?c->resolve_float(float(index),p->globals):c->float_argument(index,p->globals);}
API(enemy_script_start) i32 enemy_script_start(test::EnemyFixture* p,EclProgram* program,const char* name,u32 difficulty){auto& o=p->enemy.script;o.reset_threads(p->services);o.root.stack={};o.root.difficulty=difficulty;o.program=program;o.select_subroutine(name);p->services.error={};return o.root.instruction?0:-1;}
API(enemy_script_update) i32 enemy_script_update(test::EnemyFixture* p,float elapsed){return p->services.update(elapsed);}
API(enemy_script_release) void enemy_script_release(test::EnemyFixture* p){p->enemy.script.reset_threads(p->services);}
API(enemy_script_error) const EnemyScriptError* enemy_script_error(test::EnemyFixture* p){return &p->services.error;}
API(movement) void movement(Movement* p,u32 kind){if(kind==0)p->update_velocity();else if(kind==1)p->update();else if(kind==2)p->refresh_position();else p->snap_position();}
API(ecl_create) EclProgram* ecl_create(){return new EclProgram;}
API(ecl_delete) void ecl_delete(EclProgram* p){delete p;}
API(ecl_attach) i32 ecl_attach(EclProgram* p,const u8* b,u32 n){return p->attach(b,n);}
API(ecl_count) u32 ecl_count(EclProgram* p,u32 k){return k?p->definitions.size():p->files.size();}
API(ecl_name) const char* ecl_name(EclProgram* p,u32 i){return p->subroutine(i).name.c_str();}
API(ecl_header) const u8* ecl_header(EclProgram* p,u32 i){return p->definitions[i].file->bytes.data()+p->subroutine(i).offset;}
API(ecl_size) u32 ecl_size(EclProgram* p,u32 i){return p->subroutine(i).size;}
API(ecl_file) const u8* ecl_file(EclProgram* p,u32 i){return p->files[i]->bytes.data();}
API(ecl_find) const EclInstruction* ecl_find(EclProgram* p,const char* n){return p->find(n);}
API(ecl_include_count) u32 ecl_include_count(EclProgram* p,u32 i,u32 animation){return animation?p->files[i]->animations.size():p->files[i]->includes.size();}
API(ecl_include_name) const char* ecl_include_name(EclProgram* p,u32 i,u32 j,u32 animation){return animation?p->files[i]->animations[j].c_str():p->files[i]->includes[j].c_str();}
API(ecl_stack_push) i32 ecl_stack_push(EclStack* p,u32 type,u32 bits){return p->push(EclValueType(type),bits);}
API(ecl_update) i32 ecl_update(EclContext* p,test::EclGlobalsFixture* g,float dt){return p->update(dt,*g);}
API(ecl_fixture_control) u32 ecl_fixture_control(test::EclGlobalsFixture* p,i32 result){p->command_result=result;return p->commands;}
API(ecl_fixture_events) const void* ecl_fixture_events(test::EclGlobalsFixture* p){return p->events.data();}
API(ecl_fixture_event_count) u32 ecl_fixture_event_count(test::EclGlobalsFixture* p,u32 clear){if(clear)p->events.clear();return p->events.size();}
API(ecl_owner_create) EclOwner* ecl_owner_create(EclProgram* p){auto* o=new EclOwner;o->program=p;o->initialize_context();return o;}
API(ecl_owner_delete) void ecl_owner_delete(EclOwner* p,test::EclGlobalsFixture* s){p->release_threads(*s);delete p;}
API(ecl_owner_update) i32 ecl_owner_update(EclOwner* p,test::EclGlobalsFixture* s,float dt){return p->update_threads(dt,*s);}
API(ecl_call) i32 ecl_call(EclContext* p,EclContext* q,u32 skipped,test::EclGlobalsFixture* s){return call_subroutine(*p,*q,skipped,*s);}
API(ecl_owner_initialize) void ecl_owner_initialize(EclOwner* p){p->initialize_context();}
API(ecl_stack_frame) i32 ecl_stack_frame(EclStack* p,i32 bytes,u32 leave){if(leave){p->leave_frame();return 0;}return p->enter_frame(bytes);}
API(ecl_globals_create) test::EclGlobalsFixture* ecl_globals_create(){return new test::EclGlobalsFixture;}
API(ecl_globals_delete) void ecl_globals_delete(test::EclGlobalsFixture* p){delete p;}
API(ecl_globals_data) void* ecl_globals_data(test::EclGlobalsFixture* p,u32 floating){return floating?static_cast<void*>(p->floats):p->ints;}
API(ecl_argument) u32 ecl_argument(EclContext* p,test::EclGlobalsFixture* g,u32 kind,u32 index,u32 value){switch(kind){case 0:return u32(p->integer_argument(index,*g));case 1:return float_bits(float(p->float_argument(index,*g)));case 2:return u32(p->resolve_integer(signed_bits(value),*g));case 3:return float_bits(float(p->resolve_float(float_bits(value),*g)));case 4:return reinterpret_cast<uintptr_t>(p->integer_reference(index,*g));default:return reinterpret_cast<uintptr_t>(p->float_reference(index,*g));}}
API(crypt) int crypt(const u8* a,u8* b,u32 n,u32 key,u32 step,u32 block,u32 limit,u32 encode){return resource_crypt(a,b,n,{u8(key),u8(step),block,limit},encode);}
API(rng) u32 rng(Rng* r,u32 mode){return mode==0?r->next16():mode==1?r->next32():r->next_four();}
API(archive_create) Archive* archive_create(){return new Archive;}
API(archive_delete) void archive_delete(Archive* p){delete p;}
API(archive_open) int archive_open(Archive* p,const u8* b,u32 n){return p->open(b,n);}
API(archive_count) u32 archive_count(Archive* p){return p->entries.size();}
API(archive_name) const char* archive_name(Archive* p,u32 n){return n<p->entries.size()?p->entries[n].name.c_str():nullptr;}
API(archive_field) u32 archive_field(Archive* p,u32 n,u32 k){if(n>=p->entries.size())return 0;const auto& e=p->entries[n];return k==0?e.offset:k==1?e.size:k==2?e.compressed:e.reserved;}
API(archive_read) int archive_read(Archive* p,u32 n,u8* out,u32 cap){std::vector<u8> data;if(!p->read(n,data)||data.size()>cap)return -1;std::memcpy(out,data.data(),data.size());return data.size();}
API(anm_create) AnmResource* anm_create(){return new AnmResource;}
API(anm_delete) void anm_delete(AnmResource* p){delete p;}
API(anm_open) int anm_open(AnmResource* p,const u8* b,u32 n){return p->open(b,n);}
API(anm_count) u32 anm_count(AnmResource* p,u32 k){return k==0?p->textures.size():k==1?p->sprites.size():p->scripts.size();}
API(anm_sprite) const AnmSprite* anm_sprite(AnmResource* p,u32 n){return n<p->sprites.size()?&p->sprites[n]:nullptr;}
API(anm_script_offset) u32 anm_script_offset(AnmResource* p,u32 n){return n<p->scripts.size()?p->scripts[n].file_offset:0;}
API(anm_script_size) u32 anm_script_size(AnmResource* p,u32 n){return n<p->scripts.size()?p->scripts[n].bytes.size():0;}
API(anm_script_bytes) const u8* anm_script_bytes(AnmResource* p,u32 n){return n<p->scripts.size()?p->scripts[n].bytes.data():nullptr;}
API(anm_rgba) int anm_rgba(AnmResource* p,u32 n,u8* out,u32 cap){std::vector<u8> data;if(n>=p->textures.size()||!p->textures[n].rgba(data)||data.size()>cap)return -1;std::memcpy(out,data.data(),data.size());return data.size();}
API(rng_float) float rng_float(Rng* r,u32 mode){return mode?r->signed_unit():r->unit();}
API(timer_set) void timer_set(Timer* t,i32 v,const float* rate){t->set(v,rate);}
API(timer_tick) void timer_tick(Timer* t){t->tick();}
API(timer_advance) void timer_advance(Timer* t,float n){t->advance(n);}
API(interpolation_sample) void interpolation_sample(void* v,void* out,u32 kind,const float* rate){
    if(kind==0)*static_cast<Vec2*>(out)=sample(*static_cast<Vec2Interpolator*>(v),rate);
    if(kind==1)*static_cast<Vec3*>(out)=sample(*static_cast<Vec3Interpolator*>(v),rate);
    if(kind==2)*static_cast<Rgb*>(out)=sample(*static_cast<RgbInterpolator*>(v),rate);
    if(kind==3)*static_cast<i32*>(out)=sample(*static_cast<AlphaInterpolator*>(v),rate);
    if(kind==4)*static_cast<float*>(out)=sample(*static_cast<FloatInterpolator*>(v),rate);
}
API(anm_env_create) AnmEnvironment* anm_env_create(){return new AnmEnvironment;}
API(anm_env_delete) void anm_env_delete(AnmEnvironment* p){delete p;}
API(anm_env_rate) float* anm_env_rate(AnmEnvironment* p){return &p->rate;}
API(anm_env_camera) Vec3* anm_env_camera(AnmEnvironment* p,u32 k){return k<2?&p->reference_positions[k]:&p->camera_delta;}
API(anm_env_rng) Rng* anm_env_rng(AnmEnvironment* p,u32 visual){return visual?&p->visual_rng:&p->script_rng;}
API(anm_vm_init) void anm_vm_init(AnmVm* p){p->initialize();}
API(anm_vm_bind) int anm_vm_bind(AnmVm* p,AnmResource* r,i32 s,AnmEnvironment* e){return p->bind_script(*r,s,0,&e->rate);}
API(anm_vm_update) int anm_vm_update(AnmVm* p,AnmEnvironment* e){return p->update(*e);}
API(anm_quad) int anm_quad(AnmVm* p,Vec3* out){return anm_quad_positions(*p,*reinterpret_cast<Vec3(*)[4]>(out));}
API(anm_screen_uv) void anm_screen_uv_api(Vec3* p,Vec2* out){*out=anm_screen_uv(*p);}
API(anm_ring) int anm_ring(AnmVm* p,AnmVertex* out,u32 cap){return anm_ring_vertices(*p,out,cap);}
API(anm_manager_create) AnmManager* anm_manager_create(){return new AnmManager;}
API(anm_manager_delete) void anm_manager_delete(AnmManager* p){delete p;}
API(anm_manager_spawn) AnmVm* anm_manager_spawn(AnmManager* p,AnmResource* r,i32 script,u32 layer,u32 mode){return p->create(*r,script,0,layer,mode&1,mode&2);}
API(anm_manager_update) int anm_manager_update(AnmManager* p,u32 overlay){return p->update(overlay);}
API(anm_manager_find) AnmVm* anm_manager_find(AnmManager* p,u32 id){return p->find(id);}
API(anm_manager_count) u32 anm_manager_count(AnmManager* p){return p->active_count();}
API(anm_manager_layer) AnmVm* anm_manager_layer(AnmManager* p,u32 n){return p->layer_first(n);}
API(anm_manager_error) u32 anm_manager_error(AnmManager* p){return p->last_error;}
API(anm_ripple_init) void anm_ripple_init(AnmVm* p,AnmRipple* r,Rng* rng){p->geometry=r;anm_ripple_initialize(*p,*r,*rng);}
API(anm_ripple_tick) void anm_ripple_tick(AnmVm* p){anm_ripple_update(*p);}
API(render_create) test::RenderFixture* render_create(){return new test::RenderFixture;}
API(render_delete) void render_delete(test::RenderFixture* p){delete p;}
API(render_initialize_pipeline) void render_initialize_pipeline(test::RenderFixture* p){p->graphics.initialize_game_pipeline();}
API(render_pipeline_value) u32 render_pipeline_value(test::RenderFixture* p,u32 k){
    const auto& s=p->graphics.state;
    const u32 values[]={s.depthTest,s.depthWrite,s.blend,s.alphaTest,s.fog,s.rangeFog,s.dither,u32(s.depthCompare),u32(s.alphaCompare),u32(s.sourceBlend),u32(s.destinationBlend),u32(s.blendEquation),u32(s.cull),s.colorMask,s.alphaReference,s.fogColor,u32(s.fogMode),float_bits(s.fogNear),float_bits(s.fogFar),float_bits(s.fogDensity),u32(s.color.operation),u32(s.alpha.operation),u32(s.color.first.source),u32(s.alpha.first.source),u32(s.color.second.source),u32(s.alpha.second.source),s.textureTransform,u32(s.minFilter),u32(s.magFilter),u32(s.addressU),u32(s.addressV)};
    return k<sizeof(values)/sizeof(*values)?values[k]:0;
}
API(render_configure) void render_configure(test::RenderFixture* p,float x,float y,u32 tint,u32 enabled){p->renderer.offset={x,y};p->renderer.tint=tint;p->renderer.tint_enabled=enabled;}
API(render_draw) int render_draw(test::RenderFixture* p,AnmVm* v){v->resource=&p->resource;v->sprite=&p->sprite;return p->renderer.draw(*v);}
API(render_flush) void render_flush(test::RenderFixture* p){p->renderer.flush();}
API(render_reset) void render_reset(test::RenderFixture* p){p->renderer.invalidate();p->graphics.data.clear();p->graphics.calls=0;}
API(render_data) const u8* render_data(test::RenderFixture* p){return p->graphics.data.data();}
API(render_size) u32 render_size(test::RenderFixture* p){return p->graphics.data.size();}
API(render_state) u32 render_state(test::RenderFixture* p,u32 kind){return kind==0?u32(p->graphics.state.sourceBlend):kind==1?u32(p->graphics.state.destinationBlend):kind==2?u32(p->graphics.state.minFilter):p->graphics.calls;}

#include "laser-fixture.hpp"
#include "hud-fixture.hpp"
API(hud_create) test::HudFixture* hud_create(){return new test::HudFixture;}
API(hud_delete) void hud_delete(test::HudFixture* p){delete p;}
API(hud_load) u32 hud_load(test::HudFixture* p,u32 k,const u8* data,u32 n){return (k==0?p->front:k==1?p->text:p->logo).open(data,n);}
API(hud_start) u32 hud_start(test::HudFixture* p,u32 demo,u32 initial){p->hud.prepare_stage();return p->hud.start_stage(p->logo,p->input,demo,initial);}
API(hud_lives) void hud_lives(test::HudFixture* p,i32 count,i32 fragments){p->hud.display_lives(count,fragments);}
API(hud_notice) u32 hud_notice(test::HudFixture* p,i32 type,i32 value){return p->hud.notice(type,value);}
API(hud_notice_handle) u32 hud_notice_handle(test::HudFixture* p,u32 n){return n<8?p->hud.bonus_digits[n]:n==8?p->hud.spell_notice:n==9?p->hud.item_notice:n==10?p->hud.spell_time_animation:p->hud.show_spell_time;}
API(hud_update) i32 hud_update(test::HudFixture* p){p->sounds.clear();if(!p->hud.update(p->input))return -1;if(!p->hud.finish_update(p->input))return -2;if(!p->animations.update(false))return -3;if(!p->animations.update(true))return -4;return 1;}
API(hud_boss) void hud_boss(test::HudFixture* p,u32 active){p->enemies.bosses[0]=active?&p->boss:nullptr;}
API(hud_data) void* hud_data(test::HudFixture* p,u32 k){switch(k){case 0:return &p->economy;case 1:return &p->input;case 2:return &p->boss;case 3:return &p->completion;case 4:return p->hud.lives;case 5:return p->hud.digits;case 6:return p->hud.communication;case 7:return &p->hud.indicator;case 8:return &p->hud.elapsed;case 9:return &p->animations.rate;case 10:return &p->enemies.manager_flags;case 11:return &p->enemies.remaining_phases;case 12:return &p->animations;default:return p->sounds.data();}}
API(hud_value) u32 hud_value(test::HudFixture* p,u32 k){switch(k){case 0:return p->hud.previous_seconds;case 1:return p->hud.boss_name;case 2:return p->sounds.size();case 3:return p->hud.health;default:return p->hud.score.displayed;}}
API(hud_score) HudScore* hud_score(test::HudFixture* p){return &p->hud.score;}
API(hud_score_update) void hud_score_update(test::HudFixture* p){p->hud.score.update(p->economy.score_units);}
API(hud_health) float hud_health(test::HudFixture* p,u32 target){return target?p->hud.target_health:p->hud.displayed_health;}
API(hud_draw) u32 hud_draw(test::HudFixture* p){return p->hud.draw_inner(p->renderer,p->input)&&p->hud.draw_outer(p->renderer,p->input);}
API(hud_queue_text) u32 hud_queue_text(test::HudFixture* p){p->ascii.clear();return p->hud.queue_text(p->ascii,p->input);}
API(hud_spell_overlay) u32 hud_spell_overlay(test::HudFixture* p,u32 color,i32 frames,i32 encoded){if(!p->hud.notice(1))return 0;auto* vm=p->animations.find(p->hud.spell_time_animation);if(!vm)return 0;vm->color=color;p->input.spell_frames=frames;p->input.spell_time=encoded;return 1;}
API(hud_text_count) u32 hud_text_count(test::HudFixture* p){return p->ascii.requests.size();}
API(hud_text_string) const char* hud_text_string(test::HudFixture* p,u32 n){return p->ascii.requests.at(n).text.c_str();}
API(hud_text_position) const Vec3* hud_text_position(test::HudFixture* p,u32 n){return &p->ascii.requests.at(n).position;}
API(hud_text_style) const AsciiStyle* hud_text_style(test::HudFixture* p,u32 n){return &p->ascii.requests.at(n).style;}

#include "ascii-fixture.hpp"
API(ascii_create) test::AsciiFixture* ascii_create(){return new test::AsciiFixture;}
API(ascii_delete) void ascii_delete(test::AsciiFixture* p){delete p;}
API(ascii_load) u32 ascii_load(test::AsciiFixture* p,const u8* data,u32 n){return p->resource.open(data,n);}
API(ascii_reset) void ascii_reset(test::AsciiFixture* p){p->renderer.invalidate();p->text.clear();p->graphics.data.clear();p->graphics.submissions.clear();}
API(ascii_add) u32 ascii_add(test::AsciiFixture* p,const char* text,float x,float y,float sx,float sy,u32 color,u32 font,u32 flags){return p->text.add(text,{x,y,0},{color,{sx,sy},i32(font),bool(flags&1),bool(flags&2),i32((flags>>2)&1)});}
API(ascii_draw) u32 ascii_draw(test::AsciiFixture* p,i32 pass){const bool ok=p->text.draw(p->renderer,pass,p->full,p->play);p->renderer.flush();return ok;}
API(ascii_data) void* ascii_data(test::AsciiFixture* p){return p->graphics.data.data();}
API(ascii_size) u32 ascii_size(test::AsciiFixture* p){return p->graphics.data.size();}
API(ascii_filter) u32 ascii_filter(test::AsciiFixture* p){return u32(p->graphics.state.minFilter);}
API(title_ascii) u32 title_ascii(TitleMenu* p,test::AsciiFixture* f){f->text.clear();p->queue_ascii(f->text);return f->text.requests.size();}
API(pause_ascii) u32 pause_ascii(PauseMenu* p,test::AsciiFixture* f){f->text.clear();p->queue_ascii(f->text);return f->text.requests.size();}
API(ascii_request) const void* ascii_request(test::AsciiFixture* p,u32 i,u32 kind){const auto& r=p->text.requests[i];return kind==0?static_cast<const void*>(r.text.c_str()):kind==1?static_cast<const void*>(&r.position):&r.style;}
API(battle_spell_ascii) i32 battle_spell_ascii(GameSession* p,test::AsciiFixture* f){auto& b=*p->battle;b.ascii.clear();if(!b.draw(f->renderer,SceneDrawKind::Spell))return -1;f->text.requests=b.ascii.requests;return i32(f->text.requests.size());}
API(battle_spell_record) void battle_spell_record(GameSession* p,i32 selection,i32 id,i32 captures,i32 attempts){p->battle->spells.selection=selection;p->battle->spell_id=id;auto& r=p->spell_records.entries[selection][id];r.captures=captures;r.attempts=attempts;}
#include "laser-line-fixture.hpp"
API(ll_create) test::LaserLineFixture* ll_create(){return new test::LaserLineFixture;}
API(ll_delete) void ll_delete(test::LaserLineFixture* p){delete p;}
API(ll_load) u32 ll_load(test::LaserLineFixture* p,const u8* bytes,u32 n){return p->resource.open(bytes,n);}
API(ll_configure) void ll_configure(test::LaserLineFixture* p,float rate,i32 collision){p->rate=rate;p->animations.rate=rate;p->collision_result=collision;}
API(ll_resource) AnmResource* ll_resource(test::LaserLineFixture* p){return &p->resource;}
API(ll_initialize) u32 ll_initialize(test::LaserLineFixture* p,const LaserLineParameters* args){std::memset(&p->laser,0,sizeof p->laser);p->laser.base.initialize(&p->rate);return laser_line_initialize(p->laser,*args,p->resource,p->animations,*p);}
API(ll_update) i32 ll_update(test::LaserLineFixture* p){return laser_line_update(p->laser,p->animations,*p);}
API(ll_draw) u32 ll_draw(test::LaserLineFixture* p){p->render.graphics.data.clear();p->render.renderer.invalidate();if(!laser_line_draw(p->laser,p->render.renderer))return 0;p->render.renderer.flush();return 1;}
API(ll_render) test::RenderFixture* ll_render(test::LaserLineFixture* p){return &p->render;}
API(ll_anim_rate) float* ll_anim_rate(test::LaserLineFixture* p){return &p->animations.rate;}
API(ll_rng) Rng* ll_rng(test::LaserLineFixture* p,u32 visual){return visual?&p->animations.visual_rng:&p->animations.script_rng;}
API(li_data) void* li_data(test::LaserLineFixture* p,u32 k){return k?static_cast<void*>(&p->boss):&p->infinite;}
API(li_initialize) u32 li_initialize(test::LaserLineFixture* p,const LaserInfiniteParameters* args){std::memset(&p->infinite,0,sizeof p->infinite);p->infinite.base.initialize(&p->rate);return laser_infinite_initialize(p->infinite,*args,p->resource,p->animations,*p);}
API(li_update) i32 li_update(test::LaserLineFixture* p,u32 boss_active){p->boss_active=boss_active;return laser_infinite_update(p->infinite,p->animations,*p);}
API(li_tick) void li_tick(test::LaserLineFixture* p){p->infinite.base.lifetime.tick();}
API(li_cancel) i32 li_cancel(test::LaserLineFixture* p,u32 reward,u32 skip){return laser_infinite_cancel(p->infinite,reward,skip,*p);}
API(li_cut_rectangle) i32 li_cut_rectangle(test::LaserLineFixture* p,const Vec3* center,const Vec3* size,u32 reward,u32 skip){return laser_infinite_cut_rectangle(p->infinite,*center,*size,reward,skip,*p);}
API(li_cut_circle) i32 li_cut_circle(test::LaserLineFixture* p,const Vec3* center,float radius,u32 reward,u32 skip){return laser_infinite_cut_circle(p->infinite,*center,radius,reward,skip,*p);}
API(laser_probe) i32 laser_probe(const LaserState* p,const Vec2* center,float radius){return p->probe(*center,radius);}
API(li_draw) u32 li_draw(test::LaserLineFixture* p){p->render.graphics.data.clear();p->render.renderer.invalidate();if(!laser_infinite_draw(p->infinite,p->render.renderer))return 0;p->render.renderer.flush();return 1;}
API(laser_fixture_create) test::LaserFixture* laser_fixture_create(){return new test::LaserFixture;}
API(laser_fixture_delete) void laser_fixture_delete(test::LaserFixture* p){delete p;}
API(laser_fixture_data) void* laser_fixture_data(test::LaserFixture* p,u32 k){if(k==0)return &p->laser;if(k==1)return &p->rate;if(k==2)return &p->player;if(k==3)return p->events.data();return p->emissions.data();}
API(laser_fixture_count) u32 laser_fixture_count(test::LaserFixture* p,u32 k){if(k==2){p->events.clear();p->emissions.clear();}return k==1?p->emissions.size():p->events.size();}
API(laser_initialize) void laser_initialize(test::LaserFixture* p){p->laser.base.initialize(&p->rate);}
API(laser_transforms) i32 laser_transforms(test::LaserFixture* p){return laser_line_transforms(p->laser,*p);}
API(laser_motion) i32 laser_motion(test::LaserFixture* p,u32 k){return laser_line_motion(p->laser,k,*p);}
API(laser_cancel) i32 laser_cancel(test::LaserFixture* p,u32 reward,u32 skip){return laser_line_cancel(p->laser,reward,skip,*p);}
API(laser_cut_rectangle) i32 laser_cut_rectangle(test::LaserFixture* p,const Vec3* center,const Vec3* size,u32 reward,u32 skip){return laser_line_cut_rectangle(p->laser,*center,*size,reward,skip,*p);}
API(laser_cut_circle) i32 laser_cut_circle(test::LaserFixture* p,const Vec3* center,float radius,u32 reward,u32 skip){return laser_line_cut_circle(p->laser,*center,radius,reward,skip,*p);}
#include "../../cpp/game/TextRaster.hpp"
API(text_raster_create) TextRaster* text_raster_create(){return new TextRaster;}
API(text_raster_delete) void text_raster_delete(TextRaster* p){delete p;}
API(text_raster_load) u32 text_raster_load(TextRaster* p,u32 slot,const u8* bytes,u32 size){return p->glyphs.load(slot,bytes,size);}
API(text_raster_draw) u32 text_raster_draw(TextRaster* p,const char* text,const TextStyle* style){return p->rasterize(text,*style);}
API(text_raster_data) u8* text_raster_data(TextRaster* p){return p->scratch.pixels.data();}
API(text_glyph_draw) u32 text_glyph_draw(TextRaster* p,const char* text,const TextStyle* style){return p->glyphs.draw(p->scratch,style->offset,style->height,style->font,style->color,text);}
API(image_resample) u32 image_resample(u32 format,u32 w,u32 h,u32 tw,u32 th,const u8* data,u8* out){
    TextureImage source,target;AnmTexture description;description.kind=AnmTexture::Kind::Blank;description.format=format;description.width=w;description.height=h;if(!source.load(description))return 0;
    std::memcpy(source.pixels.data(),data,source.pixels.size());description.width=tw;description.height=th;if(!target.load(description))return 0;
    if(!ImageResample::triangle(target,{0,0,i32(tw),i32(th)},source,{0,0,i32(w),i32(h)}))return 0;std::memcpy(out,target.pixels.data(),target.pixels.size());return target.pixels.size();
}
#include "sound-fixture.hpp"
API(sound_fixture) test::SoundFixture* sound_fixture(){return new test::SoundFixture;}
API(sound_delete) void sound_delete(test::SoundFixture* p){delete p;}
API(sound_queue) SoundQueue* sound_queue(test::SoundFixture* p){return &p->effects.state;}
API(sound_enqueue) void sound_enqueue(test::SoundFixture* p,i32 id,i32 pan){p->effects.enqueue(id,pan);}
API(sound_positioned) void sound_positioned(test::SoundFixture* p,i32 id,float x){p->effects.positioned(id,x);}
API(sound_stop) void sound_stop(test::SoundFixture* p,i32 id){p->effects.stop(id);}
API(sound_process) void sound_process(test::SoundFixture* p,i32 master,u32 initialized,u32 enabled){p->calls.clear();p->effects.master_volume=master;p->effects.initialized=initialized;p->effects.enabled=enabled;p->effects.process();}
API(sound_calls) void* sound_calls(test::SoundFixture* p){return p->calls.data();}
API(sound_call_count) u32 sound_call_count(test::SoundFixture* p){return p->calls.size();}
API(sound_definitions_data) const void* sound_definitions_data(){return sound_definitions;}
API(sound_sample) const char* sound_sample(u32 n){return n<46?sound_samples[n]:nullptr;}
#include "../../cpp/game/GraphicsMath.hpp"
API(graphics_math) void graphics_math(u32 kind,void* out,const void* a,const void* b,const void* d){
    if(kind==0)*static_cast<Vec3*>(out)=GraphicsMath::normalize(*static_cast<const Vec3*>(a));
    if(kind==1)*static_cast<Matrix4*>(out)=GraphicsMath::multiply(*static_cast<const Matrix4*>(a),*static_cast<const Matrix4*>(b));
    if(kind==2){auto* v=static_cast<const float*>(a);*static_cast<Matrix4*>(out)=GraphicsMath::rotation(u32(v[0]),v[1]);}
    if(kind==3)*static_cast<Matrix4*>(out)=GraphicsMath::look_at(*static_cast<const Vec3*>(a),*static_cast<const Vec3*>(b),*static_cast<const Vec3*>(d));
    if(kind==4){auto* v=static_cast<const float*>(a);*static_cast<Matrix4*>(out)=GraphicsMath::perspective(v[0],v[1],v[2],v[3]);}
}
API(graphics_project) void graphics_project(Vec3* out,const Vec3* p,const GraphicsViewport* v,const Matrix4* projection,const Matrix4* view,const Matrix4* world){*out=GraphicsMath::project(*p,*v,*projection,*view,*world);}
API(anm_project) int anm_project(AnmVm* vm,AnmCamera* camera,Vec3* out,u32 kind){return kind?anm_projected_quad(*vm,*camera,*reinterpret_cast<Vec3(*)[4]>(out)):anm_billboard_quad(*vm,*camera,*reinterpret_cast<Vec3(*)[4]>(out));}
API(render_camera) AnmCamera* render_camera(test::RenderFixture* p){return &p->renderer.camera;}
API(render_ripple) int render_ripple(test::RenderFixture* p,AnmVm* v){v->resource=&p->resource;v->sprite=&p->sprite;return p->renderer.draw_ripple(*v);}
API(anm_world) void anm_world(AnmVm* vm,Matrix4* out,u32 anchors){*out=anm_world_matrix(*vm,anchors);}
API(render_fog) void render_fog(test::RenderFixture* p,const float* v){p->renderer.fog_origin={v[0],v[1],v[2]};p->renderer.fog_channels={v[3],v[4],v[5]};p->renderer.fog_start=v[6];}
API(render_matrix) Matrix4* render_matrix(test::RenderFixture* p,u32 uv){return uv?&p->graphics.uv:&p->graphics.world;}
API(render_pipeline) u32 render_pipeline(test::RenderFixture* p,u32 k){return k==0?p->graphics.state.depthWrite:k==1?p->graphics.state.textureFactor:k==2?p->graphics.layout:k==3?p->graphics.topology:p->graphics.state.textureTransform;}
API(texture_decode) int texture_decode(u32 format,u32 w,u32 h,const u8* data,u32 size,u8* out){AnmTexture t;t.pixel_format=u16(format);t.pixel_width=u16(w);t.pixel_height=u16(h);t.pixels.assign(data,data+size);std::vector<u8> rgba;if(!t.rgba(rgba))return 0;std::memcpy(out,rgba.data(),rgba.size());return 1;}
#include "../../cpp/game/TextureImage.hpp"
API(texture_image) u32 texture_image(u32 input,u32 output,u32 width,u32 height,u32 target_width,u32 target_height,const u8* data,u32 size,u8* out){AnmTexture t;t.format=u16(output);t.width=u16(target_width);t.height=u16(target_height);t.pixel_format=u16(input);t.pixel_width=u16(width);t.pixel_height=u16(height);t.pixels.assign(data,data+size);TextureImage image;if(!image.load(t))return 0;std::memcpy(out,image.pixels.data(),image.pixels.size());return image.pixels.size();}
