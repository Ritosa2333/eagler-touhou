#include "LiveMotionObservation.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../platform/Application.hpp"
#include "../game/AnmRenderer.hpp"
#include "../game/HighRefresh.hpp"
#include <cmath>
#include <cstring>
#include <cstdint>
#if defined(__EMSCRIPTEN__) && defined(TH_NATIVE_PLATFORM)
extern "C" void native_graphics_motion_resources(th10::u32* four_words);
extern "C" void native_graphics_motion_texture_stamp(th10::u32 owner,th10::u32 surface,
    th10::multiplayer::live_motion::TextureSourceStamp*) noexcept;
extern "C" th10::u32 native_graphics_motion_texture_copy(
    const th10::multiplayer::live_motion::TextureSourceStamp*,
    const th10::multiplayer::live_motion::Record*,
    th10::multiplayer::live_motion::TexturePacket*) noexcept;
#endif

namespace th10::multiplayer::live_motion {
namespace {
struct Identity {
    const browser::Application* app=nullptr;
    const browser::World* world=nullptr;
    const GameSession* session=nullptr;
    std::uint64_t session_id=0;
    u32 generation=0,seat=Invalid,count=0,stage=0,scope=0;
    bool operator==(const Identity& other)const noexcept {
        return app==other.app&&world==other.world&&session==other.session&&
            session_id==other.session_id&&generation==other.generation&&seat==other.seat&&
            count==other.count&&stage==other.stage&&scope==other.scope;
    }
};
struct Tracker {
    const void* source=nullptr;
    u32 object_id=0,incarnation=0,deaths=0;
    i32 state=0,file=0,script=0,time=0;
};
browser::Application* bound_app=nullptr;
Identity identity{};
Frame active{},pending{},presented{};
TextureSourceStamp active_textures[2]{},pending_textures[2]{};
TexturePacket texture_packet{};
Tracker trackers[Capacity]{};
u32 status[16]{Schema};
u32 next_observer_epoch=0,next_incarnation=0;
u32 renderer_at_callback=0,pending_renderer_frame=0;
u32 selected_bullet=Invalid,selected_bullet_id=0;
i32 object_slot=-1;
bool drawing=false,sealed=false,identity_known=false;

void reset_textures(bool disable)noexcept{
    if(disable)texture_enabled=false;
    for(auto& source:active_textures)source=TextureSourceStamp{};
    for(auto& source:pending_textures)source=TextureSourceStamp{};
    texture_packet.header=TextureHeader{};
}

u32 pointer(const void* value)noexcept{return static_cast<u32>(reinterpret_cast<std::uintptr_t>(value));}
bool finite(const Vec3& value)noexcept{
    return std::isfinite(value.x)&&std::isfinite(value.y)&&std::isfinite(value.z);
}
bool near(const Vec3& a,const Vec3& b)noexcept{
    if(!finite(a)||!finite(b))return false;
    const float x=b.x-a.x,y=b.y-a.y;
    return x*x+y*y<16384.f;
}
u32 next(u32& value)noexcept{if(!++value)++value;return value;}
Identity read_identity(const browser::Application& app)noexcept{
    const auto& runtime=app.state.netplay_runtime;
    Identity out;out.app=&app;out.world=app.world;
    out.session=app.world?app.world->actors.session:nullptr;
    out.session_id=runtime.Config().sessionId;out.generation=runtime.Generation();
    out.seat=app.world?app.world->local_player:Invalid;
    out.count=app.world?app.world->player_count:0;out.stage=u32(app.state.game.stage);
    out.scope=(runtime.Configured()?1u:0u)|(app.state.multiplayer_session.started?2u:0u)|
        (runtime.Playback()?4u:0u)|(runtime.Spectator()?8u:0u)|(runtime.Retired()?16u:0u)|
        (app.multiplayer_replay_scope?32u:0u);
    return out;
}
void forget_owners()noexcept{
    for(auto& tracker:trackers)tracker=Tracker{};
    selected_bullet=Invalid;selected_bullet_id=0;object_slot=-1;capture_active=false;
    status[11]=Invalid;status[12]=0;
}
void invalidate_pending(u32 reason)noexcept{
    if(texture_enabled)for(auto& source:pending_textures)source=TextureSourceStamp{};
    if(!(pending.header.flags&reason)){
        pending.header.flags|=reason;
        pending.header.flags&=~ContentMapped;
        if(pending.header.flags&SurfaceBound)++status[8];
    }
}
void synchronize(browser::Application& app)noexcept{
    const auto fresh=read_identity(app);
    if(!identity_known||!(fresh==identity)){
        identity=fresh;identity_known=true;next(status[3]);forget_owners();
        drawing=sealed=false;active=Frame{};
        if(texture_enabled)reset_textures(false);
        invalidate_pending(StaleCallback);
    }
}
void header(Frame& frame,browser::Application& app)noexcept{
    auto& h=frame.header;const auto& runtime=app.state.netplay_runtime;
    h.observer_epoch=status[2];h.identity_epoch=status[3];h.callback_serial=status[4];
    h.flags=EnabledAtCapture;
    if(identity.world&&identity.session&&(identity.scope&3u)==3u&&!(identity.scope&16u))
        h.flags|=IdentityValid;
    if(identity.scope&12u)h.flags|=ReplayOrSpectator;
    h.session_low=u32(identity.session_id);h.session_high=u32(identity.session_id>>32);
    h.generation=identity.generation;h.local_seat=identity.seat;h.player_count=identity.count;
    h.logical_frame=app.multiplayer_frame_open?app.multiplayer_pending_frame:runtime.LastSimulatedFrame();
    h.confirmed=runtime.ConfirmedThroughAllRemotes();h.rollbacks=app.multiplayer_rollbacks;
    h.world_flags=identity.session?identity.session->session_flags:0;
    if(h.world_flags&0x74u)h.flags|=WorldFrozen;
    h.correction_end=runtime.CorrectionEnd();h.missing_inputs=runtime.MissingRemoteInputs();
    const double scale=runtime.IntervalScale();
    h.interval_scale_milli=std::isfinite(scale)&&scale>=0&&scale<4294967.?u32(scale*1000.):Invalid;
    if(app.multiplayer_frame_open)h.flags|=FrameWasOpen;
    if(app.engine.suppress_rollback_sprite_output||(app.world&&app.world->rollback_resimulating))
        h.flags|=Historical;
}
void appearance(Record& out,const AnmVm& vm)noexcept{
    out.vm_id=vm.id;out.owner_tag=vm.owner_tag;
    out.file_index=i32(vm.file_index);out.script_index=vm.script_index;out.sprite_index=vm.sprite_index;
    out.vm_flags=vm.flags;out.color=vm.color;out.secondary_color=vm.secondary_color;
    out.script_time=vm.script_timer.current;out.position=vm.position;
    out.script_position=vm.script_position;out.child_position=vm.child_position;
    out.rotation=vm.rotation;out.scale=vm.scale;out.uv_offset=vm.uv_offset;
    if(((vm.flags>>22)&15u)>3u)out.rejections|=UnsupportedMode;
}
bool incarnation(Tracker& tracker,Record& out,const void* source,u32 id,u32 deaths,const AnmVm& vm)noexcept{
    const bool changed=tracker.source!=source||tracker.object_id!=id||tracker.deaths!=deaths||
        tracker.state!=out.state||tracker.file!=i32(vm.file_index)||
        tracker.script!=vm.script_index||vm.script_timer.current<tracker.time;
    if(changed)tracker.incarnation=next(next_incarnation);
    tracker.source=source;tracker.object_id=id;tracker.deaths=deaths;tracker.state=out.state;
    tracker.file=i32(vm.file_index);tracker.script=vm.script_index;tracker.time=vm.script_timer.current;
    out.incarnation=tracker.incarnation;if(changed)out.flags|=NewIncarnation;
    return changed;
}
bool enter(u32 slot)noexcept{
    if(!enabled||!drawing||sealed||object_slot>=0||slot>=Capacity||
       !(active.header.flags&IdentityValid)||active.header.flags&Historical){
        ++status[10];return false;
    }
    auto& out=active.objects[slot];
    if(out.flags&OwnerSeen){out.rejections|=DuplicateOwner;++status[10];return false;}
    out=Record{};out.slot=slot;out.flags=OwnerSeen;out.endpoint_frame=active.header.logical_frame;
    object_slot=i32(slot);capture_active=true;return true;
}
void recount(Frame& frame)noexcept{
    auto& h=frame.header;h.object_count=h.submitted_count=h.rejected_count=0;
    for(const auto& out:frame.objects)if(out.flags&OwnerSeen){
        ++h.object_count;if(out.flags&QuadSubmitted)++h.submitted_count;
        if(out.rejections)++h.rejected_count;
    }
}
void completed_frontier(u32 seat,const RemotePlayerPresentation::Pair& pair)noexcept{
    if(seat>=2)return;
    CompletedFrontierExtension extension;
    if(active.objects[3].kind==CompletedFrontierMagic)
        std::memcpy(&extension,&active.objects[3],sizeof(extension));
    auto& out=extension.seats[seat];
    out.epoch=pair.epoch;out.from_frame=pair.from_frame;out.to_frame=pair.to_frame;
    out.from_revision=pair.from_revision;out.to_revision=pair.to_revision;
    out.state=pair.to.state;out.deaths=pair.to.deaths;out.pair_flags=pair.flags;
    out.previous={pair.from.position.x,pair.from.position.y,pair.from.position.z};
    out.current={pair.to.position.x,pair.to.position.y,pair.to.position.z};
    extension.seat_mask|=1u<<seat;
    std::memcpy(&active.objects[3],&extension,sizeof(extension));
}
}

u32 SetEnabled(browser::Application* app,bool value)noexcept{
    if(!value){
        reset_textures(true);
        enabled=capture_active=false;status[1]=0;status[15]=0;
        drawing=sealed=false;object_slot=-1;return 0;
    }
    if(!app||app->stopped||app->error)return 0;
    reset_textures(true);
    enabled=capture_active=false;bound_app=app;identity=Identity{};identity_known=false;
    active=Frame{};pending=Frame{};presented=Frame{};
    for(auto& word:status)word=0;
    status[0]=Schema;status[1]=1;status[2]=next(next_observer_epoch);status[11]=Invalid;
    drawing=sealed=false;renderer_at_callback=pending_renderer_frame=0;forget_owners();
    enabled=true;synchronize(*app);return 1;
}
u32 SetTextureEnabled(browser::Application* app,bool value)noexcept{
    if(!app||app!=bound_app)return 0;
    if(!value){reset_textures(true);return 0;}
    if(!enabled||app->stopped||app->error||status[13])return 0;
    reset_textures(false);texture_enabled=true;return 1;
}
const TexturePacket* ReadTexture(const browser::Application* app,u32 observer_epoch,
    u32 identity_epoch,u32 callback_serial,u32 draw_serial,u32 slot,
    u32 texture_surface,u32 texture_owner)noexcept{
    texture_packet.header=TextureHeader{};auto& h=texture_packet.header;
    h.observer_epoch=observer_epoch;h.identity_epoch=identity_epoch;
    h.callback_serial=callback_serial;h.draw_serial=draw_serial;h.slot=slot;
    h.texture_surface=texture_surface;h.texture_owner=texture_owner;
    const auto fail=[&](TextureResult reason)noexcept{
        h.result=u32(reason);h.rgba_pitch=h.payload_bytes=0;return &texture_packet;
    };
    if(!enabled||!texture_enabled||!app||app!=bound_app||app->stopped||app->error||status[13])
        return fail(TextureResult::Disabled);
    const auto& frame=pending.header;
    constexpr u32 required=EnabledAtCapture|IdentityValid|DrawComplete|SurfaceBound|ContentMapped;
    constexpr u32 forbidden=Historical|Overwritten|Unattributed|StaleCallback|UnmatchedCommit|Committed;
    if(!status[15]||!pending_renderer_frame||frame.phase!=u32(Phase::Pending)||
       (frame.flags&required)!=required||(frame.flags&forbidden))
        return fail(TextureResult::NoPending);
    if(observer_epoch!=status[2]||identity_epoch!=status[3]||callback_serial!=status[4]||
       frame.observer_epoch!=observer_epoch||frame.identity_epoch!=identity_epoch||
       frame.callback_serial!=callback_serial||frame.draw_serial!=draw_serial||
       !(read_identity(*app)==identity))
        return fail(TextureResult::RequestMismatch);
    if(slot>=2)return fail(TextureResult::UnsupportedRecord);
    const auto& record=pending.objects[slot];
    if(record.texture_surface!=texture_surface||record.texture_owner!=texture_owner)
        return fail(TextureResult::RequestMismatch);
    constexpr u32 player_flags=OwnerSeen|QuadSubmitted|ScreenQuad|VisibleAtSubmit;
    if(record.kind!=u32(ObjectKind::Player)||record.slot!=slot||record.seat!=slot||
       record.object_index!=slot||record.quad_count!=1||record.rejections||
       (record.flags&player_flags)!=player_flags)
        return fail(TextureResult::UnsupportedRecord);
    const auto& source=pending_textures[slot];
    h.captured_cpu_version=source.cpu_version;h.source_format=source.format;
    h.source_width=source.width;h.source_height=source.height;h.source_pitch=source.pitch;
    h.min_filter=source.min_filter;h.mag_filter=source.mag_filter;
    h.address_u=source.address_u;h.address_v=source.address_v;h.flags=source.flags;
    if(source.result!=u32(TextureResult::Ok))return fail(TextureResult(source.result));
    if(source.texture_owner!=texture_owner||source.texture_surface!=texture_surface)
        return fail(TextureResult::NoSubmitStamp);
#if defined(__EMSCRIPTEN__) && defined(TH_NATIVE_PLATFORM)
    h.result=native_graphics_motion_texture_copy(&source,&record,&texture_packet);
    if(h.result!=u32(TextureResult::Ok))h.rgba_pitch=h.payload_bytes=0;
    return &texture_packet;
#else
    return fail(TextureResult::UnsupportedHost);
#endif
}
const Frame* Read(const browser::Application* app,u32 phase)noexcept{
    if(!app||app!=bound_app||phase>1)return nullptr;
    return phase?&pending:&presented;
}
const u32* Status()noexcept{return status;}
void BeginCallback(browser::Application* app,u32 renderer_presentations)noexcept{
    if(!enabled||app!=bound_app||!app)return;
    synchronize(*app);next(status[4]);status[13]=0;status[15]=1;
    renderer_at_callback=renderer_presentations;status[14]=renderer_presentations;
    active=Frame{};drawing=sealed=false;object_slot=-1;capture_active=false;
    if(texture_enabled)for(auto& source:active_textures)source=TextureSourceStamp{};
    if(pending.header.phase)invalidate_pending(StaleCallback);
}
void EndCallback(browser::Application* app)noexcept{
    if(app!=bound_app)return;
    status[15]=0;drawing=sealed=false;object_slot=-1;capture_active=false;
}
void Stop(browser::Application* app)noexcept{
    if(app!=bound_app)return;
    reset_textures(true);status[13]=1;EndCallback(app);invalidate_pending(StaleCallback);
}
void Invalidate(browser::Application* app)noexcept{
    if(!enabled||app!=bound_app)return;
    reset_textures(true);
    identity_known=false;identity=Identity{};forget_owners();drawing=sealed=false;active=Frame{};
    invalidate_pending(StaleCallback);
}
void BeginDraw(browser::Application& app,DrawKind kind)noexcept{
    if(!enabled||&app!=bound_app)return;
    synchronize(app);active=Frame{};header(active,app);
    if(texture_enabled)for(auto& source:active_textures)source=TextureSourceStamp{};
    auto& h=active.header;h.phase=u32(Phase::Drawing);h.draw_serial=next(status[5]);h.draw_kind=u32(kind);
    h.alpha=kind==DrawKind::Presentation?high_refresh::alpha:1.f;
    h.world_alpha=kind==DrawKind::Presentation?high_refresh::world_alpha:1.f;
    if(!status[15])h.flags|=Unattributed;
    drawing=status[15]!=0;sealed=false;object_slot=-1;capture_active=false;
    if(selected_bullet!=Invalid&&app.world&&app.world->actors.bullets){
        const auto& bullet=app.world->actors.bullets->pool[selected_bullet];
        if(bullet.state!=1||u32(bullet.id)!=selected_bullet_id){
            selected_bullet=Invalid;selected_bullet_id=0;status[11]=Invalid;status[12]=0;
        }
    }else if(!app.world||!app.world->actors.bullets){
        selected_bullet=Invalid;selected_bullet_id=0;status[11]=Invalid;status[12]=0;
    }
}
void SealDraw(browser::Application& app)noexcept{
    if(!enabled||&app!=bound_app||!drawing||sealed)return;
    if(!(read_identity(app)==identity)){
        active.header.flags|=Unattributed;drawing=false;capture_active=false;return;
    }
    active.header.flags|=DrawComplete;
    if(app.multiplayer_frame_open)active.header.flags|=Unattributed;
    if(app.engine.suppress_rollback_sprite_output||(app.world&&app.world->rollback_resimulating))
        active.header.flags|=Historical;
    active.header.confirmed=app.state.netplay_runtime.ConfirmedThroughAllRemotes();
    active.header.correction_end=app.state.netplay_runtime.CorrectionEnd();
    recount(active);sealed=true;object_slot=-1;capture_active=false;
}
void BindPendingSurface(u32 handle,u32 width,u32 height,u32 renderer_frames)noexcept{
    if(!enabled||!bound_app)return;
    if(status[15])synchronize(*bound_app);
    if(status[15]&&drawing&&sealed&&active.header.callback_serial==status[4]){
        pending=active;
        if(texture_enabled)for(u32 i=0;i<2;++i)pending_textures[i]=active_textures[i];
    }else{
        pending=Frame{};
        if(texture_enabled)for(auto& source:pending_textures)source=TextureSourceStamp{};
        if(status[15])header(pending,*bound_app);
        else{
            pending.header.observer_epoch=status[2];pending.header.identity_epoch=status[3];
            pending.header.callback_serial=status[4];pending.header.flags=EnabledAtCapture;
        }
        pending.header.flags|=Unattributed;
    }
    auto& h=pending.header;h.phase=u32(Phase::Pending);
    h.commit_serial=0;h.surface_handle=handle;h.surface_width=width;h.surface_height=height;
    h.flags|=SurfaceBound;h.flags&=~(Committed|ContentMapped);
    if(handle&&width&&height&&(h.flags&(DrawComplete|IdentityValid))==(DrawComplete|IdentityValid)&&
       !(h.flags&(Historical|Unattributed|StaleCallback|Overwritten))&&status[15])
        h.flags|=ContentMapped;
    for(auto& out:pending.objects)if(out.flags&QuadSubmitted){
        if(out.target_surface!=handle)out.rejections|=DifferentTarget;
        if(!width||!height||out.viewport_x>width||out.viewport_y>height||
           out.viewport_width>width-out.viewport_x||out.viewport_height>height-out.viewport_y)
            out.rejections|=UnsupportedViewport;
    }
    recount(pending);pending_renderer_frame=renderer_frames+1u;++status[7];
    // A second native Present without another complete Draw is not evidence
    // of a new object frame, even if the same back-surface handle is reused.
    drawing=sealed=false;object_slot=-1;capture_active=false;
}
void SurfaceWrite(u32 handle)noexcept{
    if(!enabled||!handle)return;
    if((pending.header.flags&SurfaceBound)&&pending.header.surface_handle==handle)
        invalidate_pending(Overwritten);
}
void Commit(browser::Application* app,u32 renderer_presentations)noexcept{
    if(!enabled||app!=bound_app||!app)return;
    if(renderer_presentations==status[14])return; // Immediate native Present already published.
    const u32 previous=status[14];status[14]=renderer_presentations;
    const bool matched=renderer_presentations-previous==1u&&renderer_presentations-renderer_at_callback>=1u&&
        status[15]&&pending_renderer_frame&&pending.header.phase==u32(Phase::Pending)&&
        pending.header.callback_serial==status[4]&&pending.header.identity_epoch==status[3]&&
        (pending.header.flags&SurfaceBound)&&!(pending.header.flags&Committed);
    if(matched){
        presented=pending;presented.header.phase=u32(Phase::Presented);
        presented.header.flags|=Committed;presented.header.commit_serial=renderer_presentations;
        // pending stays readable until the next callback/write, but cannot be
        // committed again without a real native Present binding another Draw.
        pending.header.flags|=Committed;++status[6];
    }else{
        presented=Frame{};
        presented.header.observer_epoch=status[2];presented.header.identity_epoch=status[3];
        presented.header.callback_serial=status[4];
        presented.header.phase=u32(Phase::Presented);
        presented.header.flags|=Committed|Unattributed|UnmatchedCommit;
        presented.header.commit_serial=renderer_presentations;++status[9];
    }
}
void CommitBound(u32 renderer_presentations)noexcept{Commit(bound_app,renderer_presentations);}

bool BeginPlayer(browser::World& world,const Pilot& pilot,const AnmVm& vm)noexcept{
    if(!enabled||!bound_app||identity.world!=&world||pilot.seat>=2||!pilot.player)return false;
    if(!enter(pilot.seat))return false;
    auto& out=active.objects[pilot.seat];const auto& source=*pilot.player;
    out.kind=u32(ObjectKind::Player);out.object_index=pilot.seat;out.object_id=pilot.seat;
    out.seat=pilot.seat;out.state=source.state;out.previous_state=pilot.presentation.state;
    out.source_pointer=pointer(&source);out.previous_position=pilot.presentation.position;
    out.current_position=source.position;if(pilot.seat==world.local_player)out.flags|=LocalPlayer;
    if(pilot.presentation.valid&&pilot.presentation.state==source.state&&near(out.previous_position,out.current_position))
        out.flags|=EndpointsContinuous;
    if(high_refresh::render_only&&high_refresh::active&&out.flags&EndpointsContinuous)
        out.flags|=InterpolationActive;
    RemotePlayerPresentation::Pair remote;
    if(world.remote_player_presentation_pair(pilot,remote)&&
       remote.to_revision==bound_app->multiplayer_rollbacks){
        // The draw uses these completed-frontier endpoints, while the two
        // logical source positions above remain independently observable.
        out.flags&=~(EndpointsContinuous|InterpolationActive);
        out.flags|=CompletedFrontierEndpoints;
        if(remote.flags&RemotePlayerPresentation::Valid){
            out.flags|=EndpointsContinuous;
            if(high_refresh::render_only&&high_refresh::active)out.flags|=InterpolationActive;
        }
        completed_frontier(pilot.seat,remote);
    }
    incarnation(trackers[pilot.seat],out,&source,pilot.seat,pilot.game.pilot.challenge_deaths,source.animation);
    appearance(out,vm);return true;
}
bool BeginBullet(const EnemyBullet& bullet,const AnmVm& vm)noexcept{
    if(!enabled||!drawing||sealed||!identity.world||!identity.world->actors.bullets||
       active.header.flags&Historical)return false;
    const auto* world=identity.world;const auto* manager=world->actors.bullets;
    const auto address=reinterpret_cast<std::uintptr_t>(&bullet),base=reinterpret_cast<std::uintptr_t>(manager->pool);
    if(address<base||address-base>=sizeof(manager->pool)||(address-base)%sizeof(EnemyBullet))return false;
    const u32 index=u32((address-base)/sizeof(EnemyBullet));
    if(selected_bullet==Invalid){
        // Select an existing visible normal bullet from the real draw list.
        // No scan, spawning, input changes, or game-state writes are performed.
        const auto& p=bullet.motion.position;
        if(bullet.state!=1||(vm.flags&3u)!=3u||!(vm.color>>24)||((vm.flags>>22)&15u)>3u||
           !finite(p)||p.x<-160.f||p.x>160.f||p.y<24.f||p.y>424.f)return false;
        selected_bullet=index;selected_bullet_id=u32(bullet.id);
        status[11]=index;status[12]=selected_bullet_id;
    }
    if(index!=selected_bullet||u32(bullet.id)!=selected_bullet_id)return false;
    if(!enter(2))return false;
    auto& out=active.objects[2];const auto& before=world->bullet_presentation[index];
    out.kind=u32(ObjectKind::Bullet);out.object_index=index;out.object_id=u32(bullet.id);
    out.state=bullet.state;out.previous_state=before.state;out.source_pointer=pointer(&bullet);
    out.previous_position=before.position;out.current_position=bullet.motion.position;
    if(before.active&&before.id==bullet.id&&before.state==bullet.state&&near(out.previous_position,out.current_position))
        out.flags|=EndpointsContinuous;
    if(high_refresh::render_only&&high_refresh::active&&out.flags&EndpointsContinuous)
        out.flags|=InterpolationActive;
    incarnation(trackers[2],out,&bullet,u32(bullet.id),0,bullet.animation);
    appearance(out,vm);return true;
}
void EndObject()noexcept{
    if(object_slot<0)return;
    auto& out=active.objects[u32(object_slot)];
    if(out.kind==u32(ObjectKind::Bullet)&&!(out.flags&QuadSubmitted)){
        // An observed candidate may be culled before submit. Release its
        // observation slot so the next real candidate is not a duplicate.
        // This fixes missing evidence, not any gameplay/culling behavior.
        out=Record{};
        selected_bullet=Invalid;selected_bullet_id=0;status[11]=Invalid;status[12]=0;
    }
    object_slot=-1;capture_active=false;
}
void CaptureQuad(const AnmVm& vm,const AnmVertex* quad,const RenderViewport& viewport)noexcept{
    if(!enabled||!capture_active||!drawing||sealed||object_slot<0||!quad)return;
    auto& out=active.objects[u32(object_slot)];++out.quad_count;
    if(out.quad_count!=1){out.rejections|=MultipleQuads;return;}
    appearance(out,vm);out.flags|=QuadSubmitted|VisibleAtSubmit;
    if(((vm.flags>>22)&15u)<=3u)out.flags|=ScreenQuad;
    out.rejections&=~NoQuad;out.viewport_x=viewport.x;out.viewport_y=viewport.y;
    out.viewport_width=viewport.width;out.viewport_height=viewport.height;
    if(!viewport.width||!viewport.height)out.rejections|=UnsupportedViewport;
    u32 resources[4]{};
#if defined(__EMSCRIPTEN__) && defined(TH_NATIVE_PLATFORM)
    native_graphics_motion_resources(resources);
#endif
    out.target_surface=resources[0];out.texture_surface=resources[1];out.texture_owner=resources[2];
    if(!out.target_surface||!out.texture_surface)out.rejections|=ResourcesUnavailable;
    std::memcpy(out.quad,quad,sizeof(out.quad));
    for(const auto& vertex:out.quad)
        if(!finite(vertex.position)||!std::isfinite(vertex.reciprocal_w)||
           !std::isfinite(vertex.uv.x)||!std::isfinite(vertex.uv.y))
            out.rejections|=NonFiniteGeometry;
    // Keep source metadata outside the 352-word frame and only pay for it when
    // both observers are enabled. A later rejected/multiple quad cannot use it.
    if(texture_enabled&&object_slot<2&&out.kind==u32(ObjectKind::Player)&&
       !out.rejections&&(out.flags&ScreenQuad)){
#if defined(__EMSCRIPTEN__) && defined(TH_NATIVE_PLATFORM)
        native_graphics_motion_texture_stamp(out.texture_owner,out.texture_surface,
            &active_textures[u32(object_slot)]);
#else
        active_textures[u32(object_slot)].result=u32(TextureResult::UnsupportedHost);
#endif
    }
}
}
extern "C" {
__attribute__((export_name("multiplayer_motion_observation_set_enabled")))
th10::u32 multiplayer_motion_observation_set_enabled(th10::browser::Application* app,th10::u32 value){
    return value>1?0:th10::multiplayer::live_motion::SetEnabled(app,value!=0);
}
__attribute__((export_name("multiplayer_motion_observation")))
const th10::multiplayer::live_motion::Frame* multiplayer_motion_observation(const th10::browser::Application* app,th10::u32 phase){
    return th10::multiplayer::live_motion::Read(app,phase);
}
__attribute__((export_name("multiplayer_motion_texture_set_enabled")))
th10::u32 multiplayer_motion_texture_set_enabled(th10::browser::Application* app,th10::u32 value){
    return value>1?0:th10::multiplayer::live_motion::SetTextureEnabled(app,value!=0);
}
__attribute__((export_name("multiplayer_motion_texture_capture")))
const th10::multiplayer::live_motion::TexturePacket* multiplayer_motion_texture_capture(
    const th10::browser::Application* app,th10::u32 observer_epoch,th10::u32 identity_epoch,
    th10::u32 callback_serial,th10::u32 draw_serial,th10::u32 slot,
    th10::u32 texture_surface,th10::u32 texture_owner){
    return th10::multiplayer::live_motion::ReadTexture(app,observer_epoch,identity_epoch,
        callback_serial,draw_serial,slot,texture_surface,texture_owner);
}
__attribute__((export_name("multiplayer_motion_observation_status")))
const th10::u32* multiplayer_motion_observation_status(){return th10::multiplayer::live_motion::Status();}
}
#endif
