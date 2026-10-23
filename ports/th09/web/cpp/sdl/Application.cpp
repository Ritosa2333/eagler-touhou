#include "browser-gamepad-input.hpp"
// SDL application integration under validation. Test-only entry points are
// compiled behind TH09_DEVELOPMENT_HARNESS; no original executable is linked.
#include "Assets.hpp"
#include "AudioDevice.hpp"
#include "FontDevice.hpp"
#include "PerformanceDiagnostics.hpp"
#include "../game/GamePresentation.hpp"
#include "../game/BulletDraw.hpp"
#include "../game/TitleMenus.hpp"
#include "../game/GameSession.hpp"
#include "../game/MusicCatalog.hpp"
#include "../game/GameConfiguration.hpp"
#include "../game/KeyboardInput.hpp"
#include "../game/HitboxDisplay.hpp"
#include "../game/NetworkInput.hpp"
#include "../multiplayer/WorldState.hpp"
#include "../multiplayer/RollbackSession.hpp"
#include "../multiplayer/FrameAudio.hpp"
#include "../multiplayer/FrameSchedule.hpp"
#include "../multiplayer/RecoverySchedule.hpp"
#include "../multiplayer/SpectatorSchedule.hpp"
#include "../multiplayer/SpectatorPlayback.hpp"
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
EM_JS(int, th09_local_keyboard, (int seat), {return globalThis.__eaglerKeyboardInputBridge?.sampleLocal?.(seat) ?? -1;});

