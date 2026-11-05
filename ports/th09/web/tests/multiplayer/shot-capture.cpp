#include "WorldState.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

using namespace th09;
using namespace th09::multiplayer;
#define CHECK(x) do { if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();} } while(0)

// Use the production WorldState capture/restore implementation. Only match
// construction is bypassed; the oracle below copies every pool byte, including
// dormant VMs, trails, pointers and padding, independently of sparse metadata.
namespace th09::multiplayer {
struct WorldStateShotCaptureTest {
    struct Capture {std::size_t initialPoolBytes=0,otherBytes=0,initialPoolBlocks=0;u32 poolHash=0;};
    static Capture Begin(WorldState& state,PlayerShots& first,PlayerShots& second,bool sparse=true,bool hash=false){
        state.Unbind();
        if(state.journal.IsFrameOpen())CHECK(state.journal.EndFrame());
        state.journal.DiscardBefore(1);state.undo.clear();state.ResetEnemyCapture();
        state.effectCount=state.attackCount=state.bulletRangeCount=state.shotRangeCount=0;
        state.ownedBytes=state.visualFirstWriteBlocks=0;
        state.sparse=sparse;state.hashEnabled=hash;state.hashParts.clear();state.digest={};
        state.good=state.journal.BeginFrame(0);CHECK(state.good);
        state.Shots(first);state.Shots(second);CHECK(state.Good());
        Capture result{state.FirstWriteBytes(),0,state.FirstWriteBlocks(),state.Fingerprint()};
        for(auto* owner:{&first,&second}){
            auto& s=*owner;
            // Same separately owned controls as WorldState::PlayerState.
            state.Fields(s.player_position,s.option_positions,s.target,s.player_scale,s.side,s.beam_time,s.beam,s.areas);
        }
        result.otherBytes=state.FirstWriteBytes()-result.initialPoolBytes;CHECK(state.Good());
        return result;
    }
    static std::size_t PoolBytes(const WorldState& state,const Capture& capture){return state.FirstWriteBytes()-capture.otherBytes;}
    static std::uint64_t Growths(const WorldState& state){return state.journal.ArenaGrowths();}
};
}
using Access=WorldStateShotCaptureTest;

struct Actions final:PlayerShotActions {
    AnmLoadedSprite sprite{};
    bool blocked=false;
    unsigned starts=0,advances=0,draws=0,effects=0;
    Actions(){sprite.widthPx=sprite.heightPx=16;}
    void start_animation(AnmVm& vm,i32 script)override{
        ++starts;vm.Initialize();vm.scriptIndex=i16(script);vm.loadedSprite=&sprite;
        vm.type=1;vm.intVar0=0;vm.pos2={3,7,11};vm.currentInstruction=nullptr;
    }
    bool advance_animation(AnmVm& vm)override{
        ++advances;++vm.intVar0;vm.matrix3.m[3][1]+=.25f;
        vm.currentTimeInScript.tick(FrameTiming{});return vm.intVar0>=11;
    }
    void draw_animation(AnmVm& vm,bool fading)override{
        ++draws;vm.matrix2.m[2][1]=vm.pos.x+vm.pos.y;
        vm.reserved29a[9]=u8(vm.reserved29a[9]+(fading?3:1));
    }
    void play_positioned_sound(i32,float)override{}
    void play_sound(i32,i32)override{}
    void effect(i32,const Vec3&,i32)override{++effects;}
    bool effect_active(i32)override{return blocked;}
    Vec3 effect_position(i32)override{return {12,240,.4f};}
};

struct Fixture {
    Actions actions;
    PlayerShots first{actions},second{actions};
    ShotResource resource;
    Fixture(){
        resource.sets.resize(8);
        for(unsigned i=0;i<8;++i){
            ShotDefinition d;d.frame=0;d.hitbox={16,16};d.angle=-1.5707963705062866f;
            d.speed=1;d.damage=3;d.animation=i16(i);d.creation=ShotCreation(i);
            resource.sets[i].push_back(d);
        }
        resource.sets[0][0].update=ShotUpdate::reimu_homing;
        resource.sets[1][0].update=ShotUpdate::marisa_beam;
        resource.sets[1][0].drawing=ShotDrawing::marisa_beam;
        resource.sets[2][0].update=ShotUpdate::sakuya_turn;
        resource.sets[3][0].update=ShotUpdate::youmu_wave;
        resource.sets[6][0].update=ShotUpdate::medicine_clockwise;
        resource.sets[6][0].drawing=ShotDrawing::layered_trail;
        resource.sets[7][0].update=ShotUpdate::medicine_counterclockwise;
    }
    void Seed(PlayerShots& owner,unsigned live,unsigned salt){
        owner.player_position={float(salt%50)-25,320,.4f};owner.target={20,90,0};
        owner.option_positions.fill(owner.player_position);owner.player_scale={1,1};
        owner.side=i32(salt%2);owner.beam=nullptr;owner.beam_time.reset();
        owner.areas.reset();owner.areas.pool.back()=AttackArea{};owner.areas.active.fill(nullptr);
        for(unsigned i=0;i<owner.shots.size();++i){
            auto& shot=owner.shots[i];
            std::memset(&shot,0xa5,sizeof(shot));AnmVm animation;
            std::memcpy(&shot.animation,&animation,sizeof(animation));
            shot.animation.loadedSprite=&actions.sprite;shot.animation.type=1;
            shot.animation.reserved29a[9]=u8(i+salt);
            shot.position={float(i%9)-4,210+float(i%50),.4f};
            for(unsigned n=0;n<shot.trail.size();++n)shot.trail[n]={float(i+n+salt),float(n+100),.4f};
            shot.hitbox={16,16,1};shot.velocity={0,-1};shot.offset={3,4};shot.speed=1;
            shot.angle=-1.5707963705062866f;shot.time.reset(i32(i%3));shot.damage=3;
            shot.state=(i*37+salt)%PlayerShots::capacity<live?1:0;
            shot.type=shot.reserved_type=shot.option=0;shot.trail_length=15;
            shot.animation_index=0;shot.hit_flash=0;shot.hits=0;
            shot.definition=&resource.sets[0][0];
        }
    }
    void Seed(unsigned firstLive,unsigned secondLive,unsigned salt){
        Seed(first,firstLive,salt);Seed(second,secondLive,salt+17);actions.blocked=false;
    }
};

