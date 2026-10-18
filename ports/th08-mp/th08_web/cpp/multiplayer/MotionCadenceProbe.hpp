#pragma once
#ifdef TH_MULTIPLAYER_FIXTURES
#include <cstddef>
#include <cstdint>

namespace th08::multiplayer::motion_probe {
// A bounded observer of submitted geometry. Nothing here is game state, and
// only a real SDL commit publishes staging to the browser. BeginDraw must run
// after presentation::begin and before the actual draw chain, including replay
// draws, so an object omitted by the last draw cannot retain an earlier center.
// Present latches the last draw associated with the renderer's pending surface;
// later correction draws whose present is suppressed cannot overwrite it.
inline constexpr std::size_t PlayerCount=3;
inline constexpr std::size_t ObjectCount=PlayerCount+1;
inline constexpr std::size_t HeaderWords=12;
inline constexpr std::size_t ObjectWords=24;
inline constexpr std::size_t WordCount=HeaderWords+ObjectCount*ObjectWords;

enum HeaderField : std::size_t {
    Version,CommitOrdinal,DisplayOrdinal,DrawOrdinal,CalculationEpoch,
    Alpha,WorldAlpha,RenderOnly,InterpolationActive,SelectedSlot,
    SelectedLifeSerial,ObjectStride
};
enum ObjectField : std::size_t {
    Valid,Kind,Slot,LifeSerial,Age,LifeState,ScriptIndex,SpriteIndex,
    AuthoritativeX,AuthoritativeY,PreviousX,PreviousY,PreviousAge,
    PreviousValid,VmX,VmY,CenterX,CenterY,ShakeX,ShakeY,
    QuadCount,DrawAttempts,VertexAlpha,VmAge
};
static_assert(ObjectStride+1==HeaderWords&&VmAge+1==ObjectWords);

namespace detail {
struct Snapshot {double words[WordCount]{};};
struct BulletTrack {
    int slot=-1,age=0;
    std::uint64_t life=0;
    bool age_valid=false;
};
struct State {
    Snapshot staging{},pending{},published{};
    BulletTrack staged_bullet{},pending_bullet{},committed_bullet{};
    std::uint64_t display=0,draw=0,commit=0;
    int owner=-1;
    bool has_draw=false,has_pending=false;
};
inline State state{};

inline double* Object(Snapshot& snapshot,std::size_t index)noexcept {
    return snapshot.words+HeaderWords+index*ObjectWords;
}
inline void Initialize(Snapshot& snapshot)noexcept {
    snapshot={};
    snapshot.words[Version]=1;
    snapshot.words[SelectedSlot]=-1;
    snapshot.words[ObjectStride]=ObjectWords;
    for(std::size_t index=0;index<ObjectCount;++index){
        auto* object=Object(snapshot,index);
        object[Kind]=index<PlayerCount?1:2;
        object[Slot]=index<PlayerCount?double(index):-1;
        object[ScriptIndex]=object[SpriteIndex]=object[PreviousAge]=-1;
    }
}
inline void SyncBullet()noexcept {
    const auto& bullet=state.staged_bullet;
    state.staging.words[SelectedSlot]=bullet.slot;
    state.staging.words[SelectedLifeSerial]=double(bullet.life);
    auto* object=Object(state.staging,PlayerCount);
    object[Slot]=bullet.slot;
    object[LifeSerial]=double(bullet.life);
}
}

inline void Reset()noexcept {
    detail::state={};
    detail::Initialize(detail::state.staging);
    detail::Initialize(detail::state.pending);
    detail::Initialize(detail::state.published);
}
inline void BeginDisplay()noexcept {++detail::state.display;}
inline void BeginDraw(std::uint64_t calculation_epoch,float alpha,float world_alpha,
                      bool active,bool render_only)noexcept {
    auto& state=detail::state;
    detail::Initialize(state.staging);
    state.staged_bullet=state.committed_bullet;
    state.owner=-1;
    state.has_draw=true;
    auto* words=state.staging.words;
    words[DisplayOrdinal]=double(state.display);
    words[DrawOrdinal]=double(++state.draw);
    words[CalculationEpoch]=double(calculation_epoch);
    words[Alpha]=alpha;
    words[WorldAlpha]=world_alpha;
    words[RenderOnly]=render_only;
    words[InterpolationActive]=active;
    detail::SyncBullet();
}

// Call once before walking the bullet draw layers. Checking just the selected
// slot avoids a second pool scan. Keep an active slot even when it is culled.
inline int SelectedBulletSlot()noexcept {return detail::state.staged_bullet.slot;}
inline void ValidateSelectedBullet(bool active,int age)noexcept {
    auto& state=detail::state;
    auto& bullet=state.staged_bullet;
    if(!state.has_draw||bullet.slot<0)return;
    if(!active){
        bullet.slot=-1;
        bullet.age_valid=false;
    }else{
        // Age regression is a conservative continuity boundary: it can be a
        // reused slot or a rollback correction. Do not join motion across it.
        if(bullet.age_valid&&age<bullet.age)++bullet.life;
        bullet.age=age;
        bullet.age_valid=true;
    }
    detail::SyncBullet();
}

class Scope {
    int previous_owner;
public:
    Scope()noexcept:previous_owner(detail::state.owner){detail::state.owner=-1;}
    ~Scope(){detail::state.owner=previous_owner;}
    Scope(const Scope&)=delete;
    Scope& operator=(const Scope&)=delete;
    void Player(int seat,int life_state,int age,float x,float y)noexcept {
        auto& state=detail::state;
        if(!state.has_draw||seat<0||std::size_t(seat)>=PlayerCount)return;
        state.owner=seat;
        auto* object=detail::Object(state.staging,std::size_t(seat));
        object[Age]=age;
        object[LifeState]=life_state;
        object[AuthoritativeX]=x;
        object[AuthoritativeY]=y;
        ++object[DrawAttempts];
        // PlayerSimulation owns its exact previous interpolation endpoint.
        // Leave PreviousValid false rather than substitute a prior commit.
    }
    bool Bullet(int slot,int life_state,int age,float x,float y)noexcept {
        auto& state=detail::state;
        auto& bullet=state.staged_bullet;
        if(!state.has_draw||life_state==0||slot<0||slot>=1537||
           (bullet.slot>=0&&bullet.slot!=slot))return false;
        if(bullet.slot<0){
            bullet.slot=slot;
            ++bullet.life;
        }
        bullet.age=age;
        bullet.age_valid=true;
        detail::SyncBullet();
        state.owner=int(PlayerCount);
        auto* object=detail::Object(state.staging,PlayerCount);
        object[Age]=age;
        object[LifeState]=life_state;
        object[AuthoritativeX]=x;
        object[AuthoritativeY]=y;
        ++object[DrawAttempts];
        return true;
    }
    void Previous(float x,float y,int age,bool valid)noexcept {
        auto& state=detail::state;
        if(state.owner!=int(PlayerCount))return;
        auto* object=detail::Object(state.staging,PlayerCount);
        object[PreviousX]=x;
        object[PreviousY]=y;
        object[PreviousAge]=age;
        object[PreviousValid]=valid;
    }
};

// Called after shake, raster offsets, visibility checks and final vertex color,
// immediately before add_quad. The center is in the renderer's 640x480 screen
// coordinates. It proves submitted geometry, not later GPU occlusion/scanout.
template<class Vm,class Vertex,class Offset>
inline void CaptureQuad(const Vm& vm,const Vertex (&quad)[4],const Offset& shake)noexcept {
    auto& state=detail::state;
    if(!state.has_draw||state.owner<0||std::size_t(state.owner)>=ObjectCount)return;
    auto* object=detail::Object(state.staging,std::size_t(state.owner));
    object[Valid]=1;
    object[ScriptIndex]=vm.scriptIndex;
    object[SpriteIndex]=vm.activeSpriteIndex;
    object[VmX]=vm.pos.x;
    object[VmY]=vm.pos.y;
    object[CenterX]=(double(quad[0].pos.x)+quad[1].pos.x+quad[2].pos.x+quad[3].pos.x)*.25;
    object[CenterY]=(double(quad[0].pos.y)+quad[1].pos.y+quad[2].pos.y+quad[3].pos.y)*.25;
    object[ShakeX]=shake.x;
    object[ShakeY]=shake.y;
    object[VertexAlpha]=(quad[0].color>>24)&255u;
    object[VmAge]=vm.currentTimeInScript.current;
    ++object[QuadCount];
}

inline void Present()noexcept {
    auto& state=detail::state;
    if(state.has_draw){
        state.pending=state.staging;
        state.pending_bullet=state.staged_bullet;
    }else{
        state.pending=state.published;
        state.pending_bullet=state.committed_bullet;
    }
    state.has_pending=true;
}
inline void Commit()noexcept {
    auto& state=detail::state;
    if(state.has_pending){
        state.published=state.pending;
        state.committed_bullet=state.pending_bullet;
        state.has_pending=false;
    }
    state.published.words[Version]=1;
    state.published.words[ObjectStride]=ObjectWords;
    state.published.words[CommitOrdinal]=double(++state.commit);
    state.published.words[DisplayOrdinal]=double(state.display);
}

// Layout: 12 header doubles followed by seats 0,1,2 and one tracked bullet,
// each 24 doubles as indexed above. Valid means a final quad was submitted by
// that draw; DrawAttempts can be nonzero with Valid=0 (hidden/culled geometry).
// Authoritative/Previous positions are world coordinates; Vm/Center are screen
// coordinates. Bullet life is a diagnostic continuity serial, not a game ID.
// Consumers must split runs on reset/generation, life/state/script changes or
// Valid=0. Same commit ordinal means no new real presentation was observed.
inline const double* Data()noexcept {return detail::state.published.words;}
}
#endif