Netplay::PeerTransport& th09_shared_transport();
EM_JS(void, th09_browser_frame, (int ok,double milliseconds), { Module["onGameFrame"]?.(ok,milliseconds); });
EM_JS(void, th09_network_result, (), { Module["onNetworkResult"]?.(); });
EM_JS(void, th09_network_replay_boundary, (std::uint32_t frame,int winner), { Module["onNetworkReplayBoundary"]?.(frame,winner); });
EM_JS(void, th09_network_spectator_begin, (std::uint32_t frame,const void* bytes,std::uint32_t size), {
    Module["onNetworkSpectatorBegin"]?.(frame,HEAPU8.slice(bytes,bytes+size));
});
EM_JS(void, th09_network_spectator_end, (std::uint32_t frame,int winner), { Module["onNetworkSpectatorEnd"]?.(frame,winner); });
EM_JS(void, th09_network_spectator_round, (std::uint32_t frame,std::uint32_t round,std::uint32_t replayFrame), { Module["onNetworkSpectatorRound"]?.(frame,round,replayFrame); });
EM_JS(void, th09_network_request, (), { Module["onNetworkRequest"]?.(); });
EM_JS(void, th09_network_send, (unsigned frame,unsigned keys,int moving,float x,float y), { Module["onNetworkInput"]?.(frame,keys,moving,x,y); });
EM_JS(void, th09_network_spectator_frame,
      (unsigned frame,unsigned left,unsigned right,int leftMode,float leftX,float leftY,int rightMode,float rightX,float rightY,const void* charge,unsigned chargeSize), {
    Module["onNetworkSpectatorFrame"]?.(frame,left,right,leftMode,leftX,leftY,rightMode,rightX,rightY,chargeSize?HEAPU8.slice(charge,charge+chargeSize):null);
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
bool rollback_active=false,network_selection=false;
bool local_charge_priority=false;
Netplay::AdonisMode adonis_mode=Netplay::AdonisMode::Rollback;
u32 adonis_snapshots=0,adonis_waits=0;
u32 rollback_published=0,rollback_catchup=0;
// A new selection can replace every match parameter. The initializer contains
// only scalar values, never resource addresses or owning state.
struct SpectatorMatchSettings {
    u8 fresh=0,difficulty=1,characters[2]{0,1},health[2]{10,10},alternate[2]{},focus[2]{},selector=1,extra_lives=0;
    std::array<u8,16> unlocked{};
};
static_assert(sizeof(SpectatorMatchSettings)==28);
struct SpectatorBeginState { Rng random; InputFrame menu; SpectatorMatchSettings match; };
static_assert(std::is_trivially_copyable_v<SpectatorBeginState>);
static_assert(offsetof(SpectatorBeginState,menu)==sizeof(Rng));
constexpr u32 spectator_begin_legacy_bytes=sizeof(Rng)+sizeof(InputFrame);
constexpr u32 spectator_begin_bytes=offsetof(SpectatorBeginState,match)+sizeof(SpectatorMatchSettings);
static_assert(spectator_begin_bytes==78);
SpectatorBeginState spectator_begin_buffer;
bool spectator_after_restart=false;
struct RollbackFrame {
    u32 number=Netplay::INVALID_FRAME,hash=0;
    std::unique_ptr<multiplayer::WorldState> state;
    bool captured=false;
    bool hasBegin=false,hasEnd=false,gameplay=false;
    u32 beginFrame=0;
    i32 winner=-1;
    i32 round=-1;
    u32 replayRoundFrame=0;
    SpectatorBeginState begin;
    using Sound=multiplayer::FrameAudio::Event;
    std::vector<Sound> sounds;
    multiplayer::FrameAudio audio;
};
std::array<RollbackFrame,multiplayer::RollbackSession::History+1> rollback_frames;
RollbackFrame* recording_frame=nullptr;
#if TH09_DEVELOPMENT_HARNESS
u32 rollback_test_limit=Netplay::INVALID_FRAME;
double probe_tick_cost[2]{}; // Update/UI and semantic Draw; diagnostic only.
bool probe_full_sprite_geometry=false;
std::array<u8,80> probe_charge_reference_bytes;
bool probe_charge_reference=false;
std::array<u32,2> probe_stop_boundary_waits{};
#endif
struct SpectatorFrame { u32 frame=0;u16 keys[2]{};NetworkInput::Motion motion[2];bool localCharge=false;std::array<ChargeSample,2> charges; };
std::array<u8,80> spectator_charge_buffer;
std::deque<SpectatorFrame> spectator_frames;
bool spectator_mode=false;
multiplayer::SpectatorPlayback spectator_playback;
u32 spectator_next=0,spectator_simulated=0;
u32 spectator_last_ticks=0,spectator_max_ticks=0,spectator_yields=0,spectator_max_work_us=0;
struct Application final:GameMedia,InGameMenuServices,TitleServices,EndingServices {
    LocalChargePlanner local_charge;
    std::array<ChargeFrame,2> charge_frames;
    Assets assets;GraphicsDevice graphics;FontDevice fonts{graphics};AudioDevice audio;
    bool platform_prepared=false;Rng loading_random;AnmExecutor loading_executor{loading_random};
    std::unique_ptr<GameResources> loading_resources;std::array<AnmVm,3> loading_vms{};
    u32 startup_background=0,startup_branding=0;
    EclWorldState state;AnmExecutor executor{state.random};GameResources resources{assets,graphics,executor};
    GamePresentation presentation{graphics,resources,*this};InGameMenus menus{resources,*this};
    std::array<u8,2> local_focus{};i32 local_versus=1;
    void clear_charge(){local_charge.clear();charge_frames={};if(session)session->charge_frames={};}
    void release_network(){network.end();clear_charge();network_selection=false;settings.auto_focus=local_focus;settings.versus=local_versus;sync_records();}
    PlayerRecords records;bool clock_running=true,host_music_enabled=true;TitleSettings settings;GameConfiguration saved_configuration;u32 storage_revision=0;std::vector<u8> import_bytes;std::unique_ptr<TitleMenus> title;bool in_title=false,launch_pending=false;std::map<std::string,u32> title_images;u32 background_image=0;
    std::unique_ptr<GameSession> session;GameWorld* world=nullptr;WorldConfiguration configuration;InputFrame device;
    ReplayFile pending_replay;u32 replay_stage=0;bool replay_pending=false,demo_pending=false;u32 ending_image=0,ending_width=640,ending_height=480;
    bool paused=false,over=false,complete=false;u32 frames=0;std::string error;Vec2 shake[3]{};
    u32 stop_state()const{
        if(!world||in_title)return 0;const auto& battle=*world->battle;
        return (battle.state.flags&0x1800u)|((battle.fields[0].script.flags&1u)<<16)|((battle.fields[1].script.flags&1u)<<17);
    }
    SpectatorBeginState match_initializer(bool fresh)const{
        SpectatorBeginState out{state.random,device};auto& m=out.match;const auto& c=configuration;
        m.fresh=fresh;m.difficulty=u8(c.selection.difficulty);m.selector=u8(c.selection.selector+2);m.extra_lives=u8(c.starting_extra_lives);m.unlocked=c.selection.unlocked;
        for(i32 s=0;s<2;++s){m.characters[s]=u8(c.selection.characters[s]);m.health[s]=u8(c.health[s]);m.alternate[s]=c.alternate[s];m.focus[s]=c.automatic_focus[s];}return out;
    }
    bool reselect_network_match(){
        // This action is called only at the driver's confirmed resource boundary.
        presentation.renderer.flush();session->finish();presentation.world=nullptr;world=nullptr;
        title=std::make_unique<TitleMenus>(resources,*this,state.random,settings);
        if(!title->initialize()){error=title->error;return false;}
        if(!title->open_network_selection()){error=title->error;return false;}in_title=true;launch_pending=false;
        paused=over=complete=false;pending_action=requested_transition=-1;
        settings.versus=4;settings.game_flags=0;settings.versus_unlocked.fill(1);
        pause_audio(false);music(0);return true;
    }
    bool checkpoint(multiplayer::WorldState& out){
        TH09_PERF_SCOPE(checkpoint_cost,Checkpoint);
        presentation.renderer.flush();graphics.backend.flush();
        if(in_title){
            if(!title||!out.Begin())return false;
            out.Fields(state.random,settings,title->state,title->input,title->descriptions,title->stages,title->leaving,executor.timing,executor.executed,executor.invalid);
            out.Vector(title->animations);out.Text(title->error);
        }else if(!out.Save(*session))return false;
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
    bool prepare_platform(){
        if(platform_prepared)return true;
        {TH09_PERF_SCOPE(graphics_cost,GraphicsInit);if(!graphics.initialize()){error=graphics.error;return false;}}
        if(!assets.open("/th09.dat")){error=assets.error;return false;}platform_prepared=true;return true;
    }
    bool prepare_loading(){
        if(!prepare_platform()||!title_background("th09logo.jpg"))return false;
        startup_background=background_image;
        // Optional in standalone development harnesses. Managed builds install
        // a separate transparent texture, never a replacement retail image.
        std::vector<u8> branding;
        if(read_file("/eagler-startup.png",branding)){
            const auto texture=graphics.image(branding.data(),u32(branding.size()));
            if(!texture.handle){error=graphics.error;return false;}
            startup_branding=texture.handle;
        }
        loading_resources=std::make_unique<GameResources>(assets,graphics,loading_executor);
        if(!loading_resources->load(AnimationFile::menu,"nowloading.anm")){error=loading_resources->error;return false;}
        for(i32 i=0;i<3;++i){if(!loading_resources->start(AnimationFile::menu,loading_vms[i],i)){error="Startup loading animation failed";return false;}loading_vms[i].pos={500,440,0};}
        presentation.begin_frame();title_begin_draw();for(auto& vm:loading_vms)presentation.renderer.draw_2d(vm);presentation.finish_frame();graphics.present();return true;
    }
    bool initialize_platform(){
        TH09_PERF_SCOPE(platform_cost,PlatformInit);
        if(!prepare_platform())return false;
        {TH09_PERF_SCOPE(font_cost,FontInit);if(!fonts.initialize()){error=fonts.error;return false;}}
        {TH09_PERF_SCOPE(warm_cost,FontWarm);fonts.prewarm_game(assets);}
        if(!audio.initialize(assets)){error=audio.error;return false;}SDL_CreateDirectory("/save/replay");std::vector<u8> bytes;if(read_file("/save/th09.cfg",bytes))saved_configuration.load(bytes.data(),u32(bytes.size()));saved_configuration.apply(settings);title_configuration();if(!write_file("/save/th09.cfg",saved_configuration.data().data(),204)){error="Unable to save configuration";return false;}if(read_file("/save/score.dat",bytes))records.load(bytes.data(),u32(bytes.size()));sync_records();records.application_clock=records.game_clock=u32(SDL_GetTicks());loading_vms={};loading_resources.reset();return true;
    }
    void update_clocks(){const u32 now=u32(SDL_GetTicks());if(clock_running)records.update_application_clock(now);else records.application_clock=now;if(clock_running&&!in_title&&!paused&&!over&&!complete&&session&&session->phase==SessionPhase::match)records.update_game_clock(now);else records.game_clock=now;}
    void clock_pause(bool on){update_clocks();clock_running=!on;}
    bool open_title(){TH09_PERF_SCOPE(title_cost,TitleInit);if(!initialize_platform())return false;state.random={0x7531,0,0};title=std::make_unique<TitleMenus>(resources,*this,state.random,settings);if(!title->initialize()||!resources.load(AnimationFile::ascii,"ascii.anm")||!presentation.ascii.initialize()){error="Title initialization "+title->error+resources.error;return false;}in_title=true;return true;}
    bool open(i32 left,i32 right,i32 mode,i32 diff){
        if(!initialize_platform())return false;
        state.random={0x7531,0,0};configuration.selection.characters[0]=left;configuration.selection.characters[1]=right;configuration.selection.mode=GameMode(mode);configuration.selection.difficulty=diff;configuration.selection.lives=2;configuration.selection.selector=left;
        configuration.controllers[1]=1;return start();
    }
    bool start(){
        TH09_PERF_SCOPE(start_cost,StartMatch);
        presentation.renderer.flush();presentation.world=nullptr;world=nullptr;session.reset();executor.invalid=false;
        session=std::make_unique<GameSession>(state,resources,executor,presentation,*this,records);
        local_charge.clear();if(!network.active&&!spectator_mode)charge_frames={};session->charge_frames=charge_frames;
        bool ok=false;if(replay_pending){replay_pending=false;ok=session->play(pending_replay,replay_stage,demo_pending);}
        else {ReplayMetadata meta;meta.configuration=saved_configuration.data();meta.mode=u8(configuration.selection.mode);meta.difficulty=u8(configuration.selection.difficulty);// Native replays describe the two controller types; web transport is not
            // part of the recorded simulation. Network play is human vs human.
            meta.versus=u8(settings.versus==4?0:settings.versus);for(i32 side=0;side<2;++side){meta.health[side]=u8(configuration.health[side]);meta.alternate[side]=configuration.alternate[side];meta.configuration[0xb4+side]=configuration.automatic_focus[side];}meta.configuration[0xac]=u8(configuration.starting_extra_lives);
            const auto now=std::time(nullptr);char date[10]="--/--/--";if(const auto* local=std::localtime(&now))std::strftime(date,sizeof(date),"%y/%m/%d",local);ok=session->begin(configuration,meta,date);}
        if(!ok){error=session->error;return false;}world=session->world.get();presentation.world=world;
        if(!presentation.ascii.initialize()){error="ASCII initialization failed";return false;}paused=over=complete=false;menus.pause={};menus.game_over={};menus.match_end={};requested_transition=-1;return true;
    }
    bool render_text(AnmVm& a,const char* s,u32 color,u32 shadow)override{TH09_PERF_SCOPE(text_cost,TextRender);if(!fonts.text(a,s,color,shadow)){error=fonts.error;return false;}return true;}
    bool replay_view_silent=false;
    void sound(i32 id,i32 pan)override{if(replay_view_silent||(spectator_mode&&spectator_playback.seeking))return;if(!queue_sound(0,id,float(pan)))audio.effects.enqueue(id,pan);}
    void positioned_sound(i32 id,float x)override{if(replay_view_silent||(spectator_mode&&spectator_playback.seeking))return;if(!queue_sound(1,id,x))audio.effects.positioned(id,x);}
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
        TH09_PERF_SCOPE(title_update_cost,Update);
        update_clocks();device.update(network_selection?u16(left|right):keys);left_device.update(left);right_device.update(right);if(!title||!error.empty())return false;
        if(!launch_pending)title->update(left_device,right_device,device);if(network.active&&title->state.screen==TitleScreen::versus_type){release_network();title->state={};th09_network_result();}save_configuration();if(!title->error.empty()){error=title->error;return false;}
        TH09_PERF_STOP(title_update_cost);
        if(launch_pending){
            // This tick constructs the world. Its first gameplay input is the
            // following tick, unlike Retry which updates its new world now.
            if(network_selection&&recording_frame){recording_frame->begin=match_initializer(true);recording_frame->hasBegin=true;recording_frame->beginFrame=rollback.Frame()+1;}
            launch_pending=false;in_title=false;if(!start())return false;
        }
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
        if(startup_branding&&background_image==startup_background){
            // Same projection and draw order as the startup background. All
            // subsequent authored animations/covers also cover the branding.
            pipeline.blend=true;pipeline.sourceBlend=BlendFactor::SourceAlpha;
            pipeline.destinationBlend=BlendFactor::InverseSourceAlpha;
            graphics.draw(pipeline,startup_branding,Topology::Strip,VertexLayout::ScreenColorUv,corners,4);
        }
    }
    void title_configuration()override{audio.music_volume=settings.music_volume;audio.music_enabled=host_music_enabled&&settings.music_mode!=0;audio.effects.enabled=settings.effects!=0;audio.effects.master_volume=settings.sound_volume;audio.refresh_volume();}
    bool write_file(const char* path,const u8* bytes,u32 size){auto* file=SDL_IOFromFile(path,"wb");const bool ok=file&&SDL_WriteIO(file,bytes,size)==size;if(file)SDL_CloseIO(file);if(ok)++storage_revision;return ok;}
    void save_configuration(){if(network.active||spectator_mode)return;if(saved_configuration.capture(settings)){title_configuration();const auto& bytes=saved_configuration.data();if(!write_file("/save/th09.cfg",bytes.data(),u32(bytes.size())))error="Unable to save th09.cfg";}}
    u32 title_clear_count(i32 c,i32 d)override{return records.clear_count(c,d);}
    PlayerRecords& title_records()override{return records;}
    void title_save_records()override{if(spectator_mode)return;update_clocks();auto local_random=state.random;auto bytes=records.save(network.active?local_random:state.random);if(!write_file("/save/score.dat",bytes.data(),u32(bytes.size())))error="Unable to save score.dat";}
    void title_ascii(const Vec3& p,const char* text,u32 color,const Vec2& scale)override{presentation.ascii.color=color;presentation.ascii.scale=scale;presentation.ascii.field_view=0;presentation.ascii.text(p,text);}
    i32 title_joy_button(i32 device)override{return joy_button(device);}
    static std::string replay_path(const char* name){
        std::string path=name?name:"";if(path.rfind("./",0)==0)path.erase(0,2);
        if(path.rfind("replay/",0)!=0||path.size()>512||path.find('\\')!=std::string::npos)return {};
        const auto relative=path.substr(7);const auto slash=relative.find('/');
        if(relative.empty()||relative=="."||relative=="..")return {};
        if(slash!=std::string::npos&&(slash==0||relative.substr(0,slash)=="."||relative.substr(0,slash)==".."||relative.find('/',slash+1)!=std::string::npos||relative.substr(slash+1).empty()||relative.substr(slash+1)=="."||relative.substr(slash+1)==".."))return {};
        return "/save/"+path;
    }
    bool title_read_replay(const char* name,std::vector<u8>& bytes)override{const auto path=replay_path(name);return !path.empty()&&read_file(path.c_str(),bytes);}
    std::vector<std::string> title_imported_replays()override{
        std::vector<std::string> names;auto* dir=opendir("/save/replay");
        if(dir){while(auto* entry=readdir(dir)){std::string n=entry->d_name;if(n.rfind("th9_ud",0)==0&&n.size()==14&&n.substr(10)==".rpy")names.push_back(n);}closedir(dir);}
        std::sort(names.begin(),names.end());return names;
    }
    bool title_save_replay(const char* name,const char* player)override{const auto path=replay_path(name);if(!session||path.empty())return false;auto replay=session->save_replay(player);const auto bytes=replay.encode();if(bytes.empty()){error=session->error;return false;}const bool ok=write_file(path.c_str(),bytes.data(),u32(bytes.size()));if(!ok)error="Unable to write replay";return ok;}
    bool title_replay_exists(const char* name)override{const auto path=replay_path(name);auto* file=path.empty()?nullptr:SDL_IOFromFile(path.c_str(),"rb");if(!file)return false;SDL_CloseIO(file);return true;}
    void title_play_replay(const ReplayFile& file,u32 stage,const char*)override{pending_replay=file;replay_stage=stage;replay_pending=launch_pending=true;demo_pending=false;}
    void title_launch(const WorldConfiguration& c)override{configuration=c;configuration.starting_extra_lives=network_selection?0:saved_configuration.extra_lives();for(i32 n=0;n<2;++n)configuration.automatic_focus[n]=settings.auto_focus[n]!=0;launch_pending=true;}
    void title_demo(u32 index)override{char name[24];std::snprintf(name,sizeof(name),"demorpy%u.rpy",index%3);std::vector<u8> bytes;if(!assets.read(name,bytes)||!pending_replay.decode(bytes.data(),u32(bytes.size()))){error="Invalid demonstration replay";return;}replay_stage=9;replay_pending=launch_pending=demo_pending=true;}
    void title_network()override{th09_network_request();}
    void title_exit()override{title_save_records();save_configuration();requested_transition=0;}
    void sync_records(){settings.versus_unlocked=records.versus_unlocked;settings.story_unlocked=records.story_unlocked;settings.extra_unlocked=records.extra_unlocked;settings.music_unlocked=records.music_unlocked;}
    bool return_title(bool score){
        clear_charge();
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
    bool tick_inputs(u16 left,u16 right,u16 keys,bool render=true,bool afterRestart=false,bool submit=true){
        TH09_PERF_SCOPE(tick_cost,TickInputs);
#if TH09_DEVELOPMENT_HARNESS
        const double cost_begin=emscripten_get_now();
#endif
        if(in_title)return tick_title(left,right,keys,render);if(!session||!world||!error.empty())return false;
        TH09_PERF_SCOPE(update_cost,Update);
        if(!afterRestart){
        update_clocks();device.update(keys);mode=world->rules.mode;difficulty=world->configuration.selection.difficulty;game_flags=world->battle->state.flags;continues=session->continues;
        PopupFrame popup;popup.paused=paused;popup.game_over=over;popup.game_flags=game_flags;for(i32 s=0;s<2;++s)popup.field_flags[s]=world->battle->fields[s].script.flags;presentation.ascii.update(popup);
        if(paused)menus.update_pause(device);else if(over)menus.update_game_over(device);else if(complete)menus.update_match_end(device);else if(!session->is_demo()&&(device.pressed&8)&&session->phase==SessionPhase::match){paused=world->paused=true;sound(34,0);pause_audio(true);}
        if(pending_action>=0){
            // Fail before destroying resources if a future menu rule bypasses
            // the driver's read-only lifetime barrier. Never rewind freed owners.
            if(recording_frame&&recording_frame->captured){error="Speculative menu resource action";return false;}
            const auto action=InGameAction(pending_action);pending_action=-1;
            if(action==InGameAction::retry||action==InGameAction::replay_retry||action==InGameAction::restart_extra){
                if(network.active){const auto begin=match_initializer(false);
                    if(recording_frame){recording_frame->begin=begin;recording_frame->hasBegin=true;}
                    else {th09_network_replay_boundary(network.frame()-1,-1);if(network.side==0)th09_network_spectator_begin(network.frame()-1,&begin,spectator_begin_bytes);}}
                presentation.renderer.flush();if(!session->retry()){error=session->error;return false;}world=session->world.get();presentation.world=world;paused=over=complete=false;menus.pause={};menus.game_over={};menus.match_end={};pause_audio(false);}
            else if(action==InGameAction::continue_match){pause_audio(true);if(!session->continue_game()){error="Unable to continue";return false;}paused=over=complete=false;pause_audio(false);}
            else if(action==InGameAction::title&&network_selection&&complete)return reselect_network_match();
            else {session->finish();return return_title(action==InGameAction::save_score);}
        }
        }
        if(!paused&&!over&&!complete){presentation.renderer.flush();if(!session->update(left,right,keys)){error=session->error;return false;}world=session->world.get();presentation.world=world;}
        over=session->phase==SessionPhase::game_over;complete=session->phase==SessionPhase::match_complete;
        if(session->phase==SessionPhase::finished)return return_title(!session->is_replay);
        requested_transition=world->transition_pending?100+i32(world->transition):-1;
        TH09_PERF_STOP(update_cost);
#if TH09_DEVELOPMENT_HARNESS
        const double cost_draw=emscripten_get_now();probe_tick_cost[0]=cost_draw-cost_begin;
#endif
        if(rollback_active){
            graphics.skipSubmission=!render;presentation.renderer.omit_sprite_geometry=!render;
#if TH09_DEVELOPMENT_HARNESS
            if(probe_full_sprite_geometry)presentation.renderer.omit_sprite_geometry=false;
#endif
            draw(render&&submit);presentation.renderer.omit_sprite_geometry=false;graphics.skipSubmission=false;
        }
        else if(render)draw();
#if TH09_DEVELOPMENT_HARNESS
        probe_tick_cost[1]=emscripten_get_now()-cost_draw;
#endif
        if(!recording_frame){audio.update();audio.pump();}
        {TH09_PERF_SCOPE(warm_cost,WarmResources);session->warm_resources();}
        ++frames;return error.empty();
    }
    void draw(bool present=true){
        TH09_PERF_SCOPE(draw_cost,SemanticDraw);
        presentation.begin_frame();if(in_title){title->draw();presentation.ascii.draw_text();presentation.ascii.clear_text();}else if(session->phase==SessionPhase::ending)session->ending->draw();else world->draw();presentation.finish_frame();
        TH09_PERF_STOP(draw_cost);
        if(present){TH09_PERF_SCOPE(present_cost,Present);graphics.present();}
    }
    i32 requested_transition=-1;
    bool import_file(u32 kind,u32 size){
        if(!in_title||size>import_bytes.size())return false;
        const u8* bytes=import_bytes.data();
        if(kind==0){PlayerRecords next;if(!next.load(bytes,size))return false;if(!write_file("/save/score.dat",bytes,size))return false;records=std::move(next);records.application_clock=records.game_clock=u32(SDL_GetTicks());sync_records();title->state.selection=0;title->change(TitleScreen::main);return true;}
        if(kind==1){ReplayFile file;touch::MotionTrack motion;ChargeReplayTrack charge;if(!file.decode(bytes,size)||!motion.load(bytes,size,9)||!charge.load(file.data().data(),u32(file.data().size())))return false;bool found=false;for(u32 stage=0;stage<10;++stage)if(file.stream_offset(0,stage)){ReplayPlayback playback;ReplayRoundSettings header;if(!playback.begin(file,stage,header))return false;found=true;}if(!found)return false;
            for(u32 n=1;n<=9999;++n){char name[40];std::snprintf(name,sizeof(name),"replay/th9_ud%04u.rpy",n);if(!title_replay_exists(name)){const auto path=replay_path(name);return write_file(path.c_str(),bytes,size);}}return false;}
        if(kind==2){GameConfiguration next;if(!next.load(bytes,size))return false;if(!write_file("/save/th09.cfg",bytes,size))return false;saved_configuration=next;saved_configuration.apply(settings);title_configuration();return true;}return false;
    }
};
std::unique_ptr<Application> probe;std::string failure;
bool running=false,suspended=false,room_preview=false;u32 loop_epoch=0;double previous_frame=-1;multiplayer::FrameSchedule cadence;
multiplayer::RecoverySchedule recovery_schedule;
u32 recovery_callbacks=0,recovery_ticks=0,recovery_last_us=0;
bool deferred_recovery_active(){
    return recovery_schedule.Enabled()&&running&&!suspended&&rollback_active&&network.active&&
        rollback.Ready()&&rollback.Mode()==Netplay::AdonisMode::Rollback&&probe&&!probe->in_title&&
        probe->session&&probe->session->phase==SessionPhase::match&&!spectator_mode&&
        !probe->paused&&!probe->over&&!probe->complete&&probe->pending_action<0;
}
struct Key {const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
touhou::input::TouchController gestures;
KeyboardInputBuffer keyboard_inputs;
int launcher_gamepad[2]{-1,-1};
SDL_Gamepad* controllers[2]{};u32 pulse_ticks[16]{};u32 auto_fire_frame=0;
constexpr SDL_GamepadButton gamepad_slots[]={
    SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH,
    SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,SDL_GAMEPAD_BUTTON_BACK,
    SDL_GAMEPAD_BUTTON_START,SDL_GAMEPAD_BUTTON_LEFT_STICK,SDL_GAMEPAD_BUTTON_RIGHT_STICK,SDL_GAMEPAD_BUTTON_GUIDE,
};
void close_controllers(){for(auto*& p:controllers)if(p){SDL_CloseGamepad(p);p=nullptr;}}
void add_controller(SDL_JoystickID id){for(const auto* p:controllers)if(p&&SDL_GetGamepadID(const_cast<SDL_Gamepad*>(p))==id)return;for(auto*& p:controllers)if(!p){p=SDL_OpenGamepad(id);break;}}
i32 joy_button(i32 n){if(eagler::input::BrowserGamepadOwned()||n<0||n>1||!controllers[n])return 32;for(i32 i=0;i<i32(std::size(gamepad_slots));++i)if(SDL_GetGamepadButton(controllers[n],gamepad_slots[i]))return i;return 32;}
u16 joy_keys(i32 side){if(!probe||side<0||side>1)return 0;
    if(launcher_gamepad[side]>=0){bool keys[256]{};eagler::input::ApplyGamepadVirtualKeys(keys,launcher_gamepad[side]);return keyboard_input(keys,2);}
    const u32 device=probe->settings.devices[side];if(device>1||!controllers[device])return 0;auto* p=controllers[device];u16 keys=0;for(i32 n=0;n<9;++n){const i32 button=probe->settings.bindings[side].gamepad[n];if(button>=0&&button<i32(std::size(gamepad_slots))&&SDL_GetGamepadButton(p,gamepad_slots[button]))keys|=u16(1u<<n);}const i32 x=SDL_GetGamepadAxis(p,SDL_GAMEPAD_AXIS_LEFTX),y=SDL_GetGamepadAxis(p,SDL_GAMEPAD_AXIS_LEFTY);if(x<-19660||SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_LEFT))keys|=64;if(x>19660||SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_RIGHT))keys|=128;if(y<-19660||SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_UP))keys|=16;if(y>19660||SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_DOWN))keys|=32;return keys;}