template<class T>void append(std::vector<u8>& bytes,const T& value){
    static_assert(std::is_trivially_copyable_v<T>);
    const auto* from=reinterpret_cast<const u8*>(&value);bytes.insert(bytes.end(),from,from+sizeof(value));
}
std::vector<u8> complete_bytes(const PlayerShots& s){
    std::vector<u8> bytes;bytes.reserve(sizeof(s.shots)+sizeof(s.areas)+256);
    append(bytes,s.shots);append(bytes,s.player_position);append(bytes,s.option_positions);
    append(bytes,s.target);append(bytes,s.player_scale);append(bytes,s.side);
    append(bytes,s.beam_time);append(bytes,s.beam);append(bytes,s.areas);return bytes;
}
struct DenseBytes {
    std::vector<u8> first,second;
    explicit DenseBytes(const Fixture& fixture):first(complete_bytes(fixture.first)),second(complete_bytes(fixture.second)){}
    void Check(const Fixture& fixture)const{CHECK(first==complete_bytes(fixture.first));CHECK(second==complete_bytes(fixture.second));}
};

void step(Fixture& f,unsigned tick){
    f.actions.blocked=false;
    for(auto* owner:{&f.first,&f.second}){
        auto& s=*owner;s.player_position.x+=float(int(tick%3)-1);
        s.fire(f.resource,tick%8,0);s.update(FrameTiming{},tick%5==4?1:0);
        const Timer protection{0,1,1};i32 direct=0,special=0;u32 token=17;
        s.hit({0,240,0},{240,400,2},protection,direct,&token,special);
        const PlayfieldGeometry geometry{};s.draw(geometry,false);s.draw(geometry,true);
        s.areas.update();
        if(tick%7==6)s.deactivate_all();
    }
}

