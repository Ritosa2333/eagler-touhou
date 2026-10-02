// SDL application integration under validation. Test-only entry points are
// compiled behind TH09_DEVELOPMENT_HARNESS; no original executable is linked.
#include "Assets.hpp"
#include "AudioDevice.hpp"
#include "FontDevice.hpp"
#include "../game/GamePresentation.hpp"
#include "../game/TitleMenus.hpp"
#include "../game/GameSession.hpp"
#include "../game/MusicCatalog.hpp"
#include "../game/GameConfiguration.hpp"
#include "../game/KeyboardInput.hpp"
#include "../game/NetworkInput.hpp"
#include "../multiplayer/WorldState.hpp"
#include "../multiplayer/RollbackSession.hpp"
#include "../multiplayer/FrameSchedule.hpp"
#include <dirent.h>
#include <ctime>
#include <algorithm>
#include <deque>
#include <iterator>
#include "../../../portable/input/TouchController.hpp"
#include <emscripten.h>
#include <emscripten/html5.h>
#include "../../../portable/sdl/FrameCadence.hpp"
// DOM keys have one owner in updated shells; preserve older-shell input.
EM_JS(int, th09_browser_keyboard, (), {return typeof Module["resetBrowserKeyboard"]==='function';});
EM_JS(void, th09_reset_browser_keyboard, (), {Module["resetBrowserKeyboard"]?.();});