touhou::input::TouchState touch_state(){
    touhou::input::TouchState s;if(!probe)return s;if(probe->in_title||probe->paused||probe->over||probe->complete||!probe->world||!probe->session)return s;
    auto& run=*probe->session;auto& w=*probe->world;if(run.is_replay){s.context=3;return s;}if(run.phase==SessionPhase::ending){s.context=2;return s;}
    s.context=w.dialogue->id>=0?2:1;
    i32 side=network.active?network.side:w.configuration.controllers[0]?1:0;if(w.configuration.controllers[side]){s.context=0;return s;}auto& p=*w.battle->fields[side].player;s.instance=1+side+2*w.configuration.selection.stage+32*w.rules.progress.round;s.ready=(p.control.player_state==0||p.control.player_state==3)&&p.motion.health>0;
    s.x=p.motion.position.x;s.y=p.motion.position.y;s.fast=p.resource.movement.normal;s.slow=p.resource.movement.focused;const auto& limit=w.battle->state.limits;s.min_x=limit.origin.x;s.max_x=limit.origin.x+limit.extent.x;s.min_y=limit.origin.y;s.max_y=limit.origin.y+limit.extent.y;return s;
}
void clear_inputs(){th09_reset_browser_keyboard();SDL_ResetKeyboard();for(auto& k:keyboard_map)k.hosted=false;keyboard_inputs.clear();gestures.reset();if(probe&&probe->session)probe->session->clear_motion();std::fill(std::begin(pulse_ticks),std::end(pulse_ticks),0);}
bool local_versus_input_context(){
    if(!probe||network.active)return false;
    const bool local_world=probe->world&&probe->world->rules.mode==GameMode::versus&&!probe->world->configuration.controllers[0]&&!probe->world->configuration.controllers[1];
    if(!probe->in_title)return local_world;
    if(!probe->title||probe->settings.versus!=0)return false;
    const auto screen=probe->title->state.screen;
    return screen==TitleScreen::versus_difficulty||screen==TitleScreen::versus_character||screen==TitleScreen::versus_stage||(local_world&&screen==TitleScreen::replay_name);
}
void sample_keys(u16 (&out)[3]){
    SDL_Event e;while(SDL_PollEvent(&e)){if(e.type==SDL_EVENT_GAMEPAD_ADDED)add_controller(e.gdevice.which);else if(e.type==SDL_EVENT_GAMEPAD_REMOVED)for(auto*& p:controllers)if(p&&SDL_GetGamepadID(p)==e.gdevice.which){SDL_CloseGamepad(p);p=nullptr;}}
    for(unsigned seat=0;seat<2;++seat)launcher_gamepad[seat]=eagler::input::BrowserGamepadState(seat);
    int local_keyboard[2];u16 local_keys[2]{};
    for(unsigned seat=0;seat<2;++seat){local_keyboard[seat]=th09_local_keyboard(seat);if(local_keyboard[seat]>=0){bool mapped[256]{};eagler::input::ApplyGamepadVirtualKeys(mapped,local_keyboard[seat]);local_keys[seat]=keyboard_input(mapped,2);}}
    bool keys[256]{};const auto* physical=th09_browser_keyboard()?nullptr:SDL_GetKeyboardState(nullptr);for(const auto& k:keyboard_map)if(k.hosted||(physical&&k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){if(k.vk)keys[k.vk]=true;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=true;if(k.scan==28||k.scan==156)keys[13]=true;}
    const int keyboardDpad=th09_keyboard_gamepad_dpad();if(keyboardDpad&1)keys[38]=true;if(keyboardDpad&2)keys[40]=true;if(keyboardDpad&4)keys[37]=true;if(keyboardDpad&8)keys[39]=true;
    const auto state=touch_state();keyboard_inputs.sample(keys,state.context==1);
    const u16 keyboard[]={keyboard_inputs.input(keys,probe->settings.devices[0]),keyboard_inputs.input(keys,probe->settings.devices[1]),keyboard_inputs.input(keys,2)};
    if(local_keyboard[0]>=0)eagler::input::ApplyGamepadVirtualKeys(keys,local_keyboard[0]|std::max(0,launcher_gamepad[0]));
    auto sample=gestures.sample(state,SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);
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
    out[2]=keyboard[2]|keyboard_input(sample.keys,2)|joy_keys(0)|joy_keys(1)|local_keys[0]|local_keys[1];for(u32 n=0;n<16;++n)if(pulse_ticks[n]){out[2]|=u16(1u<<n);--pulse_ticks[n];}
    const bool paired=local_versus_input_context();
    for(i32 side=0;side<2;++side)out[side]=paired?(local_keyboard[side]>=0?local_keys[side]|joy_keys(side)|(side==0?keyboard_input(sample.keys,2):0):keyboard[side]|keyboard_input(sample.keys,probe->settings.devices[side])|joy_keys(side)):out[2];
}

}
extern "C" {
#define TH09_EXPORT(n) __attribute__((export_name(n)))
void queue_rollback_recovery();
TH09_EXPORT("th09_local_charge_priority") u32 th09_local_charge_priority(u32 enabled){
    if(enabled>1||rollback_active)return 0;local_charge_priority=enabled!=0;
    if(probe)probe->clear_charge();return 1;
}
TH09_EXPORT("th09_rollback_recovery_mode") u32 th09_rollback_recovery_mode(u32 after){
    if(after>1||running||rollback_active)return 0;
    recovery_schedule.Configure(after!=0);recovery_callbacks=recovery_ticks=recovery_last_us=0;return 1;
}
TH09_EXPORT("th09_rollback_recovery_info") const u32* th09_rollback_recovery_info(){
    static u32 data[5];data[0]=1;data[1]=recovery_schedule.Enabled();data[2]=recovery_callbacks;
    data[3]=recovery_ticks;data[4]=recovery_last_us;return data;
}
TH09_EXPORT("th09_game_open") u32 th09_game_open(u32 seed){
    TH09_PERF_SCOPE(open_cost,GameOpen);
    network.end();spectator_mode=false;spectator_frames.clear();spectator_next=spectator_simulated=0;clear_inputs();close_controllers();SDL_InitSubSystem(SDL_INIT_GAMEPAD);i32 count=0;auto* ids=SDL_GetGamepads(&count);for(i32 n=0;n<count;++n)add_controller(ids[n]);SDL_free(ids);for(auto& k:keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);
    if(!probe||!probe->platform_prepared||probe->in_title)probe=std::make_unique<Application>();if(!probe->open_title()){failure=probe->error;return 0;}probe->state.random={u16(seed),0,0};return 1;
}
TH09_EXPORT("th09_room_preview") u32 th09_room_preview(u32 on){
    if(!probe||!probe->in_title||network.active||spectator_mode)return 0;
    clear_inputs();room_preview=on!=0;return 1;
}
TH09_EXPORT("th09_room_return") u32 th09_room_return(){if(!probe)return 0;clear_inputs();if(probe->session)probe->session->finish();if(!probe->return_title(false))return 0;spectator_mode=false;spectator_frames.clear();probe->requested_transition=-1;room_preview=true;return 1;}
TH09_EXPORT("th09_prepare_loading") u32 th09_prepare_loading(){TH09_PERF_SCOPE(loading_cost,PrepareLoading);if(running)return 0;probe=std::make_unique<Application>();if(!probe->prepare_loading()){failure=probe->error;return 0;}return 1;}
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
// Read-only diagnostics: actual input shortage is distinct from pending replay.
TH09_EXPORT("th09_network_pacing_info") const u32* th09_network_pacing_info(){
    static u32 data[6];data[0]=1;data[1]=rollback.MissingRemoteInputs();
    data[2]=u32(rollback.NetworkIntervalScale()*1000+.5);data[3]=u32(rollback.IntervalScale()*1000+.5);
    data[4]=rollback.PredictionFrontier();data[5]=rollback.RemoteInputFrontier();return data;
}
TH09_EXPORT("th09_rollback_enable") u32 th09_rollback_enable(u32 seed,u32 side,u32 idLow,u32 idHigh,u32 abi,u32 inputDelay){
    if(rollback_active||!probe||!network.active||side>1||inputDelay>9)return 0;
    for(auto& frame:rollback_frames){frame.number=Netplay::INVALID_FRAME;frame.captured=false;frame.sounds.clear();}
    Netplay::SessionConfig c;c.seed=seed;c.localPlayer=side;c.gameId=9;c.gameplayAbi=abi;c.sessionId=(std::uint64_t(idHigh)<<32)|idLow;
    if(!rollback.SetLocalChargePriority(local_charge_priority))return 0;probe->local_charge.clear();
    rollback_published=rollback_catchup=adonis_snapshots=adonis_waits=0;rollback_active=rollback.Begin(c,std::uint64_t(emscripten_get_now()),u8(inputDelay),adonis_mode);
    if(rollback_active&&!network_selection)th09_network_replay_boundary(0,-1);return rollback_active;
}
TH09_EXPORT("th09_network_selection_begin") u32 th09_network_selection_begin(u32 seed,u32 side,u32 idLow,u32 idHigh,u32 abi,u32 requestedDelay,u32 reserve){
    if(rollback_active||side>1||reserve<1||reserve>2||
       (requestedDelay!=Netplay::AdonisStartup::Automatic&&requestedDelay>9)||
       !th09_network_begin(seed,i32(side),0xffff,1,0))return 0;
    network_selection=true;
    if(!probe->title->open_network_selection()){probe->error=probe->title->error;return 0;}
    // Warm the shared selection resources before timing probes. Future world
    // construction stays at an exact, non-predicted menu transition.
    if(!probe->tick_title(0,0,0,false))return 0;
    if(adonis_mode==Netplay::AdonisMode::Rollback)
        return th09_rollback_enable(seed,side,idLow,idHigh,abi,requestedDelay==Netplay::AdonisStartup::Automatic?0:requestedDelay);
    for(auto& frame:rollback_frames){frame.number=Netplay::INVALID_FRAME;frame.captured=false;frame.sounds.clear();}
    Netplay::SessionConfig c;c.seed=seed;c.localPlayer=side;c.gameId=9;c.gameplayAbi=abi;c.sessionId=(std::uint64_t(idHigh)<<32)|idLow;
    rollback_published=rollback_catchup=adonis_snapshots=adonis_waits=0;
    if(!rollback.SetLocalChargePriority(local_charge_priority))return 0;probe->local_charge.clear();
    rollback_active=rollback.BeginMeasured(c,std::uint64_t(emscripten_get_now()),requestedDelay,adonis_mode,reserve);return rollback_active;
}
TH09_EXPORT("th09_rollback_begin") u32 th09_rollback_begin(u32 seed,u32 side,u32 difficulty,u32 left,u32 right,u32 idLow,u32 idHigh,u32 abi,u32 inputDelay){
    if(rollback_active||inputDelay>9||!th09_network_room_begin(seed,i32(side),0xffff,difficulty,0,left,right))return 0;
    // Commit the resource-owning title transition before the prediction epoch.
    if(!probe->tick_title(0,0,0,false)||probe->in_title)return 0;
    return th09_rollback_enable(seed,side,idLow,idHigh,abi,inputDelay);
}
u32 pump_rollback_transport(bool deferOrdinaryInput){
    TH09_PERF_SCOPE(network_cost,NetworkPump);
    if(!rollback_active)return 0;
    const double now=emscripten_get_now();
    if(!rollback.Pump(std::uint64_t(now),network.active,std::uint64_t(now*1000),deferOrdinaryInput)){probe->error=rollback.Error();return 0;}
    return rollback.Ready()?2:1;
}
// Receive/ACK/repair remain independent of display and recovery scheduling.
TH09_EXPORT("th09_rollback_pump") u32 th09_rollback_pump(){return pump_rollback_transport(false);}
TH09_EXPORT("th09_measured_begin") u32 th09_measured_begin(u32 seed,u32 side,u32 difficulty,u32 left,u32 right,
                                                          u32 idLow,u32 idHigh,u32 abi,u32 requestedDelay,u32 reserve){
    if(rollback_active || (requestedDelay!=Netplay::AdonisStartup::Automatic&&requestedDelay>9) ||
       reserve<1||reserve>2 || adonis_mode==Netplay::AdonisMode::Rollback ||
       !th09_network_room_begin(seed,i32(side),0xffff,difficulty,0,left,right))return 0;
    // Finish resource construction BEFORE input-lane calibration. Gameplay
    // remains at frame zero; probes are not synthetic gameplay inputs.
    if(!probe->tick_title(0,0,0,false)||probe->in_title)return 0;
    for(auto& frame:rollback_frames){frame.number=Netplay::INVALID_FRAME;frame.captured=false;frame.sounds.clear();}
    Netplay::SessionConfig c;c.seed=seed;c.localPlayer=side;c.gameId=9;c.gameplayAbi=abi;c.sessionId=(std::uint64_t(idHigh)<<32)|idLow;
    rollback_published=rollback_catchup=adonis_snapshots=adonis_waits=0;
    if(!rollback.SetLocalChargePriority(local_charge_priority))return 0;probe->local_charge.clear();
    rollback_active=rollback.BeginMeasured(c,std::uint64_t(emscripten_get_now()),requestedDelay,adonis_mode,reserve);
    if(rollback_active)th09_network_replay_boundary(0,-1);
    return rollback_active;
}
TH09_EXPORT("th09_startup_info") const u32* th09_startup_info(){
    static u32 data[27];const auto& s=rollback.Startup();const auto c=s.Selected();
    data[0]=3;data[1]=u32(s.State());data[2]=s.Request();data[3]=s.Probes();data[4]=s.Replies();
    data[5]=s.Local().p95Us;data[6]=s.Peer().p95Us;data[7]=s.Local().lost;data[8]=s.Peer().lost;
    data[9]=c.fullDelay;data[10]=c.delay;data[11]=c.prediction;
    data[12]=rollback.Channel().AdonisStatistics().PredictionAllowanceUs();data[13]=u32(rollback.Mode());
    data[14]=s.Local().received;data[15]=s.Peer().received;
    data[16]=s.Local().minUs;data[17]=s.Local().maxUs;data[18]=s.Local().meanUs;
    data[19]=s.Peer().minUs;data[20]=s.Peer().maxUs;data[21]=s.Peer().meanUs;
    data[22]=Netplay::AdonisStartup::ProbeCount;data[23]=Netplay::AdonisStartup::Samples;
    data[24]=s.NextWakeUs();data[25]=s.Attempt();data[26]=u32(s.Reason());return data;
}
TH09_EXPORT("th09_rollback_info") const u32* th09_rollback_info(){
    static u32 data[8];const auto confirmed=rollback.ConfirmedThrough();
    data[0]=rollback.Frame();data[1]=confirmed==Netplay::INVALID_FRAME?rollback_published:std::max(rollback_published,confirmed+1);data[2]=rollback.Corrections();data[3]=rollback.Resimulated();data[4]=rollback.Captures();data[5]=rollback.Channel().RepairsSent();data[6]=rollback.Ready();data[7]=rollback_published;return data;
}
TH09_EXPORT("th09_spectator_begin") u32 th09_spectator_begin(u32 seed,u32 difficulty,u32 left,u32 right){
    if(!th09_network_room_begin(seed,0,0xffff,difficulty,0,left,right))return 0;
    spectator_playback={};spectator_last_ticks=spectator_max_ticks=spectator_yields=spectator_max_work_us=0;
    if(!probe->tick_title(0,0,0,false)||probe->in_title)return 0;network.end();spectator_frames.clear();spectator_next=spectator_simulated=0;spectator_mode=true;return 1;
}
TH09_EXPORT("th09_spectator_prepare") u32 th09_spectator_prepare(){
    if(!probe||!probe->in_title||network.active||rollback_active)return 0;
    spectator_playback={};spectator_frames.clear();spectator_next=spectator_simulated=0;
    spectator_mode=true;return 1;
}
TH09_EXPORT("th09_spectator_feed") u32 th09_spectator_feed(u32 frame,u32 left,u32 right,u32 leftMode,float leftX,float leftY,u32 rightMode,float rightX,float rightY){
    if(!spectator_mode||frame!=spectator_next||left>65535||right>65535||spectator_frames.size()>=multiplayer::SpectatorPlayback::Capacity)return 0;
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
TH09_EXPORT("th09_spectator_charge_buffer") u8* th09_spectator_charge_buffer(){return spectator_charge_buffer.data();}
TH09_EXPORT("th09_spectator_feed_charge") u32 th09_spectator_feed_charge(u32 frame,u32 left,u32 right,u32 leftMode,float leftX,float leftY,u32 rightMode,float rightX,float rightY){
    std::array<ChargeSample,2> samples;
    for(u32 side=0;side<2;++side)if(!charge_record::decode(spectator_charge_buffer.data()+40*side,samples[side]))return 0;
    if(!th09_spectator_feed(frame,left,right,leftMode,leftX,leftY,rightMode,rightX,rightY))return 0;
    auto& packet=spectator_frames.back();packet.localCharge=true;packet.charges=samples;return 1;
}
TH09_EXPORT("th09_spectator_control") u32 th09_spectator_control(u32 action,u32 frame){
    if(!spectator_mode||!spectator_playback.Control(action,frame,spectator_simulated,spectator_next))return 0;
    if(probe)probe->audio.suspend(spectator_playback.paused);
    return 1;
}
TH09_EXPORT("th09_spectator_finish") u32 th09_spectator_finish(u32 frame){
    if(!spectator_mode||frame!=spectator_next)return 0;spectator_playback.ended=true;return 1;
}
TH09_EXPORT("th09_spectator_begin_buffer") void* th09_spectator_begin_buffer(){return &spectator_begin_buffer;}
TH09_EXPORT("th09_confirmed_frame_hash") const u32* th09_confirmed_frame_hash(u32 frame){
    static u32 data[2];const auto& record=rollback_frames[frame%rollback_frames.size()];
    data[0]=record.number==frame?frame:Netplay::INVALID_FRAME;data[1]=record.number==frame?record.hash:0;return data;
}
TH09_EXPORT("th09_spectator_restart") u32 th09_spectator_restart(u32 frame,u32 size,u32 restore=0){
    if(!spectator_mode||!probe||restore>1||(size!=spectator_begin_bytes&&size!=spectator_begin_legacy_bytes)||
       (restore?size!=spectator_begin_bytes:frame<spectator_next))return 0;
    probe->presentation.renderer.flush();probe->state.random=spectator_begin_buffer.random;
    const auto& m=spectator_begin_buffer.match;const bool fresh=size==spectator_begin_bytes&&m.fresh;
    if(size==spectator_begin_bytes&&(m.fresh>1||m.difficulty>3||m.characters[0]>15||m.characters[1]>15||m.selector>17||m.extra_lives>9))return 0;
    // Version 2 is self-contained even for Retry: a late viewer has no prior
    // session to retry. The kind only controls the first tick's menu edges.
    if(size==spectator_begin_bytes){
        WorldConfiguration c;c.selection.mode=GameMode::versus;c.selection.difficulty=m.difficulty;c.selection.selector=i32(m.selector)-2;c.selection.unlocked=m.unlocked;c.selection.lives=2;c.controllers[0]=c.controllers[1]=0;c.starting_extra_lives=m.extra_lives;
        for(i32 s=0;s<2;++s){if(m.health[s]<1||m.health[s]>10||m.alternate[s]>1||m.focus[s]>1)return 0;c.selection.characters[s]=m.characters[s];c.health[s]=m.health[s];c.alternate[s]=m.alternate[s];c.automatic_focus[s]=m.focus[s];}
        probe->settings.versus=4;probe->configuration=c;probe->in_title=false;
        probe->music(-1);if(!probe->start())return 0;
    }else if(!probe->session||!probe->session->retry()){if(probe->session)probe->error=probe->session->error;return 0;}
    probe->world=probe->session->world.get();probe->presentation.world=probe->world;
    probe->device=spectator_begin_buffer.menu;probe->paused=probe->over=probe->complete=false;
    probe->pending_action=-1;probe->requested_transition=-1;probe->menus.pause={};probe->menus.game_over={};probe->menus.match_end={};
    spectator_frames.clear();spectator_next=spectator_simulated=frame;spectator_playback={};
    spectator_playback.roundStart=spectator_playback.target=frame;spectator_after_restart=!fresh;
    probe->audio.suspend(false);return 1;
}
TH09_EXPORT("th09_spectator_playback_info") const u32* th09_spectator_playback_info(){
    static u32 data[11];data[0]=1;data[1]=spectator_simulated;data[2]=spectator_next;
    data[3]=spectator_playback.Limit(spectator_next);data[4]=spectator_playback.target;
    data[5]=spectator_playback.paused;data[6]=spectator_playback.seeking;
    data[7]=spectator_playback.following;data[8]=spectator_playback.roundStart;data[9]=spectator_playback.round;
    data[10]=spectator_playback.ended;return data;
}
TH09_EXPORT("th09_spectator_info") const u32* th09_spectator_info(){
    static u32 data[8];data[0]=1;data[1]=spectator_next;data[2]=spectator_simulated;data[3]=u32(spectator_frames.size());
    data[4]=spectator_last_ticks;data[5]=spectator_max_ticks;data[6]=spectator_yields;data[7]=spectator_max_work_us;return data;
}
TH09_EXPORT("th09_spectator_end") void th09_spectator_end(){spectator_mode=false;spectator_frames.clear();if(probe)probe->clear_charge();clear_inputs();}
TH09_EXPORT("th09_network_receive") u32 th09_network_receive(u32 frame,u32 keys,u32 mode,float x,float y){return keys<=65535&&mode<=NetworkInput::MotionTargetUnlimited&&network.submit(1-network.side,frame,u16(keys),u8(mode),x,y);}
TH09_EXPORT("th09_network_end") void th09_network_end(){rollback_active=false;rollback.Clear();rollback_published=rollback_catchup=0;if(!network.active)return;if(probe)probe->release_network();else network.end();clear_inputs();if(probe){probe->requested_transition=-1;probe->settings.game_flags=0;if(probe->session&&!probe->in_title){probe->session->finish();probe->return_title(false);}else if(probe->title){probe->return_title(false);}probe->sync_records();}}
TH09_EXPORT("th09_network_hash") u32 th09_network_hash(){
    if(!probe)return 0;u32 hash=2166136261u;const auto word=[&](u32 value){for(u32 n=0;n<4;++n){hash^=u8(value>>(n*8));hash*=16777619u;}};const auto real=[&](float f){u32 value;std::memcpy(&value,&f,4);word(value);};
    word(probe->state.random.seed);word(probe->state.random.calls);word(probe->in_title);word(probe->paused);word(probe->over);word(probe->complete);
    if(probe->in_title&&probe->title){const auto& t=probe->title->state;word(i32(t.screen));word(t.state);word(t.frames);word(t.selection);for(u32 side=0;side<2;++side){word(t.character_selection[side]);word(t.confirmed[side]);word(t.health[side]);}}
    else if(probe->world){const auto& w=*probe->world;word(w.battle->state.flags);word(w.scene->phase);word(w.dialogue->id);word(w.rules.progress.round);for(u32 side=0;side<2;++side){const auto& p=*w.battle->fields[side].player;real(p.motion.position.x);real(p.motion.position.y);real(p.motion.health);real(p.control.charge);real(p.control.available);word(p.control.player_state);word(w.rules.scores[side].points);word(p.combo_state.best_hits);}}
    if(!probe->in_title&&probe->world&&(rollback.LocalChargePriority()||(probe->session&&probe->session->charge_replay.enabled)||
       (spectator_mode&&probe->charge_frames[0].enabled))){
        for(const auto& field:probe->world->battle->fields){const auto& pending=field.player->pending_charge;
            word(pending.count);word(pending.invalid);for(u32 i=0;i<pending.count;++i)real(pending.values[i]);}
    }
    return hash;
}
TH09_EXPORT("th09_network_info") const u32* th09_network_info(){static u32 data[6]{};if(probe){data[0]=0;for(u32 n=0;n<16;++n)if(probe->records.versus_unlocked[n])data[0]|=1u<<n;data[1]=std::min(3u,u32(probe->settings.difficulty));data[2]=probe->settings.auto_focus[0];data[3]=rollback_active?rollback.Frame():network.frame();data[4]=network.active;data[5]=network.side;}return data;}
TH09_EXPORT("th09_network_selection_info") const i32* th09_network_selection_info(){
    static i32 data[16]{};if(!probe)return data;data[0]=network_selection;data[1]=probe->in_title;
    if(probe->title){const auto& s=probe->title->state;data[2]=i32(s.screen);for(i32 side=0;side<2;++side){data[3+side]=s.character_selection[side];data[5+side]=s.confirmed[side];}}
    const auto& c=probe->configuration;data[7]=probe->in_title?probe->settings.difficulty:c.selection.difficulty;
    for(i32 s=0;s<2;++s){data[8+s]=probe->in_title?probe->settings.characters[s]:c.selection.characters[s];data[10+s]=probe->in_title?probe->settings.health[s]:c.health[s];data[12+s]=probe->in_title?probe->settings.alternate[s]:c.alternate[s];}
    data[14]=c.selection.selector;data[15]=probe->world?probe->world->configuration.selection.background:-1;return data;
}
TH09_EXPORT("th09_game_close") void th09_game_close(){recovery_schedule.Cancel();rollback_active=false;rollback.Clear();running=false;++loop_epoch;close_controllers();clear_inputs();network.end();spectator_mode=false;spectator_frames.clear();probe.reset();}
TH09_EXPORT("th09_music_enabled") void th09_music_enabled(u32 enabled){if(probe){probe->host_music_enabled=enabled!=0;probe->title_configuration();}}
TH09_EXPORT("th09_game_restart") u32 th09_game_restart(){if(!probe||!probe->in_title)return 0;clear_inputs();probe->requested_transition=-1;probe->settings.game_flags=0;return probe->return_title(false);}
// Output slots also own confirmed audio, Replay boundaries and spectator data.
// A later correction cannot make an earlier unpublished record expendable.
bool publish_rollback_outputs(){
    TH09_PERF_SCOPE(publish_cost,ConfirmedPublish);
    const auto through=rollback.ConfirmedThrough();
    while(through!=Netplay::INVALID_FRAME&&rollback_published<=through){
        auto& record=rollback_frames[rollback_published%rollback_frames.size()];
        if(record.number!=rollback_published){probe->error="Confirmed output history expired";return false;}
        TH09_PERF_COUNT(PublishedCommands,record.sounds.size());
        record.audio.Emit(record.sounds,true,[](const auto& s){TH09_PERF_COUNT(PublishedSfx,1);if(s.kind==0)probe->audio.effects.enqueue(s.id,i32(s.value));else probe->audio.effects.positioned(s.id,s.value);});
        for(const auto& s:record.sounds)switch(s.kind){case 2:TH09_PERF_COUNT(PublishedMusic,1);probe->music(s.id);break;case 3:TH09_PERF_COUNT(PublishedOtherAudio,1);probe->audio.fade_music();break;case 4:TH09_PERF_COUNT(PublishedOtherAudio,1);probe->audio.pause_music(s.id!=0);break;case 5:probe->encountered(s.id);break;case 6:probe->defeated(s.id);break;}
        record.sounds.clear();
        // Both players archive only reconciled full-match boundaries.
        if(record.hasBegin)th09_network_replay_boundary(record.beginFrame,-1);
        if(record.hasEnd)th09_network_replay_boundary(rollback_published+1,record.winner);
        if(network.side==0){std::array<Netplay::FrameInput,Netplay::MAX_PLAYERS> in;if(!rollback.ConfirmedInputs(rollback_published,&in))return false;const auto mode=[](const Netplay::FrameInput& i){return i.analogMode==Netplay::AnalogMode::None?0:i.analogMode==Netplay::AnalogMode::Joystick?1:i.unlimited?3:2;};
            if(record.hasBegin)th09_network_spectator_begin(record.beginFrame,&record.begin,spectator_begin_bytes);
            std::array<u8,80> charges{};
            if(rollback.LocalChargePriority())for(u32 side=0;side<2;++side){
                const auto* sample=rollback.ChargeInput(side,rollback_published);if(!sample)return false;charge_record::encode(*sample,charges.data()+40*side);
            }
            if(!network_selection||record.gameplay)th09_network_spectator_frame(rollback_published,in[0].buttons,in[1].buttons,mode(in[0]),in[0].x,in[0].y,mode(in[1]),in[1].x,in[1].y,charges.data(),rollback.LocalChargePriority()?80:0);}
        if(record.round>0)th09_network_spectator_round(rollback_published+1,u32(record.round),record.replayRoundFrame);
        if(network.side==0&&record.hasEnd)th09_network_spectator_end(rollback_published+1,record.winner);
        if(network.active&&(rollback_published+1)%120==0&&!rollback.Verify(rollback_published+1,record.hash)){probe->error=rollback.Error();return false;}
        ++rollback_published;
        TH09_PERF_COUNT(PublishedFrames,1);
    }
    return true;
}
u32 rollback_tick(u32 render,bool recoveryOnly=false,bool forceRecovery=false){
    TH09_PERF_SCOPE(rollback_cost,RollbackTick);
    // Recovery and transport work consume the same callback budget as replay.
    const double start=emscripten_get_now();
    bool deferOrdinaryInput=true;
#if TH09_DEVELOPMENT_HARNESS
    deferOrdinaryInput=rollback.Frame()<rollback_test_limit;
#endif
    bool forwardOnly=!recoveryOnly&&!forceRecovery&&deferred_recovery_active();
    const auto pumped=pump_rollback_transport(deferOrdinaryInput&&!recoveryOnly);
    const auto flush=[](){
        if(!rollback.FlushDeferredInputs(std::uint64_t(emscripten_get_now()))){probe->error=rollback.Error();return false;}
        return true;
    };
    if(!pumped){flush();return 0;}
    if(!rollback.Ready()){if(!flush())return 0;return 2;}
    const auto capture=[](){
#if TH09_DEVELOPMENT_HARNESS
        if(rollback.Frame()>=rollback_test_limit)return true;
#endif
        if(!rollback.NeedsCapture())return true;
        TH09_PERF_SCOPE(input_cost,InputCapture);
        u16 keys[3]{};sample_keys(keys);Netplay::FrameInput in(keys[2]);
        if(probe->session){const auto& m=probe->session->motion_input[network.side];if(m.enabled){in.analogMode=m.target?Netplay::AnalogMode::DirectTouch:Netplay::AnalogMode::Joystick;in.x=m.x;in.y=m.y;in.unlimited=m.unlimited;in.touchUsed=true;}}
        if(!rollback.Capture(in,std::uint64_t(emscripten_get_now()))){probe->error=rollback.Error();return false;}
        return true;
    };
    // Sample the visible frontier once, before recovery can span callbacks.
    // The peer receives this input while we reconcile; historical ticks reuse
    // retained packets and never consume physical input or gesture edges.
    // Complete the old send obligation even when capture is rejected. All
    // deferral ends here, before Restore, Prepare, or a callback budget exit.
    const bool captured=recoveryOnly||capture();
    const bool flushed=flush();
    if(!captured||!flushed)return 0;
    if(!publish_rollback_outputs())return 0;
    auto correction=rollback.RollbackFrame();
    // Higan also catches up at the input point when its horizon/boundary is
    // reached. Never draw a partially restored world or overrun our history.
    // Delayed recovery must preserve the oldest unpublished OUTPUT too, even
    // when the first prediction mismatch is several frames later.
    if(forwardOnly){
        const auto next=rollback.Prepare(true);
        // Skill cut-ins and field time-stop remain reversible world ticks.
        // Only the history horizon and real resource boundaries require
        // reconciliation before submitting the next picture.
        if(rollback.Frame()-rollback_published>=rollback_frames.size()||!next.canAdvance)forwardOnly=false;
    }
    if(correction!=Netplay::INVALID_FRAME&&!forwardOnly){
        TH09_PERF_SCOPE(restore_cost,Restore);
        rollback_catchup=std::max(rollback_catchup,rollback.Frame());
        probe->presentation.renderer.flush();probe->graphics.backend.flush();
        for(u32 f=rollback.Frame();f-->correction;){auto& saved=rollback_frames[f%rollback_frames.size()];if(saved.number!=f||!saved.captured||!saved.state||!saved.state->Restore()){probe->error="Rollback world restore failed";return 0;}TH09_PERF_COUNT(RestoredCheckpoints,1);saved.sounds.clear();}
        if(!rollback.Restored(correction)){probe->error=rollback.Error();return 0;}
    }
    // One callback starts at most eight recovery ticks. Its caller also has a
    // wall-time budget; physical input is never sampled on these replay ticks.
    for(unsigned count=0;count<9;++count){
        if(recoveryOnly&&rollback.Frame()>=rollback_catchup)break;
#if TH09_DEVELOPMENT_HARNESS
        if(rollback.Frame()>=rollback_test_limit)break;
#endif
        if(!recoveryOnly&&!capture())return 0;
        auto decision=rollback.Prepare(forwardOnly);if(!decision.canAdvance){++adonis_waits;return 2;}
        const u32 f=rollback.Frame();const auto confirmed=rollback.ConfirmedThrough();
        const bool reconciled=rollback.InputsReconciledBeforeNext();
        // Menu animation and selection belong to the existing snapshot. Wait
        // only at a step that can replace resources, not on every menu frame.
        // Use the current world's flags/continues, before tick_inputs refreshes
        // its cached InGameMenuServices fields.
        const u32 flags=probe->world?probe->world->battle->state.flags:0;
        // Menu navigation/animation is reversible. Only world construction,
        // transport departure and initial resource setup require exact input.
        bool titleRetires=false;
        if(probe->in_title){
            const auto& t=probe->title->state;
            auto menu=probe->device;
            menu.update(u16(decision.inputs[0].buttons|decision.inputs[1].buttons));
            titleRetires=t.load_frame==0||
                (t.screen!=TitleScreen::versus_difficulty&&t.screen!=TitleScreen::versus_character&&t.screen!=TitleScreen::versus_stage)||
                (t.screen==TitleScreen::versus_stage&&(menu.pressed&0x1001))||
                (t.screen==TitleScreen::versus_difficulty&&(menu.pressed&10));
        }
        const bool retire=titleRetires||probe->pending_action>=0||
            (probe->paused?probe->menus.pause_retires_world():
             probe->over?probe->menus.game_over_retires_world(flags,probe->session->continues):
             probe->complete&&probe->menus.match_end_retires_world(flags));
        if(retire&&(decision.predictedMask||!reconciled)){
            if(forwardOnly&&rollback.RollbackFrame()!=Netplay::INVALID_FRAME)return rollback_tick(render,false,true);
            return 2;
        }
        auto& record=rollback_frames[f%rollback_frames.size()];
        if(record.number!=Netplay::INVALID_FRAME&&record.number!=f&&record.number>=rollback_published){
            probe->error="Unpublished rollback output would be overwritten";return 0;
        }
        if(record.number!=f)record.audio.Clear();record.number=f;record.sounds.clear();
        const bool speculative=decision.predictedMask||!reconciled||
            (!retire&&rollback.LocalChargePriority()&&!rollback.ChargeInput(1-network.side,f));
        if(rollback.Mode()==Netplay::AdonisMode::Delay&&speculative){probe->error="Lockstep attempted speculative state";return 0;}
        if(speculative)++adonis_snapshots;
        record.captured=speculative;
        if(speculative){if(!record.state)record.state=std::make_unique<multiplayer::WorldState>();if(!probe->checkpoint(*record.state)){probe->error="Rollback world capture failed";return 0;}probe->graphics.checkpoint=record.state.get();}
        // Retain arena capacity across exact and speculative frames.
        record.hasBegin=record.hasEnd=false;record.round=-1;record.beginFrame=f;record.gameplay=!probe->in_title;recording_frame=&record;
        if(probe->session&&!probe->in_title)for(unsigned side=0;side<2;++side){const auto& in=decision.inputs[side];probe->session->motion_input[side]={in.analogMode!=Netplay::AnalogMode::None,in.x,in.y,in.unlimited,in.analogMode==Netplay::AnalogMode::DirectTouch};}
        // Recovery does not consume the wall-clock tick's forward step. Replay
        // through the old frontier, then capture/simulate its new input once.
        const bool final=f>=rollback_catchup;
        // Repaint the completed old frontier on its existing replay tick.
        // This also runs from post-Present recovery while the next tick waits
        // for input. Intermediate restored frames never reach the screen.
        const bool refresh=(!final&&f+1==rollback_catchup&&!probe->in_title&&(render||recoveryOnly));
        const bool paint=(render&&final)||refresh;
        TH09_PERF_COUNT(ForwardTicks,final?1:0);TH09_PERF_COUNT(ReplayTicks,final?0:1);TH09_PERF_COUNT(SpeculativeTicks,speculative?1:0);
        const bool wasComplete=probe->complete;
        const auto priorRoundFrames=probe->world?probe->world->rules.progress.round_frames:0;
        const bool deferPresent=refresh||(render&&final&&speculative&&!probe->in_title);
        probe->charge_frames={};
        // Resource replacement requires exact keys and a reconciled past.
        // This shared boundary uses native charge rules once; accepted charge
        // records resume on the new world's first reversible input frame.
        if(retire)probe->local_charge.clear();
        if(!retire&&rollback.LocalChargePriority())for(u32 side=0;side<2;++side){
            auto& step=probe->charge_frames[side];step.enabled=true;
            if(const auto* sample=rollback.ChargeInput(side,f)){step.supplied=true;step.input=*sample;}
            else if(side==u32(network.side))step.owner=&probe->local_charge;
        }
        if(probe->session){probe->session->charge_frames=probe->charge_frames;probe->session->charge_input_recorded=false;}
        const auto ok=probe->tick_inputs(decision.inputs[0].buttons,decision.inputs[1].buttons,decision.inputs[0].buttons|decision.inputs[1].buttons,paint,false,!deferPresent);
        std::array<ChargeSample,2> samples;
        if(rollback.LocalChargePriority()){
            if(probe->world&&!probe->in_title)for(auto& field:probe->world->battle->fields)field.player->finish_charge_frame();
            for(u32 side=0;side<2;++side){
                const auto& step=probe->session?probe->session->charge_frames[side]:probe->charge_frames[side];
                samples[side]=step.used?step.output:ChargeSample{};
                if(step.used&&!step.supplied&&probe->world&&!probe->in_title){
                    const auto& control=probe->world->battle->fields[side].player->control;
                    samples[side].charge=control.charge;samples[side].charging_time=control.charging_time;samples[side].full_charge=control.full_charge;
                    if(side==u32(network.side))probe->local_charge.accept(control);
                }
            }
            if(probe->session&&!probe->in_title&&!probe->session->record_charge_frame(samples)){probe->error=probe->session->error;return 0;}
            // Accept the owner's result once; recovery reuses this record.
            if(!rollback.CaptureLocalCharge(f,samples[network.side])){probe->error=rollback.Error();return 0;}
        }
        if(!record.hasBegin&&probe->world&&probe->world->rules.progress.round_frames<priorRoundFrames&&probe->world->rules.progress.round>0){
            record.round=probe->world->rules.progress.round;record.replayRoundFrame=probe->session->recording.frame();}
        record.hasEnd=!wasComplete&&probe->complete;
        if(record.hasEnd)record.winner=probe->world->scene->winner;
        recording_frame=nullptr;probe->graphics.checkpoint=nullptr;
        if(record.captured){if(!record.state->Seal()){probe->error="Rollback checkpoint seal failed";return 0;}TH09_PERF_COUNT(SealedCheckpoints,1);}
        if(!ok)return 0;
        if(!rollback.CommitChargeFrame(f,samples)){probe->error=rollback.Error();return 0;}
        if(!rollback.Complete(decision,forwardOnly)){probe->error=rollback.Error();return 0;}
        if(deferPresent)probe->graphics.present();
        // Historical replay stays silent. Fresh forward feedback need not
        // wait for remote confirmation; its later publication is deduplicated.
        if(final)record.audio.Emit(record.sounds,false,[](const auto& s){if(s.kind==0)probe->audio.effects.enqueue(s.id,i32(s.value));else probe->audio.effects.positioned(s.id,s.value);});
        if(!network.active)rollback.FinishExactBoundary();
        record.hash=th09_network_hash();
        // Catch-up may reach its old frontier and continue into a fresh tick
        // in this same call. Release reconciled output before slot reuse.
        if(!publish_rollback_outputs())return 0;
        if(final||!network.active)break;
        if(emscripten_get_now()-start>=8)break;
    }
    if(!publish_rollback_outputs())return 0;
    probe->audio.update();probe->audio.pump();
    return (recoveryOnly?rollback.Frame()<rollback_catchup:rollback.Frame()<=rollback_catchup)?2:1;
}
void queue_rollback_recovery(){
    if(!deferred_recovery_active())return;
    const auto ticket=recovery_schedule.Schedule(loop_epoch);if(!ticket)return;
    // A timer returns control to the browser after this RAF's GL submission.
    // This is an opportunity to present, not a physical-display acknowledgement.
    emscripten_async_call([](void* token){
        const auto ticket=u32(uintptr_t(token));
        if(!recovery_schedule.Take(ticket,loop_epoch)||!deferred_recovery_active())return;
        const double begin=emscripten_get_now();const auto before=rollback.Resimulated();
        const auto ok=rollback_tick(0,true);
        ++recovery_callbacks;recovery_ticks+=rollback.Resimulated()-before;
        recovery_last_us=u32((emscripten_get_now()-begin)*1000);
        if(!ok){running=false;probe->audio.suspend(true);th09_browser_frame(0,emscripten_get_now()-begin);}
    },reinterpret_cast<void*>(uintptr_t(ticket)),0);
}
// A packet may unblock a tick whose clock credit was already spent in rAF.
// Finish only that obligation here; packet rate cannot create fresh ticks.
TH09_EXPORT("th09_rollback_wake") void th09_rollback_wake(){
    if(!running||suspended||!probe||spectator_mode||!rollback_active||!network.active||
       rollback.Mode()!=Netplay::AdonisMode::Rollback||!cadence.retry_pending())return;
    if(rollback.RollbackFrame()==Netplay::INVALID_FRAME&&!rollback.Prepare().canAdvance)return;
    const double begin=emscripten_get_now();const auto ok=rollback_tick(1);
    if(ok==2)cadence.blocked(true);
    else{
        cadence.complete();
        // The stalled interval earned no catch-up debt. Resume from the wake,
        // keeping only the fractional clock credit retained by blocked().
        previous_frame=emscripten_get_now();
    }
    probe->audio.pump();if(ok)queue_rollback_recovery();
    if(!ok){running=false;probe->audio.suspend(true);}
    th09_browser_frame(ok,emscripten_get_now()-begin);
}
u32 spectator_tick(u32 render,u32 dueTicks=1){
        spectator_last_ticks=0;
        if(spectator_frames.empty())return 2;
        const bool fast=spectator_playback.Fast(spectator_simulated,spectator_next);
        const auto available=spectator_playback.Available(spectator_simulated,spectator_next);
        const auto count=fast?available:std::min(available,std::min(multiplayer::SpectatorSchedule::MaxTicks,std::min(dueTicks,4u)));
        if(!count)return 2;
        const double began=emscripten_get_now();u32 ok=1;
        for(u32 n=0;n<count&&ok&&!spectator_frames.empty();++n){
            if(!multiplayer::SpectatorSchedule::CanStart(n,emscripten_get_now()-began,fast)){++spectator_yields;break;}
            const auto packet=spectator_frames.front();spectator_frames.pop_front();
            if(packet.frame!=spectator_simulated)return 0;
            const bool was_title=probe->in_title;
            const auto priorRoundFrames=probe->world?probe->world->rules.progress.round_frames:0;
            const auto priorSessionFrames=probe->session?probe->session->frames:0;
            if(!was_title&&probe->session)for(i32 s=0;s<2;++s)probe->session->motion_input[s]={packet.motion[s].enabled,packet.motion[s].x,packet.motion[s].y,packet.motion[s].unlimited,packet.motion[s].target};
            const bool prior=rollback_active;rollback_active=true;
            probe->charge_frames={};
            if(packet.localCharge)for(u32 side=0;side<2;++side){auto& step=probe->charge_frames[side];step.enabled=step.supplied=true;step.input=packet.charges[side];}
            if(probe->session)probe->session->charge_frames=probe->charge_frames;
            ok=probe->tick_inputs(packet.keys[0],packet.keys[1],u16(packet.keys[0]|packet.keys[1]),render!=0&&multiplayer::SpectatorSchedule::Render(n,count),spectator_after_restart);
            if(packet.localCharge&&probe->world&&!probe->in_title)for(auto& field:probe->world->battle->fields)field.player->finish_charge_frame();
            spectator_after_restart=false;
            rollback_active=prior;++spectator_simulated;++spectator_last_ticks;
            if(probe->world&&probe->session&&(probe->world->rules.progress.round_frames<priorRoundFrames||probe->session->frames<priorSessionFrames))
                spectator_playback.Boundary(spectator_simulated,u32(probe->world->rules.progress.round));
            if(!was_title&&probe->in_title){spectator_mode=false;spectator_frames.clear();break;}
        }
        spectator_max_ticks=std::max(spectator_max_ticks,spectator_last_ticks);
        spectator_max_work_us=std::max(spectator_max_work_us,u32((emscripten_get_now()-began)*1000));
        return ok;
}
TH09_EXPORT("th09_game_tick") u32 th09_game_tick(u32 render){TH09_PERF_SCOPE(game_cost,GameTick);if(!probe)return 0;u16 keys[3]{};
    if(room_preview){
        if(!probe->in_title||!probe->title)return 0;
        probe->title->state.idle_frames=0;
        const u32 ok=probe->tick_inputs(0,0,0,false);if(ok&&render)probe->draw();return ok;
    }
    if(spectator_mode)return spectator_tick(render);
    if(rollback_active&&network.active)return rollback_tick(render);
    if(network.active){if(network.wants_input()){sample_keys(keys);const u32 frame=network.sending_frame();const auto m=probe->session&&!probe->in_title?probe->session->motion_input[network.side]:GameSession::MotionSample{};const u8 mode=!m.enabled?NetworkInput::MotionNone:m.target?(m.unlimited?NetworkInput::MotionTargetUnlimited:NetworkInput::MotionTarget):NetworkInput::MotionVelocity;if(!network.submit(network.side,frame,keys[2],mode,m.x,m.y))return 0;th09_network_send(frame,keys[2],i32(mode),m.x,m.y);}if(network.failed){probe->error="Network input order invalid";return 0;}NetworkInput::Motion motion[2];const u32 frame=network.frame();const bool publisher=network.side==0;if(!network.take(keys,motion))return 2;if(probe->session)for(i32 s=0;s<2;++s)probe->session->motion_input[s]={motion[s].enabled,motion[s].x,motion[s].y,motion[s].unlimited,motion[s].target};
        const bool wasComplete=probe->complete;
        const auto priorRoundFrames=probe->world?probe->world->rules.progress.round_frames:0;
        const u32 ok=probe->tick_inputs(keys[0],keys[1],keys[2],render!=0);
        if(ok&&publisher){const auto mode=[](const NetworkInput::Motion& m){return !m.enabled?0:m.target?(m.unlimited?3:2):1;};
            th09_network_spectator_frame(frame,keys[0],keys[1],mode(motion[0]),motion[0].x,motion[0].y,mode(motion[1]),motion[1].x,motion[1].y,nullptr,0);}
        if(ok&&probe->world&&probe->world->rules.progress.round_frames<priorRoundFrames&&probe->world->rules.progress.round>0)
            th09_network_spectator_round(frame+1,u32(probe->world->rules.progress.round),probe->session->recording.frame());
        if(ok&&publisher&&!wasComplete&&probe->complete)th09_network_spectator_end(frame+1,probe->world->scene->winner);
        return ok;}
    sample_keys(keys);u32 ok=1;for(u32 extra=0;extra<3&&ok;++extra){ok=probe->tick_inputs(keys[0],keys[1],keys[2],false);if(!ok||probe->in_title||probe->paused||probe->over||probe->complete||!probe->session->is_replay||probe->session->phase!=SessionPhase::match||probe->session->playback_schedule()!=6)break;}if(ok&&render)probe->draw();return ok;
}
TH09_EXPORT("th09_loop_pause") void th09_loop_pause(u32 on){recovery_schedule.Cancel();suspended=on!=0;previous_frame=-1;cadence.reset();clear_inputs();if(probe){probe->clock_pause(suspended);probe->audio.suspend(suspended);}}
TH09_EXPORT("th09_loop_stop") void th09_loop_stop(){recovery_schedule.Cancel();running=false;++loop_epoch;if(probe){probe->clock_pause(true);probe->audio.suspend(true);}}
TH09_EXPORT("th09_loop_start") void th09_loop_start(){
    if(!probe||running)return;running=true;suspended=false;previous_frame=-1;cadence.reset();probe->clock_pause(false);probe->audio.suspend(false);
    emscripten_request_animation_frame_loop([](double time,void* epoch)->EM_BOOL{
        if(!running||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;
        TH09_PERF_SCOPE(callback_cost,FrameCallback);
        const double begin=emscripten_get_now(),delta=previous_frame<0?0:(time-previous_frame)/1000.;previous_frame=time;
        if(suspended||!probe){cadence.reset();return EM_TRUE;}
        const bool active=rollback_active&&network.active;
        // A pending due tick ignores new wall-time credit. Keep phase advice
        // queued until the scheduler can actually apply it to a future tick;
        // consuming it on a retry would silently discard the correction.
        const auto elapsed=active&&!cadence.retry_pending()?rollback.PacedElapsedMs(delta*1000)/1000/rollback.IntervalScale():delta;
        const auto ticks=cadence.advance(elapsed,active);u32 ok=1;
        if(spectator_mode&&ticks){
            // Flatten the old four outer ticks x four inner steps. One shared
            // 6-step/8ms start budget owns this callback; pending inputs remain.
            ok=spectator_tick(1,ticks);
            if(ok==2)cadence.blocked(false);else cadence.complete();
        }else for(u32 n=0;n<ticks&&ok;++n){
            // A netplay tick can exhaust the callback budget by itself. Draw
            // its current result before that exit instead of waiting for a
            // later due tick that may never run. Historical resimulation is
            // still filtered by rollback_tick's catch-up frontier.
            ok=th09_game_tick(rollback_active||n+1==ticks);if(ok==2){cadence.blocked(rollback_active&&network.active);break;}
            cadence.complete();
            if(rollback_active&&emscripten_get_now()-begin>=10){
                // Drop overdue work only when due ticks were left unfinished.
                // A costly but completed callback must keep its fractional
                // clock remainder, otherwise recovery itself slows the game.
                cadence.exhausted(n+1,ticks);break;
            }
        }
        probe->audio.pump();if(ok)queue_rollback_recovery();TH09_PERF_STOP(callback_cost);th09_browser_frame(ok,emscripten_get_now()-begin);if(!ok){running=false;probe->audio.suspend(true);}return running?EM_TRUE:EM_FALSE;
    },reinterpret_cast<void*>(uintptr_t(++loop_epoch)));
}
TH09_EXPORT("th09_game_draw") void th09_game_draw(){if(probe)probe->draw();}
TH09_EXPORT("th09_key") void th09_key(u32 scan,u32 down){if(spectator_mode)return;for(auto& k:keyboard_map)if(k.scan==scan){k.hosted=down!=0;keyboard_inputs.event(k.vk,k.hosted);}}
// Local versus has two independent frontend owners. Single-player and LAN
// retain their ordinary full-keyboard path; native local settings remain the
// fallback until the Launcher explicitly enables its local two-player profile.
TH09_EXPORT("th09_keyboard_profile") u32 th09_keyboard_profile(){
    return local_versus_input_context()?5:2;
}
TH09_EXPORT("th09_keys_clear") void th09_keys_clear(){clear_inputs();}
TH09_EXPORT("th09_pulse") void th09_pulse(u32 mask){for(u32 n=0;n<16;++n)if(mask&(1u<<n))pulse_ticks[n]=2;}
TH09_EXPORT("th09_touch") void th09_touch(u32 type,i32 id,float x,float y){gestures.pointer(type,id,x,y,SDL_GetTicks(),touch_state(),false);}
TH09_EXPORT("th09_touch_cancel") void th09_touch_cancel(){gestures.cancel();}
TH09_EXPORT("th09_always_hitbox") void th09_always_hitbox(u32 on){th09::presentation::always_hitbox=on!=0;}
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
TH09_EXPORT("th09_probe_stop_boundary_waits") const u32* th09_probe_stop_boundary_waits(){return probe_stop_boundary_waits.data();}
TH09_EXPORT("th09_probe_stop_draw") const u32* th09_probe_stop_draw(){
    static u32 words[9]{};if(probe&&probe->world&&!probe->in_title){words[8]=probe->stop_state();
        for(u32 side=0;side<2;++side){const auto& p=*probe->world->battle->fields[side].player;
            std::memcpy(words+4*side,&p.motion.position.x,4);std::memcpy(words+4*side+1,&p.motion.position.y,4);
            std::memcpy(words+4*side+2,&p.body.pos.x,4);std::memcpy(words+4*side+3,&p.body.pos.y,4);}}
    return words;
}
TH09_EXPORT("th09_probe_charge_reference_buffer") u8* th09_probe_charge_reference_buffer(){return probe_charge_reference_bytes.data();}
TH09_EXPORT("th09_probe_charge_reference_mode") u32 th09_probe_charge_reference_mode(u32 enabled){
    if(enabled>1)return 0;probe_charge_reference=enabled!=0;
    if(!enabled&&probe){probe->charge_frames={};if(probe->session)probe->session->charge_frames={};}return 1;
}
TH09_EXPORT("th09_probe_charge_visual") const u32* th09_probe_charge_visual(){
    static u32 words[8]{};if(probe&&probe->world)for(u32 side=0;side<2;++side){
        const auto& player=*probe->world->battle->fields[side].player;auto* out=words+4*side;
        std::memcpy(out,&player.control.charge,4);std::memcpy(out+1,&probe->world->huds[side]->animations[17].scale.x,4);
        std::memcpy(out+2,&player.control.available,4);out[3]=player.pending_charge.count;
    }return words;
}
TH09_EXPORT("th09_probe_charge_recording") const u32* th09_probe_charge_recording(){
    static u32 words[5]{};if(probe&&probe->session){const auto& session=*probe->session;
        words[0]=session.charge_replay.enabled;words[1]=session.recording.frame();words[2]=probe->world?u32(probe->world->rules.progress.stage):0;words[3]=session.is_replay;
        words[4]=0;for(const auto& entries:session.charge_replay.stages)words[4]+=u32(entries.size());
    }return words;
}
TH09_EXPORT("th09_probe_charge_bridges") u32 th09_probe_charge_bridges(){
    u32 bits=0;if(probe)for(u32 side=0;side<2;++side){if(probe->charge_frames[side].enabled)bits|=1u<<side;
        if(probe->session&&probe->session->charge_frames[side].enabled)bits|=4u<<side;}return bits;
}
TH09_EXPORT("th09_probe_replay") u32 th09_probe_replay(u32 size,u32 stage){if(!probe||size>probe->import_bytes.size())return 0;ReplayFile f;if(!f.decode(probe->import_bytes.data(),size))return 0;probe->title_play_replay(f,stage,"");probe->launch_pending=false;probe->in_title=false;return probe->start();}
TH09_EXPORT("th09_probe_return_title") u32 th09_probe_return_title(u32 score){if(!probe)return 0;if(probe->session)probe->session->finish();return probe->return_title(score!=0);}
TH09_EXPORT("th09_probe_menu_hash_isolation") u32 th09_probe_menu_hash_isolation(){
    if(!probe||!probe->in_title||!probe->world)return 0;
    auto& pending=probe->world->battle->fields[0].player->pending_charge;
    const auto saved=pending;const auto savedMode=spectator_mode;const auto savedEnabled=probe->charge_frames[0].enabled;
    spectator_mode=true;probe->charge_frames[0].enabled=true;
    const auto before=th09_network_hash();pending.count=1;pending.invalid^=1;pending.values[0]=123.5f;
    const auto after=th09_network_hash();pending=saved;spectator_mode=savedMode;probe->charge_frames[0].enabled=savedEnabled;
    return before==after;
}
TH09_EXPORT("th09_probe_open") u32 th09_probe_open(i32 a,i32 b,i32 mode,i32 difficulty){TH09_PERF_SCOPE(open_cost,GameOpen);probe=std::make_unique<Application>();if(!probe->open(a,b,mode,difficulty)){failure=probe->error;return 0;}return 1;}
#endif
#if TH09_DEVELOPMENT_HARNESS
TH09_EXPORT("th09_probe_frame_limit") void th09_probe_frame_limit(u32 frame){rollback_test_limit=frame;}
TH09_EXPORT("th09_probe_tick_cost") const double* th09_probe_tick_cost(){return probe_tick_cost;}
TH09_EXPORT("th09_probe_performance_enable") void th09_probe_performance_enable(u32 on){performance::enabled=on!=0;}
TH09_EXPORT("th09_probe_performance_reset") void th09_probe_performance_reset(){performance::reset();}
TH09_EXPORT("th09_probe_performance") const double* th09_probe_performance(){return performance::snapshot();}
TH09_EXPORT("th09_probe_performance_schema") const char* th09_probe_performance_schema(){return performance::schema;}
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
    for(i32 continues:{0,3})for(u16 keys:{u16(0),u16(1),u16(8),u16(16),u16(32),u16(0x200),u16(0x1001),u16(0x208),u16(0x4000),u16(0x4208)}){
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
    // Exercise the real Draw first-write barrier across VM identities 63/64,
    // another page, and the final sentinel VM. Reuse ONE physical record both
    // after Restore and after confirmation-like retirement without Restore.
    struct DrawBarrier final:BulletDrawServices {
        u32 calls=0;
        void begin_field()override{}
        void upper_effects()override{}
        void draw_animation(AnmVm& vm)override{
            ++calls;vm.matrix3.m[3][3]=float(91+calls);vm.reserved29a[9]^=0xa5;
        }
    };
    multiplayer::WorldState pages;
    constexpr std::size_t pageBytes=64*sizeof(AnmVm);
    for(u32 pass=0;pass<3;++pass){
        auto& field=battle.fields[0];auto& pool=field.bullets->pool;auto& visual=*field.bullet_visuals;
        field.bullets->reset_pool();field.bullets->draw_heads.fill(-1);
        for(auto& laser:field.lasers->pool)laser.active=false;
        const bool tail=pass!=1;const u32 tailSlot=pass==2?BulletManager::update_count:25;
        if(!visual.prepare(pool[12],12,0,0,8))return 0;
        pool[12].state=4;pool[12].position={float(13+pass),17,0};pool[12].draw_next=tail?i32(tailSlot):-1;
        if(tail){
            if(!visual.prepare(pool[tailSlot],tailSlot,0,0,pass==2?0:8))return 0;
            pool[tailSlot].state=pass==2?5:4;pool[tailSlot].position={19,float(23+pass),0};pool[tailSlot].draw_next=-1;
        }
        field.bullets->draw_heads[0]=12;
        // Distinct bytes on EVERY reuse expose stale bitmap entries and stale
        // page contents even in VMs that this pass never writes.
        for(auto& f:battle.fields)for(auto& slot:f.bullet_visuals->instances)
            for(auto& vm:slot)vm.reserved29a[0]=u8(0x31+pass);
        const auto baseline=hash();
        for(unsigned side=0;side<2;++side){
            auto& b=battle.fields[side].bullets->pool;std::memcpy(bytes[side].data(),b.data(),bytes[side].size());
            auto& v=battle.fields[side].bullet_visuals->instances;std::memcpy(visualBytes[side].data(),v.data(),visualBytes[side].size());
        }
        if(!pages.Save(session))return 0;
        const auto beforeBytes=pages.FirstWriteBytes(),beforeBlocks=pages.FirstWriteBlocks(),beforeTotal=pages.Bytes();
        DrawBarrier draw;draw.geometry=battle.state.geometry[0];
        BulletDraw::draw(*field.bullets,visual,*field.lasers,draw);
        pool[12].state=5; // Already captured as a live Bullet by Save.
        BulletDraw::draw(*field.bullets,visual,*field.lasers,draw);
        const std::size_t drawn=2+std::size_t(tail);
        good=draw.calls==2*(1+u32(tail))&&good;
        good=pages.FirstWriteBlocks()==beforeBlocks+drawn&&good;
        good=pages.FirstWriteBytes()==beforeBytes+drawn*sizeof(AnmVm)&&good;
        // The full-slot write must keep the two Draw-captured VM originals.
        if(!visual.change_type(pool[12],12,1,0)||!visual.change_type(pool[12],12,1,0))return 0;
        const std::size_t captured=drawn+3;
        good=pages.FirstWriteBlocks()==beforeBlocks+captured&&good;
        good=pages.FirstWriteBytes()==beforeBytes+captured*sizeof(AnmVm)&&good;
        good=pages.Bytes()==beforeTotal+captured*sizeof(AnmVm)&&good;
        good=pages.VisualPageCapacityBytes()==(pass==2?4:3)*pageBytes&&good;
        if(!pages.Seal())return 0;
        if(pass==1)continue; // The next Save must retire/reset this record itself.
        if(!pages.Restore())return 0;
        good=pages.FirstWriteBytes()==0&&pages.FirstWriteBlocks()==0&&good;
        good=pages.VisualPageCapacityBytes()==(pass==2?4:3)*pageBytes&&good;
        for(unsigned side=0;side<2;++side){
            good=!std::memcmp(bytes[side].data(),battle.fields[side].bullets->pool.data(),bytes[side].size())&&good;
            good=!std::memcmp(visualBytes[side].data(),battle.fields[side].bullet_visuals->instances.data(),visualBytes[side].size())&&good;
        }
        good=hash()==baseline&&good;
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
    std::array<ChargeSample,2> samples;
    if(probe_charge_reference){
        probe->charge_frames={};
        for(u32 side=0;side<2;++side){
            if(!charge_record::decode(probe_charge_reference_bytes.data()+40*side,samples[side]))return 0;
            auto& step=probe->charge_frames[side];step.enabled=step.supplied=true;step.input=samples[side];
        }
        if(probe->session){probe->session->charge_frames=probe->charge_frames;probe->session->charge_input_recorded=false;}
    }
    const bool active=rollback_active;rollback_active=true;RollbackFrame output;recording_frame=&output;
    const bool ok=probe->tick_inputs(u16(left),u16(right),u16(left|right),render!=0);
    if(probe_charge_reference){
        if(probe->world&&!probe->in_title)for(auto& field:probe->world->battle->fields)field.player->finish_charge_frame();
        if(probe->session&&!probe->session->record_charge_frame(samples))return 0;
    }
    recording_frame=nullptr;rollback_active=active;return ok;
}
TH09_EXPORT("th09_probe_tick") u32 th09_probe_tick(u32 keys){if(!probe)return 0;return probe->tick(u16(keys));}
#endif
static char auto_replay_folder[512]{};
TH09_EXPORT("th09_auto_replay_folder_buffer") char* th09_auto_replay_folder_buffer(){return auto_replay_folder;}
TH09_EXPORT("th09_auto_save_replay") u32 th09_auto_save_replay(u32 slot){
    if(!probe||!network.active||spectator_mode||!probe->complete||!probe->session||slot>0xffff)return 0;
    auto_replay_folder[sizeof(auto_replay_folder)-1]=0;
    if(!auto_replay_folder[0])return 0;
    char name[560];std::snprintf(name,sizeof(name),"replay/%s/th9_ud%04x.rpy",auto_replay_folder,slot);
    if(probe->title_replay_exists(name))return 0;
    // Encoding consumes random bytes. Local saving cannot change simulation RNG
    // or turn a storage failure into a fatal live-session error.
    const auto random=probe->state.random;
    const auto error=probe->error,session_error=probe->session->error;
    const bool ok=probe->title_save_replay(name,"AUTO");
    probe->state.random=random;probe->error=error;probe->session->error=session_error;return ok;
}
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
TH09_EXPORT("th09_file_valid") u32 th09_file_valid(u32 kind,u32 size){if(!size||size!=file_validation_bytes.size())return 0;const auto* p=file_validation_bytes.data();if(kind==0){PlayerRecords records;return records.load(p,size);}if(kind==2){GameConfiguration config;return config.load(p,size);}if(kind!=1)return 0;ReplayFile file;touch::MotionTrack track;ChargeReplayTrack charge;if(!file.decode(p,size)||!track.load(file.data().data(),file.data().size(),9)||!charge.load(file.data().data(),u32(file.data().size())))return 0;bool found=false;for(u32 stage=0;stage<10;++stage)if(file.stream_offset(0,stage)){ReplayPlayback playback;ReplayRoundSettings settings;if(!playback.begin(file,stage,settings))return 0;found=true;}return found;}
// Direct file-manager playback uses the same decoder/session as the title menu.
static u32 replay_view_offsets[10]{},replay_view_total=0;
TH09_EXPORT("th09_play_replay") u32 th09_play_replay(u32 size){
    if(!probe||network.active||spectator_mode||(!probe->in_title&&(!probe->session||!probe->session->is_replay)))return 0;
    if(!th09_file_valid(1,size))return 0;
    ReplayFile file;if(!file.decode(file_validation_bytes.data(),size)||file.data()[0x1e4]>2||file.data()[0xd7]>4)return 0;
    u32 stage=0;while(stage<10&&!file.stream_offset(0,stage))++stage;
    if(stage>=10){probe->error="Replay has no playable stage";return 0;}
    replay_view_total=0;
    for(u32 s=0;s<10;++s){replay_view_offsets[s]=replay_view_total;if(file.stream_offset(0,s)){ReplayPlayback playback;ReplayRoundSettings settings;playback.begin(file,s,settings);replay_view_total+=playback.length();}}
    clear_inputs();probe->device={};probe->pending_action=-1;probe->error.clear();probe->state.random={0x7531,0,0};
    probe->settings.game_flags|=8u;probe->title_play_replay(file,stage,"");
    probe->launch_pending=false;probe->in_title=false;return probe->start();
}
TH09_EXPORT("th09_replay_view_info") const u32* th09_replay_view_info(){
    static u32 values[9]{};values[0]=2;values[2]=replay_view_total;
    const bool active=probe&&!network.active&&!spectator_mode&&!probe->in_title&&probe->session&&probe->session->is_replay&&probe->world;
    values[3]=!active||(active&&(probe->over||probe->complete));
    if(active){const auto& selection=probe->world->configuration.selection;const auto stage=u32(selection.stage);values[1]=(stage<10?replay_view_offsets[stage]:0)+probe->session->replay_frame();values[4]=selection.difficulty;values[5]=selection.characters[0];values[6]=selection.characters[1];values[7]=stage;values[8]=u32(probe->world->rules.progress.round);}
    return values;
}
TH09_EXPORT("th09_replay_view_step") u32 th09_replay_view_step(u32 render){
    if(th09_replay_view_info()[3])return 1;
    // One logical input tick, independent of display frequency or playback rate.
    // TH09 Draw has native side effects. Execute it once per logical tick,
    // omitting GPU submission for intermediate fast-forward frames.
    const bool prior=rollback_active;rollback_active=true;
    const auto ok=probe->tick_inputs(0,0,0,render!=0);rollback_active=prior;return ok;
}
TH09_EXPORT("th09_replay_view_silent") void th09_replay_view_silent(u32 on){if(probe)probe->replay_view_silent=on!=0;}
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