int main(){
    Fixture fixture;WorldState state;unsigned cases=0;
    std::uint64_t sparseBytes=0,denseBytes=0;
    // Empty, fragmented, nearly full and fully occupied pools use the actual
    // run helper; later first writes may not overlap or replace original runs.
    for(unsigned live:{0u,1u,2u,17u,64u,127u,128u}){
        fixture.Seed(live,128-live,live+7);const DenseBytes before(fixture);
        auto capture=Access::Begin(state,fixture.first,fixture.second);
        CHECK(capture.initialPoolBytes==128*sizeof(PlayerShot));
        for(auto* owner:{&fixture.first,&fixture.second})for(unsigned i=0;i<PlayerShots::capacity;++i){
            owner->before_write(owner->shots[i]);CHECK(state.Good());
            owner->shots[i].animation.reserved29a[9]^=0x5a;
            owner->before_write(owner->shots[i]);owner->shots[i].trail[31].z+=1;
        }
        CHECK(Access::PoolBytes(state,capture)==2*PlayerShots::capacity*sizeof(PlayerShot));
        CHECK(state.Restore());before.Check(fixture);
        CHECK(!fixture.first.checkpoint&&!fixture.second.checkpoint);++cases;
    }
    // Dense and hash modes remain complete independent inventories, with no
    // sparse before-write observer installed and the old hash ordering intact.
    for(bool hash:{false,true}){
        fixture.Seed(0,0,31);const DenseBytes before(fixture);
        StateChecksum expected;expected.Add(fixture.first.shots,fixture.second.shots);
        auto capture=Access::Begin(state,fixture.first,fixture.second,!hash?false:true,hash);
        CHECK(capture.initialPoolBytes==2*sizeof(fixture.first.shots));
        CHECK(capture.initialPoolBlocks==2);
        if(hash)CHECK(capture.poolHash==expected.value);
        CHECK(!fixture.first.checkpoint&&!fixture.second.checkpoint);
        step(fixture,1);CHECK(state.Restore());before.Check(fixture);++cases;
    }
    // Rejected definitions and effect-only definitions cannot allocate a cold
    // snapshot. This also protects the first writable Marisa fields that occur
    // before initialize(), and both draw passes after a newly created shot.
    for(unsigned kind=0;kind<8;++kind){
        fixture.Seed(0,0,50+kind);const DenseBytes before(fixture);
        auto capture=Access::Begin(state,fixture.first,fixture.second);
        CHECK(capture.initialPoolBytes==0);
        fixture.first.fire(fixture.resource,kind,1);CHECK(Access::PoolBytes(state,capture)==0);
        if(kind==5){
            fixture.actions.blocked=true;fixture.first.fire(fixture.resource,kind,0);
            CHECK(Access::PoolBytes(state,capture)==0);fixture.actions.blocked=false;
        }
        fixture.first.fire(fixture.resource,kind,0);
        const std::size_t expected=kind==4?0:sizeof(PlayerShot);
        CHECK(Access::PoolBytes(state,capture)==expected);
        if(kind==1)CHECK(fixture.first.beam==&fixture.first.shots[0]);
        fixture.first.update(FrameTiming{},0);
        fixture.first.draw(PlayfieldGeometry{},false);fixture.first.draw(PlayfieldGeometry{},true);
        fixture.first.deactivate_all();
        // A second creation reuses the same slot inside the same checkpoint.
        fixture.first.fire(fixture.resource,kind,0);
        CHECK(Access::PoolBytes(state,capture)==expected);
        CHECK(state.Restore());before.Check(fixture);++cases;
    }
    // A checkpoint may cover multiple logical ticks. Exact bytes on rewind
    // and on a second unsnapshotted simulation must match, including round
    // deactivation, collision fading, shot type changes and stale cold data.
    for(unsigned span:{1u,2u,8u})for(unsigned round=0;round<16;++round){
        fixture.Seed(round%19,round%13,round*5);const DenseBytes before(fixture);
        auto capture=Access::Begin(state,fixture.first,fixture.second);
        for(unsigned tick=0;tick<span;++tick)step(fixture,tick+round);
        sparseBytes+=Access::PoolBytes(state,capture);
        denseBytes+=2*sizeof(fixture.first.shots);
        const DenseBytes after(fixture);
        CHECK(state.Seal());CHECK(!fixture.first.checkpoint&&!fixture.second.checkpoint);
        CHECK(state.Restore());before.Check(fixture);
        for(unsigned tick=0;tick<span;++tick)step(fixture,tick+round);
        after.Check(fixture);++cases;
    }
    // Exercise real reverse record order at depths 1, 2 and 8, then reuse the
    // same record storage. Each intermediate target is independently checked.
    std::array<std::unique_ptr<WorldState>,8> history;
    for(auto& record:history)record=std::make_unique<WorldState>();
    for(unsigned depth:{1u,2u,8u})for(unsigned round=0;round<8;++round){
        fixture.Seed(round+1,round+3,round*7);std::vector<DenseBytes> targets;
        for(unsigned tick=0;tick<depth;++tick){
            targets.emplace_back(fixture);
            Access::Begin(*history[tick],fixture.first,fixture.second);
            step(fixture,tick+round);CHECK(history[tick]->Seal());
        }
        const DenseBytes after(fixture);
        for(unsigned tick=depth;tick;--tick){
            CHECK(history[tick-1]->Restore());targets[tick-1].Check(fixture);
        }
        for(unsigned tick=0;tick<depth;++tick)step(fixture,tick+round);
        after.Check(fixture);++cases;
    }
    // Full warm-up retains arenas across rewind and subsequent capture.
    fixture.Seed(128,128,81);
    Access::Begin(state,fixture.first,fixture.second);CHECK(state.Restore());
    const auto growths=Access::Growths(state);
    for(unsigned repeat=0;repeat<8;++repeat){
        Access::Begin(state,fixture.first,fixture.second);CHECK(state.Restore());
        CHECK(Access::Growths(state)==growths);++cases;
    }
    // Invalid owners/indices fail closed without reading an out-of-range slot.
    Access::Begin(state,fixture.first,fixture.second);
    state.BeforePlayerShot(fixture.first,PlayerShots::capacity);CHECK(!state.Good());CHECK(!state.Restore());++cases;
    Access::Begin(state,fixture.first,fixture.second);
    PlayerShots foreign(fixture.actions);state.BeforePlayerShot(foreign,0);CHECK(!state.Good());CHECK(!state.Restore());++cases;
    CHECK(sparseBytes<denseBytes);
    std::printf("PlayerShot sparse capture: %u exact-byte/creation/draw/reset/history/reuse cases PASS; sizeof=%zu; representative saved=%llu dense=%llu\n",
        cases,sizeof(PlayerShot),static_cast<unsigned long long>(sparseBytes),static_cast<unsigned long long>(denseBytes));
}
