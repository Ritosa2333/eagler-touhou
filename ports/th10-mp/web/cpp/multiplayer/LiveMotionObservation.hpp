#pragma once
// Optional, host-only observation of actual MP sprite submissions. This state
// is never part of World, a rollback snapshot, input, or canonical state.
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../game/AnmFile.hpp"
#include <cstddef>

namespace th10 { struct RenderViewport; struct EnemyBullet; }
namespace th10::browser { struct Application; struct World; }
namespace th10::multiplayer { struct Pilot; }
namespace th10::multiplayer::live_motion {
constexpr u32 Schema=1,Capacity=4,HeaderWords=32,RecordWords=80,FrameWords=352;
constexpr u32 Invalid=0xffffffffu;
enum class DrawKind:u32 {None=0,Authored=1,Presentation=2};
enum class Phase:u32 {Empty=0,Drawing=1,Pending=2,Presented=3};
enum class ObjectKind:u32 {None=0,Player=1,Bullet=2};
enum FrameFlag:u32 {
    EnabledAtCapture=1u,IdentityValid=2u,DrawComplete=4u,SurfaceBound=8u,
    ContentMapped=16u,Committed=32u,Historical=64u,Overwritten=128u,
    Unattributed=256u,StaleCallback=512u,UnmatchedCommit=1024u,
    WorldFrozen=2048u,ReplayOrSpectator=4096u,FrameWasOpen=8192u,
};
enum RecordFlag:u32 {
    OwnerSeen=1u,QuadSubmitted=2u,EndpointsContinuous=4u,
    InterpolationActive=8u,ScreenQuad=16u,VisibleAtSubmit=32u,
    LocalPlayer=64u,NewIncarnation=128u,CompletedFrontierEndpoints=256u,
};
enum Rejection:u32 {
    NoQuad=1u,UnsupportedMode=2u,MultipleQuads=4u,ResourcesUnavailable=8u,
    DifferentTarget=16u,NonFiniteGeometry=32u,UnsupportedViewport=64u,
    DuplicateOwner=128u,HistoricalOwner=256u,
};

// All words are little-endian 32-bit. Header words 28/29 are IEEE float32;
// every other header word is uint32 (Invalid is the frame/seat sentinel).
struct Header {
    u32 schema=Schema,frame_words=FrameWords,record_words=RecordWords,capacity=Capacity; // 0..3
    u32 observer_epoch=0,identity_epoch=0,callback_serial=0,draw_serial=0;             // 4..7
    u32 commit_serial=0,phase=0,flags=0,draw_kind=0;                                 // 8..11
    u32 session_low=0,session_high=0,generation=0,local_seat=Invalid;                 // 12..15
    u32 player_count=0,logical_frame=Invalid,confirmed=Invalid,rollbacks=0;           // 16..19
    u32 world_flags=0,correction_end=Invalid,missing_inputs=0,interval_scale_milli=1000; // 20..23
    u32 surface_handle=0,surface_width=0,surface_height=0,object_count=0;             // 24..27
    float alpha=1.f,world_alpha=1.f;                                                // 28..29
    u32 submitted_count=0,rejected_count=0;                                         // 30..31
};

// Stable identity is (observer_epoch, identity_epoch, kind, object_index,
// object_id, incarnation), never source_pointer or a temporary copied VM.
// Slots 0/1 are native seats 0/1; slot 2 follows one naturally drawn normal
// bullet; slot 3 contains the optional RFP1 extension below, never an owner.
// Unsupported/missing objects are not a PASS.
//
// uint32 words: 0..5,7..11,15..17,19..27,79.
// int32 words: 6(state),12(file),13(script),14(sprite),18(script timer),78(previous state).
// object_id word 3 is a uint32 seat for players and the bit pattern of the
// native signed bullet.id for bullets. source_pointer word 27 is diagnostic
// only. Resource words 19/20/21 are GraphicsHost surface/texture/target handles,
// not WebGL objects or registry ids. They are scoped by observer/identity epoch.
// float32 words: 28..49. Quad words 50..77 are four actual AnmVertex values:
// each [x:f32,y:f32,z:f32,reciprocal_w:f32,color:u32,u:f32,v:f32].
// Native packed colors are 0xAARRGGBB; the renderer converts the little-endian
// BGRA attribute bytes in the shader. This observer never recolors the quad.
//
// Geometry is the actual submitted screen quad AFTER offset/half-pixel/cull,
// final vertex color/UV and apply_state, immediately before append(). It is
// not a newly calculated lerp result. Native screen coordinates have x right,
// y down. The GLES screen shader adds .5 to x/y; the final native blit is
// (0,0,surfaceW,surfaceH)->(0,480,640,0). For that exact blit, default-buffer
// bottom-left coordinates are ((x+.5)*640/surfaceW,
// 480-(y+.5)*480/surfaceH). JS must check the observed blit/viewport/size rather
// than assume this mapping for another render path. Model-local geometry is
// deliberately unsupported. Appearance/shape equality across samples must
// still be checked before treating ScreenQuad as a translation window.
struct Record {
    u32 kind=0,slot=0,object_index=0,object_id=0,incarnation=0,seat=Invalid;           // 0..5
    i32 state=0;u32 flags=0,rejections=NoQuad,quad_count=0,vm_id=0,owner_tag=0;         // 6..11
    i32 file_index=0,script_index=0,sprite_index=0;u32 vm_flags=0,color=0,secondary_color=0; // 12..17
    i32 script_time=0;u32 texture_surface=0,texture_owner=0,target_surface=0;          // 18..21
    u32 viewport_x=0,viewport_y=0,viewport_width=0,viewport_height=0;                 // 22..25
    u32 endpoint_frame=Invalid,source_pointer=0;                                    // 26..27
    // Always the ORIGINAL logical previous/current sources. When flag 256 is
    // set, the actual display endpoints are explicitly supplied by RFP1.
    Vec3 previous_position{},current_position{};                                   // 28..33
    Vec3 position{},script_position{},child_position{},rotation{};                  // 34..45
    Vec2 scale{},uv_offset{};                                                       // 46..49
    AnmVertex quad[4]{};                                                           // 50..77
    i32 previous_state=0;u32 part=0;                                               // 78..79: part 0=screen quad
};
// ABI-preserving extension in reserved record 3 (80 words). Main records keep
// their logical endpoint meaning. RFP1 current must be bit-identical to the
// corresponding main logical current; only the display previous is different.
// Words 0..7: magic, version, seatMask, strideWords, capacity, 0, 0, 0.
// Word 7 is Record::flags and MUST stay zero, so no owner/accounting is added.
// Seat 0/1 lives at 8+seat*16:
//   epoch, fromFrame, toFrame, fromRevision, toRevision, state(i32), deaths,
//   pairFlags, previous.xyz(f32), current.xyz(f32), 0, 0.
// Pair flags: Valid=1, Snap=2, Consumed=4. Snap is from==current; Consumed means
// a previous real alpha=1/authored Present consumed that pair (also Valid|Snap).
// The remaining 40 words are zero. With no bridge the entire slot is zero.
constexpr u32 CompletedFrontierMagic=0x52465031u;
struct CompletedFrontierSeat {
    u32 epoch=0,from_frame=0,to_frame=0,from_revision=0,to_revision=0;
    i32 state=0;u32 deaths=0,pair_flags=0;
    Vec3 previous{},current{};
    u32 reserved[2]{};
};
struct CompletedFrontierExtension {
    u32 magic=CompletedFrontierMagic,version=1,seat_mask=0,stride_words=16,capacity=2;
    u32 reserved[3]{};
    CompletedFrontierSeat seats[2]{};
    u32 trailing[40]{};
};
static_assert(sizeof(CompletedFrontierSeat)==64&&sizeof(CompletedFrontierExtension)==sizeof(Record));
static_assert(offsetof(CompletedFrontierExtension,seats)==8*4&&
              offsetof(CompletedFrontierSeat,previous)==8*4);
struct Frame {
    Header header{};Record objects[Capacity]{};
    Frame()noexcept{
        // Record's ordinary defaults contain three sentinels. Slot 3 is an
        // extension payload and must instead start as 80 entirely zero words.
        objects[3].seat=objects[3].rejections=objects[3].endpoint_frame=0;
    }
};
static_assert(sizeof(Header)==128&&sizeof(Record)==320&&sizeof(Frame)==1408);
static_assert(offsetof(Header,alpha)==28*4&&offsetof(Record,quad)==50*4);
static_assert(offsetof(Record,previous_position)==28*4&&offsetof(Frame,objects)==128);

// LMT1 is a separate optional CPU-source packet; Frame/Record stay 352 words.
// It is independent foreground evidence, never a final blend/occlusion oracle.
// No pixel readback, GL query, upload or resource reference is performed.
constexpr u32 TextureMagic=0x4c4d5431u,TextureSchema=1,TextureHeaderWords=32;
constexpr u32 TextureMaxSide=128,TextureCapacityBytes=TextureMaxSide*TextureMaxSide*4;
enum class TextureResult:u32 {
    Ok=1,Disabled=2,NoPending=3,RequestMismatch=4,UnsupportedRecord=5,
    ResourceMissing=6,ResourceBindingMismatch=7,VersionMismatch=8,
    GpuUnavailable=9,GpuWritten=10,UnsupportedFormat=11,UnsupportedSampling=12,
    InvalidUv=13,FootprintOutOfBounds=14,CropTooLarge=15,InvalidStorage=16,
    UnsupportedHost=17,NoSubmitStamp=18,
};
enum TextureFlag:u32 {
    SubmitStamped=1u,ExistingGpuEqualClean=2u,StraightRgba8=4u,
    CpuRowsTopToBottom=8u,FullOneTexelHalo=16u,NoActualWrap=32u,
};
// All header words are uint32. Version fields and sampler fields are Invalid
// until acquired. source_format is the original asset code, not a C++ enum.
// Filters: 0 Nearest, 1 Linear. Addresses: 0 Repeat, 1 Mirror, 2 Clamp.
// Crop is [x,x+width) x [y,y+height) in the ORIGINAL CPU image. Source
// row zero is texture v=0 (top); packet row zero is source crop_y. No Y flip.
// Packet texel (column,row) has center UV ((crop_x+column+.5)/source_width,
// (crop_y+row+.5)/source_height), independently of default-framebuffer axes.
// RGBA8 is straight/unpremultiplied. Low-bit source channels use the renderer's
// 5/6/4-bit expansion; this is not a claim about final framebuffer colors.
struct TextureHeader {
    u32 magic=TextureMagic,version=TextureSchema,header_words=TextureHeaderWords;
    u32 result=u32(TextureResult::Disabled);                                      // 0..3
    u32 observer_epoch=0,identity_epoch=0,callback_serial=0,draw_serial=0;          // 4..7
    u32 slot=Invalid,texture_owner=0,texture_surface=0,captured_cpu_version=Invalid; // 8..11
    u32 current_cpu_version=Invalid,current_gpu_version=Invalid,source_format=0,source_width=0; // 12..15
    u32 source_height=0,source_pitch=0,crop_x=0,crop_y=0;                           // 16..19
    u32 crop_width=0,crop_height=0,rgba_pitch=0,payload_bytes=0;                    // 20..23
    u32 rgba_offset_bytes=TextureHeaderWords*4,max_side=TextureMaxSide;
    u32 min_filter=Invalid,mag_filter=Invalid;                                    // 24..27
    u32 address_u=Invalid,address_v=Invalid,flags=0,reserved=0;                    // 28..31
};
struct TexturePacket {TextureHeader header{};u8 rgba[TextureCapacityBytes]{};};
static_assert(sizeof(TextureHeader)==128&&sizeof(TexturePacket)==65664);
static_assert(offsetof(TexturePacket,rgba)==128&&offsetof(TextureHeader,payload_bytes)==23*4);

// C++-only fixed sidecar captured at the real player CaptureQuad. Never part
// of the raw Frame ABI, a World/Application owner, journal, input or hash.
// The private host epoch prevents handle/version reuse across renderer reset.
struct TextureSourceStamp {
    u32 result=u32(TextureResult::NoSubmitStamp),flags=0,host_epoch=0;
    u32 texture_owner=0,texture_surface=0,cpu_version=Invalid;
    u32 format=0,width=0,height=0,pitch=0;
    u32 min_filter=Invalid,mag_filter=Invalid,address_u=Invalid,address_v=Invalid;
};
// Main observer enable/disable, Stop and Invalidate also turn this option OFF.
// Enabling after a draw does not manufacture a stamp for that old draw.
// Read only inside the current callback's real default-blit hook, before
// Commit. Echo every tuple field and both original record texture ids.
// Every call resets the header. Failure has payload_bytes=rgba_pitch=0;
// unused/stale bytes beyond payload_bytes are never part of the packet.
// The pointer is stable, contents last only until the next getter/reset.
// JS must immediately copy header+payload from a fresh HEAP view, then bind
// that copy to the corresponding subsequently committed raw Frame.
inline bool texture_enabled=false;
u32 SetTextureEnabled(browser::Application*,bool) noexcept;
const TexturePacket* ReadTexture(const browser::Application*,u32 observer_epoch,
    u32 identity_epoch,u32 callback_serial,u32 draw_serial,u32 slot,
    u32 texture_surface,u32 texture_owner) noexcept;

// Status has 16 uint32 words:
// schema, enabled, observerEpoch, identityEpoch, callbackSerial, drawSerial,
// publishedCount, boundCount, surfaceInvalidations, unmatchedCommits,
// scopesRejected, selectedBulletIndex(Invalid when none), selectedBulletId,
// stopped, lastRendererPresentations, callbackOpen.
// Getter phase 0 is presented; phase 1 is pending. Both pointers remain stable.
// pending is readable during the real default blit; consumers must subsequently
// match its draw_serial/callback_serial/surface with the committed presented
// record. Pending commit_serial is zero; presented commit_serial is the actual
// Renderer::stats.presentations counter, not a count of observer getters.
// Disabling/stopping preserves the last presented record.
inline bool enabled=false;
inline bool capture_active=false;
u32 SetEnabled(browser::Application*,bool) noexcept;
const Frame* Read(const browser::Application*,u32 phase) noexcept;
const u32* Status() noexcept;
void BeginCallback(browser::Application*,u32 renderer_presentations) noexcept;
void EndCallback(browser::Application*) noexcept;
void Stop(browser::Application*) noexcept;
void Invalidate(browser::Application*) noexcept;
void BeginDraw(browser::Application&,DrawKind) noexcept;
void SealDraw(browser::Application&) noexcept;
// Bind only at the real native_graphics_present_frame(host.back) call.
void BindPendingSurface(u32 handle,u32 width,u32 height,u32 renderer_frames) noexcept;
// Called on actual target draw/clear/copy writes; old pending cannot survive an
// overwrite merely because a later frame uses the same resource handle.
void SurfaceWrite(u32 handle) noexcept;
void Commit(browser::Application*,u32 renderer_presentations) noexcept;
void CommitBound(u32 renderer_presentations) noexcept;
bool BeginPlayer(browser::World&,const Pilot&,const AnmVm&) noexcept;
bool BeginBullet(const EnemyBullet&,const AnmVm&) noexcept;
void EndObject() noexcept;
void CaptureQuad(const AnmVm&,const AnmVertex*,const RenderViewport&) noexcept;

class ObjectScope {
    bool entered_=false;
public:
    ObjectScope(browser::World& world,const Pilot& pilot,const AnmVm& vm) noexcept {
        if(enabled)entered_=BeginPlayer(world,pilot,vm);
    }
    ObjectScope(const EnemyBullet& bullet,const AnmVm& vm) noexcept {
        if(enabled)entered_=BeginBullet(bullet,vm);
    }
    ~ObjectScope(){if(entered_)EndObject();}
    ObjectScope(const ObjectScope&)=delete;
    ObjectScope& operator=(const ObjectScope&)=delete;
};
}
#endif
