#ifdef TH_NATIVE_PLATFORM
#include "PlatformHost.hpp"
#include "GraphicsHost.hpp"
#include "FrameCadence.hpp"
#include "PresentationCadence.hpp"
#include "AudioAudit.hpp"
#include "Renderer.hpp"
#include "../game/PresentationAudit.hpp"
#include "../game/RngAudit.hpp"
#include "../game/DialogueTouchScene.hpp"
#include "ThpracUi.hpp"
#include "../../../portable/input/TouchController.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <memory>
#include <iterator>
#include <cstdint>
#include <algorithm>
#include <cstring>
#include <vector>
#include <eagler/input/BrowserGamepad.hpp>
#include <eagler/diagnostics/LanguageTrace.hpp>
// New browser shells own all DOM keys; old shells keep the SDL fallback.
EM_JS(int, th08_browser_keyboard, (), {return typeof Module['resetBrowserKeyboard']==='function';});
EM_JS(void, th08_reset_browser_keyboard, (), {Module['resetBrowserKeyboard']?.();});
EM_JS(int, th08_frame_ready, (), {return Module['runtimePrepare']?Module['runtimePrepare']():1;});
EM_JS(void, th08_frame_finished, (int result,double ms), {if(Module['runtimeFinish'])Module['runtimeFinish'](result,ms);});
EM_JS(int, th08_limit_presentation_to_60, (), {return Module['eaglerOptions']?.limitPresentationTo60?1:0;});
EM_JS(int, th08_keyboard_gamepad_dpad, (), {
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
namespace th08 {
void sdl_validate_capture();
namespace {
#if defined(TH_PRESENTATION_AUDIT)
struct AuditTimingSample {
    double timestamp_ms=0;
    float delta_ms=0,alpha=1;
    u32 flags=0,simulation_frame=0;
    float camera[26]{};
};
static_assert(sizeof(AuditTimingSample)==128);
constexpr u32 audit_timing_capacity=512;
AuditTimingSample audit_timing[audit_timing_capacity]{};u32 audit_timing_next=0,audit_timing_count=0;
// total milliseconds, maximum milliseconds, call count for Update, semantic
// Draw, presentation Draw, and audio work. Diagnostic builds only.
double audit_phase_profile[12]{};
bool audit_direct_frame=false;u32 audit_direct_frame_sequence=0,audit_direct_frame_result=0xffffffffu;
void audit_phase(u32 slot,double started){const double ms=emscripten_get_now()-started;auto* p=audit_phase_profile+slot*3;p[0]+=ms;p[1]=std::max(p[1],ms);p[2]+=1;}
void camera_values(float* out,const SceneCamera& c){
    for(const auto& v:{c.position,c.target_offset,c.up,c.eye_offset}){*out++=v.x;*out++=v.y;*out++=v.z;}*out=c.field_of_view;
}
#endif
std::unique_ptr<BrowserRuntime> runtime;touhou::input::TouchController touch;
u32 runtime_generation=0,dialogue_touch_request_serial=0;
u32 dialogue_touch_request_status=u32(DialogueTouchRequestStatus::Never),dialogue_touch_request_entry=0xffffffffu,dialogue_touch_request_timeline=0xffffffffu;
struct Key{const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
SDL_Gamepad* gamepad=nullptr;u32 prepared=0;bool running=false,suspended=false;double elapsed=0,last=-1,frame_begin=0;u32 frames=0,loop_epoch=0,warm_mask=0;touhou::sdl::FrameCadence cadence;touhou::sdl::PresentationCadence display_cadence;touhou::sdl::PresentationGate presentation_gate;
constexpr SDL_GamepadButton gamepad_slots[]={
    SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH,
    SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,SDL_GAMEPAD_BUTTON_BACK,
    SDL_GAMEPAD_BUTTON_START,SDL_GAMEPAD_BUTTON_LEFT_STICK,SDL_GAMEPAD_BUTTON_RIGHT_STICK,SDL_GAMEPAD_BUTTON_GUIDE,
};
PracticeCadence practice_cadence;double practice_period=1./60.;
int gamepad_axis(SDL_GamepadAxis axis){const int value=SDL_GetGamepadAxis(gamepad,axis);return value<0?value*1000/32768:value*1000/32767;}
u64 dialogue_touch_hash(const u8* bytes,u32 size){u64 value=14695981039346656037ull;for(u32 i=0;i<size;++i)value=(value^bytes[i])*1099511628211ull;return value;}
struct DialogueTouchWait {bool reachable=false;u32 offset=0,time=0,count=0,instruction_count=0;i32 timeout=0;};
struct DialogueTouchCandidate {bool used=false;u32 entry=0,timeline=0,frame=0,difficulties=0,ecl_offset=0,reference_count=0;DialogueTouchWait wait;};
DialogueTouchManifestV1 dialogue_touch_manifest{};DialogueTouchObservationV1 dialogue_touch_observation{};
void dialogue_touch_empty_manifest(DialogueTouchManifestReason reason){
    dialogue_touch_manifest={};auto* w=dialogue_touch_manifest.words;
    w[0]=dialogue_touch_manifest_magic;w[1]=dialogue_touch_scene_version;w[2]=dialogue_touch_manifest_words;
    w[4]=u32(reason);w[5]=runtime_generation;w[6]=runtime?u32(reinterpret_cast<std::uintptr_t>(runtime.get())):0;
    w[7]=loop_epoch;w[23]=dialogue_touch_manifest_candidate_words;
}
bool dialogue_touch_fixture_ready(BrowserRuntime& r,DialogueTouchManifestReason& reason){
    if(!r.app.active()||!r.app.in_game()||r.app.loading_game()||!r.app.game.ready()){
        reason=DialogueTouchManifestReason::RuntimeNotReady;return false;
    }
    auto& g=r.app.game;const u32 flags=g.globals.game_flags;
    if(g.globals.stage!=0||(flags&(1u|2u|8u|0x60u|0x4000u))||g.paused||g.retrying||g.menus.context.pause_state){
        reason=DialogueTouchManifestReason::WrongRun;return false;
    }
    if(!r.validation_scene_used||!g.enemies.validation_scene().enabled()){
        reason=DialogueTouchManifestReason::FixtureGateMissing;return false;
    }
    const auto& write=r.validation_scene_writes.receipt();
    if(!write.locked||write.load_scope||write.committed_bytes){
        reason=DialogueTouchManifestReason::WriteLockInvalid;return false;
    }
    reason=DialogueTouchManifestReason::Ready;return true;
}
bool dialogue_touch_initial_wait(const MessageProgram& program,u32 index,DialogueTouchWait& result){
    result={};const auto* entry=program.entry(i32(index));if(!entry)return false;
    const auto base=reinterpret_cast<std::uintptr_t>(program.bytes()),address=reinterpret_cast<std::uintptr_t>(entry);
    if(address<base||address-base>=program.size())return false;const u32 begin=u32(address-base);u32 end=program.size();
    for(u32 i=0;i<program.count();++i){const auto* other=program.entry(i);if(!other)continue;const auto p=reinterpret_cast<std::uintptr_t>(other);
        if(p>=base&&p-base>begin&&p-base<end)end=u32(p-base);}
    if(end<=begin)return false;u32 cursor=begin;bool branch_before_wait=false;
    while(cursor<end){if(end-cursor<sizeof(MessageInstruction))return false;MessageInstruction ins{};std::memcpy(&ins,program.bytes()+cursor,sizeof(ins));
        const u32 total=u32(sizeof(MessageInstruction))+ins.size;if(total>end-cursor)return false;++result.instruction_count;
        if(ins.opcode==MessageOpcode::Wait){
            ++result.count;
            if(!branch_before_wait&&!result.reachable&&ins.size>=sizeof(i32)){
                i32 timeout=0;std::memcpy(&timeout,program.bytes()+cursor+sizeof(MessageInstruction),sizeof(timeout));
                if(timeout>0){result.reachable=true;result.offset=cursor;result.time=ins.time;result.timeout=timeout;}
            }
        }
        if(ins.opcode==MessageOpcode::Delete||ins.opcode==MessageOpcode::Selection||
           ins.opcode==MessageOpcode::ReadSelected||ins.opcode==MessageOpcode::Halt||ins.opcode==MessageOpcode::StageEnd)
            branch_before_wait=true;
        cursor+=total;
    }
    return true;
}
DialogueTouchManifestV1* dialogue_touch_build_manifest(){
    dialogue_touch_empty_manifest(runtime?DialogueTouchManifestReason::RuntimeNotReady:DialogueTouchManifestReason::NoRuntime);
    if(!runtime)return &dialogue_touch_manifest;auto& words=dialogue_touch_manifest.words;auto& app=runtime->app;auto& g=app.game;
    DialogueTouchManifestReason reason=DialogueTouchManifestReason::RuntimeNotReady;
    if(!dialogue_touch_fixture_ready(*runtime,reason)){words[4]=u32(reason);return &dialogue_touch_manifest;}
    const i32 character=g.dialogue_context.character;
    if(character<0||character>=12||!g.program.data()||!g.program.size()||!g.program.timeline_count()||
       !g.dialogue.program.bytes()||!g.dialogue.program.size()||!g.dialogue.program.count()){
        words[4]=u32(DialogueTouchManifestReason::SourceMissing);return &dialogue_touch_manifest;
    }
    const u32 team=character<4?u32(character):u32((character-4)/2);
    if(team>=4){words[4]=u32(DialogueTouchManifestReason::InvalidSource);return &dialogue_touch_manifest;}
    const auto& ecl=g.program;const auto& messages=g.dialogue.program;
    words[5]=runtime_generation;words[6]=u32(reinterpret_cast<std::uintptr_t>(runtime.get()));words[7]=loop_epoch;
    words[8]=g.globals.stage;words[9]=u32(character);words[10]=team;words[11]=g.globals.difficulty_mask;
    words[12]=ecl.size();const u64 eh=dialogue_touch_hash(ecl.data(),ecl.size());words[13]=u32(eh);words[14]=u32(eh>>32);
    words[15]=ecl.timeline_count();words[16]=messages.count();words[17]=messages.size();
    const u64 mh=dialogue_touch_hash(messages.bytes(),messages.size());words[18]=u32(mh);words[19]=u32(mh>>32);
    words[20]=1;words[21]=team+1;words[23]=dialogue_touch_manifest_candidate_words;

    std::vector<DialogueTouchWait> waits(messages.count());std::vector<u8> wait_valid(messages.count());
    for(u32 i=0;i<messages.count();++i){DialogueTouchWait wait{};if(!dialogue_touch_initial_wait(messages,i,wait)){
            words[4]=u32(DialogueTouchManifestReason::InvalidSource);return &dialogue_touch_manifest;}
        waits[i]=wait;wait_valid[i]=wait.reachable?1:0;}
    DialogueTouchCandidate candidates[dialogue_touch_manifest_candidate_capacity]{};u32 candidate_count=0;bool overflow=false;
    const auto* ecl_base=ecl.data();const auto ecl_base_address=reinterpret_cast<std::uintptr_t>(ecl_base);
    for(u32 ti=0;ti<ecl.timeline_count();++ti){const auto* timeline=ecl.timeline(i32(ti));const u32 length=ecl.timeline_size(ti);
        if(!timeline||!length){words[4]=u32(DialogueTouchManifestReason::InvalidSource);return &dialogue_touch_manifest;}
        const auto address=reinterpret_cast<std::uintptr_t>(timeline);if(address<ecl_base_address||address-ecl_base_address>ecl.size()||length>ecl.size()-(address-ecl_base_address)){
            words[4]=u32(DialogueTouchManifestReason::InvalidSource);return &dialogue_touch_manifest;}
        const u32 start=u32(address-ecl_base_address);u32 cursor=0;
        while(cursor+sizeof(EclTimelineInstruction)<=length){EclTimelineInstruction ins{};std::memcpy(&ins,ecl_base+start+cursor,sizeof(ins));
            if(ins.time<0)break;if(ins.size<sizeof(EclTimelineInstruction)||ins.size>length-cursor){words[4]=u32(DialogueTouchManifestReason::InvalidSource);return &dialogue_touch_manifest;}
            if(ins.opcode==6&&ins.size>=sizeof(EclTimelineInstruction)+sizeof(i32)&&(ins.difficulties&g.globals.difficulty_mask)){
                i32 entry=-1;std::memcpy(&entry,ecl_base+start+cursor+sizeof(EclTimelineInstruction),sizeof(entry));
                if(entry>=0&&u32(entry)<messages.count()&&wait_valid[u32(entry)]){
                    u32 slot=0;while(slot<candidate_count&&candidates[slot].entry!=u32(entry))++slot;
                    if(slot==candidate_count){if(candidate_count==dialogue_touch_manifest_candidate_capacity){overflow=true;break;}
                        auto& c=candidates[candidate_count++];c.used=true;c.entry=u32(entry);c.timeline=ti;c.frame=u32(ins.time);
                        c.difficulties=ins.difficulties;c.ecl_offset=start+cursor;c.wait=waits[u32(entry)];}
                    auto& c=candidates[slot];if(c.reference_count<0xffffffffu)++c.reference_count;
                }
            }
            cursor+=ins.size;
        }
        if(overflow)break;
    }
    if(overflow){words[4]=u32(DialogueTouchManifestReason::CandidateOverflow);return &dialogue_touch_manifest;}
    if(!candidate_count){words[4]=u32(DialogueTouchManifestReason::NoConfirmEntry);return &dialogue_touch_manifest;}
    words[22]=candidate_count;
    for(u32 i=0;i<candidate_count;++i){const auto& c=candidates[i];u32* row=words+dialogue_touch_manifest_header_words+i*dialogue_touch_manifest_candidate_words;
        row[0]=c.entry;row[1]=c.timeline;row[2]=c.frame;row[3]=c.difficulties;row[4]=c.ecl_offset;row[5]=c.reference_count;
        row[6]=c.wait.offset;row[7]=c.wait.time;row[8]=u32(c.wait.timeout);row[9]=c.wait.instruction_count;}
    words[3]=1;words[4]=u32(DialogueTouchManifestReason::Ready);return &dialogue_touch_manifest;
}
void dialogue_touch_fill_observation(){
    dialogue_touch_observation={};u32* w=dialogue_touch_observation.words;w[0]=dialogue_touch_observation_magic;w[1]=dialogue_touch_scene_version;w[2]=dialogue_touch_observation_words;
    w[4]=runtime?u32(DialogueTouchManifestReason::Ready):u32(DialogueTouchManifestReason::NoRuntime);w[5]=runtime_generation;
    w[6]=runtime?u32(reinterpret_cast<std::uintptr_t>(runtime.get())):0;w[7]=loop_epoch;w[8]=frames;
    w[37]=touch.fire;w[38]=touch.current_context();w[39]=runtime&&runtime->keyboard_state()[90];
    w[40]=dialogue_touch_request_serial;w[41]=dialogue_touch_request_status;w[42]=dialogue_touch_request_entry;w[43]=dialogue_touch_request_timeline;
    w[22]=w[23]=w[24]=w[25]=0xffffffffu;
    if(!runtime)return;auto& app=runtime->app;auto& g=app.game;const auto& write=runtime->validation_scene_writes.receipt();
    w[3]=app.active()&&app.in_game()&&!app.loading_game();w[9]=app.active();w[10]=app.in_game();w[11]=g.globals.stage;w[12]=u32(g.dialogue_context.character);w[13]=g.globals.game_flags;
    w[14]=runtime->validation_scene_used;w[15]=g.enemies.validation_scene().enabled();w[16]=g.enemies.validation_scene().update_started();
    w[17]=g.enemies.validation_scene().withheld_updates();w[18]=write.locked;w[19]=write.load_scope;w[20]=write.committed_bytes;
    const auto& d=g.display.dialogue;w[21]=g.dialogue.present();w[22]=u32(d.message);w[26]=u32(d.timer.current);w[27]=u32(d.paused_frames);
    const auto& consumer=g.gui_flow.dialogue_input_receipt();w[28]=u32(d.minimum_wait);w[29]=d.skippable;w[30]=d.selected;
    w[31]=consumer.current();w[32]=consumer.previous();constexpr u32 confirm_mask=1u;
    w[33]=consumer.valid()&&bool((consumer.current()&confirm_mask)&&!(consumer.previous()&confirm_mask));
    w[34]=consumer.sequence();w[35]=u32(g.control.state.frames);w[36]=u32(g.globals.frame_count);
    const auto* base=g.dialogue.program.bytes();const auto* instruction=d.instruction;
    if(base&&instruction){const auto b=reinterpret_cast<std::uintptr_t>(base),p=reinterpret_cast<std::uintptr_t>(instruction);
        if(p>=b&&p-b<=g.dialogue.program.size()&&g.dialogue.program.size()-(p-b)>=sizeof(MessageInstruction)){
            MessageInstruction current{};std::memcpy(&current,instruction,sizeof(current));if(current.size<=g.dialogue.program.size()-(p-b)-sizeof(current)){
                w[23]=u32(p-b);w[24]=u32(current.opcode);w[25]=current.time;
            }
        }
    }
    w[44]=consumer.valid();
}
constexpr const char* warmAnimations[]={"etama.anm","enemy.anm","front.anm","times.anm","stg1bg.anm","stg1enm.anm","eff01.anm","stg1txt.anm","stg2bg.anm","stg2enm.anm","eff02.anm","stg2txt.anm","player00.anm","player01.anm","player02.anm","player03.anm","staff01.anm"};
constexpr u32 warmCount=sizeof(warmAnimations)/sizeof(*warmAnimations);
touhou::input::TouchState touch_state(){touhou::input::TouchState s;if(!runtime)return s;const auto& a=runtime->app;const auto& g=a.game;const auto& p=g.player_state;
    if(a.supervisor.state.active==i32(th08::Scene::Ending)){s.context=2;return s;}
    if(a.in_game()&&(g.globals.game_flags&8)){s.context=3;return s;}
    if(!a.in_game()||a.loading_game()||!g.ready()||g.paused||g.menus.context.pause_state||p.context.game_over||(g.globals.game_flags&0x60))return s;
    s.context=g.dialogue.present()?2:1;s.choice=g.dialogue.choosing();s.ready=p.life.state!=1&&p.life.state!=2;s.instance=g.globals.stage+1;
    s.x=p.motion.movement.position.x;s.y=p.motion.movement.position.y;s.fast=g.shots[0].settings().normal_speed*g.player.timing.rate;s.slow=g.shots[1].settings().focus_speed*g.player.timing.rate;
    s.min_x=p.input.minimum.x;s.min_y=p.input.minimum.y;s.max_x=s.min_x+p.input.extent.x;s.max_y=s.min_y+p.input.extent.y;return s;
}
int touch_stage(){return runtime&&runtime->app.in_game()?runtime->app.game.globals.stage:-1;}
void language_scene(){if(runtime)eagler::diagnostics::language_trace().set_scene(runtime->app.supervisor.state.active);}
void sync_touch_context(const touhou::input::TouchState& state){const int previous=touch.current_context();if(runtime&&previous!=state.context&&(previous==1||previous==2)&&state.context!=1&&state.context!=2)runtime->motion.touch_cancel(touch_stage());}
// Ownership is acquired on pointer-down and retained through release, even
// when the drag leaves an ImGui window. Other fingers remain game-owned.
bool practice_pointer_active=false;int practice_pointer_id=0;
void pointer(int type,int id,float x,float y){
    if(type<0||type>2||!std::isfinite(x)||!std::isfinite(y))return;
    if(type==0&&!practice_pointer_active&&ThpracUi::captures_pointer(x*640.f,y*480.f)){practice_pointer_active=true;practice_pointer_id=id;}
    if(practice_pointer_active&&id==practice_pointer_id){ThpracUi::mouse(type==0?1:type==1?0:2,x*640.f,y*480.f);if(type==2)practice_pointer_active=false;return;}
    const auto state=touch_state();sync_touch_context(state);if(runtime&&(state.context==1||state.context==2))runtime->motion.touch_event(touch_stage(),type,id,x,y);touch.pointer(type,id,x,y,SDL_GetTicks(),state,runtime&&runtime->keyboard_state()[16]);
}
void cancel_touch(){practice_pointer_active=false;ThpracUi::cancel_pointer();if(runtime){runtime->motion.touch_cancel(touch_stage());runtime->motion.target(0,0,0);}touch.cancel_transient();}
bool interpolation_ready(){
    if(!runtime||runtime->app.loading_game()||runtime->app.title.modal())return false;
    if(runtime->app.title.active())return true;
    if(!runtime->app.in_game())return false;
    const auto& g=runtime->app.game;
    return g.menus.context.supervisor_state==2&&!g.control.state.load_state&&(g.globals.game_flags&0x60)!=0x20;
}
void poll(){if(!runtime)return;SDL_Event event;while(SDL_PollEvent(&event)){
    ThpracUi::process_event(event);
    if(event.type==SDL_EVENT_FINGER_CANCELED)cancel_touch();
    if(event.type==SDL_EVENT_FINGER_DOWN||event.type==SDL_EVENT_FINGER_MOTION||event.type==SDL_EVENT_FINGER_UP)pointer(event.type==SDL_EVENT_FINGER_DOWN?0:event.type==SDL_EVENT_FINGER_MOTION?1:2,int(event.tfinger.fingerID),event.tfinger.x,event.tfinger.y);
    if(event.type==SDL_EVENT_GAMEPAD_ADDED&&!gamepad)gamepad=SDL_OpenGamepad(event.gdevice.which);
    if(event.type==SDL_EVENT_GAMEPAD_REMOVED&&gamepad&&SDL_GetGamepadID(gamepad)==event.gdevice.which){SDL_CloseGamepad(gamepad);gamepad=nullptr;}
    }
    auto* keys=runtime->keyboard_state();std::memset(keys,0,256);const bool* physical=th08_browser_keyboard()?nullptr:SDL_GetKeyboardState(nullptr);
    for(const auto& k:keyboard_map)if(k.hosted||(physical&&k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native])){keys[k.vk]=128;if(k.vk>=160&&k.vk<=165)keys[16+(k.vk-160)/2]=128;}
    const int mappedPad=eagler::input::BrowserGamepadState();eagler::input::ApplyGamepadVirtualKeys(keys,mappedPad);
    if(mappedPad<0&&gamepad&&SDL_GamepadConnected(gamepad)){u8 buttons[128]{};for(size_t i=0;i<sizeof(gamepad_slots)/sizeof(*gamepad_slots);++i)buttons[i]=SDL_GetGamepadButton(gamepad,gamepad_slots[i])?128:0;
        int x=gamepad_axis(SDL_GAMEPAD_AXIS_LEFTX),y=gamepad_axis(SDL_GAMEPAD_AXIS_LEFTY);
        const int dx=int(SDL_GetGamepadButton(gamepad,SDL_GAMEPAD_BUTTON_DPAD_RIGHT))-int(SDL_GetGamepadButton(gamepad,SDL_GAMEPAD_BUTTON_DPAD_LEFT));
        const int dy=int(SDL_GetGamepadButton(gamepad,SDL_GAMEPAD_BUTTON_DPAD_DOWN))-int(SDL_GetGamepadButton(gamepad,SDL_GAMEPAD_BUTTON_DPAD_UP));
        if(dx)x=dx*1000;if(dy)y=dy*1000;runtime->controller_state(x,y,buttons,128,true);
    }else runtime->controller_state(0,0,nullptr,0,false);
    const int keyboardDpad=th08_keyboard_gamepad_dpad();
    if(keyboardDpad&1)keys[38]=128;if(keyboardDpad&2)keys[40]=128;if(keyboardDpad&4)keys[37]=128;if(keyboardDpad&8)keys[39]=128;
    const auto state=touch_state();sync_touch_context(state);const auto input=touch.sample(state,SDL_GetTicks(),keys[16],keys[37]||keys[38]||keys[39]||keys[40]);for(int i=0;i<256;i++)if(input.keys[i])keys[i]=128;
    runtime->motion.target(input.motion,input.x,input.y);runtime->touch_sensitivity=touch.sensitivity;
    ThpracUi::update_input(*runtime);
    if(ThpracUi::captures_game_input())for(const int vk:{16,27,37,38,39,40,88,90})keys[vk]=0;
}
int tick(){poll();if(runtime)runtime->app.session.practice.input.gui_tick();return !runtime||!runtime->step(false)?runtime&&(runtime->status(2)||runtime->status(4))?2:1:0;}
EM_BOOL frame(double now,void* epoch){
    bool active=running;
#if defined(TH_PRESENTATION_AUDIT)
    active=active||audit_direct_frame;
#endif
    if(!active||uintptr_t(epoch)!=loop_epoch)return EM_FALSE;const double delta=last<0?0:std::max(0.,(now-last)/1000.);last=now;frame_begin=emscripten_get_now();
    if(suspended||!th08_frame_ready()){sdl_audio_pause(true);practice_cadence.reset();cadence.reset();display_cadence.reset();presentation_gate.reset();return EM_TRUE;}sdl_audio_pause(false);int result=0;
    const bool limit60=th08_limit_presentation_to_60()!=0;
    const double period=ThpracUi::simulation_interval(*runtime);if(period!=practice_period){practice_period=practice_cadence.period=period;practice_cadence.reset();cadence.reset();display_cadence.reset();presentation_gate.reset();}
    const bool custom_speed=std::abs(period-1./60.)>1e-12;
    const bool ready=interpolation_ready()&&!limit60&&!custom_speed,fast=touhou::sdl::PresentationCadence::fast_sample(delta);if(ready)display_cadence.advance(delta);else display_cadence.reset();
    const unsigned ticks=custom_speed?practice_cadence.advance(delta):cadence.advance(delta);const bool tick_due=ticks!=0;
    const bool high=display_cadence.high_refresh&&interpolation_ready()&&!custom_speed;const bool interpolate=presentation_gate.advance(high,tick_due);bool presented=false;
    for(unsigned tick_index=0;tick_index<ticks&&!result;++tick_index){
        elapsed+=touhou::sdl::FrameCadence::interval;
#if defined(TH_PRESENTATION_AUDIT)
        const double update_started=emscripten_get_now();
#endif
        result=tick();
#if defined(TH_PRESENTATION_AUDIT)
        audit_phase(0,update_started);
#endif
        if(!result&&runtime){
            // Preserve TH08's authoritative update+draw tick exactly. At high
            // presentation rates every fixed-tick draw is semantic but hidden;
            // the visible frame below is a second, side-effect-free presentation
            // pass. Missed original deadlines are skipped rather than caught up.
            const bool hidden=high||tick_index+1<ticks;const bool lightweight=hidden&&runtime&&!runtime->visual_capture_pending();if(hidden)sdl_defer(1);
            if(lightweight){runtime->suppress_visual_draw(true);runtime->app.renderer.visual_geometry_suppressed=true;}
#if defined(TH_PRESENTATION_AUDIT)
            const double semantic_started=emscripten_get_now();
#endif
            language_scene();if(!runtime->app.draw(1.0f,false,false))result=(runtime->status(2)||runtime->status(4))?2:1;
            else if(runtime->status(2)||runtime->status(4))result=2;
#if defined(TH_PRESENTATION_AUDIT)
            audit_phase(1,semantic_started);
#endif
            if(lightweight){runtime->app.renderer.visual_geometry_suppressed=false;runtime->suppress_visual_draw(false);}
            if(hidden)sdl_defer(0);else presented=true;
            if(!result){++frames;
#if defined(TH_PRESENTATION_AUDIT)
                const double audio_started=emscripten_get_now();
#endif
                if(!runtime->audio_tick(u32(elapsed*1000)))result=2;
#if defined(TH_PRESENTATION_AUDIT)
                audit_phase(3,audio_started);
#endif
            }
        }
    }
    float presentation_alpha=1.0f;
    if(!result&&runtime&&high){
        const bool frozen=runtime->app.in_game()&&(runtime->app.game.paused||runtime->app.game.retrying||runtime->app.game.menus.context.pause_state||runtime->app.game.menus.context.show_retry);
#if defined(TH_PRESENTATION_AUDIT)
        const double presentation_started=emscripten_get_now();
#endif
        presentation_alpha=interpolate?float(cadence.interpolation_alpha()):1.0f;language_scene();presented=runtime->app.draw(presentation_alpha,interpolate,true,!frozen);
#if defined(TH_PRESENTATION_AUDIT)
        audit_phase(2,presentation_started);
#endif
    }
    if(presented&&runtime)runtime->app.statistics.presentation_frame();
#if defined(TH_PRESENTATION_AUDIT)
    if(runtime){auto& sample=audit_timing[audit_timing_next];sample={};sample.timestamp_ms=now;sample.delta_ms=float(delta*1000.);sample.alpha=presentation_alpha;
        sample.flags=u32(ready)|(u32(fast)<<1)|(u32(high)<<2)|(u32(tick_due)<<3)|(u32(interpolate)<<4)|(u32(presented)<<5)|(u32(custom_speed)<<8);
        if(runtime->app.in_game()){sample.simulation_frame=u32(runtime->app.game.control.state.frames);SceneCamera previous,current;bool rebased=false;if(runtime->app.game.background_script.audit_cameras(previous,current,rebased)){sample.flags|=1u<<6;if(rebased)sample.flags|=1u<<7;camera_values(sample.camera,previous);camera_values(sample.camera+13,current);}}
        audit_timing_next=(audit_timing_next+1)%audit_timing_capacity;audit_timing_count=std::min(audit_timing_count+1,audit_timing_capacity);
    }
#endif
    sdl_audio_pump();th08_frame_finished(result,emscripten_get_now()-frame_begin);
#if defined(TH_PRESENTATION_AUDIT)
    if(audit_direct_frame){++audit_direct_frame_sequence;audit_direct_frame_result=u32(result);}
#endif
    return running?EM_TRUE:EM_FALSE;
}
}
u32 sdl_game_time(){return u32(elapsed*1000);}
extern "C" {
#define EX(name) __attribute__((export_name(name)))
#if defined(TH_PRESENTATION_AUDIT)
EX("audit_rng_enable") void audit_rng_enable(u32 value){using namespace th08::rng_audit;enabled=value!=0;counts.fill(0);event_write=event_count=event_overflow=total_calls=0;enemy_write=enemy_count=enemy_overflow=0;}
// Candidate diagnostic ABI | status 0 schema,1 capacity,2 count,3 write,4 overwritten,5 words.
EX("audit_rng_enemy_snapshots_v1") const th08::rng_audit::EnemySnapshot* audit_rng_enemy_snapshots_v1(){return th08::rng_audit::enemy_snapshots.data();}
EX("audit_rng_enemy_status_v1") u32 audit_rng_enemy_status_v1(u32 field){using namespace th08::rng_audit;
    switch(field){case 0:return 1;case 1:return enemy_capacity;case 2:return enemy_count;case 3:return enemy_write;case 4:return enemy_overflow;case 5:return sizeof(EnemySnapshot)/sizeof(u32);default:return 0;}}
EX("audit_rng_counts") const u32* audit_rng_counts(){return th08::rng_audit::counts.data();}
EX("audit_rng_details") const th08::rng_audit::Event* audit_rng_details(){return th08::rng_audit::events.data();}
EX("audit_rng_detail_status") u32 audit_rng_detail_status(u32 field){using namespace th08::rng_audit;
    switch(field){case 0:return 1;case 1:return event_capacity;case 2:return event_count;case 3:return event_write;case 4:return event_overflow;case 5:return sizeof(Event)/sizeof(u32);default:return 0;}}
#endif
void sdl_keys_clear();
EX("language_trace_enable_v1") void language_trace_enable_v1(u32 enabled){eagler::diagnostics::language_trace().enable(enabled!=0);}
EX("language_trace_json_size_v1") u32 language_trace_json_size_v1(){const auto size=eagler::diagnostics::language_trace().json_size();return size>0xffffffffu?0:u32(size);}
EX("language_trace_json_v1") u32 language_trace_json_v1(char* output,u32 capacity){const auto size=eagler::diagnostics::language_trace().write_json(output,capacity);return size>0xffffffffu?0:u32(size);}
EX("sdl_game_open") BrowserRuntime* sdl_game_open(u32 milliseconds){if(runtime)return nullptr;sdl_keys_clear();prepared=frames=warm_mask=0;if(++runtime_generation==0)++runtime_generation;dialogue_touch_request_status=u32(DialogueTouchRequestStatus::Never);dialogue_touch_request_entry=dialogue_touch_request_timeline=0xffffffffu;elapsed=double(milliseconds)/1000.;cadence.reset();display_cadence.reset();presentation_gate.reset();last=-1;touch.begin_session();
#if defined(TH_PRESENTATION_AUDIT)
    audit_timing_next=audit_timing_count=0;std::fill(std::begin(audit_phase_profile),std::end(audit_phase_profile),0.0);
#endif
    runtime=std::make_unique<BrowserRuntime>();if(!sdl_attach(runtime.get())||!sdl_load_assets(*runtime)){runtime.reset();sdl_detach();return nullptr;}
    if(!(SDL_WasInit(SDL_INIT_GAMEPAD)&SDL_INIT_GAMEPAD))SDL_InitSubSystem(SDL_INIT_GAMEPAD);for(auto& k:keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);int count=0;auto* ids=SDL_GetGamepads(&count);if(count)gamepad=SDL_OpenGamepad(ids[0]);SDL_free(ids);return runtime.get();}
EX("sdl_prepare_total") u32 sdl_prepare_total(){return runtime?runtime->resources().size()+runtime->native_font_steps()+warmCount:0;}
EX("sdl_prepare_loading") bool sdl_prepare_loading(){return runtime&&runtime->prepare_loading();}
EX("sdl_draw_loading") bool sdl_draw_loading(u32 advance){return runtime&&runtime->draw_loading(advance!=0);}
EX("sdl_prepare_next") i32 sdl_prepare_next(){if(!runtime)return -1;if(prepared>=sdl_prepare_total())return 0;const auto assets=runtime->resources().size();
    bool ok=true;const auto fonts=runtime->native_font_steps();
    if(prepared<assets)ok=sdl_prepare_asset(*runtime,prepared);
    else if(prepared<assets+fonts)ok=runtime->native_font_step(prepared-assets);
    else {const auto& bytes=runtime->file(warmAnimations[prepared-assets-fonts]);
        // Optional bounded cache: exhaustion must never prevent launching.
        if(!bytes.empty()&&runtime->app.library.preload(bytes.data(),bytes.size()))warm_mask|=1u<<(prepared-assets-fonts);}
    ++prepared;return ok?i32(prepared):-1;}
EX("sdl_warm_assets") u32 sdl_warm_assets(){return warm_mask;}
EX("sdl_game_initialize") bool sdl_game_initialize(){if(!runtime||prepared!=sdl_prepare_total()||!runtime->initialize()||!ThpracUi::initialize())return false;auto& trace=eagler::diagnostics::language_trace();trace.set_phase(eagler::diagnostics::LanguageTrace::Phase::Runtime);language_scene();sdl_validate_capture();return true;}
EX("sdl_loop_start") void sdl_loop_start(){if(running||!runtime)return;running=true;last=-1;practice_cadence.reset();cadence.reset();display_cadence.reset();presentation_gate.reset();emscripten_request_animation_frame_loop(frame,reinterpret_cast<void*>(uintptr_t(++loop_epoch)));}
EX("sdl_loop_stop") void sdl_loop_stop(){running=false;++loop_epoch;practice_cadence.reset();sdl_audio_pause(true);}
EX("sdl_loop_pause") void sdl_loop_pause(u32 pause){suspended=pause!=0;last=-1;practice_cadence.reset();cadence.reset();display_cadence.reset();presentation_gate.reset();sdl_audio_pause(suspended);}
EX("sdl_loop_time") double sdl_loop_time(){return elapsed;}
#if defined(TH_PRESENTATION_AUDIT)
// Invoke the production RAF callback with a controlled timestamp while no
// browser RAF loop is running. The callback body and scheduler remain shared.
EX("sdl_presentation_audit_frame_at_v1") u32 sdl_presentation_audit_frame_at_v1(double now_ms){
    if(!runtime||running||suspended||audit_direct_frame||!std::isfinite(now_ms)||now_ms<0||
       (last>=0&&now_ms<=last))return 0;
    const u32 before=audit_direct_frame_sequence;
    audit_direct_frame=true;audit_direct_frame_result=0xffffffffu;
    frame(now_ms,reinterpret_cast<void*>(uintptr_t(loop_epoch)));
    audit_direct_frame=false;
    return audit_direct_frame_sequence==before+1&&audit_direct_frame_result==0?1:0;
}
// Diagnostic freeze is NOT the in-game pause. It retains the last endpoint pair
// and never polls input, advances clocks, executes ANM/ECL, or pumps audio.
EX("audit_draw") i32 audit_draw(float alpha,u32 world){
    if(running||!runtime||!std::isfinite(alpha)||alpha<0||alpha>1)return -1;
    if(runtime->app.title.modal())return -2; // No Draw occurs: do not reuse stale captures.
    language_scene();if(!runtime->app.draw(alpha,true,true,world!=0))return 1;
    // This deliberately mutates one synchronous audio operation only when the
    // explicit Lab negative control is enabled. The audio digest must reject it.
    if(th08::audit::audio_pump_injected())sdl_audio_pump();
    return 0;
}
EX("audit_gate") u32 audit_gate(){return interpolation_ready()?1:0;}
EX("audit_world_frozen") u32 audit_world_frozen(){return runtime&&runtime->app.in_game()&&(runtime->app.game.paused||runtime->app.game.retrying||runtime->app.game.menus.context.pause_state||runtime->app.game.menus.context.show_retry);}
EX("audit_scene") const i32* audit_scene(){
    static i32 out[16]{};std::fill(out,out+16,0);if(!runtime)return out;const auto& a=runtime->app;const auto& g=a.game;
    out[0]=a.supervisor.state.active;out[1]=g.globals.stage;out[2]=g.globals.difficulty;out[3]=g.globals.shot;
    out[4]=g.playback.stream.frame;out[5]=g.enemies.state.frames;out[6]=g.control.state.frames;
    out[7]=g.globals.spell_flags;out[8]=g.globals.spell_number;out[9]=g.globals.current_spell;
    out[10]=a.session.numbers.score;out[11]=g.control.state.load_state;out[12]=g.globals.game_flags;
    out[13]=g.paused;out[14]=g.retrying;out[15]=g.globals.stage_completion;return out;
}
EX("audit_spell_name") const char* audit_spell_name(){return runtime?runtime->app.game.globals.spell_name:"";}
EX("audit_effect_sample") const float* audit_effect_sample(uintptr_t object){return runtime?runtime->app.game.effect_system.audit_presentation_sample(object):nullptr;}
EX("audit_ascii_sample") const float* audit_ascii_sample(uintptr_t object){return runtime?runtime->app.ascii.audit_presentation_sample(object):nullptr;}
EX("audit_player_bomb_sample") const float* audit_player_bomb_sample(uintptr_t object,u32 part){return runtime?runtime->app.game.player.audit_bomb_presentation(object,part):nullptr;}
EX("audit_enemy_sample") const float* audit_enemy_sample(uintptr_t object,u32 part){return runtime?runtime->app.game.enemies.audit_presentation_sample(object,part):nullptr;}
EX("audit_calculation_epoch") u32 audit_calculation_epoch(){return u32(th08::presentation::calculation_epoch);}
EX("audit_timing_records") const AuditTimingSample* audit_timing_records(){return audit_timing;}
EX("audit_timing_capacity") u32 audit_timing_capacity_value(){return audit_timing_capacity;}
EX("audit_timing_count") u32 audit_timing_count_value(){return audit_timing_count;}
EX("audit_timing_next") u32 audit_timing_next_value(){return audit_timing_next;}
EX("audit_timing_stride") u32 audit_timing_stride(){return sizeof(AuditTimingSample);}
EX("audit_phase_profile") const double* audit_phase_profile_value(){return audit_phase_profile;}
EX("audit_state_words") u32 audit_state_words(){return 17;}
// Selected authoritative-state fingerprints, independent of render captures.
// They deliberately omit renderer caches/pointers/padding and wall-clock data.
// Each word covers the field list exported by the adapter. Untagged engine
// state remains incomplete and cannot inherit a pass from these domains.
EX("audit_state") const u32* audit_state(){
    static u32 out[17];std::fill(out,out+17,2166136261u);if(!runtime)return out;auto& a=runtime->app;auto& g=a.game;
    const auto add=[](u32& h,const auto& value){const auto* p=reinterpret_cast<const u8*>(&value);for(size_t i=0;i<sizeof(value);++i){h^=p[i];h*=16777619u;}};
    const auto instruction_offset=[&](u32& h,const void* instruction,const void* base,size_t length){
        const bool present=instruction!=nullptr;add(h,present);if(!present)return;
        const auto address=reinterpret_cast<std::uintptr_t>(instruction),origin=reinterpret_cast<std::uintptr_t>(base);
        const bool valid=base&&address>=origin&&address-origin<length;add(h,valid);
        const u32 offset=valid?u32(address-origin):0;add(h,offset);
    };
    const auto anm_offset=[&](u32& h,const AnmVm& vm,const AnmRawInstr* instruction){
        const bool present=instruction!=nullptr;add(h,present);if(!present)return;
        const auto address=reinterpret_cast<std::uintptr_t>(instruction),origin=reinterpret_cast<std::uintptr_t>(vm.beginningOfScript);
        const bool valid=vm.beginningOfScript&&address>=origin&&address-origin<=0xffffffffu;add(h,valid);
        const u32 offset=valid?u32(address-origin):0;add(h,offset);
    };
    const auto v2=[&](u32& h,const Vec2& v){add(h,v.x);add(h,v.y);};
    const auto v3=[&](u32& h,const Vec3& v){add(h,v.x);add(h,v.y);add(h,v.z);};
    const auto timer=[&](u32& h,const Timer& t){add(h,t.previous);add(h,t.fraction);add(h,t.current);};
    const auto vm=[&](u32& h,const AnmVm& v){
        v3(h,v.pos);v3(h,v.pos2);v2(h,v.scale);v2(h,v.scaleGrowth);v2(h,v.spriteSize);v3(h,v.rotation);v3(h,v.angleVel);
        v2(h,v.uvScrollPos);v2(h,v.uvScrollVel);timer(h,v.currentTimeInScript);timer(h,v.waitTimer);
        for(const auto& t:v.interpCurrentTimers)timer(h,t);for(const auto& t:v.interpEndTimers)timer(h,t);
        for(const auto mode:v.interpModes)add(h,mode);add(h,v.intVar0);add(h,v.intVar1);add(h,v.intVar2);add(h,v.intVar3);
        add(h,v.floatVar0);add(h,v.floatVar1);add(h,v.floatVar2);add(h,v.floatVar3);add(h,v.counterVar0);add(h,v.counterVar1);
        add(h,v.color1.d3dColor);add(h,v.color2.d3dColor);add(h,v.flags);add(h,v.type);add(h,v.pendingInterrupt);
        add(h,v.playerBulletHitAnimationType);add(h,v.activeSpriteIndex);add(h,v.anmFileIndex);add(h,v.baseSpriteIndex);add(h,v.scriptIndex);
        anm_offset(h,v,v.currentInstruction);timer(h,v.interruptReturnTime);anm_offset(h,v,v.interruptReturnInstruction);
        v3(h,v.posInitial);v3(h,v.posFinal);v3(h,v.rotateInitial);v3(h,v.rotateFinal);v2(h,v.scaleInitial);v2(h,v.scaleFinal);
        add(h,v.color1Initial.d3dColor);add(h,v.color1Final.d3dColor);add(h,v.color2Initial.d3dColor);add(h,v.color2Final.d3dColor);add(h,v.timeOfLastSpriteSet);
    };
    const auto field_array=[&](u32& h,const auto& values){for(const auto& value:values)add(h,value);};
    const auto game_values=[&](u32& h,const GameGlobals& n){
        add(h,n.display_score);add(h,n.graze_stage);add(h,n.score);add(h,n.graze);add(h,n.score_increment);add(h,n.high_score);add(h,n.high_score_retries);
        add(h,n.captured_spells);add(h,n.gauge_copy);add(h,n.gauge);add(h,n.point_value);add(h,n.clock_time);add(h,n.retries);add(h,n.points_stage);add(h,n.points);
        add(h,n.point_extends);add(h,n.next_point_extend);add(h,n.time_orbs);add(h,n.last_spell_requirement);add(h,n.total_time_orbs);
        field_array(h,n.rng1);add(h,n.deaths);add(h,n.deaths_stage);field_array(h,n.rng2);add(h,n.lives);field_array(h,n.rng3);add(h,n.bombs);
        add(h,n.bombs_used);add(h,n.bombs_used_stage);field_array(h,n.rng4);add(h,n.power);field_array(h,n.rng5);add(h,n.rng6);field_array(h,n.rng7);
        add(h,n.integrity_value);add(h,n.integrity_checksum);field_array(h,n.rng8);
    };
    const auto configuration=[&](u32& h,const GameConfiguration& c){field_array(h,c.controller);add(h,c.reserved12);add(h,c.version);add(h,c.pad_x);add(h,c.pad_y);add(h,c.lives);add(h,c.bombs);add(h,c.color16);add(h,c.music);add(h,c.sounds);add(h,c.difficulty);add(h,c.windowed);add(h,c.frameskip);add(h,c.effects);add(h,c.slow_mode);add(h,c.shot_slow);add(h,c.music_volume);add(h,c.sound_volume);field_array(h,c.reserved29);add(h,c.options);};
    const auto chapter=[&](u32& h,const ScoreChapter& c){add(h,c.magic);add(h,c.length);add(h,c.copy_length);add(h,c.version);add(h,c.unknown);add(h,c.padding);};
    const auto spell_history=[&](u32& h,const SpellHistory& s){field_array(h,s.max_bonus);field_array(h,s.attempts);field_array(h,s.captures);};
    const auto spell_record=[&](u32& h,const SpellRecord& s){chapter(h,s.header);add(h,s.number);add(h,s.unknown);add(h,s.difficulty);add(h,s.name);add(h,s.owner);add(h,s.comment1);add(h,s.comment2);spell_history(h,s.game);spell_history(h,s.practice);add(h,s.unknown228);};
    const auto clear_record=[&](u32& h,const ClearRecord& c){chapter(h,c.header);field_array(h,c.without_retries);field_array(h,c.with_retries);add(h,c.unknown);add(h,c.shot);add(h,c.padding);};
    const auto practice_record=[&](u32& h,const PracticeRecord& p){chapter(h,p.header);for(const auto& row:p.attempts)field_array(h,row);for(const auto& row:p.high_scores)field_array(h,row);add(h,p.shot);add(h,p.used);add(h,p.padding);};
    const auto play_record=[&](u32& h,const PlayRecord& p){chapter(h,p.header);field_array(h,p.total_time);field_array(h,p.game_time);for(const auto& c:p.counts){add(h,c.total);field_array(h,c.characters);add(h,c.restarts);add(h,c.clears);add(h,c.continues);add(h,c.practices);}field_array(h,p.music_unlocked);};
    const auto high_score=[&](u32& h,const HighScore& p){chapter(h,p.header);add(h,p.score);add(h,p.lag);add(h,p.character);add(h,p.difficulty);add(h,p.stage);add(h,p.name);add(h,p.date);add(h,p.retries);add(h,p.unknown27);configuration(h,p.config);add(h,p.play_frames);add(h,p.points);add(h,p.unknown6c);add(h,p.deaths);add(h,p.bombs);add(h,p.last_spells);add(h,p.pauses);add(h,p.time_orbs);add(h,p.humanity);field_array(h,p.spell_counters);};
    const auto practice_config=[&](u32& h,const PracticeConfig& p){add(h,p.mode);add(h,p.stage);add(h,p.warp);add(h,p.section);add(h,p.phase);add(h,p.frame);add(h,p.dlg);add(h,p.score);add(h,p.life);add(h,p.bomb);add(h,p.power);add(h,p.gauge);add(h,p.graze);add(h,p.point);add(h,p.point_total);add(h,p.point_stage);add(h,p.time);add(h,p.value);add(h,p.night);add(h,p.familiar);add(h,p.rank);add(h,p.rankLock);add(h,p.sp1_pts);add(h,p.bsX);add(h,p.bsY);add(h,p.legacy_blue_replay);};
    const auto practice_state=[&](u32& h,const PracticeState& p){
        add(h,p.enabled);add(h,p.active);add(h,p.replay);add(h,p.menu);add(h,p.accepted);practice_config(h,p.configured);practice_config(h,p.run);practice_config(h,p.replay_candidate);add(h,p.replay_candidate_valid);
        add(h,p.cheats);add(h,p.assisted);add(h,p.familiar_pending);add(h,p.everlasting_bgm);add(h,p.el_bgm_lock);add(h,p.el_bgm_block);add(h,p.all_clear_bonus);add(h,p.doswnc);
        const auto& in=p.input;add(h,in.enable_auto_shoot);add(h,in.shoot_key_DIK);add(h,in.last_is_auto_shoot_key_down);add(h,in.is_auto_shooting);add(h,in.is_th128);add(h,in.disable_xkey);add(h,in.disable_Ckey_at_same_time);add(h,in.disable_shiftkey);add(h,in.force_shiftkey);add(h,in.disable_zkey);add(h,in.enable_fast_retry);add(h,in.fast_retry_count_down);
        const auto& monitor=p.key_monitor;field_array(h,monitor.g_keys_down);field_array(h,monitor.g_key_mask);add(h,monitor.g_aps_cur);add(h,monitor.g_record_key_aps);add(h,monitor.g_recorded_aps.size());for(const auto value:monitor.g_recorded_aps)add(h,value);add(h,monitor.g_recorded_keys.size());for(const auto value:monitor.g_recorded_keys)add(h,value);add(h,monitor.keys_per_sec.size());for(const auto value:monitor.keys_per_sec)add(h,value);
        add(h,p.show_keyboard_monitor);add(h,p.map_inf_life_to_no_continue);add(h,p.force_last_spell);add(h,p.show_lock_timer);add(h,p.disable_master);add(h,p.show_bullet_hitbox);add(h,p.use_custom_rsqrt);add(h,p.rsqrt_cpu);
        add(h,p.speed.fps_status);add(h,p.speed.fps);add(h,p.speed.fps_replay_slow);add(h,p.speed.fps_replay_fast);add(h,p.speed.fps_debug_acc);add(h,p.tracker_last_spell_captures);add(h,p.tracker_dissolve_count);
    };
    const auto replay_input=[&](u32& h,const ReplayInputState& p){add(h,p.physical);add(h,p.current);add(h,p.previous);add(h,p.repeat);add(h,p.held_frames);add(h,p.flags);add(h,p.slow_mode);add(h,p.speedhack);add(h,p.timing_level);add(h,p.timing_forced);};
    const auto replay_metadata=[&](u32& h,const ReplayMetadata& p){
        const auto& header=p.header;add(h,header.magic);add(h,header.version);add(h,header.unknown6);add(h,header.unknown7);add(h,header.unknown8);add(h,header.file_size);add(h,header.checksum);add(h,header.unknown14);add(h,header.key);add(h,header.unknown16);add(h,header.unknown17);add(h,header.compressed_size);add(h,header.payload_size);field_array(h,header.stage_offsets);field_array(h,header.timing_offsets);
        add(h,p.unknown68);add(h,p.minor_version);add(h,p.shot_type);add(h,p.difficulty);add(h,p.date);add(h,p.player_name);add(h,p.unknown7a);add(h,p.practice);add(h,p.spell_number);add(h,p.spell_name);add(h,p.major_version);add(h,p.spell_score);configuration(h,p.configuration);add(h,p.reservedf0);add(h,p.lag);add(h,p.clear_state);add(h,p.padding11d);add(h,p.unknown120);add(h,p.exe_size);add(h,p.exe_checksum);add(h,p.exe_version);add(h,p.padding132);
    };
    const auto replay_stage=[&](u32& h,const ReplayStage& p){add(h,p.end_score);add(h,p.points);add(h,p.graze);add(h,p.extends);add(h,p.next_extend);add(h,p.point_value);add(h,p.humanity);add(h,p.seed);add(h,p.power);add(h,p.lives);add(h,p.bombs);add(h,p.rank);add(h,p.power_item_counter);add(h,p.captured_spells);add(h,p.clock);add(h,p.reserved23);};
    const auto byte_vector=[&](u32& h,const std::vector<u8>& values){add(h,values.size());for(const auto value:values)add(h,value);};
    const auto replay_stream=[&](u32& h,const ReplayStream& s){add(h,s.frame);add(h,s.transition_frames);add(h,s.input_cursor);add(h,s.timing_cursor);add(h,s.input_end);add(h,s.timing_end);byte_vector(h,s.input);byte_vector(h,s.timing);
#if defined(TH_PRESENTATION_AUDIT)
        add(h,s.audit_stride());
#endif
    };
    const auto ecl_locals=[&](u32& h,const EclContext::Locals& l){field_array(h,l.integers);field_array(h,l.floats);field_array(h,l.counters);field_array(h,l.extra);field_array(h,l.integer_arguments);field_array(h,l.float_arguments);};
    const auto ecl_context=[&](u32& h,const EclContext& c){
        add(h,c.subroutine);add(h,c.call_depth);add(h,c.branched);add(h,c.returned);
        instruction_offset(h,c.instruction,g.program.data(),g.program.size());instruction_offset(h,c.branch_instruction,g.program.data(),g.program.size());
        ecl_locals(h,c.locals);for(const auto& i:c.interpolations){add(h,i.active);timer(h,i.time);add(h,i.duration);add(h,i.curve);add(h,i.easing);field_array(h,i.values);add(h,i.target);}
        for(i32 i=0;i<std::clamp(c.call_depth,0,16);++i)instruction_offset(h,c.call_stack[i],g.program.data(),g.program.size());
        timer(h,c.timer);timer(h,c.wait_timer);add(h,c.native_callback);instruction_offset(h,c.native_instruction,g.program.data(),g.program.size());
        for(i32 i=0;i<std::clamp(c.call_depth,0,16);++i){const auto& f=c.call_frames[i];timer(h,f.time);timer(h,f.wait);add(h,f.subroutine);ecl_locals(h,f.locals);
            for(const auto& it:f.interpolations){add(h,it.active);timer(h,it.time);add(h,it.duration);add(h,it.curve);add(h,it.easing);field_array(h,it.values);add(h,it.target);}
            add(h,f.native_callback);instruction_offset(h,f.native_instruction,g.program.data(),g.program.size());}
    };
    const auto population_index=[&](const EclVm* target){if(!target)return i32(-1);for(i32 i=0;i<481;++i)if(g.enemies.population.at(u32(i))==target)return i;return i32(-2);};
    const auto effect_index=[&](const EffectState* target){if(!target)return i32(-1);for(i32 i=0;i<i32(std::size(g.effect_pool.objects));++i)if(&g.effect_pool.objects[i]==target)return i;return i32(-2);};
    const auto vm_effect_index=[&](const AnmVm* target){if(!target)return i32(-1);for(i32 i=0;i<i32(std::size(g.effect_pool.objects));++i)if(static_cast<const AnmVm*>(&g.effect_pool.objects[i])==target)return i;return i32(-2);};
    const auto laser_index=[&](const LaserState* target){if(!target)return i32(-1);for(i32 i=0;i<i32(std::size(g.projectile_pool.lasers));++i)if(&g.projectile_pool.lasers[i]==target)return i;return i32(-2);};
    const auto template_index=[&](const BulletTemplate* target){if(!target)return i32(-1);for(i32 i=0;i<i32(std::size(g.projectile_pool.templates.types));++i)if(&g.projectile_pool.templates.types[i]==target)return i;return i32(-2);};
    const auto bullet_emission=[&](u32& h,const BulletEmission& b){
        add(h,b.sprite);add(h,b.color);v3(h,b.position);add(h,b.angle);add(h,b.spread);add(h,b.speed);add(h,b.ending_speed);
        for(const auto& e:b.extras){add(h,e.float_a);add(h,e.float_b);add(h,e.integer_a);add(h,e.integer_b);add(h,e.flags);add(h,e.mode);}
        field_array(h,b.reserved1d0);add(h,b.count);add(h,b.layers);add(h,b.pattern);add(h,b.unknown1fa);add(h,b.flags);add(h,b.sound);add(h,b.transform_sound);add(h,b.extra_index);add(h,template_index(b.type));
    };
    const auto ecl_vm=[&](u32& h,const EclVm& e){
        add(h,u32(e.failure));add(h,e.program==&g.program);add(h,e.random==&a.session.random);add(h,e.environment==&g.globals);
        add(h,population_index(e.parent));add(h,population_index(e.next_familiar));add(h,population_index(e.previous_familiar));add(h,population_index(e.next_in_layer));
        i32 active_context=-1;if(e.active_context==&e.main_context)active_context=0;else for(i32 i=0;i<4;++i)if(e.active_context==e.asynchronous[i].get())active_context=i+1;add(h,active_context);
        add(h,e.active_slot);add(h,e.scratch_depth);for(const auto generation:e.asynchronous_generations)add(h,generation);
        field_array(h,e.shared_integer);field_array(h,e.shared_real);ecl_context(h,e.main_context);
        for(const auto& context:e.asynchronous){add(h,bool(context));if(context)ecl_context(h,*context);}
        add(h,e.life);add(h,e.initial_life);add(h,e.remaining_life);add(h,e.pool_index);add(h,e.point_items);add(h,e.power_items);timer(h,e.lifetime);add(h,e.last_damage);add(h,e.boss_id);field_array(h,e.life_thresholds);add(h,e.item_reward);add(h,e.score_reward);add(h,e.pending_interrupt);add(h,e.death_subroutine);field_array(h,e.interrupt_subroutines);
        for(const auto& v:e.animation)vm(h,v);field_array(h,e.poses);add(h,e.pose_direction);bullet_emission(h,e.emitter);add(h,e.rank_speed_low);add(h,e.rank_speed_high);add(h,e.rank_count_low);add(h,e.rank_count_high);add(h,e.rank_layers_low);add(h,e.rank_layers_high);
        for(const auto byte:e.repeated_shot)add(h,byte);add(h,e.emission_period);timer(h,e.emission_time);bullet_emission(h,e.laser_emitter);for(const auto* laser:e.laser_slots)add(h,laser_index(laser));add(h,e.laser_slot);
        field_array(h,e.life_subroutines);add(h,e.timeout);add(h,e.timeout_subroutine);for(const auto* effect:e.effects)add(h,effect_index(effect));add(h,e.effect_count);add(h,e.remaining_seconds);add(h,e.animation_color);add(h,e.effect_radius);
        v3(h,e.position_offset);v3(h,e.resolved_position);v3(h,e.emission_offset);v3(h,e.origin);v3(h,e.target);v3(h,e.last_delta);v3(h,e.previous_position);add(h,e.difficulty_flags);
        add(h,e.angular_velocity);add(h,e.speed);add(h,e.acceleration);add(h,e.orbit_velocity);add(h,e.orbit_growth);field_array(h,e.bounds);timer(h,e.movement_time);add(h,e.movement_duration);
        v3(h,e.position);v3(h,e.velocity);v3(h,e.direction);v3(h,e.hitbox);v3(h,e.low_damage_hitbox);add(h,e.player_protect_squared);add(h,e.orbit_angle);add(h,e.orbit_radius);add(h,e.flags);add(h,e.flags2);add(h,vm_effect_index(e.familiar_effect));add(h,e.waiting);add(h,e.finished);add(h,e.invalid);timer(h,e.damage_protection);field_array(h,e.death_effects);add(h,e.summoned_familiars);add(h,e.practice_familiar);add(h,e.time_items);add(h,e.draw_layer);add(h,e.hit_flash);
        add(h,e.trail.flags);add(h,e.trail.length);add(h,e.trail.collision_length);add(h,e.trail.step);for(const auto& point:e.trail.points){v3(h,point.position);v3(h,point.velocity);add(h,point.angle);}
    };
    // 0: deterministic inputs and all declared mutable game-value fields.
    add(out[0],a.session.random.seed);add(out[0],a.session.random.backup);add(out[0],a.session.random.calls);game_values(out[0],a.session.numbers);
    add(out[0],a.session.rank.value);add(out[0],a.session.rank.maximum);add(out[0],a.session.rank.minimum);add(out[0],a.session.rank.fraction);
    add(out[0],a.session.thresholds.minimum);add(out[0],a.session.thresholds.maximum);add(out[0],a.session.thresholds.human_bonus);add(out[0],a.session.thresholds.youkai_bonus);add(out[0],a.session.thresholds.human);add(out[0],a.session.thresholds.youkai);
    // Persistent GameplaySession and its title-visible history are read by
    // results, practice, replay and unlock flows; include every declared field.
    const auto& session=a.session;practice_state(out[0],session.practice);configuration(out[0],session.config);configuration(out[0],session.display_config);high_score(out[0],session.history);
    for(const auto& record:session.records)spell_record(out[0],record);for(const auto& record:session.previous_records)spell_record(out[0],record);
    for(const auto& record:session.clears)clear_record(out[0],record);for(const auto& record:session.practices)practice_record(out[0],record);play_record(out[0],session.statistics);
    add(out[0],session.stall_frames);add(out[0],session.stage_copy);add(out[0],session.replay_seed);
    chapter(out[0],session.last_words.header);field_array(out[0],session.last_words.unlocked);field_array(out[0],session.last_words.padding);
    chapter(out[0],session.last_name.header);add(out[0],session.last_name.name);field_array(out[0],session.last_name.padding);add(out[0],session.total_clock);add(out[0],session.values.expected_checksum);
    // The title keeps an independently mutable copy of save/history data.
    const auto& tc=a.title.context;add(out[0],tc.flags.isPracticeMode);add(out[0],tc.flags.isDemoMode);add(out[0],tc.flags.isActive);add(out[0],tc.flags.isReplay);add(out[0],tc.flags.unk4);add(out[0],tc.flags.gameState);add(out[0],tc.flags.unk7);add(out[0],tc.flags.unk9);add(out[0],tc.flags.unk10);add(out[0],tc.flags.isGoingToFinalB);add(out[0],tc.flags.unk13);add(out[0],tc.flags.isSpellPractice);add(out[0],tc.flags.isExtraUnlocked);add(out[0],tc.flags.isSpellPracticeUnlocked);add(out[0],tc.flags.isExtraUnlockedWithAllTeams);add(out[0],tc.flags.reserved);
    add(out[0],tc.supervisor_state);add(out[0],tc.supervisor_previous);add(out[0],tc.difficulty);add(out[0],tc.character);add(out[0],tc.shotType);add(out[0],tc.currentStage);add(out[0],tc.replayMode);add(out[0],tc.currentDemoReplay);add(out[0],tc.lastKeyChanged);add(out[0],tc.bgmVolume);add(out[0],tc.sfxVolume);add(out[0],tc.replayFilename);add(out[0],tc.currentSpellCardNumber);add(out[0],tc.software_texturing);
    for(const auto& record:tc.clears)clear_record(out[0],record);play_record(out[0],tc.play);for(const auto& record:tc.spells)spell_record(out[0],record);chapter(out[0],tc.last_words.header);field_array(out[0],tc.last_words.unlocked);field_array(out[0],tc.last_words.padding);for(const auto& record:tc.practice_scores)practice_record(out[0],record);
    // 1: player motion, life, bomb activation and consumed logical controls.
    const auto& ps=g.player_state;const auto& motion=ps.motion;v3(out[1],motion.movement.position);v3(out[1],motion.movement.velocity);for(const auto& p:motion.movement.history)v3(out[1],p);
    for(const auto& p:motion.movement.half_boxes)v3(out[1],p);for(const auto& p:motion.movement.bounds)v3(out[1],p);v2(out[1],motion.movement.multiplier);v2(out[1],motion.movement.delta);add(out[1],motion.movement.direction);
    vm(out[1],motion.animation);add(out[1],motion.form.focused);add(out[1],motion.form.youkai);add(out[1],motion.form.frames);timer(out[1],motion.form.transition);
    for(const auto& o:motion.options){v3(out[1],o.position);v3(out[1],o.center);v3(out[1],o.velocity);add(out[1],o.state);add(out[1],o.pose);add(out[1],o.index);add(out[1],o.angle);add(out[1],o.shot_angle);timer(out[1],o.timer);add(out[1],o.update);}
    timer(out[1],motion.gauge.idle);add(out[1],ps.life.state);add(out[1],ps.life.deathbomb);add(out[1],ps.life.auto_bomb);timer(out[1],ps.life.timer);add(out[1],ps.life.predead_count);add(out[1],ps.life.clear_frames);
    add(out[1],ps.bomb.active);add(out[1],ps.bomb.type);add(out[1],ps.bomb.duration);add(out[1],ps.bomb.consumed);add(out[1],ps.bomb.sequence);timer(out[1],ps.bomb.timer);add(out[1],ps.bomb.cooldown);add(out[1],ps.bomb.triggered);
    add(out[1],ps.input.buttons);add(out[1],ps.input.bomb);add(out[1],ps.input.bomb_type);add(out[1],ps.input.character);add(out[1],ps.input.gui_blocked);add(out[1],ps.input.tampered);add(out[1],ps.input.enemy_present);add(out[1],ps.input.always_hitbox);v2(out[1],ps.input.minimum);v2(out[1],ps.input.extent);
    add(out[1],ps.context.game_flags);add(out[1],ps.context.hud_flags);add(out[1],ps.context.pause);add(out[1],ps.context.bombs);add(out[1],ps.context.lives);add(out[1],ps.context.power);add(out[1],ps.context.time_orbs);add(out[1],ps.context.last_spell_requirement);add(out[1],ps.context.replay_flags);add(out[1],ps.context.character);add(out[1],ps.context.focused);add(out[1],ps.context.time_spell);add(out[1],ps.context.game_over);add(out[1],ps.context.miss_control);add(out[1],ps.context.cheats);
    // 2-3: active bullet and laser simulation data; draw/resource caches omitted.
    add(out[2],g.projectile_pool.active_count);add(out[2],g.projectile_pool.cancel_frames);timer(out[2],g.projectile_pool.timer);add(out[2],g.projectile_pool.unknown_counter);
    for(u32 i=0;i<std::size(g.projectile_pool.bullets);++i){const auto& b=g.projectile_pool.bullets[i];add(out[2],i);add(out[2],b.state);if(!b.state)continue;
        v3(out[2],b.position);v3(out[2],b.velocity);v3(out[2],b.legacy_acceleration);add(out[2],b.speed);add(out[2],b.legacy_float_a);add(out[2],b.change_speed);add(out[2],b.angle);add(out[2],b.legacy_float_b);add(out[2],b.change_angle);
        timer(out[2],b.since_fired);timer(out[2],b.active_time);add(out[2],b.legacy_integer);add(out[2],b.change_interval);add(out[2],b.change_count);add(out[2],b.change_limit);add(out[2],b.despawn_protection);add(out[2],b.extra_flags);add(out[2],b.flags);add(out[2],b.sprite_offset);add(out[2],b.unknown_db6);add(out[2],b.despawn_counter);add(out[2],b.unknown_dbc);add(out[2],b.grazed);add(out[2],b.barrier_cooldown);add(out[2],b.transform_sound);add(out[2],b.current_extra);
        for(const auto& e:b.extras){add(out[2],e.float_a);add(out[2],e.float_b);add(out[2],e.integer_a);add(out[2],e.integer_b);add(out[2],e.flags);add(out[2],e.mode);}for(const auto& e:b.extra_state){timer(out[2],e.timer);add(out[2],e.float_a);add(out[2],e.float_b);v3(out[2],e.vector);add(out[2],e.integer_a);add(out[2],e.integer_b);add(out[2],e.integer_c);}}
    for(u32 i=0;i<std::size(g.projectile_pool.lasers);++i){const auto& l=g.projectile_pool.lasers[i];add(out[3],i);add(out[3],l.in_use);if(!l.in_use)continue;
        add(out[3],l.state);v3(out[3],l.position);add(out[3],l.angle);add(out[3],l.start_offset);add(out[3],l.end_offset);add(out[3],l.length);add(out[3],l.width);add(out[3],l.width2);add(out[3],l.speed);add(out[3],l.start);add(out[3],l.hitbox_start);add(out[3],l.duration);add(out[3],l.stop);add(out[3],l.hitbox_stop);timer(out[3],l.timer);add(out[3],l.flags);add(out[3],l.color);for(const auto& v:l.animation)vm(out[3],v);}
    // 4: active ECL executions, program event snapshot and validation gate.
    const auto& es=g.enemies.state;timer(out[4],es.timer);add(out[4],es.frames);add(out[4],es.unfocused_frames);add(out[4],es.active_frames);add(out[4],es.active_count);add(out[4],g.enemies.validation_scene().enabled());add(out[4],g.enemies.validation_scene().withheld_updates());add(out[4],runtime->validation_scene_used);add(out[4],g.program.timeline_count());
    const auto& eg=g.globals;add(out[4],eg.practice_force_last_spell);add(out[4],eg.practice_enemy_invincible);add(out[4],eg.practice_tools_enabled);add(out[4],eg.practice_lock_frames);
    for(const auto* file:eg.enemy_animation_files)add(out[4],bool(file));add(out[4],bool(eg.bullet_actions));add(out[4],bool(eg.laser_actions));add(out[4],bool(eg.phase_actions));add(out[4],bool(eg.player_frame));add(out[4],bool(eg.enemy_actions));add(out[4],bool(eg.scene_actions));add(out[4],bool(eg.spell_actions));add(out[4],bool(eg.native_services));add(out[4],bool(eg.frame_count_value));add(out[4],bool(eg.rank_value));add(out[4],eg.values==&a.session.numbers);add(out[4],bool(eg.gui));add(out[4],bool(eg.ascii));add(out[4],bool(eg.timeline_actions));
    add(out[4],eg.current_spell);add(out[4],eg.frame_count);add(out[4],eg.timeline_parameter);add(out[4],eg.player_state);timer(out[4],eg.player_state_timer);add(out[4],eg.game_flags);v3(out[4],eg.player);v3(out[4],eg.boss);add(out[4],eg.stage);add(out[4],eg.difficulty);add(out[4],eg.difficulty_mask);add(out[4],eg.paused);add(out[4],eg.rank);add(out[4],eg.shot);add(out[4],eg.youkai);
    field_array(out[4],eg.integer_arguments);field_array(out[4],eg.float_arguments);add(out[4],eg.pending_time);add(out[4],eg.uncollected_time_items);add(out[4],bool(eg.live_values));add(out[4],eg.gui_blocks_spawn);add(out[4],eg.dialogue_active);add(out[4],eg.stage_completion);add(out[4],eg.stop_spawn);field_array(out[4],eg.timeline_signals);
    add(out[4],eg.event_count);add(out[4],eg.bullets_created);add(out[4],eg.enemies_created);add(out[4],eg.boss_life);add(out[4],eg.boss_lives);add(out[4],eg.stage_interrupt);add(out[4],eg.bullet_rank);add(out[4],eg.player_protect_range);add(out[4],eg.player_nullified);bullet_emission(out[4],eg.emitter_template);add(out[4],bool(eg.combat));
    for(const auto* boss:eg.boss_slots)add(out[4],population_index(boss));
    for(u32 i=0;i<std::min(eg.event_count,u32(256));++i){const auto& event=eg.events[i];add(out[4],event.opcode);field_array(out[4],event.arguments);add(out[4],event.frame);}
    for(u32 i=0;i<std::size(eg.enemies);++i){const auto& value=eg.enemies[i];add(out[4],i);add(out[4],value.active);if(!value.active)continue;add(out[4],value.kind);add(out[4],value.flags);v3(out[4],value.position);add(out[4],value.life);add(out[4],value.hitbox);add(out[4],value.anm);}
    for(u32 i=0;i<std::size(eg.bullets);++i){const auto& value=eg.bullets[i];add(out[4],i);add(out[4],value.active);if(!value.active)continue;add(out[4],value.kind);add(out[4],value.color);v3(out[4],value.position);v3(out[4],value.velocity);add(out[4],value.angle);add(out[4],value.speed);add(out[4],value.radius);add(out[4],value.flags);}
    for(u32 i=0;i<std::size(eg.lasers);++i){const auto& value=eg.lasers[i];add(out[4],i);add(out[4],value.active);if(!value.active)continue;add(out[4],value.kind);add(out[4],value.flags);v3(out[4],value.position);add(out[4],value.angle);add(out[4],value.length);add(out[4],value.width);add(out[4],value.index);}
    for(u32 i=0;i<std::size(eg.items);++i){const auto& value=eg.items[i];add(out[4],i);add(out[4],value.active);if(!value.active)continue;add(out[4],value.type);v3(out[4],value.position);add(out[4],value.count);}
    for(u32 i=0;i<std::size(es.timelines);++i){const auto& t=es.timelines[i];add(out[4],i);add(out[4],t.timeline);timer(out[4],t.timer);add(out[4],t.difficulty);add(out[4],t.finished);add(out[4],t.invalid);}
    for(u32 i=0;i<481;++i){const auto* e=g.enemies.population.at(i);if(!e||!(e->flags&1))continue;add(out[4],i);ecl_vm(out[4],*e);
    }
    // 5: live effects; geometry vertices and renderer allocations are not state.
    add(out[5],g.effect_pool.active_count);add(out[5],g.effect_pool.frames);
    for(u32 i=0;i<std::size(g.effect_pool.objects);++i){const auto& e=g.effect_pool.objects[i];if(!e.active)continue;add(out[5],i);vm(out[5],e);v3(out[5],e.position);v3(out[5],e.parameters);v3(out[5],e.velocity);v3(out[5],e.acceleration);v3(out[5],e.world_position);v3(out[5],e.center);v3(out[5],e.direction);v3(out[5],e.up);add(out[5],e.radius);add(out[5],e.angle);add(out[5],e.width);add(out[5],e.segments);add(out[5],e.slot);add(out[5],e.height);add(out[5],e.angle_y);add(out[5],e.frequency);timer(out[5],e.age);add(out[5],e.kind);add(out[5],e.dying);add(out[5],e.fade_frames);add(out[5],e.layer);add(out[5],e.alternative);add(out[5],e.ignore_pause);}
    // 6: live items; linked pool bookkeeping and authored state, not textures.
    const auto& is=g.items.status();add(out[6],is.next_index);add(out[6],is.count);
    for(u32 i=0;i<ItemPoolState::capacity+1;++i){const auto& it=is.items[i];if(!it.active)continue;add(out[6],i);v3(out[6],it.position);v3(out[6],it.velocity);v3(out[6],it.target);timer(out[6],it.timer);add(out[6],it.type);add(out[6],it.onscreen);add(out[6],it.state);add(out[6],it.max_value);vm(out[6],it.animation);}
    // 7: shot and bomb state, including fixed pool slots active at this sample.
    const auto& shots=ps.shots;v3(out[7],shots.position);for(const auto& p:shots.options)v3(out[7],p);v3(out[7],shots.enemy_target);v3(out[7],shots.target);v3(out[7],shots.homing_target);add(out[7],shots.option_angle);add(out[7],shots.orbit_angle);add(out[7],shots.bomb);add(out[7],shots.game_flags);add(out[7],shots.focused);add(out[7],shots.enemy_available);add(out[7],shots.youkai_bonus);add(out[7],shots.character);add(out[7],shots.player_state);add(out[7],shots.gui_blocked);add(out[7],shots.option_active);timer(out[7],shots.shooting_timer);timer(out[7],shots.collision_timer);add(out[7],shots.time_item_threshold);add(out[7],shots.time_spell);add(out[7],shots.human_bonus);add(out[7],shots.effect_counter);
    for(u32 i=0;i<std::size(shots.shots);++i){const auto& s=shots.shots[i];if(!s.state)continue;add(out[7],i);v3(out[7],s.position);for(const auto& p:s.history)v3(out[7],p);v3(out[7],s.size);v3(out[7],s.velocity);add(out[7],s.speed);add(out[7],s.angle);timer(out[7],s.timer);add(out[7],s.damage);add(out[7],s.state);add(out[7],s.kind);add(out[7],s.laser_slot);add(out[7],s.option);add(out[7],s.interval);add(out[7],s.focused);add(out[7],s.animation_index);add(out[7],s.gauge_bonus);vm(out[7],s.animation);}
    for(const auto& slot:shots.lasers)timer(out[7],slot.timer);
    for(u32 i=0;i<std::size(ps.bomb_objects.objects);++i){const auto& b=ps.bomb_objects.objects[i];if(!b.state)continue;add(out[7],i);add(out[7],b.state);add(out[7],b.frame);add(out[7],b.speed);add(out[7],b.distance);add(out[7],b.angle);v3(out[7],b.position);for(const auto& p:b.history)v3(out[7],p);v3(out[7],b.velocity);v3(out[7],b.acceleration);for(const auto& v:b.animation)vm(out[7],v);timer(out[7],b.timer);}
    // 8: scene selection, title, control and logical frame state.
    add(out[8],a.supervisor.state.active);add(out[8],a.supervisor.state.target);add(out[8],a.supervisor.state.previous);add(out[8],a.supervisor.state.calculations);add(out[8],a.supervisor.state.background_status);add(out[8],a.supervisor.state.background_wait);add(out[8],a.supervisor.state.close_requested);add(out[8],a.supervisor.state.background_running);add(out[8],a.supervisor.state.keep_resources);add(out[8],a.supervisor.state.sticky_input);add(out[8],a.supervisor.state.practice);add(out[8],a.supervisor.state.difficulty);add(out[8],a.supervisor.state.stage);add(out[8],a.supervisor.state.screen_effect_counter);add(out[8],a.supervisor.state.fog);
    add(out[8],a.title.context.supervisor_state);add(out[8],a.title.context.supervisor_previous);add(out[8],a.title.context.difficulty);add(out[8],a.title.context.character);add(out[8],a.title.context.shotType);add(out[8],a.title.context.currentStage);add(out[8],a.title.context.replayMode);add(out[8],a.title.context.lastKeyChanged);add(out[8],a.title.menus.state.currentScreen);add(out[8],a.title.menus.state.stateTimer2);add(out[8],a.title.menus.state.startMenuIdleFrames);add(out[8],a.title.menus.state.vmCount);
    if(a.title.menus.state.vms)for(i32 i=0;i<std::clamp(a.title.menus.state.vmCount,0,256);++i)vm(out[8],a.title.menus.state.vms[i]);
    const auto& cs=g.control.state;add(out[8],cs.frames);add(out[8],cs.restore_viewport);add(out[8],cs.next_scene);add(out[8],cs.load_state);add(out[8],cs.load_frames);add(out[8],cs.start_music);add(out[8],cs.slow_frames);add(out[8],cs.play_frames);add(out[8],cs.demo_frames);add(out[8],cs.demo_index);add(out[8],cs.replay_mode);add(out[8],cs.stage_mask);add(out[8],cs.sticky_input);add(out[8],cs.loading_images_setup);
    add(out[8],g.globals.game_flags);add(out[8],g.globals.stage);add(out[8],g.globals.difficulty);add(out[8],g.globals.shot);add(out[8],g.globals.spell_flags);add(out[8],g.globals.spell_number);add(out[8],g.globals.current_spell);add(out[8],g.enemies.state.frames);add(out[8],g.dialogue.present());add(out[8],g.dialogue.choosing());
    // 9: authored background script state and camera endpoints, no matrices.
    const auto& bg=g.background;timer(out[9],bg.time);add(out[9],bg.instruction_index);add(out[9],bg.frames);add(out[9],bg.stage);v3(out[9],bg.position);add(out[9],bg.clear_color);add(out[9],bg.youkai_tint);timer(out[9],bg.tint_time);vm(out[9],bg.tint_vm);
    add(out[9],bg.fog.near_plane);add(out[9],bg.fog.far_plane);add(out[9],bg.fog.color.d3dColor);add(out[9],bg.fog_initial.near_plane);add(out[9],bg.fog_initial.far_plane);add(out[9],bg.fog_initial.color.d3dColor);add(out[9],bg.fog_final.near_plane);add(out[9],bg.fog_final.far_plane);add(out[9],bg.fog_final.color.d3dColor);add(out[9],bg.fog_duration);timer(out[9],bg.fog_time);add(out[9],bg.fog_needs_setup);
    add(out[9],bg.spell_state);add(out[9],bg.spell_frames);add(out[9],bg.spell_flag);add(out[9],bg.spell_vm_count);add(out[9],bg.pending_interrupt);add(out[9],bg.use_tint);add(out[9],bg.distance_limit);add(out[9],bg.camera_effect);add(out[9],bg.effect_flags);add(out[9],bg.effect_visible);
    for(const auto& v:bg.layers)vm(out[9],v);for(i32 i=0;i<std::clamp(bg.spell_vm_count,0,32);++i)vm(out[9],bg.spell_vms[i]);vm(out[9],bg.extra_vm);for(const auto& p:bg.effect_positions)v3(out[9],p);
    const auto camera=[&](const SceneCamera& c){v3(out[9],c.position);v3(out[9],c.target_offset);v3(out[9],c.up);v3(out[9],c.unused24);v3(out[9],c.right);v3(out[9],c.eye_offset);add(out[9],c.field_of_view);};
    camera(bg.camera);camera(bg.camera_initial);camera(bg.camera_final);camera(bg.camera_initial_derivative);camera(bg.camera_final_derivative);
    field_array(out[9],bg.camera_durations);field_array(out[9],bg.camera_modes);for(const auto& t:bg.camera_timers)timer(out[9],t);v3(out[9],bg.next_position);add(out[9],bg.next_time);v3(out[9],bg.previous_position);add(out[9],bg.previous_time);add(out[9],bg.jumped);add(out[9],bg.tint_color.d3dColor);
    // 10: authored screen-effect state only; renderer shake/output caches excluded.
    add(out[10],g.screen.audit_state_fingerprint());add(out[10],a.screen.audit_state_fingerprint());
    // 11: active Replay playback and recording, including native byte buffers.
    for(i32 i=12;i<=21;++i)add(out[11],runtime->status(i));
    const auto& playback=g.playback;add(out[11],playback.stage_mask());replay_metadata(out[11],playback.metadata());replay_input(out[11],playback.input);replay_stream(out[11],playback.stream);
    const auto& replay_bytes=playback.audit_decoded_payload();byte_vector(out[11],replay_bytes);
    const auto& recording=g.recording;add(out[11],recording.ready());add(out[11],recording.current());replay_metadata(out[11],recording.metadata());replay_input(out[11],recording.input);add(out[11],recording.frame_state.seed);add(out[11],recording.frame_state.flags);
    add(out[11],g.replay_stage_mask);for(i32 i=0;i<9;++i){const auto* stage=recording.stage(i);add(out[11],stage!=nullptr);if(!stage)continue;add(out[11],stage->present);replay_stage(out[11],stage->snapshot);replay_stream(out[11],stage->stream);}
    // 12: successful storage writes and exact payload digests retained by this Runtime.
    for(i32 i=22;i<=24;++i)add(out[12],runtime->status(i));add(out[12],runtime->audit_saved_payloads.size());for(const auto& [name,payload]:runtime->audit_saved_payloads){add(out[12],name.size());for(const auto byte:name)add(out[12],byte);add(out[12],payload.first);add(out[12],payload.second);}
    // 13: synchronous audio operation counters; no asynchronous queue read.
    const auto* audio=th08::audit::audio_snapshot();for(u32 i=0;i<th08::audit::audio_operation_count+2;++i)add(out[13],audio[i]);
    // 14-15: stable scheduler/input state; RAF timestamps and device caches omitted.
    add(out[14],frames);add(out[14],loop_epoch);add(out[14],running);add(out[14],suspended);add(out[14],elapsed);add(out[14],cadence.debt);
    add(out[14],display_cadence.fast);add(out[14],display_cadence.slow);add(out[14],display_cadence.high_refresh);add(out[14],presentation_gate.primed);add(out[14],practice_cadence.debt);add(out[14],practice_cadence.period);add(out[14],practice_period);
    for(u32 i=0;i<256;++i)add(out[15],runtime->keyboard_state()[i]);runtime->audit_controller_state(out[15]);
    u32 touch_digest=2166136261u;touch.audit_state(touch_digest);add(out[15],touch_digest);
    const auto observed_touch=touch_state();add(out[15],observed_touch.context);add(out[15],observed_touch.instance);add(out[15],observed_touch.ready);add(out[15],observed_touch.choice);add(out[15],observed_touch.x);add(out[15],observed_touch.y);add(out[15],observed_touch.fast);add(out[15],observed_touch.slow);add(out[15],observed_touch.min_x);add(out[15],observed_touch.min_y);add(out[15],observed_touch.max_x);add(out[15],observed_touch.max_y);
    const auto& track=runtime->motion;for(const auto& stage:track.stages){add(out[15],stage.size());for(const auto& entry:stage){add(out[15],entry.tick);add(out[15],entry.x);add(out[15],entry.y);}}
    for(const auto& stage:track.touch_stages){add(out[15],stage.size());for(const auto& entry:stage){add(out[15],entry.tick);add(out[15],entry.id);add(out[15],entry.x);add(out[15],entry.y);add(out[15],entry.action);}}
    field_array(out[15],track.ticks);field_array(out[15],track.cursors);field_array(out[15],track.touch_cursors);add(out[15],track.record_points.size());for(const auto& [id,point]:track.record_points){add(out[15],id);add(out[15],point.id);add(out[15],point.x);add(out[15],point.y);add(out[15],point.released);}
    add(out[15],track.playback_points.size());for(const auto& [id,point]:track.playback_points){add(out[15],id);add(out[15],point.id);add(out[15],point.x);add(out[15],point.y);add(out[15],point.released);}
    add(out[15],track.playback_stage);add(out[15],track.playing);add(out[15],track.recording);add(out[15],track.invalid);add(out[15],track.active);add(out[15],track.unlimited);add(out[15],track.cheat_movement_used);add(out[15],track.target_x);add(out[15],track.target_y);
    add(out[15],runtime->touch_trace_callbacks);add(out[15],runtime->touch_sensitivity);const auto& trace=runtime->pending_touch_trace;add(out[15],trace.frame);add(out[15],trace.mode);add(out[15],trace.flags);add(out[15],trace.before_x);add(out[15],trace.before_y);add(out[15],trace.target_x);add(out[15],trace.target_y);add(out[15],trace.after_x);add(out[15],trace.after_y);add(out[15],trace.speed);add(out[15],trace.scale_x);add(out[15],trace.scale_y);add(out[15],trace.sensitivity);add(out[15],trace.seat);add(out[15],trace.execution_kind);
    const auto& title_input=a.title.context.input;add(out[15],title_input.current);add(out[15],title_input.previous);add(out[15],title_input.scrolling);add(out[15],title_input.held);
    // 16: active ASCII/HUD/dialogue/spell presentation state used by Draw.
    const auto& ascii=a.ascii.state;add(out[16],ascii.frame);add(out[16],ascii.string_count);add(out[16],ascii.color);add(out[16],ascii.scale_x);add(out[16],ascii.scale_y);add(out[16],ascii.gui);add(out[16],ascii.selected);add(out[16],ascii.gauge_interrupt);add(out[16],ascii.space_width);add(out[16],ascii.next_score);add(out[16],ascii.next_player);add(out[16],ascii.next_time);add(out[16],ascii.blindness_radius);add(out[16],ascii.blindness_color);vm(out[16],ascii.large_text);vm(out[16],ascii.small_score_text);vm(out[16],ascii.popup_text);vm(out[16],ascii.gauge);vm(out[16],ascii.human_icon);vm(out[16],ascii.youkai_icon);vm(out[16],ascii.cursor);vm(out[16],ascii.percentage);vm(out[16],ascii.border);vm(out[16],ascii.demo);vm(out[16],ascii.blindness);
    for(const auto& v:ascii.boss_markers)vm(out[16],v);field_array(out[16],ascii.boss_states);
    for(i32 i=0;i<std::clamp(ascii.string_count,0,256);++i){const auto& s=ascii.strings[i];add(out[16],s.text);v3(out[16],s.position);add(out[16],s.color);add(out[16],s.scale_x);add(out[16],s.scale_y);add(out[16],s.selected);add(out[16],s.gui);}
    for(u32 i=0;i<std::size(ascii.score_popups);++i){const auto& p=ascii.score_popups[i];if(!p.in_use)continue;add(out[16],i);add(out[16],p.text);v3(out[16],p.position);add(out[16],p.color);timer(out[16],p.timer);add(out[16],p.scale_x);add(out[16],p.scale_y);add(out[16],p.in_use);add(out[16],p.characters);}
    for(u32 i=0;i<std::size(ascii.time_popups);++i){const auto& p=ascii.time_popups[i];if(!p.in_use)continue;add(out[16],i);add(out[16],p.text);v3(out[16],p.position);add(out[16],p.color);timer(out[16],p.timer);add(out[16],p.scale_x);add(out[16],p.scale_y);add(out[16],p.in_use);add(out[16],p.characters);}
    add(out[16],ascii.pause.state);add(out[16],ascii.pause.frames);for(const auto& v:ascii.pause.sprites)vm(out[16],v);vm(out[16],ascii.pause.background);add(out[16],ascii.retry.state);add(out[16],ascii.retry.frames);for(const auto& v:ascii.retry.sprites)vm(out[16],v);vm(out[16],ascii.retry.background);
    const auto& hud=g.hud;add(out[16],hud.frame);add(out[16],hud.flags);add(out[16],hud.boss_opacity);add(out[16],hud.ecl_lives);add(out[16],hud.spell_seconds);add(out[16],hud.previous_spell_seconds);add(out[16],hud.boss_present);add(out[16],hud.boss_life_max);add(out[16],hud.boss_life);field_array(out[16],hud.segment_end);field_array(out[16],hud.segment_start);field_array(out[16],hud.segment_colors);
    for(const auto& v:g.display.front)vm(out[16],v);for(const auto& v:g.display.stage_text)vm(out[16],v);vm(out[16],g.display.clock_intro);vm(out[16],g.display.loading_portrait);vm(out[16],g.display.unknown3a1c);vm(out[16],g.display.arcade);for(const auto& v:g.display.arcade_blur)vm(out[16],v);vm(out[16],g.display.nullify);for(i32 i=0;i<std::clamp(g.display.transition_count,0,168);++i)vm(out[16],g.display.transition[i]);add(out[16],g.display.transition_count);vm(out[16],g.display.stage_rank);vm(out[16],g.display.clock);
    const auto& d=g.display.dialogue;add(out[16],d.message);timer(out[16],d.timer);add(out[16],d.paused_frames);add(out[16],d.minimum_wait);for(const auto& v:d.portraits)vm(out[16],v);for(const auto& v:d.lines)vm(out[16],v);for(const auto& v:d.intro)vm(out[16],v);field_array(out[16],d.colors);field_array(out[16],d.outline_colors);add(out[16],d.font_size);add(out[16],d.ignore_wait);add(out[16],d.skippable);add(out[16],d.color);add(out[16],d.reset_lines);add(out[16],d.next_line);add(out[16],d.portrait);add(out[16],d.textbox_visible);add(out[16],d.selected);
    add(out[16],g.gui_context.difficulty);v3(out[16],g.gui_context.player);add(out[16],g.gui_context.stage_frames);add(out[16],g.gui_context.human_frames);add(out[16],g.gui_context.youkai_frames);add(out[16],g.gui_context.practice_replay);add(out[16],g.gui_context.time_stopped);add(out[16],g.gui_context.paused);add(out[16],g.gui_context.retry);add(out[16],g.gui_context.spell_active);add(out[16],g.gui_context.boss_exists);add(out[16],g.gui_context.input);add(out[16],g.gui_context.familiar_count);add(out[16],g.gui_context.familiar_multiplier);add(out[16],g.gui_context.graphics_options);
    add(out[16],eg.spell_flags);add(out[16],eg.spell_number);add(out[16],eg.spell_time_items);add(out[16],eg.spell_bonus);add(out[16],eg.spell_bonus_decay);add(out[16],eg.spell_pending_bonus);timer(out[16],eg.spell_remaining);timer(out[16],eg.spell_initial);add(out[16],eg.spell_name);add(out[16],eg.spell_comment1);add(out[16],eg.spell_comment2);for(const auto& v:eg.spell_vms)vm(out[16],v);add(out[16],eg.spell_panel_color);add(out[16],eg.spell_capture_bonus);
    return out;
}
#endif
EX("sdl_loop_tick") i32 sdl_loop_tick(BrowserRuntime* r,double seconds,u32){
    if(running||r!=runtime.get())return -1;elapsed+=seconds;int result=tick();if(result||!runtime)return result;
    language_scene();if(!runtime->app.draw(1.0f,false,false))return (runtime->status(2)||runtime->status(4))?2:1;
    if(runtime->status(2)||runtime->status(4))return 2;
    ++frames;return runtime->audio_tick(u32(elapsed*1000))?0:2;
}
EX("sdl_game_close") void sdl_game_close(){sdl_loop_stop();sdl_keys_clear();touch.reset();ThpracUi::shutdown();runtime.reset();if(gamepad)SDL_CloseGamepad(gamepad);gamepad=nullptr;sdl_audio_shutdown();sdl_fonts_shutdown();sdl_detach();}
EX("sdl_key") void sdl_key(const char* code,u32 down){for(auto& key:keyboard_map)if(!std::strcmp(key.code,code)){key.hosted=down!=0;break;}}
EX("sdl_keys_clear") void sdl_keys_clear(){th08_reset_browser_keyboard();SDL_ResetKeyboard();for(auto& key:keyboard_map)key.hosted=false;if(runtime){std::memset(runtime->keyboard_state(),0,256);runtime->app.session.practice.input.reset();runtime->clear_live_gameplay_input();}cancel_touch();touch.reset();}
EX("sdl_touch") void sdl_touch(u32 type,i32 id,float x,float y){
    pointer(type,id,x,y);
}
EX("sdl_touch_cancel") void sdl_touch_cancel(){cancel_touch();}
EX("sdl_thprac_mouse") void sdl_thprac_mouse(u32 type,float x,float y){ThpracUi::mouse(type,x,y);}
EX("sdl_touch_options") void sdl_touch_options(u32 on,u32 free,float speed){touch.enabled=on;touch.unlimited=free;touch.sensitivity=std::clamp(speed,1.f,3.f);if(!on)sdl_touch_cancel();}
EX("sdl_touch_gestures") void sdl_touch_gestures(u32 two,u32 taps){touch.two_finger=two;touch.double_tap=taps;}
EX("sdl_touch_mode") void sdl_touch_mode(u32 mode){if(touch.set_mode(static_cast<int>(mode))&&runtime)runtime->motion.target(0,0,0);}
EX("sdl_touch_trace_v2") const std::uint32_t* sdl_touch_trace_v2(u32 seat,u32 execution_kind){
    return runtime?runtime->touch_trace.snapshot(seat,execution_kind):nullptr;
}
EX("sdl_replay_trace_control_v1") i32 sdl_replay_trace_control_v1(u32 enabled,u32 reset){
    return runtime&&runtime->replay_trace_control(enabled,reset)?1:0;
}
EX("sdl_replay_trace_status_v1") i32 sdl_replay_trace_status_v1(u32 field){
    return runtime?runtime->status(i32(field)):-1;
}
EX("sdl_touch_controls") void sdl_touch_controls(u32 fire,u32 focus,u32 bomb,u32 escape,float x,float y){touch.controls(fire,focus,bomb,escape,x,y);}
EX("sdl_dialogue_touch_manifest_v1") const std::uint32_t* sdl_dialogue_touch_manifest_v1(){return dialogue_touch_build_manifest()->words;}
EX("sdl_dialogue_touch_observation_v1") const std::uint32_t* sdl_dialogue_touch_observation_v1(){dialogue_touch_fill_observation();return dialogue_touch_observation.words;}
EX("sdl_dialogue_touch_request_v1") i32 sdl_dialogue_touch_request_v1(i32 entry){
    if(++dialogue_touch_request_serial==0)++dialogue_touch_request_serial;
    dialogue_touch_request_status=u32(DialogueTouchRequestStatus::InvalidEntry);dialogue_touch_request_entry=entry>=0?u32(entry):0xffffffffu;dialogue_touch_request_timeline=0xffffffffu;
    if(entry<0)return i32(dialogue_touch_request_status);
    if(!runtime){dialogue_touch_request_status=u32(DialogueTouchRequestStatus::RuntimeUnavailable);return i32(dialogue_touch_request_status);}
    DialogueTouchManifestReason reason=DialogueTouchManifestReason::RuntimeNotReady;
    if(!dialogue_touch_fixture_ready(*runtime,reason)){
        dialogue_touch_request_status=reason==DialogueTouchManifestReason::FixtureGateMissing?u32(DialogueTouchRequestStatus::FixtureGateMissing):
            reason==DialogueTouchManifestReason::WriteLockInvalid?u32(DialogueTouchRequestStatus::WriteLockInvalid):u32(DialogueTouchRequestStatus::WrongRun);
        return i32(dialogue_touch_request_status);
    }
    auto& g=runtime->app.game;
    if(g.dialogue.present()){dialogue_touch_request_status=u32(DialogueTouchRequestStatus::DialogueAlreadyPresent);return i32(dialogue_touch_request_status);}
    if(!touch.fire){dialogue_touch_request_status=u32(DialogueTouchRequestStatus::TouchFireNotHeld);return i32(dialogue_touch_request_status);}
    const auto* manifest=dialogue_touch_build_manifest();if(!manifest||!manifest->words[3]){
        dialogue_touch_request_status=u32(DialogueTouchRequestStatus::EntryNotReferenced);return i32(dialogue_touch_request_status);}
    const auto* words=manifest->words;u32 timeline=0xffffffffu;
    for(u32 i=0;i<words[22];++i){const u32* row=words+dialogue_touch_manifest_header_words+i*dialogue_touch_manifest_candidate_words;
        if(row[0]==u32(entry)){timeline=row[1];break;}}
    if(timeline==0xffffffffu){dialogue_touch_request_status=u32(DialogueTouchRequestStatus::EntryNotReferenced);return i32(dialogue_touch_request_status);}
    if(!g.globals.timeline_actions){dialogue_touch_request_status=u32(DialogueTouchRequestStatus::TimelineActionMissing);return i32(dialogue_touch_request_status);}
    dialogue_touch_request_timeline=timeline;
    // This is the real TimelineActions owner used by ECL opcode 6. It dispatches
    // through EnemySimulation::message and GameplayScene::message to Dialogue::read.
    g.globals.timeline_actions->message(entry);
    const auto& d=g.display.dialogue;
    if(g.dialogue.present()&&d.message==entry&&d.instruction){dialogue_touch_request_status=u32(DialogueTouchRequestStatus::Accepted);return i32(dialogue_touch_request_status);}
    dialogue_touch_request_status=u32(DialogueTouchRequestStatus::OpenFailed);return i32(dialogue_touch_request_status);
}
EX("sdl_touch_display") void sdl_touch_display(u32 hitbox){if(runtime)runtime->app.game.always_hitbox=hitbox!=0;}
EX("sdl_game_status") const i32* sdl_game_status(){static i32 out[10]{};if(runtime){out[0]=runtime->status(0);out[1]=runtime->status(3);out[2]=runtime->status(2)||runtime->status(4);out[3]=number(runtime->app.session.numbers.lives).truncate_int();out[4]=runtime->app.session.numbers.power;out[5]=touch.current_context();out[6]=touch.active();out[7]=touch.fire;out[8]=touch.focus;out[9]=frames;}return out;}
}
}
#endif

#include "LocalReplay.inc"
