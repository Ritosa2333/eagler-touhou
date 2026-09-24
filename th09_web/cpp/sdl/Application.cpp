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
#include <dirent.h>
#include <ctime>
#include <algorithm>
#include <iterator>
#include "../../../portable/input/TouchController.hpp"
#include <emscripten.h>
#include <emscripten/html5.h>
#include "../../../portable/sdl/FrameCadence.hpp"
EM_JS(void, th09_browser_frame, (int ok,double milliseconds), { Module["onGameFrame"]?.(ok,milliseconds); });
EM_JS(void, th09_network_result, (), { Module["onNetworkResult"]?.(); });
EM_JS(void, th09_network_request, (), { Module["onNetworkRequest"]?.(); });
EM_JS(void, th09_network_send, (unsigned frame,unsigned keys,int moving,float x,float y), { Module["onNetworkInput"]?.(frame,keys,moving,x,y); });
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
    void sound(i32 id,i32 pan)override{audio.effects.enqueue(id,pan);}
    void positioned_sound(i32 id,float x)override{audio.effects.positioned(id,x);}
    void music(i32 n)override{if(!audio.music(n))error=audio.error;else if(const auto* entry=music_track(n))if(!session||!session->is_replay)records.music_unlocked[entry->unlock]=1;}
    void fade_music()override{audio.fade_music();}
    void encountered(i32 c)override{if(!session||!session->is_replay)records.count_encounter(c);}
    void defeated(i32 c)override{if(c>=0&&c<16&&(!session||!session->is_replay))records.versus_unlocked[c]=1;}
    void overlay()override{if(paused)menus.draw_pause();if(over)menus.draw_game_over();if(complete)menus.draw_match_end();}
    void menu_sound(i32 n)override{sound(n,0);}
    void menu_action(InGameAction action)override{
        if(action==InGameAction::resume){paused=false;world->paused=false;audio.pause_music(false);}
        else {pending_action=i32(action);}
    }
    void menu_view()override{presentation.begin_field(2);}
    void menu_sprite(AnmVm& a)override{presentation.renderer.draw_no_rotation(a);}
    i32 pending_action=-1;
    bool tick_title(u16 left,u16 right,u16 keys,bool render=true){
        update_clocks();device.update(keys);left_device.update(left);right_device.update(right);if(!title||!error.empty())return false;
        title->update(left_device,right_device,device);if(network.active&&title->state.screen==TitleScreen::versus_type){release_network();title->state={};th09_network_result();}save_configuration();if(!title->error.empty()){error=title->error;return false;}
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
    void save_configuration(){if(network.active)return;if(saved_configuration.capture(settings)){title_configuration();const auto& bytes=saved_configuration.data();if(!write_file("/save/th09.cfg",bytes.data(),u32(bytes.size())))error="Unable to save th09.cfg";}}
    u32 title_clear_count(i32 c,i32 d)override{return records.clear_count(c,d);}
    PlayerRecords& title_records()override{return records;}
    void title_save_records()override{update_clocks();auto local_random=state.random;auto bytes=records.save(network.active?local_random:state.random);if(!write_file("/save/score.dat",bytes.data(),u32(bytes.size())))error="Unable to save score.dat";}
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
        title->leaving=false;in_title=true;paused=over=complete=false;audio.pause_music(false);
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
        if(in_title)return tick_title(left,right,keys,render);if(!session||!world||!error.empty())return false;update_clocks();device.update(keys);mode=world->rules.mode;difficulty=world->configuration.selection.difficulty;game_flags=world->battle->state.flags;continues=session->continues;
        PopupFrame popup;popup.paused=paused;popup.game_over=over;popup.game_flags=game_flags;for(i32 s=0;s<2;++s)popup.field_flags[s]=world->battle->fields[s].script.flags;presentation.ascii.update(popup);
        if(paused)menus.update_pause(device);else if(over)menus.update_game_over(device);else if(complete)menus.update_match_end(device);else if(!session->is_demo()&&(device.pressed&8)&&session->phase==SessionPhase::match){paused=world->paused=true;sound(34,0);audio.pause_music(true);}
        if(pending_action>=0){const auto action=InGameAction(pending_action);pending_action=-1;
            if(action==InGameAction::retry||action==InGameAction::replay_retry||action==InGameAction::restart_extra){presentation.renderer.flush();if(!session->retry()){error=session->error;return false;}world=session->world.get();presentation.world=world;paused=over=complete=false;menus.pause={};menus.game_over={};menus.match_end={};audio.pause_music(false);}
            else if(action==InGameAction::continue_match){audio.music(-1);if(!session->continue_game()){error="Unable to continue";return false;}paused=over=complete=false;audio.pause_music(false);}
            else {session->finish();return return_title(action==InGameAction::save_score);}
        }
        if(!paused&&!over&&!complete){presentation.renderer.flush();if(!session->update(left,right,keys)){error=session->error;return false;}world=session->world.get();presentation.world=world;}
        over=session->phase==SessionPhase::game_over;complete=session->phase==SessionPhase::match_complete;
        if(session->phase==SessionPhase::finished)return return_title(!session->is_replay);
        requested_transition=world->transition_pending?100+i32(world->transition):-1;
        if(render)draw();audio.update();audio.pump();session->warm_resources();++frames;return error.empty();
    }
    void draw(){presentation.begin_frame();if(in_title){title->draw();presentation.ascii.draw_text();presentation.ascii.clear_text();}else if(session->phase==SessionPhase::ending)session->ending->draw();else world->draw();presentation.finish_frame();graphics.present();}
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
bool running=false,suspended=false;u32 loop_epoch=0;double previous_frame=-1;touhou::sdl::FrameCadence cadence;
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
void clear_inputs(){for(auto& k:keyboard_map)k.hosted=false;gestures.reset();if(probe&&probe->session)probe->session->clear_motion();std::fill(std::begin(pulse_ticks),std::end(pulse_ticks),0);}
void sample_keys(u16 (&out)[3]){
    SDL_Event e;while(SDL_PollEvent(&e)){if(e.type==SDL_EVENT_GAMEPAD_ADDED)add_controller(e.gdevice.which);else if(e.type==SDL_EVENT_GAMEPAD_REMOVED)for(auto*& p:controllers)if(p&&SDL_GetGamepadID(p)==e.gdevice.which){SDL_CloseGamepad(p);p=nullptr;}}
    bool keys[256]{};const auto* physical=SDL_GetKeyboardState(nullptr);for(const auto& k:keyboard_map)if(k.hosted||(k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){if(k.vk)keys[k.vk]=true;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=true;if(k.scan==28||k.scan==156)keys[13]=true;}
    const int keyboardDpad=th09_keyboard_gamepad_dpad();if(keyboardDpad&1)keys[38]=true;if(keyboardDpad&2)keys[40]=true;if(keyboardDpad&4)keys[37]=true;if(keyboardDpad&8)keys[39]=true;
    const auto state=touch_state();auto sample=gestures.sample(state,SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);
    // TH09 shoots ordinary bullets on repeated presses, while holding Z charges.
    // Auto-fire therefore produces real press/release input, retained by .rpy.
    if(state.context==1&&gestures.enabled&&gestures.fire&&!keys[90])sample.keys[90]=((auto_fire_frame++%6)<3);
    for(u32 n=0;n<256;++n)keys[n]=keys[n]||sample.keys[n];
    if(probe->session)probe->session->clear_motion();
    if(sample.motion&&state.ready&&!keys[37]&&!keys[38]&&!keys[39]&&!keys[40]){
        const i32 side=network.active?network.side:probe->world->configuration.controllers[0]?1:0;const auto& player=*probe->world->battle->fields[side].player;const auto& m=player.motion;
        const bool focus=probe->world->configuration.automatic_focus[side]?player.input.fire_frames>=7:keys[16];const float speed=focus?player.resource.movement.focused:player.resource.movement.normal;
        const float sx=m.base_scale.x*m.effect_scale.x,sy=m.base_scale.y*m.effect_scale.y;float x=sx?(sample.x-state.x)/sx:0,y=sy?(sample.y-state.y)/sy:0;if(sample.motion!=2||network.active)touch::limit_vector(x,y,speed);
        probe->session->motion_input[side]={true,x,y,sample.motion==2&&!network.active};
    }
    out[2]=keyboard_input(keys,2)|joy_keys(0)|joy_keys(1);for(u32 n=0;n<16;++n)if(pulse_ticks[n]){out[2]|=u16(1u<<n);--pulse_ticks[n];}
    const bool paired=probe->in_title?(probe->settings.versus==0&&probe->title&&probe->title->state.screen==TitleScreen::versus_character):(probe->world&&probe->world->rules.mode==GameMode::versus&&!probe->world->configuration.controllers[0]&&!probe->world->configuration.controllers[1]);
    for(i32 side=0;side<2;++side)out[side]=paired?keyboard_input(keys,probe->settings.devices[side])|joy_keys(side):out[2];
}

}
extern "C" {
#define TH09_EXPORT(n) __attribute__((export_name(n)))
TH09_EXPORT("th09_game_open") u32 th09_game_open(u32 seed){
    network.end();clear_inputs();close_controllers();SDL_InitSubSystem(SDL_INIT_GAMEPAD);i32 count=0;auto* ids=SDL_GetGamepads(&count);for(i32 n=0;n<count;++n)add_controller(ids[n]);SDL_free(ids);for(auto& k:keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);
    probe=std::make_unique<Application>();if(!probe->open_title()){failure=probe->error;return 0;}probe->state.random={u16(seed),0,0};return 1;
}
TH09_EXPORT("th09_game_metrics") const u32* th09_game_metrics(){static u32 data[10]{};if(probe){auto& s=probe->graphics.backend.stats;data[0]=s.batches;data[1]=s.uploadBytes;data[2]=s.readBytes;data[3]=s.vertexUploadBytes;data[4]=s.programCompiles;data[5]=s.bufferReplacements;data[6]=s.bufferSubUpdates;data[7]=s.frames;data[8]=s.resamples;data[9]=s.presentations;}return data;}
TH09_EXPORT("th09_network_begin") u32 th09_network_begin(u32 seed,i32 side,u32 unlocked,u32 difficulty,u32 focus){
    if(!probe||!probe->in_title||side<0||side>1||difficulty>3||network.active)return 0;probe->local_focus=probe->settings.auto_focus;probe->local_versus=probe->settings.versus==4?1:probe->settings.versus;clear_inputs();probe->state.random={u16(seed),0,0};probe->settings.versus=4;probe->settings.difficulty=u8(difficulty);probe->settings.game_flags=0;probe->settings.characters[0]=0;probe->settings.characters[1]=1;
    for(u32 n=0;n<16;++n)probe->settings.versus_unlocked[n]=bool(unlocked&(1u<<n));for(u32 n=0;n<2;++n){probe->settings.auto_focus[n]=bool(focus&(1u<<n));probe->settings.health[n]=10;probe->settings.alternate[n]=false;}
    probe->title->state={};probe->title->leaving=false;probe->title->change(TitleScreen::versus_difficulty);probe->device=probe->left_device=probe->right_device={};network.begin(side);return !network.failed;
}
TH09_EXPORT("th09_network_receive") u32 th09_network_receive(u32 frame,u32 keys,u32 moving,float x,float y){return keys<=65535&&moving<=1&&network.submit(1-network.side,frame,u16(keys),moving!=0,x,y);}
TH09_EXPORT("th09_network_end") void th09_network_end(){if(!network.active)return;if(probe)probe->release_network();else network.end();clear_inputs();if(probe){probe->requested_transition=-1;probe->settings.game_flags=0;if(probe->session&&!probe->in_title){probe->session->finish();probe->return_title(false);}else if(probe->title){probe->return_title(false);}probe->sync_records();}}
TH09_EXPORT("th09_network_hash") u32 th09_network_hash(){
    if(!probe)return 0;u32 hash=2166136261u;const auto word=[&](u32 value){for(u32 n=0;n<4;++n){hash^=u8(value>>(n*8));hash*=16777619u;}};const auto real=[&](float f){u32 value;std::memcpy(&value,&f,4);word(value);};
    word(probe->state.random.seed);word(probe->state.random.calls);word(probe->in_title);word(probe->paused);word(probe->over);word(probe->complete);
    if(probe->in_title&&probe->title){const auto& t=probe->title->state;word(i32(t.screen));word(t.state);word(t.frames);word(t.selection);for(u32 side=0;side<2;++side){word(t.character_selection[side]);word(t.confirmed[side]);word(t.health[side]);}}
    else if(probe->world){const auto& w=*probe->world;word(w.battle->state.flags);word(w.scene->phase);word(w.dialogue->id);word(w.rules.progress.round);for(u32 side=0;side<2;++side){const auto& p=*w.battle->fields[side].player;real(p.motion.position.x);real(p.motion.position.y);real(p.motion.health);real(p.control.charge);real(p.control.available);word(p.control.player_state);word(w.rules.scores[side].points);word(p.combo_state.best_hits);}}
    return hash;
}
TH09_EXPORT("th09_network_info") const u32* th09_network_info(){static u32 data[6]{};if(probe){data[0]=0;for(u32 n=0;n<16;++n)if(probe->records.versus_unlocked[n])data[0]|=1u<<n;data[1]=std::min(3u,u32(probe->settings.difficulty));data[2]=probe->settings.auto_focus[0];data[3]=network.frame();data[4]=network.active;data[5]=network.side;}return data;}
TH09_EXPORT("th09_game_close") void th09_game_close(){running=false;++loop_epoch;close_controllers();clear_inputs();network.end();probe.reset();}
TH09_EXPORT("th09_music_enabled") void th09_music_enabled(u32 enabled){if(probe){probe->host_music_enabled=enabled!=0;probe->title_configuration();}}
TH09_EXPORT("th09_game_restart") u32 th09_game_restart(){if(!probe||!probe->in_title)return 0;clear_inputs();probe->requested_transition=-1;probe->settings.game_flags=0;return probe->return_title(false);}
TH09_EXPORT("th09_game_tick") u32 th09_game_tick(u32 render){if(!probe)return 0;u16 keys[3]{};
    if(network.active){if(network.wants_input()){sample_keys(keys);const u32 frame=network.sending_frame();const auto m=probe->session&&!probe->in_title?probe->session->motion_input[network.side]:GameSession::MotionSample{};if(!network.submit(network.side,frame,keys[2],m.enabled,m.x,m.y))return 0;th09_network_send(frame,keys[2],m.enabled,m.x,m.y);}if(network.failed){probe->error="Network input order invalid";return 0;}NetworkInput::Motion motion[2];if(!network.take(keys,motion))return 2;if(probe->session)for(i32 s=0;s<2;++s)probe->session->motion_input[s]={motion[s].enabled,motion[s].x,motion[s].y};
        return probe->tick_inputs(keys[0],keys[1],keys[2],render!=0);}
    sample_keys(keys);u32 ok=1;for(u32 extra=0;extra<3&&ok;++extra){ok=probe->tick_inputs(keys[0],keys[1],keys[2],false);if(!ok||probe->in_title||probe->paused||probe->over||probe->complete||!probe->session->is_replay||probe->session->phase!=SessionPhase::match||probe->session->playback_schedule()!=6)break;}if(ok&&render)probe->draw();return ok;
}
TH09_EXPORT("th09_loop_pause") void th09_loop_pause(u32 on){suspended=on!=0;previous_frame=-1;cadence.reset();clear_inputs();if(probe){probe->clock_pause(suspended);probe->audio.suspend(suspended);}}
TH09_EXPORT("th09_loop_stop") void th09_loop_stop(){running=false;++loop_epoch;if(probe){probe->clock_pause(true);probe->audio.suspend(true);}}
TH09_EXPORT("th09_loop_start") void th09_loop_start(){
    if(!probe||running)return;running=true;suspended=false;previous_frame=-1;cadence.reset();probe->clock_pause(false);probe->audio.suspend(false);
    emscripten_request_animation_frame_loop([](double time,void* epoch)->EM_BOOL{
        if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;const double begin=emscripten_get_now(),delta=previous_frame<0?0:(time-previous_frame)/1000.;previous_frame=time;
        if(suspended||!probe){cadence.reset();return EM_TRUE;}const auto ticks=cadence.advance(delta);u32 ok=1;for(u32 n=0;n<ticks&&ok;++n){ok=th09_game_tick(n+1==ticks);if(ok==2){cadence.reset();break;}}
        probe->audio.pump();th09_browser_frame(ok,emscripten_get_now()-begin);if(!ok){running=false;probe->audio.suspend(true);}return running?EM_TRUE:EM_FALSE;
    },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
TH09_EXPORT("th09_game_draw") void th09_game_draw(){if(probe)probe->draw();}
TH09_EXPORT("th09_key") void th09_key(u32 scan,u32 down){for(auto& k:keyboard_map)if(k.scan==scan)k.hosted=down!=0;}
TH09_EXPORT("th09_keys_clear") void th09_keys_clear(){clear_inputs();}
TH09_EXPORT("th09_pulse") void th09_pulse(u32 mask){for(u32 n=0;n<16;++n)if(mask&(1u<<n))pulse_ticks[n]=2;}
TH09_EXPORT("th09_touch") void th09_touch(u32 type,i32 id,float x,float y){gestures.pointer(type,id,x,y,SDL_GetTicks(),touch_state(),false);}
TH09_EXPORT("th09_touch_cancel") void th09_touch_cancel(){gestures.cancel();}
TH09_EXPORT("th09_touch_controls") void th09_touch_controls(u32 enabled,u32 fire,u32 focus,u32 bomb,u32 escape){gestures.enabled=enabled!=0;gestures.controls(fire!=0,focus!=0,bomb,escape,0,0);}
TH09_EXPORT("th09_touch_options") void th09_touch_options(u32 enabled,u32 mode,float sensitivity,u32 two_finger,u32 double_tap){gestures.enabled=enabled!=0;gestures.mode=mode<=3?i32(mode):0;gestures.unlimited=mode==1;gestures.sensitivity=std::clamp(sensitivity,.25f,4.f);gestures.two_finger=two_finger!=0;gestures.double_tap=double_tap!=0;}
TH09_EXPORT("th09_touch_stick") void th09_touch_stick(float x,float y){gestures.stick_x=std::clamp(x/32767.f,-1.f,1.f);gestures.stick_y=std::clamp(y/32767.f,-1.f,1.f);}
TH09_EXPORT("th09_touch_state") const i32* th09_touch_state(){static i32 data[4]{};const auto s=touch_state();data[0]=s.context;data[1]=s.ready;data[2]=gestures.active();data[3]=probe&&probe->in_title;return data;}
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