Netplay::PeerTransport& th09_shared_transport();
EM_JS(void, th09_browser_frame, (int ok,double milliseconds), { Module["onGameFrame"]?.(ok,milliseconds); });
EM_JS(void, th09_network_result, (), { Module["onNetworkResult"]?.(); });
EM_JS(void, th09_network_request, (), { Module["onNetworkRequest"]?.(); });
EM_JS(void, th09_network_send, (unsigned frame,unsigned keys,int moving,float x,float y), { Module["onNetworkInput"]?.(frame,keys,moving,x,y); });
EM_JS(void, th09_network_spectator_frame,
      (unsigned frame,unsigned left,unsigned right,int leftMode,float leftX,float leftY,int rightMode,float rightX,float rightY), {
    Module["onNetworkSpectatorFrame"]?.(frame,left,right,leftMode,leftX,leftY,rightMode,rightX,rightY);
});
EM_JS(int, th09_keyboard_gamepad_dpad, (), {
    if (!navigator.getGamepads) return 0;
    let bits = 0;
    for (const pad of navigator.getGamepads()) {
        if (!pad || !pad.buttons || pad.buttons.length < 16) continue;
        if (!/keyboard|\bkb\b/i.test(String(pad.id || 0))) continue;
        if (pad.buttons[12]?.pressed) bits |= 1;
        if (pad.buttons[13]?.pressed) bits |= 2;
        if (pad.buttons[14]?.pressed) bits |= 4;
        if (pad.buttons[15]?.pressed) bits |= 8;
    }
    return bits;
});
#include <memory>
#if TH09_DEVELOPMENT_HARNESS
#include "../../tests/world-snapshot.hpp"
#endif
namespace th09::sdl {
namespace {
i32 joy_button(i32);
NetworkInput network;
multiplayer::RollbackSession rollback(th09_shared_transport());
bool rollback_active=false;
Netplay::AdonisMode adonis_mode=Netplay::AdonisMode::Rollback;
u32 adonis_snapshots=0,adonis_waits=0;
u32 rollback_published=0,rollback_catchup=0;
struct RollbackFrame {
    u32 number=Netplay::INVALID_FRAME,hash=0;
    std::unique_ptr<multiplayer::WorldState> state;
    bool captured=false;
    struct Sound {i32 kind,id;float value;};
    std::vector<Sound> sounds;
};
std::array<RollbackFrame,multiplayer::RollbackSession::History+1> rollback_frames;
RollbackFrame* recording_frame=nullptr;
#if TH09_DEVELOPMENT_HARNESS
u32 rollback_test_limit=Netplay::INVALID_FRAME;
double probe_tick_cost[2]{}; // Update/UI and semantic Draw; diagnostic only.
bool probe_full_sprite_geometry=false;
#endif
struct SpectatorFrame { u32 frame=0;u16 keys[2]{};NetworkInput::Motion motion[2]; };
std::deque<SpectatorFrame> spectator_frames;
bool spectator_mode=false;
u32 spectator_next=0,spectator_simulated=0;
struct Application final:GameMedia,InGameMenuServices,TitleServices,EndingServices {
    Assets assets;GraphicsDevice graphics;FontDevice fonts{graphics};AudioDevice audio;
    EclWorldState state;AnmExecutor executor{state.random};GameResources resources{assets,graphics,executor};
    GamePresentation presentation{graphics,resources,*this};InGameMenus menus{resources,*this};
    std::array<u8,2> local_focus{};i32 local_versus=1;
    void release_network(){network.end();settings.auto_focus=local_focus;settings.versus=local_versus;sync_records();}
    PlayerRecords records;bool clock_running=true,host_music_enabled=true;TitleSettings settings;GameConfiguration saved_configuration;u32 storage_revision=0;std::vector<u8> import_bytes;std::unique_ptr<TitleMenus> title;bool in_title=false,launch_pending=false;std::map<std::string,u32> title_images;u32 background_image=0;
    std::unique_ptr<GameSession> session;GameWorld* world=nullptr;WorldConfiguration configuration;InputFrame device;
    ReplayFile pending_replay;u32 replay_stage=0;bool replay_pending=false,demo_pending=false;u32 ending_image=0,ending_width=640,ending_height=480;
    bool paused=false,over=false,complete=false;u32 frames=0;std::string error;Vec2 shake[3]{};
    bool checkpoint(multiplayer::WorldState& out){
        presentation.renderer.flush();graphics.backend.flush();
        if(!out.Save(*session))return false;
        out.Fields(device,left_device,right_device,paused,over,complete,frames,shake,pending_action,requested_transition,mode,difficulty,game_flags,continues,capture_enabled);
        out.Text(error);
        // Records and wall clocks are committed output, outside rollback.
        out.Fields(menus.pause,menus.game_over,menus.match_end,presentation.offsets,presentation.active_field,presentation.stage_state);
        auto& a=presentation.ascii;out.Fields(a.glyph,a.digit,a.queue,a.popups,a.count,a.color,a.scale,a.field_view,a.spacing,a.popup_cursor);
        auto& r=presentation.renderer;out.Fields(r.state,r.view,r.shake,r.tint,r.tint_enabled,r.quad,r.texture,r.blend_mode,r.depth_disabled,r.camera,r.last_world,r.texture_matrix,r.current_sprite,r.background_camera_data,r.camera_mode,r.world_vertices);
        auto& b=presentation.stages;out.Fields(b.geometry,b.side,b.fog_supported,b.tint,b.tint_enabled,b.field_view,b.culling_camera);
        out.Fields(graphics.backend.state);
        return out.Good();
    }
    bool queue_sound(i32 kind,i32 id,float value=0){if(!recording_frame)return false;recording_frame->sounds.push_back({kind,id,value});return true;}
    void pause_audio(bool on){if(!queue_sound(4,on))audio.pause_music(on);}
    bool initialize_platform(){if(!graphics.initialize()){error=graphics.error;return false;}if(!assets.open("/th09.dat")){error=assets.error;return false;}if(!fonts.initialize()){error=fonts.error;return false;}fonts.prewarm_game(assets);if(!audio.initialize(assets)){error=audio.error;return false;}SDL_CreateDirectory("/save/replay");std::vector<u8> bytes;if(read_file("/save/th09.cfg",bytes))saved_configuration.load(bytes.data(),u32(bytes.size()));saved_configuration.apply(settings);title_configuration();if(!write_file("/save/th09.cfg",saved_configuration.data().data(),204)){error="Unable to save configuration";return false;}if(read_file("/save/score.dat",bytes))records.load(bytes.data(),u32(bytes.size()));sync_records();records.application_clock=records.game_clock=u32(SDL_GetTicks());return true;}
    void update_clocks(){const u32 now=u32(SDL_GetTicks());if(clock_running)records.update_application_clock(now);else records.application_clock=now;if(clock_running&&!in_title&&!paused&&!over&&!complete&&session&&session->phase==SessionPhase::match)records.update_game_clock(now);else records.game_clock=now;}
    void clock_pause(bool on){update_clocks();clock_running=!on;}
    bool open_title(){if(!initialize_platform())return false;state.random={0x7531,0,0};title=std::make_unique<TitleMenus>(resources,*this,state.random,settings);if(!title->initialize()||!resources.load(AnimationFile::ascii,"ascii.anm")||!presentation.ascii.initialize()){error="Title initialization "+title->error+resources.error;return false;}in_title=true;return true;}
    bool open(i32 left,i32 right,i32 mode,i32 diff){
        if(!initialize_platform())return false;
        state.random={0x7531,0,0};configuration.selection.characters[0]=left;configuration.selection.characters[1]=right;configuration.selection.mode=GameMode(mode);configuration.selection.difficulty=diff;configuration.selection.lives=2;configuration.selection.selector=left;
        configuration.controllers[1]=1;return start();
    }
    bool start(){
        presentation.renderer.flush();presentation.world=nullptr;world=nullptr;session.reset();executor.invalid=false;
        session=std::make_unique<GameSession>(state,resources,executor,presentation,*this,records);
        bool ok=false;if(replay_pending){replay_pending=false;ok=session->play(pending_replay,replay_stage,demo_pending);}
        else {ReplayMetadata meta;meta.configuration=saved_configuration.data();meta.mode=u8(configuration.selection.mode);meta.difficulty=u8(configuration.selection.difficulty);// Native replays describe the two controller types; web transport is not
            // part of the recorded simulation. Network play is human vs human.
            meta.versus=u8(settings.versus==4?0:settings.versus);for(i32 side=0;side<2;++side){meta.health[side]=u8(configuration.health[side]);meta.alternate[side]=configuration.alternate[side];meta.configuration[0xb4+side]=configuration.automatic_focus[side];}meta.configuration[0xac]=u8(configuration.starting_extra_lives);
            const auto now=std::time(nullptr);char date[10]="--/--/--";if(const auto* local=std::localtime(&now))std::strftime(date,sizeof(date),"%y/%m/%d",local);ok=session->begin(configuration,meta,date);}
        if(!ok){error=session->error;return false;}world=session->world.get();presentation.world=world;
        if(!presentation.ascii.initialize()){error="ASCII initialization failed";return false;}paused=over=complete=false;menus.pause={};menus.game_over={};menus.match_end={};requested_transition=-1;return true;
    }
    bool render_text(AnmVm& a,const char* s,u32 color,u32 shadow)override{if(!fonts.text(a,s,color,shadow)){error=fonts.error;return false;}return true;}
    void sound(i32 id,i32 pan)override{if(!queue_sound(0,id,float(pan)))audio.effects.enqueue(id,pan);}
    void positioned_sound(i32 id,float x)override{if(!queue_sound(1,id,x))audio.effects.positioned(id,x);}
    void music(i32 n)override{if(queue_sound(2,n))return;if(!audio.music(n))error=audio.error;else if(const auto* entry=music_track(n))if(!session||!session->is_replay)records.music_unlocked[entry->unlock]=1;}
    void fade_music()override{if(!queue_sound(3,0))audio.fade_music();}
    void encountered(i32 c)override{if(!queue_sound(5,c)&&(!session||!session->is_replay))records.count_encounter(c);}
    void defeated(i32 c)override{if(!queue_sound(6,c)&&c>=0&&c<16&&(!session||!session->is_replay))records.versus_unlocked[c]=1;}
    void overlay()override{if(paused)menus.draw_pause();if(over)menus.draw_game_over();if(complete)menus.draw_match_end();}
    void menu_sound(i32 n)override{sound(n,0);}
    void menu_action(InGameAction action)override{
        if(action==InGameAction::resume){paused=false;world->paused=false;pause_audio(false);}
        else {pending_action=i32(action);}
    }
    void menu_view()override{presentation.begin_field(2);}
    void menu_sprite(AnmVm& a)override{presentation.renderer.draw_no_rotation(a);}
    i32 pending_action=-1;
    bool tick_title(u16 left,u16 right,u16 keys,bool render=true){
        update_clocks();device.update(keys);left_device.update(left);right_device.update(right);if(!title||!error.empty())return false;
        if(!launch_pending)title->update(left_device,right_device,device);if(network.active&&title->state.screen==TitleScreen::versus_type){release_network();title->state={};th09_network_result();}save_configuration();if(!title->error.empty()){error=title->error;return false;}
        if(launch_pending){launch_pending=false;in_title=false;if(!start())return false;}
        else{if(render)draw();audio.update();audio.pump();++frames;}return error.empty();
    }
    InputFrame left_device,right_device;
    bool title_background(const char* name)override{const auto it=title_images.find(name);if(it!=title_images.end()){background_image=it->second;return true;}std::vector<u8> bytes;if(!assets.read(name,bytes)){error="Missing title image "+std::string(name);return false;}const auto texture=graphics.image(bytes.data(),u32(bytes.size()));if(!texture.handle){error=graphics.error;return false;}background_image=title_images[name]=texture.handle;return true;}
    void title_sound(i32 id)override{sound(id,0);}
    void title_music(i32 id)override{music(id);}
    void title_music_file(const char* name)override{if(!audio.music_file(name))error=audio.error;}
    void title_music_pause(bool p)override{audio.pause_music(p);}
    void title_music_fade()override{audio.fade_music();}
    void title_text(AnmVm& a,const char* s,u32 c,u32 shadow)override{render_text(a,s,c,shadow);}
    void title_sprite(AnmVm& a,bool rotate)override{if(rotate)presentation.renderer.draw_2d(a);else presentation.renderer.draw_no_rotation(a);}
    void title_begin_draw()override{
        presentation.begin_field(2);presentation.renderer.flush();if(!background_image)return;PipelineState pipeline;pipeline.depthTest=false;pipeline.depthWrite=false;pipeline.fog=false;pipeline.blend=false;pipeline.color.operation=pipeline.alpha.operation=ColorOperation::First;pipeline.color.first=pipeline.alpha.first={ArgumentSource::Texture};
        const SpriteVertex corners[4]={{{-.5f,-.5f,0},1,0xffffffff,{0,0}},{{639.5f,-.5f,0},1,0xffffffff,{1,0}},{{-.5f,479.5f,0},1,0xffffffff,{0,1}},{{639.5f,479.5f,0},1,0xffffffff,{1,1}}};graphics.draw(pipeline,background_image,Topology::Strip,VertexLayout::ScreenColorUv,corners,4);
    }
    void title_configuration()override{audio.music_volume=settings.music_volume;audio.music_enabled=host_music_enabled&&settings.music_mode!=0;audio.effects.enabled=settings.effects!=0;audio.effects.master_volume=settings.sound_volume;audio.refresh_volume();}
    bool write_file(const char* path,const u8* bytes,u32 size){auto* file=SDL_IOFromFile(path,"wb");const bool ok=file&&SDL_WriteIO(file,bytes,size)==size;if(file)SDL_CloseIO(file);if(ok)++storage_revision;return ok;}
    void save_configuration(){if(network.active||spectator_mode)return;if(saved_configuration.capture(settings)){title_configuration();const auto& bytes=saved_configuration.data();if(!write_file("/save/th09.cfg",bytes.data(),u32(bytes.size())))error="Unable to save th09.cfg";}}
    u32 title_clear_count(i32 c,i32 d)override{return records.clear_count(c,d);}
    PlayerRecords& title_records()override{return records;}
    void title_save_records()override{if(spectator_mode)return;update_clocks();auto local_random=state.random;auto bytes=records.save(network.active?local_random:state.random);if(!write_file("/save/score.dat",bytes.data(),u32(bytes.size())))error="Unable to save score.dat";}
    void title_ascii(const Vec3& p,const char* text,u32 color,const Vec2& scale)override{presentation.ascii.color=color;presentation.ascii.scale=scale;presentation.ascii.field_view=0;presentation.ascii.text(p,text);}
    i32 title_joy_button(i32 device)override{return joy_button(device);}
    static std::string replay_path(const char* name){std::string path=name?name:"";if(path.rfind("./",0)==0)path.erase(0,2);if(path.rfind("replay/",0)!=0||path.find("..")!=std::string::npos||path.find('\\')!=std::string::npos||path.find('/',7)!=std::string::npos||path.size()>64)return {};return "/save/"+path;}
    bool title_read_replay(const char* name,std::vector<u8>& bytes)override{const auto path=replay_path(name);return !path.empty()&&read_file(path.c_str(),bytes);}
    std::vector<std::string> title_imported_replays()override{std::vector<std::string> names;auto* dir=opendir("/save/replay");if(dir){while(auto* entry=readdir(dir)){std::string n=entry->d_name;if(n.rfind("th9_ud",0)==0&&n.size()==14)names.push_back(n);}closedir(dir);}std::sort(names.begin(),names.end());return names;}
    bool title_save_replay(const char* name,const char* player)override{const auto path=replay_path(name);if(!session||path.empty())return false;auto replay=session->save_replay(player);const auto bytes=replay.encode();if(bytes.empty()){error=session->error;return false;}const bool ok=write_file(path.c_str(),bytes.data(),u32(bytes.size()));if(!ok)error="Unable to write replay";return ok;}
    bool title_replay_exists(const char* name)override{const auto path=replay_path(name);auto* file=path.empty()?nullptr:SDL_IOFromFile(path.c_str(),"rb");if(!file)return false;SDL_CloseIO(file);return true;}
    void title_play_replay(const ReplayFile& file,u32 stage,const char*)override{pending_replay=file;replay_stage=stage;replay_pending=launch_pending=true;demo_pending=false;}
    void title_launch(const WorldConfiguration& c)override{configuration=c;configuration.starting_extra_lives=saved_configuration.extra_lives();for(i32 n=0;n<2;++n)configuration.automatic_focus[n]=settings.auto_focus[n]!=0;launch_pending=true;}
    void title_demo(u32 index)override{char name[24];std::snprintf(name,sizeof(name),"demorpy%u.rpy",index%3);std::vector<u8> bytes;if(!assets.read(name,bytes)||!pending_replay.decode(bytes.data(),u32(bytes.size()))){error="Invalid demonstration replay";return;}replay_stage=9;replay_pending=launch_pending=demo_pending=true;}
    void title_network()override{th09_network_request();}
    void title_exit()override{title_save_records();save_configuration();requested_transition=0;}
    void sync_records(){settings.versus_unlocked=records.versus_unlocked;settings.story_unlocked=records.story_unlocked;settings.extra_unlocked=records.extra_unlocked;settings.music_unlocked=records.music_unlocked;}
    bool return_title(bool score){
        presentation.renderer.flush();const bool was_network=network.active;if(was_network)release_network();sync_records();title=std::make_unique<TitleMenus>(resources,*this,state.random,settings);if(!title->initialize()){error=title->error;return false;}
        title->leaving=false;in_title=true;paused=over=complete=false;pause_audio(false);
        if(session){settings.game_flags=session->world?session->world->battle->state.flags:0;if(session->is_replay)score=false;if(session->is_demo())settings.game_flags&=~10u;if(!session->recordable)settings.game_flags|=0x2000;title->score_candidate=session->result;title->result_mode=GameMode(session->result.difficulty==4?1:session->world?i32(session->world->rules.mode):0);settings.difficulty=session->result.difficulty;}
        title->state.screen=score?TitleScreen::score_name:TitleScreen::main;title->state.selection=0;device=left_device=right_device={};presentation.ascii.clear_text();title_save_records();if(was_network)th09_network_result();return true;
    }
    bool ending_picture(const char* path)override{std::string name=path?path:"";const auto slash=name.find_last_of("/\\");if(slash!=std::string::npos)name.erase(0,slash+1);std::vector<u8> bytes;if(!assets.read(name.c_str(),bytes))return false;const auto image=graphics.image(bytes.data(),u32(bytes.size()));if(!image.handle)return false;presentation.renderer.flush();if(ending_image)graphics.destroy(ending_image);ending_image=image.handle;ending_width=image.width;ending_height=image.height;return true;}
    void ending_music(i32 track)override{if(track<0||audio.statistics()[5]!=u32(track))music(track);}
    void ending_music_fade(i32 seconds)override{audio.fade_music(seconds*60);}
    void ending_text(AnmVm& vm,const char* text,u32 color)override{render_text(vm,text,color,0);}
    void ending_background(i32 x,i32 y)override{
        presentation.begin_field(2);presentation.renderer.flush();if(!ending_image)return;
        PipelineState pipeline;pipeline.depthTest=pipeline.depthWrite=pipeline.fog=pipeline.blend=false;pipeline.color.operation=pipeline.alpha.operation=ColorOperation::First;pipeline.color.first=pipeline.alpha.first={ArgumentSource::Texture};
        const float u0=float(x)/ending_width,v0=float(y)/ending_height,u1=float(x+640)/ending_width,v1=float(y+480)/ending_height;
        const SpriteVertex quad[4]={{{-.5f,-.5f,0},1,0xffffffff,{u0,v0}},{{639.5f,-.5f,0},1,0xffffffff,{u1,v0}},{{-.5f,479.5f,0},1,0xffffffff,{u0,v1}},{{639.5f,479.5f,0},1,0xffffffff,{u1,v1}}};graphics.draw(pipeline,ending_image,Topology::Strip,VertexLayout::ScreenColorUv,quad,4);
    }
    void ending_sprite(AnmVm& vm)override{presentation.renderer.draw_2d(vm);}
    void ending_cover(u32 color)override{presentation.renderer.rectangle(0,0,640,480,color,color);}
    bool tick(u16 keys){return tick_inputs(keys,0,keys);}
    bool tick_inputs(u16 left,u16 right,u16 keys,bool render=true){
#if TH09_DEVELOPMENT_HARNESS
        const double cost_begin=emscripten_get_now();
#endif
        if(in_title)return tick_title(left,right,keys,render);if(!session||!world||!error.empty())return false;update_clocks();device.update(keys);mode=world->rules.mode;difficulty=world->configuration.selection.difficulty;game_flags=world->battle->state.flags;continues=session->continues;
        PopupFrame popup;popup.paused=paused;popup.game_over=over;popup.game_flags=game_flags;for(i32 s=0;s<2;++s)popup.field_flags[s]=world->battle->fields[s].script.flags;presentation.ascii.update(popup);
        if(paused)menus.update_pause(device);else if(over)menus.update_game_over(device);else if(complete)menus.update_match_end(device);else if(!session->is_demo()&&(device.pressed&8)&&session->phase==SessionPhase::match){paused=world->paused=true;sound(34,0);pause_audio(true);}
        if(pending_action>=0){
            // Fail before destroying resources if a future menu rule bypasses
            // the driver's read-only lifetime barrier. Never rewind freed owners.
            if(recording_frame&&recording_frame->captured){error="Speculative menu resource action";return false;}
            const auto action=InGameAction(pending_action);pending_action=-1;
            if(action==InGameAction::retry||action==InGameAction::replay_retry||action==InGameAction::restart_extra){presentation.renderer.flush();if(!session->retry()){error=session->error;return false;}world=session->world.get();presentation.world=world;paused=over=complete=false;menus.pause={};menus.game_over={};menus.match_end={};pause_audio(false);}
            else if(action==InGameAction::continue_match){audio.music(-1);if(!session->continue_game()){error="Unable to continue";return false;}paused=over=complete=false;pause_audio(false);}
            else {session->finish();return return_title(action==InGameAction::save_score);}
        }
        if(!paused&&!over&&!complete){presentation.renderer.flush();if(!session->update(left,right,keys)){error=session->error;return false;}world=session->world.get();presentation.world=world;}
        over=session->phase==SessionPhase::game_over;complete=session->phase==SessionPhase::match_complete;
        if(session->phase==SessionPhase::finished)return return_title(!session->is_replay);
        requested_transition=world->transition_pending?100+i32(world->transition):-1;
#if TH09_DEVELOPMENT_HARNESS
        const double cost_draw=emscripten_get_now();probe_tick_cost[0]=cost_draw-cost_begin;
#endif
        if(rollback_active){
            graphics.skipSubmission=!render;presentation.renderer.omit_sprite_geometry=!render;
#if TH09_DEVELOPMENT_HARNESS
            if(probe_full_sprite_geometry)presentation.renderer.omit_sprite_geometry=false;
#endif
            draw(render);presentation.renderer.omit_sprite_geometry=false;graphics.skipSubmission=false;
        }
        else if(render)draw();
#if TH09_DEVELOPMENT_HARNESS
        probe_tick_cost[1]=emscripten_get_now()-cost_draw;
#endif
        if(!recording_frame){audio.update();audio.pump();}session->warm_resources();++frames;return error.empty();
    }
    void draw(bool present=true){presentation.begin_frame();if(in_title){title->draw();presentation.ascii.draw_text();presentation.ascii.clear_text();}else if(session->phase==SessionPhase::ending)session->ending->draw();else world->draw();presentation.finish_frame();if(present)graphics.present();}
    i32 requested_transition=-1;
    bool import_file(u32 kind,u32 size){
        if(!in_title||size>import_bytes.size())return false;
        const u8* bytes=import_bytes.data();
        if(kind==0){PlayerRecords next;if(!next.load(bytes,size))return false;if(!write_file("/save/score.dat",bytes,size))return false;records=std::move(next);records.application_clock=records.game_clock=u32(SDL_GetTicks());sync_records();title->state.selection=0;title->change(TitleScreen::main);return true;}
        if(kind==1){ReplayFile file;touch::MotionTrack motion;if(!file.decode(bytes,size)||!motion.load(bytes,size,9))return false;bool found=false;for(u32 stage=0;stage<10;++stage)if(file.stream_offset(0,stage)){ReplayPlayback playback;ReplayRoundSettings header;if(!playback.begin(file,stage,header))return false;found=true;}if(!found)return false;
            for(u32 n=1;n<=9999;++n){char name[40];std::snprintf(name,sizeof(name),"replay/th9_ud%04u.rpy",n);if(!title_replay_exists(name)){const auto path=replay_path(name);return write_file(path.c_str(),bytes,size);}}return false;}
        if(kind==2){GameConfiguration next;if(!next.load(bytes,size))return false;if(!write_file("/save/th09.cfg",bytes,size))return false;saved_configuration=next;saved_configuration.apply(settings);title_configuration();return true;}return false;
    }
};
std::unique_ptr<Application> probe;std::string failure;
bool running=false,suspended=false;u32 loop_epoch=0;double previous_frame=-1;multiplayer::FrameSchedule cadence;
struct Key {const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
touhou::input::TouchController gestures;
SDL_Gamepad* controllers[2]{};u32 pulse_ticks[16]{};u32 auto_fire_frame=0;
constexpr SDL_GamepadButton gamepad_slots[]={
    SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH,
    SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,SDL_GAMEPAD_BUTTON_BACK,
    SDL_GAMEPAD_BUTTON_START,SDL_GAMEPAD_BUTTON_LEFT_STICK,SDL_GAMEPAD_BUTTON_RIGHT_STICK,SDL_GAMEPAD_BUTTON_GUIDE,
};
void close_controllers(){for(auto*& p:controllers)if(p){SDL_CloseGamepad(p);p=nullptr;}}
void add_controller(SDL_JoystickID id){for(const auto* p:controllers)if(p&&SDL_GetGamepadID(const_cast<SDL_Gamepad*>(p))==id)return;for(auto*& p:controllers)if(!p){p=SDL_OpenGamepad(id);break;}}
i32 joy_button(i32 n){if(n<0||n>1||!controllers[n])return 32;for(i32 i=0;i<i32(std::size(gamepad_slots));++i)if(SDL_GetGamepadButton(controllers[n],gamepad_slots[i]))return i;return 32;}
u16 joy_keys(i32 side){if(!probe||side<0||side>1)return 0;const u32 device=probe->settings.devices[side];if(device>1||!controllers[device])return 0;auto* p=controllers[device];u16 keys=0;for(i32 n=0;n<9;++n){const i32 button=probe->settings.bindings[side].gamepad[n];if(button>=0&&button<i32(std::size(gamepad_slots))&&SDL_GetGamepadButton(p,gamepad_slots[button]))keys|=u16(1u<<n);}const i32 x=SDL_GetGamepadAxis(p,SDL_GAMEPAD_AXIS_LEFTX),y=SDL_GetGamepadAxis(p,SDL_GAMEPAD_AXIS_LEFTY);if(x<-19660||SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_LEFT))keys|=64;if(x>19660||SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_RIGHT))keys|=128;if(y<-19660||SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_UP))keys|=16;if(y>19660||SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_DOWN))keys|=32;return keys;}
touhou::input::TouchState touch_state(){
    touhou::input::TouchState s;if(!probe)return s;if(probe->in_title||probe->paused||probe->over||probe->complete||!probe->world||!probe->session)return s;
    auto& run=*probe->session;auto& w=*probe->world;if(run.is_replay){s.context=3;return s;}if(run.phase==SessionPhase::ending){s.context=2;return s;}if(w.dialogue->id>=0){s.context=2;return s;}
    i32 side=network.active?network.side:w.configuration.controllers[0]?1:0;if(w.configuration.controllers[side])return s;auto& p=*w.battle->fields[side].player;s.context=1;s.instance=1+side+2*w.configuration.selection.stage+32*w.rules.progress.round;s.ready=(p.control.player_state==0||p.control.player_state==3)&&p.motion.health>0;
    s.x=p.motion.position.x;s.y=p.motion.position.y;s.fast=p.resource.movement.normal;s.slow=p.resource.movement.focused;const auto& limit=w.battle->state.limits;s.min_x=limit.origin.x;s.max_x=limit.origin.x+limit.extent.x;s.min_y=limit.origin.y;s.max_y=limit.origin.y+limit.extent.y;return s;
}
void clear_inputs(){th09_reset_browser_keyboard();SDL_ResetKeyboard();for(auto& k:keyboard_map)k.hosted=false;gestures.reset();if(probe&&probe->session)probe->session->clear_motion();std::fill(std::begin(pulse_ticks),std::end(pulse_ticks),0);}
void sample_keys(u16 (&out)[3]){
    SDL_Event e;while(SDL_PollEvent(&e)){if(e.type==SDL_EVENT_GAMEPAD_ADDED)add_controller(e.gdevice.which);else if(e.type==SDL_EVENT_GAMEPAD_REMOVED)for(auto*& p:controllers)if(p&&SDL_GetGamepadID(p)==e.gdevice.which){SDL_CloseGamepad(p);p=nullptr;}}
    bool keys[256]{};const auto* physical=th09_browser_keyboard()?nullptr:SDL_GetKeyboardState(nullptr);for(const auto& k:keyboard_map)if(k.hosted||(physical&&k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){if(k.vk)keys[k.vk]=true;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=true;if(k.scan==28||k.scan==156)keys[13]=true;}
    const int keyboardDpad=th09_keyboard_gamepad_dpad();if(keyboardDpad&1)keys[38]=true;if(keyboardDpad&2)keys[40]=true;if(keyboardDpad&4)keys[37]=true;if(keyboardDpad&8)keys[39]=true;
    const auto state=touch_state();auto sample=gestures.sample(state,SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);
    // TH09 shoots ordinary bullets on repeated presses, while holding Z charges.
    // Auto-fire therefore produces real press/release input, retained by .rpy.
    if(state.context==1&&gestures.enabled&&gestures.fire&&!keys[90])sample.keys[90]=((auto_fire_frame++%6)<3);
    for(u32 n=0;n<256;++n)keys[n]=keys[n]||sample.keys[n];
    if(probe->session)probe->session->clear_motion();
    if(sample.motion&&state.ready&&!keys[37]&&!keys[38]&&!keys[39]&&!keys[40]){
        const i32 side=network.active?network.side:probe->world->configuration.controllers[0]?1:0;const auto& player=*probe->world->battle->fields[side].player;const auto& m=player.motion;
        // A LAN drag ships the absolute field target and each peer converts it on
        // the frame it simulates: converting here would aim from a position the
        // lockstep delay made stale, so the player would orbit the finger. The
        // analog stick is a velocity device whose vector does not depend on the
        // player's position, so it converts here exactly as in single player.
        if(network.active&&!sample.stick)probe->session->motion_input[side]={true,sample.x,sample.y,sample.motion==2,true};
        else{
            const bool focus=probe->world->configuration.automatic_focus[side]?player.input.fire_frames>=7:keys[16];const float speed=focus?player.resource.movement.focused:player.resource.movement.normal;
            const float sx=m.base_scale.x*m.effect_scale.x,sy=m.base_scale.y*m.effect_scale.y;float x=sx?(sample.x-state.x)/sx:0,y=sy?(sample.y-state.y)/sy:0;if(sample.motion!=2)touch::limit_vector(x,y,speed);
            probe->session->motion_input[side]={true,x,y,sample.motion==2,false};
        }
    }
    out[2]=keyboard_input(keys,2)|joy_keys(0)|joy_keys(1);for(u32 n=0;n<16;++n)if(pulse_ticks[n]){out[2]|=u16(1u<<n);--pulse_ticks[n];}
    const bool paired=probe->in_title?(probe->settings.versus==0&&probe->title&&probe->title->state.screen==TitleScreen::versus_character):(probe->world&&probe->world->rules.mode==GameMode::versus&&!probe->world->configuration.controllers[0]&&!probe->world->configuration.controllers[1]);
    for(i32 side=0;side<2;++side)out[side]=paired?keyboard_input(keys,probe->settings.devices[side])|joy_keys(side):out[2];
}

}
extern "C" {
#define TH09_EXPORT(n) __attribute__((export_name(n)))
TH09_EXPORT("th09_game_open") u32 th09_game_open(u32 seed){
    network.end();spectator_mode=false;spectator_frames.clear();spectator_next=spectator_simulated=0;clear_inputs();close_controllers();SDL_InitSubSystem(SDL_INIT_GAMEPAD);i32 count=0;auto* ids=SDL_GetGamepads(&count);for(i32 n=0;n<count;++n)add_controller(ids[n]);SDL_free(ids);for(auto& k:keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);
    probe=std::make_unique<Application>();if(!probe->open_title()){failure=probe->error;return 0;}probe->state.random={u16(seed),0,0};return 1;
}
TH09_EXPORT("th09_game_metrics") const u32* th09_game_metrics(){static u32 data[10]{};if(probe){auto& s=probe->graphics.backend.stats;data[0]=s.batches;data[1]=s.uploadBytes;data[2]=s.readBytes;data[3]=s.vertexUploadBytes;data[4]=s.programCompiles;data[5]=s.bufferReplacements;data[6]=s.bufferSubUpdates;data[7]=s.frames;data[8]=s.resamples;data[9]=s.presentations;}return data;}
TH09_EXPORT("th09_network_begin") u32 th09_network_begin(u32 seed,i32 side,u32 unlocked,u32 difficulty,u32 focus){
    if(!probe||!probe->in_title||side<0||side>1||difficulty>3||network.active)return 0;probe->local_focus=probe->settings.auto_focus;probe->local_versus=probe->settings.versus==4?1:probe->settings.versus;clear_inputs();probe->state.random={u16(seed),0,0};probe->settings.versus=4;probe->settings.difficulty=u8(difficulty);probe->settings.game_flags=0;probe->settings.characters[0]=0;probe->settings.characters[1]=1;
    for(u32 n=0;n<16;++n)probe->settings.versus_unlocked[n]=bool(unlocked&(1u<<n));for(u32 n=0;n<2;++n){probe->settings.auto_focus[n]=bool(focus&(1u<<n));probe->settings.health[n]=10;probe->settings.alternate[n]=false;}
    probe->title->state={};probe->title->leaving=false;probe->title->change(TitleScreen::versus_difficulty);probe->device=probe->left_device=probe->right_device={};network.begin(side);return !network.failed;
}
TH09_EXPORT("th09_network_room_begin") u32 th09_network_room_begin(u32 seed,i32 side,u32 unlocked,u32 difficulty,u32 focus,u32 left,u32 right){
    if(left>=16||right>=16)return 0;
    if(!th09_network_begin(seed,side,unlocked|(1u<<left)|(1u<<right),difficulty,focus))return 0;
    probe->settings.characters[0]=i32(left);probe->settings.characters[1]=i32(right);
    probe->title->launch_network_match();
    return 1;
}
TH09_EXPORT("th09_adonis_configure") u32 th09_adonis_configure(u32 mode){
    if(rollback_active||mode>2)return 0;adonis_mode=static_cast<Netplay::AdonisMode>(mode);return 1;
}
TH09_EXPORT("th09_adonis_info") const u32* th09_adonis_info(){
    static u32 data[6];data[0]=static_cast<u32>(rollback.Mode());data[1]=rollback.InputDelay();
    data[2]=adonis_snapshots;data[3]=rollback.Channel().AdonisStatistics().Adjustments();
    data[4]=u32(rollback.Channel().AdonisStatistics().TotalDelayUs());data[5]=adonis_waits;return data;
}
TH09_EXPORT("th09_rollback_enable") u32 th09_rollback_enable(u32 seed,u32 side,u32 idLow,u32 idHigh,u32 abi,u32 inputDelay){
    if(rollback_active||!probe||!network.active||side>1||inputDelay>9)return 0;
    for(auto& frame:rollback_frames){frame.number=Netplay::INVALID_FRAME;frame.captured=false;frame.sounds.clear();}
    Netplay::SessionConfig c;c.seed=seed;c.localPlayer=side;c.gameId=9;c.gameplayAbi=abi;c.sessionId=(std::uint64_t(idHigh)<<32)|idLow;
    rollback_published=rollback_catchup=adonis_snapshots=adonis_waits=0;rollback_active=rollback.Begin(c,std::uint64_t(emscripten_get_now()),u8(inputDelay),adonis_mode);return rollback_active;
}
TH09_EXPORT("th09_rollback_begin") u32 th09_rollback_begin(u32 seed,u32 side,u32 difficulty,u32 left,u32 right,u32 idLow,u32 idHigh,u32 abi,u32 inputDelay){
    if(rollback_active||inputDelay>9||!th09_network_room_begin(seed,i32(side),0xffff,difficulty,0,left,right))return 0;
    // Commit the resource-owning title transition before the prediction epoch.
    if(!probe->tick_title(0,0,0,false)||probe->in_title)return 0;
    return th09_rollback_enable(seed,side,idLow,idHigh,abi,inputDelay);
}
TH09_EXPORT("th09_rollback_pump") u32 th09_rollback_pump(){
    if(!rollback_active)return 0;
    if(!rollback.Pump(std::uint64_t(emscripten_get_now()),network.active)){probe->error=rollback.Error();return 0;}
    return rollback.Ready()?2:1;
}
TH09_EXPORT("th09_rollback_info") const u32* th09_rollback_info(){
    static u32 data[8];const auto confirmed=rollback.ConfirmedThrough();
    data[0]=rollback.Frame();data[1]=confirmed==Netplay::INVALID_FRAME?rollback_published:std::max(rollback_published,confirmed+1);data[2]=rollback.Corrections();data[3]=rollback.Resimulated();data[4]=rollback.Captures();data[5]=rollback.Channel().RepairsSent();data[6]=rollback.Ready();data[7]=rollback_published;return data;
}
TH09_EXPORT("th09_spectator_begin") u32 th09_spectator_begin(u32 seed,u32 difficulty,u32 left,u32 right){
    if(!th09_network_room_begin(seed,0,0xffff,difficulty,0,left,right))return 0;
    if(!probe->tick_title(0,0,0,false)||probe->in_title)return 0;network.end();spectator_frames.clear();spectator_next=spectator_simulated=0;spectator_mode=true;return 1;
}
TH09_EXPORT("th09_spectator_feed") u32 th09_spectator_feed(u32 frame,u32 left,u32 right,u32 leftMode,float leftX,float leftY,u32 rightMode,float rightX,float rightY){
    if(!spectator_mode||frame!=spectator_next||left>65535||right>65535||spectator_frames.size()>=8192)return 0;
    const auto motion=[](u32 mode,float x,float y,NetworkInput::Motion& out){
        const bool target=mode==NetworkInput::MotionTarget||mode==NetworkInput::MotionTargetUnlimited;
        const float limit=target?4096.f:16.f;
        if(mode>NetworkInput::MotionTargetUnlimited||!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>limit||std::abs(y)>limit)return false;
        out={mode!=NetworkInput::MotionNone,target,mode==NetworkInput::MotionTargetUnlimited,x,y};return true;
    };
    SpectatorFrame packet;packet.frame=frame;packet.keys[0]=u16(left);packet.keys[1]=u16(right);
    if(!motion(leftMode,leftX,leftY,packet.motion[0])||!motion(rightMode,rightX,rightY,packet.motion[1]))return 0;
    spectator_frames.push_back(packet);++spectator_next;return 1;
}
TH09_EXPORT("th09_spectator_frame") u32 th09_spectator_frame(){return spectator_simulated;}
TH09_EXPORT("th09_spectator_end") void th09_spectator_end(){spectator_mode=false;spectator_frames.clear();clear_inputs();}
TH09_EXPORT("th09_network_receive") u32 th09_network_receive(u32 frame,u32 keys,u32 mode,float x,float y){return keys<=65535&&mode<=NetworkInput::MotionTargetUnlimited&&network.submit(1-network.side,frame,u16(keys),u8(mode),x,y);}
TH09_EXPORT("th09_network_end") void th09_network_end(){rollback_active=false;rollback.Clear();rollback_published=rollback_catchup=0;if(!network.active)return;if(probe)probe->release_network();else network.end();clear_inputs();if(probe){probe->requested_transition=-1;probe->settings.game_flags=0;if(probe->session&&!probe->in_title){probe->session->finish();probe->return_title(false);}else if(probe->title){probe->return_title(false);}probe->sync_records();}}
TH09_EXPORT("th09_network_hash") u32 th09_network_hash(){
    if(!probe)return 0;u32 hash=2166136261u;const auto word=[&](u32 value){for(u32 n=0;n<4;++n){hash^=u8(value>>(n*8));hash*=16777619u;}};const auto real=[&](float f){u32 value;std::memcpy(&value,&f,4);word(value);};
    word(probe->state.random.seed);word(probe->state.random.calls);word(probe->in_title);word(probe->paused);word(probe->over);word(probe->complete);
    if(probe->in_title&&probe->title){const auto& t=probe->title->state;word(i32(t.screen));word(t.state);word(t.frames);word(t.selection);for(u32 side=0;side<2;++side){word(t.character_selection[side]);word(t.confirmed[side]);word(t.health[side]);}}
    else if(probe->world){const auto& w=*probe->world;word(w.battle->state.flags);word(w.scene->phase);word(w.dialogue->id);word(w.rules.progress.round);for(u32 side=0;side<2;++side){const auto& p=*w.battle->fields[side].player;real(p.motion.position.x);real(p.motion.position.y);real(p.motion.health);real(p.control.charge);real(p.control.available);word(p.control.player_state);word(w.rules.scores[side].points);word(p.combo_state.best_hits);}}
    return hash;
}
TH09_EXPORT("th09_network_info") const u32* th09_network_info(){static u32 data[6]{};if(probe){data[0]=0;for(u32 n=0;n<16;++n)if(probe->records.versus_unlocked[n])data[0]|=1u<<n;data[1]=std::min(3u,u32(probe->settings.difficulty));data[2]=probe->settings.auto_focus[0];data[3]=rollback_active?rollback.Frame():network.frame();data[4]=network.active;data[5]=network.side;}return data;}
TH09_EXPORT("th09_game_close") void th09_game_close(){rollback_active=false;rollback.Clear();running=false;++loop_epoch;close_controllers();clear_inputs();network.end();spectator_mode=false;spectator_frames.clear();probe.reset();}
TH09_EXPORT("th09_music_enabled") void th09_music_enabled(u32 enabled){if(probe){probe->host_music_enabled=enabled!=0;probe->title_configuration();}}
TH09_EXPORT("th09_game_restart") u32 th09_game_restart(){if(!probe||!probe->in_title)return 0;clear_inputs();probe->requested_transition=-1;probe->settings.game_flags=0;return probe->return_title(false);}
u32 rollback_tick(u32 render){
    if(!th09_rollback_pump())return 0;
    if(!rollback.Ready())return 2;
    auto correction=rollback.RollbackFrame();
    if(correction!=Netplay::INVALID_FRAME){
        rollback_catchup=std::max(rollback_catchup,rollback.Frame());
        probe->presentation.renderer.flush();probe->graphics.backend.flush();
        for(u32 f=rollback.Frame();f-->correction;){auto& saved=rollback_frames[f%rollback_frames.size()];if(saved.number!=f||!saved.captured||!saved.state||!saved.state->Restore()){probe->error="Rollback world restore failed";return 0;}saved.sounds.clear();}
        if(!rollback.Restored(correction)){probe->error=rollback.Error();return 0;}
    }
    // One callback starts at most eight recovery ticks. Its caller also has a
    // wall-time budget; physical input is never sampled on these replay ticks.
    const double start=emscripten_get_now();
    for(unsigned count=0;count<9;++count){
#if TH09_DEVELOPMENT_HARNESS
        if(rollback.Frame()>=rollback_test_limit)break;
#endif
        if(rollback.NeedsCapture()){
            u16 keys[3]{};sample_keys(keys);Netplay::FrameInput in(keys[2]);
            if(probe->session){const auto& m=probe->session->motion_input[network.side];if(m.enabled){in.analogMode=m.target?Netplay::AnalogMode::DirectTouch:Netplay::AnalogMode::Joystick;in.x=m.x;in.y=m.y;in.unlimited=m.unlimited;in.touchUsed=true;}}
            if(!rollback.Capture(in,std::uint64_t(emscripten_get_now()))){probe->error=rollback.Error();return 0;}
        }
        auto decision=rollback.Prepare();if(!decision.canAdvance){++adonis_waits;return 2;}
        const u32 f=rollback.Frame();const auto confirmed=rollback.ConfirmedThrough();
        const bool reconciled=rollback.InputsReconciledBeforeNext();
        // Menu animation and selection belong to the existing snapshot. Wait
        // only at a step that can replace resources, not on every menu frame.
        // Use the current world's flags/continues, before tick_inputs refreshes
        // its cached InGameMenuServices fields.
        const u32 flags=probe->world?probe->world->battle->state.flags:0;
        const bool retire=probe->in_title||probe->pending_action>=0||
            (probe->paused?probe->menus.pause_retires_world():
             probe->over?probe->menus.game_over_retires_world(flags,probe->session->continues):
             probe->complete&&probe->menus.match_end_retires_world(flags));
        if(retire&&(decision.predictedMask||!reconciled))return 2;
        auto& record=rollback_frames[f%rollback_frames.size()];record.number=f;record.sounds.clear();
        const bool speculative=decision.predictedMask||!reconciled;
        if(rollback.Mode()==Netplay::AdonisMode::Delay&&speculative){probe->error="Lockstep attempted speculative state";return 0;}
        if(speculative)++adonis_snapshots;
        record.captured=speculative;
        if(speculative){if(!record.state)record.state=std::make_unique<multiplayer::WorldState>();if(!probe->checkpoint(*record.state)){probe->error="Rollback world capture failed";return 0;}probe->graphics.checkpoint=record.state.get();}
        // Retain arena capacity across exact and speculative frames.
        recording_frame=&record;
        if(probe->session&&!probe->in_title)for(unsigned side=0;side<2;++side){const auto& in=decision.inputs[side];probe->session->motion_input[side]={in.analogMode!=Netplay::AnalogMode::None,in.x,in.y,in.unlimited,in.analogMode==Netplay::AnalogMode::DirectTouch};}
        // Recovery does not consume the wall-clock tick's forward step. Replay
        // through the old frontier, then capture/simulate its new input once.
        const bool final=f>=rollback_catchup;
        const auto ok=probe->tick_inputs(decision.inputs[0].buttons,decision.inputs[1].buttons,decision.inputs[0].buttons|decision.inputs[1].buttons,render&&final);
        recording_frame=nullptr;probe->graphics.checkpoint=nullptr;
        if(record.captured&&!record.state->Seal()){probe->error="Rollback checkpoint seal failed";return 0;}
        if(!ok||!rollback.Complete(decision)){if(ok)probe->error=rollback.Error();return 0;}
        if(!network.active)rollback.FinishExactBoundary();
        record.hash=th09_network_hash();
        if(final||!network.active)break;
        if(emscripten_get_now()-start>=8)break;
    }
    // Publish side effects and spectator input only from reconciled history.
    const auto through=rollback.ConfirmedThrough();
    while(through!=Netplay::INVALID_FRAME&&rollback_published<=through){
        auto& record=rollback_frames[rollback_published%rollback_frames.size()];
        if(record.number!=rollback_published){probe->error="Confirmed output history expired";return 0;}
        for(const auto& s:record.sounds)switch(s.kind){case 0:probe->audio.effects.enqueue(s.id,i32(s.value));break;case 1:probe->audio.effects.positioned(s.id,s.value);break;case 2:probe->music(s.id);break;case 3:probe->audio.fade_music();break;case 4:probe->audio.pause_music(s.id!=0);break;case 5:probe->encountered(s.id);break;case 6:probe->defeated(s.id);break;}
        record.sounds.clear();
        if(network.side==0){std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS> in;if(!rollback.ConfirmedInputs(rollback_published,&in))return 0;const auto mode=[](const Netplay::FrameInput& i){return i.analogMode==Netplay::AnalogMode::None?0:i.analogMode==Netplay::AnalogMode::Joystick?1:i.unlimited?3:2;};th09_network_spectator_frame(rollback_published,in[0].buttons,in[1].buttons,mode(in[0]),in[0].x,in[0].y,mode(in[1]),in[1].x,in[1].y);}
        if(network.active&&(rollback_published+1)%120==0&&!rollback.Verify(rollback_published+1,record.hash)){probe->error=rollback.Error();return 0;}
        ++rollback_published;
    }
    probe->audio.update();probe->audio.pump();
    return rollback.Frame()<=rollback_catchup?2:1;
}
TH09_EXPORT("th09_game_tick") u32 th09_game_tick(u32 render){if(!probe)return 0;u16 keys[3]{};
    if(spectator_mode){
        if(spectator_frames.empty())return 2;
        const u32 count=u32(spectator_frames.size()>8?4:spectator_frames.size()>4?2:1);u32 ok=1;
        for(u32 n=0;n<count&&ok&&!spectator_frames.empty();++n){const auto packet=spectator_frames.front();spectator_frames.pop_front();
            if(packet.frame!=spectator_simulated)return 0;
            const bool was_title=probe->in_title;
            if(!was_title&&probe->session)for(i32 s=0;s<2;++s)probe->session->motion_input[s]={packet.motion[s].enabled,packet.motion[s].x,packet.motion[s].y,packet.motion[s].unlimited,packet.motion[s].target};
            const bool prior=rollback_active;rollback_active=true;
            ok=probe->tick_inputs(packet.keys[0],packet.keys[1],u16(packet.keys[0]|packet.keys[1]),render!=0&&n+1==count);rollback_active=prior;++spectator_simulated;
            if(!was_title&&probe->in_title){spectator_mode=false;spectator_frames.clear();break;}
        }
        return ok;
    }
    if(rollback_active&&network.active)return rollback_tick(render);
    if(network.active){if(network.wants_input()){sample_keys(keys);const u32 frame=network.sending_frame();const auto m=probe->session&&!probe->in_title?probe->session->motion_input[network.side]:GameSession::MotionSample{};const u8 mode=!m.enabled?NetworkInput::MotionNone:m.target?(m.unlimited?NetworkInput::MotionTargetUnlimited:NetworkInput::MotionTarget):NetworkInput::MotionVelocity;if(!network.submit(network.side,frame,keys[2],mode,m.x,m.y))return 0;th09_network_send(frame,keys[2],i32(mode),m.x,m.y);}if(network.failed){probe->error="Network input order invalid";return 0;}NetworkInput::Motion motion[2];const u32 frame=network.frame();const bool publisher=network.side==0;if(!network.take(keys,motion))return 2;if(probe->session)for(i32 s=0;s<2;++s)probe->session->motion_input[s]={motion[s].enabled,motion[s].x,motion[s].y,motion[s].unlimited,motion[s].target};
        const u32 ok=probe->tick_inputs(keys[0],keys[1],keys[2],render!=0);
        if(ok&&publisher){const auto mode=[](const NetworkInput::Motion& m){return !m.enabled?0:m.target?(m.unlimited?3:2):1;};
            th09_network_spectator_frame(frame,keys[0],keys[1],mode(motion[0]),motion[0].x,motion[0].y,mode(motion[1]),motion[1].x,motion[1].y);}
        return ok;}
    sample_keys(keys);u32 ok=1;for(u32 extra=0;extra<3&&ok;++extra){ok=probe->tick_inputs(keys[0],keys[1],keys[2],false);if(!ok||probe->in_title||probe->paused||probe->over||probe->complete||!probe->session->is_replay||probe->session->phase!=SessionPhase::match||probe->session->playback_schedule()!=6)break;}if(ok&&render)probe->draw();return ok;
}
TH09_EXPORT("th09_loop_pause") void th09_loop_pause(u32 on){suspended=on!=0;previous_frame=-1;cadence.reset();clear_inputs();if(probe){probe->clock_pause(suspended);probe->audio.suspend(suspended);}}
TH09_EXPORT("th09_loop_stop") void th09_loop_stop(){running=false;++loop_epoch;if(probe){probe->clock_pause(true);probe->audio.suspend(true);}}
TH09_EXPORT("th09_loop_start") void th09_loop_start(){
    if(!probe||running)return;running=true;suspended=false;previous_frame=-1;cadence.reset();probe->clock_pause(false);probe->audio.suspend(false);
    emscripten_request_animation_frame_loop([](double time,void* epoch)->EM_BOOL{
        if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;const double begin=emscripten_get_now(),delta=previous_frame<0?0:(time-previous_frame)/1000.;previous_frame=time;
        if(suspended||!probe){cadence.reset();return EM_TRUE;}
        const bool active=rollback_active&&network.active;
        // A pending due tick ignores new wall-time credit. Keep phase advice
        // queued until the scheduler can actually apply it to a future tick;
        // consuming it on a retry would silently discard the correction.
        const auto elapsed=active&&!cadence.retry_pending()?rollback.PacedElapsedMs(delta*1000)/1000/rollback.IntervalScale():delta;
        const auto ticks=cadence.advance(elapsed,active);u32 ok=1;
        for(u32 n=0;n<ticks&&ok;++n){
            ok=th09_game_tick(n+1==ticks);if(ok==2){cadence.blocked(rollback_active&&network.active);break;}
            cadence.complete();
            if(rollback_active&&emscripten_get_now()-begin>=10){
                // Drop overdue work only when due ticks were left unfinished.
                // A costly but completed callback must keep its fractional
                // clock remainder, otherwise recovery itself slows the game.
                cadence.exhausted(n+1,ticks);break;
            }
        }
        probe->audio.pump();th09_browser_frame(ok,emscripten_get_now()-begin);if(!ok){running=false;probe->audio.suspend(true);}return running?EM_TRUE:EM_FALSE;
    },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
TH09_EXPORT("th09_game_draw") void th09_game_draw(){if(probe)probe->draw();}
TH09_EXPORT("th09_key") void th09_key(u32 scan,u32 down){if(spectator_mode)return;for(auto& k:keyboard_map)if(k.scan==scan)k.hosted=down!=0;}
TH09_EXPORT("th09_keys_clear") void th09_keys_clear(){clear_inputs();}
TH09_EXPORT("th09_pulse") void th09_pulse(u32 mask){for(u32 n=0;n<16;++n)if(mask&(1u<<n))pulse_ticks[n]=2;}
TH09_EXPORT("th09_touch") void th09_touch(u32 type,i32 id,float x,float y){gestures.pointer(type,id,x,y,SDL_GetTicks(),touch_state(),false);}
TH09_EXPORT("th09_touch_cancel") void th09_touch_cancel(){gestures.cancel();}
TH09_EXPORT("th09_touch_controls") void th09_touch_controls(u32 enabled,u32 fire,u32 focus,u32 bomb,u32 escape){gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
TH09_EXPORT("th09_touch_options") void th09_touch_options(u32 enabled,u32 mode,float sensitivity,u32 two_finger,u32 double_tap){gestures.enabled=enabled!=0;gestures.mode=mode<=3?i32(mode):0;gestures.unlimited=mode==1;gestures.sensitivity=std::clamp(sensitivity,.25f,4.f);gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;}
TH09_EXPORT("th09_touch_stick") void th09_touch_stick(float x,float y){gestures.stick_x=std::clamp(x/32767.f,-1.f,1.f);gestures.stick_y=std::clamp(y/32767.f,-1.f,1.f);}
TH09_EXPORT("th09_touch_state") const i32* th09_touch_state(){static i32 data[4]{};const auto s=touch_state();data[0]=s.context;data[1]=s.ready;data[2]=gestures.active();data[3]=probe&&probe->in_title;return data;}
// Read-only touch/lockstep diagnostics: the local side plus its player's
// position, velocity and the motion sample it is currently applying. The touch
// regression gate needs this because a gesture that never converges looks the
// same as one that does when only the drawn canvas is observed.
TH09_EXPORT("th09_touch_probe") const float* th09_touch_probe(){static float data[8]{};if(!probe||!probe->world)return data;const i32 side=network.active?network.side:0;const auto& p=*probe->world->battle->fields[side].player;const auto& m=probe->session?probe->session->motion_input[side]:GameSession::MotionSample{};data[0]=p.motion.position.x;data[1]=p.motion.position.y;data[2]=p.motion.velocity.x;data[3]=p.motion.velocity.y;data[4]=m.enabled?1.f:0.f;data[5]=m.target?1.f:0.f;data[6]=m.x;data[7]=m.y;return data;}
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_title_open") u32 th09_title_open(){probe=std::make_unique<Application>();if(!probe->open_title()){failure=probe->error;return 0;}return 1;}
#endif
TH09_EXPORT("th09_title_status") const i32* th09_title_status(){static i32 values[8]{};if(probe&&probe->title){const auto& s=probe->title->state;values[0]=probe->frames;values[1]=probe->in_title;values[2]=i32(s.screen);values[3]=s.state;values[4]=s.selection;values[5]=s.load_frame;values[6]=probe->title->leaving;values[7]=probe->requested_transition;}return values;}
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_title_details") const i32* th09_probe_title_details(){static i32 v[12]{};if(!probe||!probe->title)return v;auto& p=*probe;v[0]=p.background_image;v[1]=p.title_images["title00.png"];v[2]=p.title_images["select00.png"];v[3]=p.title->animations.size()>2?p.title->animations[2].color1.d3dColor:0;v[4]=p.settings.extra_lives;v[5]=p.settings.frameskip;v[6]=p.settings.extra_unlocked[12];return v;}
TH09_EXPORT("th09_probe_save_replay") u32 th09_probe_save_replay(){return probe&&probe->title_save_replay("replay/th9_25.rpy","TEST");}
TH09_EXPORT("th09_probe_world") const i32* th09_probe_world(){return probe?audit::snapshot(probe->world,probe->state):nullptr;}
TH09_EXPORT("th09_probe_replay") u32 th09_probe_replay(u32 size,u32 stage){if(!probe||size>probe->import_bytes.size())return 0;ReplayFile f;if(!f.decode(probe->import_bytes.data(),size))return 0;probe->title_play_replay(f,stage,"");probe->launch_pending=false;probe->in_title=false;return probe->start();}
TH09_EXPORT("th09_probe_return_title") u32 th09_probe_return_title(u32 score){if(!probe)return 0;if(probe->session)probe->session->finish();return probe->return_title(score!=0);}
TH09_EXPORT("th09_probe_open") u32 th09_probe_open(i32 a,i32 b,i32 mode,i32 difficulty){probe=std::make_unique<Application>();if(!probe->open(a,b,mode,difficulty)){failure=probe->error;return 0;}return 1;}
#endif
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_frame_limit") void th09_probe_frame_limit(u32 frame){rollback_test_limit=frame;}
TH09_EXPORT("th09_probe_tick_cost") const double* th09_probe_tick_cost(){return probe_tick_cost;}
// Exercise the actual menu implementation against its lifetime preflight.
// Animation assets are real; actions are observed rather than executed here.
TH09_EXPORT("th09_probe_menu_barrier") const u32* th09_probe_menu_barrier(){
    static u32 data[4]{};std::fill_n(data,4,0u);
    if(!probe||!probe->session||!probe->world)return data;
    multiplayer::WorldState saved;saved.sparse=false;if(!probe->checkpoint(saved))return data;
    struct Services final:InGameMenuServices {
        i32 action=-1;
        void menu_sound(i32)override{}
        void menu_action(InGameAction a)override{action=i32(a);}
        void menu_view()override{}
        void menu_sprite(AnmVm&)override{}
    } services;
    InGameMenus menus(probe->resources,services);bool valid=true;
    for(unsigned kind=0;kind<3;++kind)for(i32 state=0;state<=9;++state)
    for(i32 age:{0,3,4,19,20,21})for(u32 flags:{0u,1u,8u,9u})
    for(i32 continues:{0,3})for(u16 keys:{u16(0),u16(1),u16(8),u16(16),u16(32),u16(0x200),u16(0x1001),u16(0x208)}){
        menus.pause={};menus.game_over={};menus.match_end={};services.action=-1;
        services.game_flags=flags;services.continues=continues;
        InputFrame input;input.pressed=keys;bool barrier=false;
        if(kind==0){menus.pause.state=state;menus.pause.frames=age;barrier=menus.pause_retires_world();menus.update_pause(input);}
        else if(kind==1){menus.game_over.state=state;menus.game_over.frames=age;barrier=menus.game_over_retires_world(flags,continues);menus.update_game_over(input);}
        else {menus.match_end.state=state;menus.match_end.frames=age;barrier=menus.match_end_retires_world(flags);menus.update_match_end(input);}
        const bool retires=services.action>=0&&services.action!=i32(InGameAction::resume);
        valid=valid&&(!retires||barrier);++data[1];data[2]+=retires;data[3]+=!barrier;
    }
    data[0]=saved.Restore()&&valid;return data;
}
// Read-only density and byte accounting; not present in the release ABI.
TH09_EXPORT("th09_probe_load") const u32* th09_probe_load(){
    static u32 data[20]{};std::fill_n(data,20,0u);if(!probe||!probe->world)return data;
    auto& b=*probe->world->battle;
    for(unsigned side=0;side<2;++side){auto& f=b.fields[side];
        for(const auto& bullet:f.bullets->pool)if(bullet.state&&bullet.state!=6)++data[side];
        data[2+side]=f.enemies->alive;data[4+side]=f.effects->count;
    }
    auto& f=b.fields[0];data[6]=b.cross_effects->count;data[7]=b.attack_queue->counts[0]+b.attack_queue->counts[1];
    data[8]=f.bullets->pool.size()*sizeof(Bullet);data[9]=sizeof(f.bullet_visuals->instances[0]);
    data[10]=sizeof(f.player->shots.shots);data[11]=sizeof(f.player->shots.areas);data[12]=sizeof(f.player->items.items);data[13]=sizeof(f.lasers->pool);
    data[14]=sizeof(probe->presentation.ascii.queue);data[15]=sizeof(probe->world->huds[0]->animations);data[16]=sizeof(Bullet);data[17]=f.bullets->pool.size();
    return data;
}
TH09_EXPORT("th09_probe_draw_mode") void th09_probe_draw_mode(u32 full){probe_full_sprite_geometry=full!=0;}
std::unique_ptr<multiplayer::WorldState> test_checkpoint;
TH09_EXPORT("th09_probe_checkpoint") u32 th09_probe_checkpoint(u32 dense){
    if(!probe||!probe->session)return 0;if(!test_checkpoint)test_checkpoint=std::make_unique<multiplayer::WorldState>();test_checkpoint->sparse=!dense;
    if(!probe->checkpoint(*test_checkpoint))return 0;probe->graphics.checkpoint=test_checkpoint.get();return test_checkpoint->Bytes();
}
TH09_EXPORT("th09_probe_restore") u32 th09_probe_restore(){
    probe->graphics.checkpoint=nullptr;if(!test_checkpoint||!test_checkpoint->Restore())return 0;
    return 1;
}
TH09_EXPORT("th09_probe_enemy_detail") const char* th09_probe_enemy_detail(u32 side,u32 index){
    static std::string text;text.clear();const auto& e=probe->world->battle->fields[side].enemies->enemies[index];
    const auto part=[&](const char* name,const auto& value){StateChecksum h;h.Add(value);text+=std::string(name)+"="+std::to_string(h.value)+"\n";};
#define DETAIL(x) part(#x,e.x)
    auto values=e.values;values.locals=nullptr;part("values",values);
    DETAIL(values.shared_integer);DETAIL(values.shared_real);DETAIL(values.position);DETAIL(values.resolved_position);DETAIL(values.origin);DETAIL(values.target);DETAIL(values.last_delta);DETAIL(values.direction);DETAIL(values.angular_velocity);DETAIL(values.speed);DETAIL(values.acceleration);DETAIL(values.orbit_radius);DETAIL(values.orbit_angle);DETAIL(values.orbit_velocity);DETAIL(values.lifetime);DETAIL(values.life);DETAIL(values.last_damage);DETAIL(values.life_thresholds);DETAIL(values.item_reward);DETAIL(values.score_reward);DETAIL(values.drop_count);DETAIL(values.drop_item);DETAIL(values.flags);DETAIL(values.boss_id);
    DETAIL(program);DETAIL(primary);DETAIL(generations);DETAIL(active_slot);DETAIL(scratch_depth);DETAIL(behavior_flags);DETAIL(difficulty_flags);DETAIL(pending_interrupt);DETAIL(interrupt_subroutines);DETAIL(position_offset);DETAIL(velocity);DETAIL(movement);DETAIL(emitter);DETAIL(lasers);DETAIL(animation);DETAIL(status);DETAIL(trail);DETAIL(finished);DETAIL(invalid);DETAIL(failed_opcode);
#undef DETAIL
    for(auto& c:e.asynchronous){part("async-present",bool(c));if(c)part("async",*c);}
    return text.c_str();
}
// Adversarial owning-pool gate: cold-slot reuse, duplicate first writes,
// capacity overflow, reserved slots and destructive clear, against a dense hash.
TH09_EXPORT("th09_probe_pool_boundary") u32 th09_probe_pool_boundary(){
    if(!probe||!probe->session||!probe->world)return 0;
    auto& session=*probe->session;auto& battle=*probe->world->battle;
    const auto hash=[&]{multiplayer::WorldState h;h.hashEnabled=true;return h.Save(session)?h.Fingerprint():0;};
    const auto initial=hash();
    for(bool dense:{false,true}){
        multiplayer::WorldState cp;cp.sparse=!dense;if(!cp.Save(session))return 0;
        for(auto* manager:{battle.fields[0].effects.get(),battle.fields[1].effects.get(),battle.cross_effects.get()}){
            manager->clear();
            for(u32 i=0;i<manager->capacity+2;++i){auto* a=manager->create(EffectKind::hit,{float(i),17,0});if(!a)return 0;a->reserved=i+19;}
            for(u32 i=0;i<manager->reserved_slots;++i){auto* a=manager->slotted(EffectKind::hit,{4,5,0},i);if(!a)return 0;a->flags2=37;}
            manager->clear();auto* a=manager->create(EffectKind::hit,{31,41,0});if(!a)return 0;a->angle=1.25f;
        }
        auto& queue=*battle.attack_queue;queue.clear();queue.limits[0]=256;
        for(u32 i=0;i<queue.capacity+2;++i){auto* a=queue.create(0,0,{float(i),23,0});if(!a)return 0;a->color=i+17;}
        queue.clear();
        if(!cp.Restore()||hash()!=initial)return 0;
    }
    return 1;
}
TH09_EXPORT("th09_probe_bullet_boundary") u32 th09_probe_bullet_boundary(){
    if(!probe||!probe->session||!probe->world)return 0;
    auto& session=*probe->session;auto& battle=*probe->world->battle;
    multiplayer::WorldState original;original.sparse=false;if(!original.Save(session))return 0;
    for(auto& field:battle.fields){
        field.bullets->reset_pool();
        field.bullets->pool[0].state=1;
        if(!field.bullet_visuals->prepare(field.bullets->pool[0],0,0,0,0))return 0;
        // An inactive but still referenced hazard must not escape the snapshot.
        field.bullets->pool[1].speed=3.25f;field.bullets->pool[1].extras[8].integer_a=71;
        field.player->hazards.count=1;field.player->hazards.entries[0].bullet=&field.bullets->pool[1];
    }
    const auto hash=[&]{multiplayer::WorldState h;h.hashEnabled=true;return h.Save(session)?h.Fingerprint():0;};
    const auto initial=hash();
    std::array<std::vector<u8>,2> bytes,visualBytes;
    for(unsigned side=0;side<2;++side){auto& pool=battle.fields[side].bullets->pool;bytes[side].resize(pool.size()*sizeof(Bullet));std::memcpy(bytes[side].data(),pool.data(),bytes[side].size());}
    for(unsigned side=0;side<2;++side){auto& pool=battle.fields[side].bullet_visuals->instances;visualBytes[side].resize(pool.size()*sizeof(pool[0]));std::memcpy(visualBytes[side].data(),pool.data(),visualBytes[side].size());}
    bool good=true;
    for(bool dense:{false,true}){
        multiplayer::WorldState cp;cp.sparse=!dense;if(!cp.Save(session))return 0;
        for(auto& field:battle.fields)field.player->hazards.entries[0].bullet->state=5;
        for(auto& field:battle.fields){
            auto& visual=*field.bullet_visuals;auto& b=field.bullets->pool[0];
            // A single-VM first write followed by a full-slot write must retain
            // the original body, not the already advanced version or an overlap.
            if(!visual.set_sprite(b,0,visual.appearances[1].base_sprite))return 0;
            visual.advance(b,0,BulletAnimation::body);
            if(!visual.change_type(b,0,1,0))return 0;
            for(u32 animation=0;animation<5;++animation){
                const u32 flags=animation>0&&animation<4?1u<<animation:0;
                if(!visual.prepare(b,0,0,0,flags))return 0;
                visual.advance(b,0,static_cast<BulletAnimation>(animation));
            }
        }
        // Real round reset writes the whole pool, including sentinels and cold
        // bytes. A second reset tests deduplication within the same record.
        for(unsigned repeat=0;repeat<2;++repeat)for(unsigned side=0;side<2;++side){
            battle.reset_field(side,5);
            auto& pool=battle.fields[side].bullets->pool;
            for(u32 i=0;i<pool.size();++i)pool[i].extras[4].integer_a=i32(19+i+repeat);
        }
        good=cp.Restore()&&good;
        for(unsigned side=0;side<2;++side)good=!std::memcmp(bytes[side].data(),battle.fields[side].bullets->pool.data(),bytes[side].size())&&good;
        for(unsigned side=0;side<2;++side)good=!std::memcmp(visualBytes[side].data(),battle.fields[side].bullet_visuals->instances.data(),visualBytes[side].size())&&good;
        good=hash()==initial&&good;
    }
    return original.Restore()&&good;
}
TH09_EXPORT("th09_probe_recording_boundary") u32 th09_probe_recording_boundary(){
    if(!probe||!probe->session||!probe->world)return 0;auto& session=*probe->session;
    GameInput inputs[3];probe->world->copy_inputs(inputs);const bool cpu[2]={false,false};
    while(session.recording.frame()<ReplayRecording::chunk_frames-2)session.recording.record(4,false,inputs,cpu,60);
    multiplayer::WorldState before;if(!before.Save(session))return 0;
    const auto append=[&]{for(u32 f=0;f<12;++f){for(u32 p=0;p<3;++p)inputs[p].held=u16((f+1)*(p+1));session.recording.record(4,false,inputs,cpu,60);session.motion.record(18,true,float(f),float(f+1));}};
    const auto serialize=[&]{auto random=probe->state.random;return session.recording.finish(random,"BOUND","26/09/27",probe->world->configuration.selection.characters[0],probe->world->configuration.selection.characters[1]).data();};
    append();const auto expected=serialize(),motion=session.motion.trailer(9);
    if(!before.Restore())return 0;append();return expected==serialize()&&motion==session.motion.trailer(9);
}
std::string test_hash_parts;
TH09_EXPORT("th09_probe_hash_parts") const char* th09_probe_hash_parts(){return test_hash_parts.c_str();}
TH09_EXPORT("th09_probe_state_hash") u32 th09_probe_state_hash(){
    multiplayer::WorldState state;state.hashEnabled=true;if(!probe->checkpoint(state))return 0;test_hash_parts.clear();for(const auto& p:state.hashParts)test_hash_parts+=p.first+"="+std::to_string(p.second)+"\n";return state.Fingerprint();
}
TH09_EXPORT("th09_probe_rollback_step") u32 th09_probe_rollback_step(u32 left,u32 right,u32 render){
    const bool active=rollback_active;rollback_active=true;RollbackFrame output;recording_frame=&output;
    const bool ok=probe->tick_inputs(u16(left),u16(right),u16(left|right),render!=0);
    recording_frame=nullptr;rollback_active=active;return ok;
}
TH09_EXPORT("th09_probe_tick") u32 th09_probe_tick(u32 keys){if(!probe)return 0;return probe->tick(u16(keys));}
#endif
TH09_EXPORT("th09_session_status") const i32* th09_session_status(){static i32 values[8]{};if(probe&&probe->session){const auto& s=*probe->session;values[0]=i32(s.phase);values[1]=s.frames;values[2]=s.replay_frame();values[3]=s.replay_length();values[4]=s.continues;values[5]=s.world?s.world->configuration.selection.stage:-1;values[6]=s.is_replay;values[7]=s.recordable;}return values;}
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_inputs") u32 th09_probe_inputs(u32 left,u32 right,u32 menu){return probe&&probe->tick_inputs(u16(left),u16(right),u16(menu));}
#endif
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_end_round") void th09_probe_end_round(i32 winner){if(probe&&probe->world)probe->world->scene->end_round(winner);}
#endif
TH09_EXPORT("th09_error") const char* th09_error(){return probe&&!probe->error.empty()?probe->error.c_str():failure.c_str();}
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_status") const i32* th09_probe_status(){static i32 values[16]{};if(probe&&probe->world){auto& w=*probe->world;values[0]=probe->frames;values[1]=probe->requested_transition;values[2]=w.scene->phase;values[3]=w.dialogue->id;values[4]=w.rules.progress.stage;values[5]=w.battle->fields[0].player->control.player_state;values[6]=w.battle->fields[0].player->motion.health;values[7]=w.battle->fields[1].player->motion.health;values[8]=w.music_track;values[9]=probe->paused;}return values;}
#endif
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_audio") const u32* th09_probe_audio(){return probe?probe->audio.statistics():nullptr;}
#endif
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_error") const char* th09_probe_error(){return th09_error();}
#endif
TH09_EXPORT("th09_storage_revision") u32 th09_storage_revision(){return probe?probe->storage_revision:0;}
// Validate file-manager uploads without opening a game or writing the save tree.
std::vector<u8> file_validation_bytes;
TH09_EXPORT("th09_file_buffer") u8* th09_file_buffer(u32 size){if(size>16*1024*1024)return nullptr;file_validation_bytes.resize(size);return file_validation_bytes.data();}
TH09_EXPORT("th09_file_valid") u32 th09_file_valid(u32 kind,u32 size){if(!size||size!=file_validation_bytes.size())return 0;const auto* p=file_validation_bytes.data();if(kind==0){PlayerRecords records;return records.load(p,size);}if(kind==2){GameConfiguration config;return config.load(p,size);}if(kind!=1)return 0;ReplayFile file;touch::MotionTrack track;if(!file.decode(p,size)||!track.load(file.data().data(),file.data().size(),9))return 0;bool found=false;for(u32 stage=0;stage<10;++stage)if(file.stream_offset(0,stage)){ReplayPlayback playback;ReplayRoundSettings settings;if(!playback.begin(file,stage,settings))return 0;found=true;}return found;}
TH09_EXPORT("th09_import_buffer") u8* th09_import_buffer(u32 size){if(!probe||size>16*1024*1024)return nullptr;probe->import_bytes.resize(size);return probe->import_bytes.data();}
TH09_EXPORT("th09_import_file") u32 th09_import_file(u32 kind,u32 size){return probe&&probe->import_file(kind,size);}
TH09_EXPORT("th09_save_snapshot") u32 th09_save_snapshot(){if(!probe)return 0;probe->update_clocks();auto rng=probe->state.random;const auto bytes=probe->records.save(rng);probe->save_configuration();return probe->write_file("/save/score.dat",bytes.data(),u32(bytes.size()));}
TH09_EXPORT("th09_audio_pump") void th09_audio_pump(){if(probe)probe->audio.pump();}
TH09_EXPORT("th09_suspend") void th09_suspend(u32 on){if(probe)probe->audio.suspend(on!=0);}
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_close") void th09_probe_close(){th09_loop_stop();close_controllers();clear_inputs();probe.reset();}
#endif
}
}

