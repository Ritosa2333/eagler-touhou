#include "GameApplication.hpp"
#include "Presentation.hpp"
#include <algorithm>
#include <cstdio>
namespace th08 {
namespace {
u32 flag_bits(const TitleGameFlags& flags){u32 value;std::memcpy(&value,&flags,4);return value;}
void set_flags(TitleGameFlags& flags,u32 value){std::memcpy(&flags,&value,4);}
std::string replay_path(i32 slot){char path[64];std::snprintf(path,sizeof(path),"replay/th8_%02d.rpy",slot);return path;}
}
GameApplication::GameApplication(ApplicationPlatform& p,SpriteBackend& backend)
 :platform(p),library(textures),renderer(backend),animations(session.random),ascii(animations,renderer,p),screen(chain,renderer,session.random),loading(library,animations,renderer,ascii,loading_io),statistics(ascii,p),supervisor(renderer,supervisor_io),
 title(session,library,renderer,ascii,ascii_context,p,screen,title_io),game(session,textures,library,renderer,game_io,&chain,&ascii),results(session,library,renderer,ascii,p,result_io),music(animations,renderer,ascii,p,music_io),ending(library,animations,renderer,p,ending_io){}
AnmLoaded* GameApplication::load_animation(i32 slot,const char* path){const auto bytes=platform.read(path);return library.load(slot,bytes.data(),bytes.size());}
bool GameApplication::initialize(u32 frequency){
    if(initialized)return false;failed=stopping=false;animations.invalid=false;
    const auto config=platform.read("th08.cfg"),wave=platform.read_prefix("thbgm.dat",16);
    if(!load_configuration(session.display_config,config.data(),config.size(),wave.empty()?nullptr:wave.data(),wave.size()))return false;
    if(!platform.write("th08.cfg",reinterpret_cast<const u8*>(&session.display_config),sizeof(session.display_config)))return false;
    const auto& options=session.display_config.options;library.force_16bit=options&4;renderer.vertex_buffer_disabled=options&2;renderer.depth_test_disabled=options&64;renderer.color_compositing_disabled=options&256;renderer.fog_disabled=options&1024;ascii_context.fog_disabled=renderer.fog_disabled;
    session.statistics={};session.statistics.header={fourcc('P','L','S','T'),sizeof(PlayRecord),sizeof(PlayRecord),2,0,0};
    const auto bytes=platform.read("score.dat");ScoreFile score;score.decode(bytes.data(),bytes.size());
    score.copy_chapter(fourcc('P','L','S','T'),2,&session.statistics,sizeof(session.statistics),false);score.clears(session.clears);score.practice(session.practices);score.spells(session.records);score.copy_chapter(fourcc('F','L','S','P'),1,&session.last_words,sizeof(session.last_words),true);
    std::memcpy(title.context.clears,session.clears,sizeof(session.clears));title.context.flags.isExtraUnlocked=title.context.IsExtraUnlocked();title.context.flags.isSpellPracticeUnlocked=title.context.IsSpellPracticeUnlocked();title.context.flags.isExtraUnlockedWithAllTeams=title.context.IsExtraUnlockedWithAllTeams();
    version=platform.read("th08_0100d.ver");if(version.empty())return false;
    if(!platform.load_surface(8,"title/th08logo.jpg"))return false;
    if(!load_animation(2,"nowloading.anm"))return false;
    show_loading({500,440,0},false);session.total_clock=platform.milliseconds();session.random.seed=u16(session.total_clock);
    if(!load_animation(0,"text.anm")||!load_animation(1,"ascii.anm")||!load_animation(3,"capture.anm"))return false;
    ascii.state.ascii=library.get(1);ascii.state.capture=library.get(3);ascii.initialize_vms(ascii_context);
    platform.apply_volume(session.display_config);if(!(options&8192))platform.start_bgm();
    statistics.context.frequency=frequency;supervisor.state.active=0;supervisor.state.target=1;bind_jobs();initialized=running=true;return !invalid();
}
void GameApplication::bind_jobs(){
    const auto bind=[&](ChainElement& job,i32 priority,bool draw,JobCallback call){job.set_callback(call);job.argument=this;chain.add(&job,priority,draw);};
    bind(supervisor_job,0,false,[](void* p){return static_cast<GameApplication*>(p)->update_supervisor();});
    bind(ascii_calc,1,false,[](void* p){auto& a=*static_cast<GameApplication*>(p);if(!a.game_attached){a.ascii.tick_popups(a.ascii_context,a.timing);a.ascii.tick_vms(false);}return JobResult::Continue;});
    bind(background_draw,0,true,[](void* p){auto& a=*static_cast<GameApplication*>(p);a.loading.background();return JobResult::Continue;});
    bind(loading_draw,2,true,[](void* p){static_cast<GameApplication*>(p)->loading.draw();return JobResult::Continue;});
    bind(fps_draw,16,true,[](void* p){auto& a=*static_cast<GameApplication*>(p);if(presentation::render_only)a.statistics.draw_text();else a.statistics.calculate(true);return JobResult::Continue;});
    bind(ascii_draw,20,true,[](void* p){auto& a=*static_cast<GameApplication*>(p);if(!a.game_attached){a.ascii.draw_strings(a.ascii_context);a.ascii.state.string_count=0;}return JobResult::Continue;});
}
void GameApplication::start_effect(){if(!transition_effect)transition_effect=screen.create(ScreenEffectType::MenuFullFade,60,0,0,0,1);}
void GameApplication::finish_effect(){if(transition_effect){transition_effect->phase=1;transition_effect->timer.set(0);transition_effect=nullptr;}}
void GameApplication::show_loading(const Vec3& p,bool capture){failed|=!loading.show(p,capture);}
void GameApplication::fade_loading(){loading.fade();finish_effect();}
bool GameApplication::enter_title(bool replay_finished){
    session.practice.menu=session.practice.accepted=false;
    music.detach();results.detach();title.detach();
    title.context.supervisor_state=1;title.context.supervisor_previous=supervisor.state.previous;
    if(replay_finished)title.context.flags.isReplay=true;
    ascii_context=AsciiContext{};ascii_context.fog_disabled=renderer.fog_disabled;timing={1,false};
    return title.attach(chain);
}
bool GameApplication::enter_game(){
    const auto target=Scene(supervisor.state.target);const bool from_title=supervisor.state.previous==i32(Scene::Title);
    const bool initial=target!=Scene::Reinitialize&&target!=Scene::SpellRestart&&target!=Scene::NextStage;
    auto& g=game.globals;
    if(from_title){const auto& c=title.context;g.stage=c.currentStage;g.difficulty=c.difficulty;g.shot=c.character;g.current_spell=c.currentSpellCardNumber;g.game_flags=flag_bits(c.flags);supervisor.state.stage=g.stage;supervisor.state.difficulty=g.difficulty;supervisor.state.practice=g.game_flags&1;}
    else if(target==Scene::Restart)g.stage=supervisor.state.stage;
    const auto request=GameplayLoad{i32(g.stage),i32(g.difficulty),g.shot,g.current_spell,g.game_flags,initial,supervisor.state.keep_resources,initial,i32(target)};
    if(from_title){
        auto& p=session.practice;p.cheats=0;p.assisted=false;p.replay=bool(g.game_flags&8);p.active=false;
        if(p.replay){const auto bytes=platform.read(title.context.replayFilename);
            // THGuiRep::State(3) already copied the replay's PRAC parameters
            // into run at the title menu. Without them (vanilla replay, an
            // unreadable block, or thprac disabled) playback is just Original;
            // attract demos never go through the replay menu and stay vanilla.
            if(!game.load_replay(bytes.data(),bytes.size())){platform.replay_error();return false;}
            p.active=p.enabled&&p.run.mode==1&&!(g.game_flags&2);
        }else p.active=p.enabled&&(g.game_flags&1)&&!(g.game_flags&0x4002)&&p.run.mode==1;
    }
    title.detach();show_loading(from_title?Vec3{500,440,0}:Vec3{280,430,0},true);if(from_title)start_effect();
    GameplayLoad prepared=request;if((prepared.flags&0x60)>=0x40)prepared.flags=(prepared.flags&~0x60u)|0x20;
    if(!game.load(prepared,true))return false;game_attached=true;game.menus.context.supervisor_state=2;game.control.state.load_state=1;game.control.state.replay_mode=title.context.replayMode;game.control.state.demo_index=title.context.currentDemoReplay;
    // The platform implements the original capture request. Enable the ANM
    // pause/retry background after load() resets the menu context each stage.
    game.menus.context.lockable_backbuffer=true;
    if(target!=Scene::Reinitialize)statistics.state.rendered=statistics.state.total=0;
    synchronize();statistics.calculate(false);loading_gate=true;loading_hidden=false;return true;
}
ResultContext GameApplication::result_context()const{
    const auto& g=game.globals;ResultContext c;c.character=g.shot;c.difficulty=g.difficulty;c.stage=g.stage;c.flags=g.game_flags;c.slow_mode=session.config.slow_mode;c.cheat_movement_used=platform.cheat_movement_used();c.play_frames=game.control.state.frames;c.human_frames=game.enemies.state.unfocused_frames;c.active_frames=game.enemies.state.active_frames;c.rendered_frames=statistics.state.rendered;c.total_frames=statistics.state.total;c.total_game_frames=game.enemies.state.frames;c.software_texturing=ascii_context.software_texturing;c.supervisor_state=supervisor.state.target;return c;
}
void GameApplication::leave_game(){
    if(!game_attached)return;last_game=result_context();const auto target=Scene(supervisor.state.target);const bool release=target!=Scene::Reinitialize&&target!=Scene::SpellRestart&&target!=Scene::NextStage;
    game.screen_counter=1;ascii.state.blindness_color=0;
    if(!(game.globals.game_flags&0x4000)||release){platform.stop_audio();if(session.display_config.music==2)platform.midi_reset();}platform.process_sounds();
    game.unload(supervisor.state.keep_resources,release);game_attached=false;loading_gate=false;
    if(!(game.globals.game_flags&8))accumulate_play_time(session.statistics.game_time,game.menus.context.system_time,platform.milliseconds());game.menus.context.system_time=0;results.scores.update_time(platform.milliseconds());
    game.globals.game_flags&=~4u;ascii.reset();game.control.state.sticky_input=false;
    session.numbers.score=std::min(session.numbers.score,999999999u);session.numbers.display_score=session.numbers.score;timing={1,false};
}
bool GameApplication::enter_results(bool from_game){title.detach();music.detach();auto context=from_game?last_game:result_context();context.supervisor_state=from_game?6:5;return results.attach(chain,from_game?RESULT_SCREEN_ACTION_GAME_RESULTS:RESULT_SCREEN_ACTION_TITLESCREEN,context);}
bool GameApplication::enter_music(){title.detach();music.context={};music.context.text=library.get(0);music.context.preload=session.display_config.options&8192;music.context.software_texturing=ascii_context.software_texturing;music.context.supervisor_state=8;return music.attach(chain);}
bool GameApplication::enter_ending(){title.detach();if(!load_animation(24,"staff01.anm"))return false;return ending.attach(chain,session,game.globals.shot,game.globals.difficulty,game.globals.stage,game.globals.game_flags);}
void GameApplication::publish_scene(){
    auto& s=supervisor.state;
    switch(Scene(s.active)){
    case Scene::Title:s.target=title.context.supervisor_state;break;
    case Scene::Game:
        s.target=game.menus.context.supervisor_state;s.stage=game.globals.stage;s.difficulty=game.globals.difficulty;s.practice=game.globals.game_flags&1;s.sticky_input=game.control.state.sticky_input;
        set_flags(title.context.flags,game.globals.game_flags);title.context.character=game.globals.shot;title.context.difficulty=game.globals.difficulty;title.context.currentStage=game.globals.stage;break;
    case Scene::Results:case Scene::GameResults:s.target=results.controls.context.supervisor_state;break;
    case Scene::Music:s.target=music.context.supervisor_state;break;
    default:break;
    }
}
void GameApplication::synchronize(){
    if(game_attached)timing=game.player.timing;
    animations.timing=timing;const auto& input=supervisor.input;title.input(input,timing);results.input(input,timing);music.context.keys=input.current;music.context.previous=input.previous;music.context.scrolling=input.scrolling;ending.set_input(input.current,input.previous);
    screen.context.timing=timing;screen.context.paused=game_attached&&game.paused;screen.context.retry=game_attached&&game.retrying;screen.context.terminating=!running;screen.context.transition_state=supervisor.state.target;
    auto& f=statistics.context;f.game_flags=game_attached?game.globals.game_flags:flag_bits(title.context.flags);f.paused=game_attached&&game.paused;f.frameskip=session.display_config.frameskip;f.software_texturing=ascii_context.software_texturing;
    if(game_attached){statistics.state.replay_fps=game.globals.game_flags&8?game.playback.input.timing_level:statistics.state.replay_fps;game.menus.context.timing_level=statistics.state.replay_fps;}
}
void GameApplication::finish_loading(){
    if(!loading_gate)return;
    if(!loading_hidden){const i32 minimum=game.globals.game_flags&8?80:30;if(game.control.state.load_frames<minimum)return;loading.hide();finish_effect();loading_hidden=true;}
    if(game.globals.game_flags&0x60)return;
    game.control.state.load_state=0;game.globals.game_flags&=~512u;supervisor.state.keep_resources=false;game.screen_counter=2;loading_gate=false;
}
JobResult GameApplication::update_supervisor(){
    publish_scene();const auto result=supervisor.update();if(result!=JobResult::Continue)return result;
    synchronize();if(title.modal())return JobResult::Break;
    if(game_attached){finish_loading();if(!game.prepare_frame(supervisor.input.current,timing.rate,timing.force_step))return JobResult::Error;}
    return invalid()?JobResult::Error:JobResult::Continue;
}
bool GameApplication::update(){
    if(!running||invalid())return false;
    // ECL callbacks 18/28/29 write the game's persistent global time scale.
    // The browser's next presentation does not reset it to a unit timestep.
    if(game_attached)timing=game.player.timing;animations.timing=timing;
    if(title.modal()){title.modal_step();return !invalid();}
    platform.begin_frame();const i32 value=chain.run();publish_scene();synchronize();
    if(value<=0){failed|=value<0;running=false;}failed|=invalid();return running&&!failed;
}
bool GameApplication::draw(float presentation_alpha,bool presentation_active,bool presentation_only,bool world_interpolate){
    if(!running||invalid())return false;if(title.modal())return true;
    const auto string_count=ascii.state.string_count;const u32 ascii_color=ascii.state.color;const float ascii_scale_x=ascii.state.scale_x,ascii_scale_y=ascii.state.scale_y;const i32 ascii_gui=ascii.state.gui,ascii_selected=ascii.state.selected,ascii_space=ascii.state.space_width;const Vec2 saved_shake=renderer.shake;
    if(presentation_only&&presentation_shake_valid)renderer.shake=presentation_shake;else if(!presentation_only){presentation_shake=renderer.shake;presentation_shake_valid=true;}
    presentation::begin(presentation_alpha,presentation_active,presentation_only,world_interpolate);
    const i32 value=chain.run(true);renderer.flush();if(presentation_only){ascii.state.string_count=string_count;ascii.state.color=ascii_color;ascii.state.scale_x=ascii_scale_x;ascii.state.scale_y=ascii_scale_y;ascii.state.gui=ascii_gui;ascii.state.selected=ascii_selected;ascii.state.space_width=ascii_space;}
    if(!presentation_only&&value<=0){failed|=value<0;running=false;}
    if(!presentation_only){if(game_attached&&!(game.globals.game_flags&8))game.recording.input.timing_level=statistics.state.replay_fps;platform.process_sounds();}
    if(!platform.present())platform.reset_device();if(presentation_only)renderer.shake=saved_shake;presentation::end();if(!presentation_only)failed|=invalid();return running&&!failed;
}
bool GameApplication::save_score(){auto context=game_attached?result_context():last_game;return results.attach(chain,RESULT_SCREEN_ACTION_SAVE_SCORE,context);}
// ResultScreen indexes rows 0..14; replay filenames and the public save API use 1..15.
std::vector<u8> GameApplication::ResultIo::read_replay(i32 slot){const auto path=replay_path(slot+1);return a.platform.read(path.c_str());}
void GameApplication::save_replay(i32 slot,const char* name){
    if(slot<1||slot>15||!name||!game.recording.ready())return;ReplayExportContext context;std::memcpy(context.player_name,name,std::min<std::size_t>(8,std::strlen(name)));platform.calendar(context.date,context.timestamp);
    context.rendered_frames=last_game.rendered_frames;context.total_frames=last_game.total_frames;context.human_frames=last_game.human_frames;context.active_frames=last_game.active_frames;context.cheat_movement_used=last_game.cheat_movement_used;
    const auto bytes=export_replay(game.recording,session,game.globals,context);const auto path=replay_path(slot);failed|=bytes.empty()||!platform.write(path.c_str(),bytes.data(),bytes.size());
}
void GameApplication::export_records(){char date[6]{},stamp[20]{};platform.calendar(date,stamp);const auto text=score_report(session,results.scores,stamp,platform.milliseconds());failed|=!platform.write("score.txt",reinterpret_cast<const u8*>(text.data()),text.size());}
bool GameApplication::MusicIo::load(MusicRoom& room){if(!a.platform.load_surface(0,"result/music.jpg"))return false;auto* animation=a.load_animation(23,"music00.anm");if(!animation)return false;const auto bytes=a.platform.read("sprt/musiccmt.txt");return room.initialize(*animation,bytes.data(),bytes.size(),a.session.statistics.music_unlocked);}
void GameApplication::MusicIo::release(){a.renderer.flush();a.library.release(23);a.platform.release_surface(0);}
void GameApplication::shutdown(){
    if(stopping)return;stopping=true;
    if(initialized){if(game_attached){supervisor.state.target=-1;leave_game();save_score();}title.detach();results.detach();music.detach();ending.detach();platform.write("th08.cfg",reinterpret_cast<const u8*>(&session.display_config),sizeof(session.display_config));}
    finish_effect();screen.clear();chain.release();renderer.flush();platform.discard_graphics();for(i32 i=0;i<256;i++)library.release(i);platform.release_surface(8);platform.stop_audio();if(session.display_config.music==2)platform.midi_reset();game.recording.reset();initialized=running=false;
}
}
