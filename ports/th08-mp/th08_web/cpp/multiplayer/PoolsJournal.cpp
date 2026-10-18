#include "PoolsJournal.hpp"
#include "../game/BulletSystem.hpp"
#include <cstddef>
#if defined(__EMSCRIPTEN__) && !defined(__wasm64__)
#include <emscripten/emscripten.h>
// Same batched gather/scatter as TH07. Acquire current heap views after
// arena growth; one JS transition per batch rather than one per Bullet part.
static_assert(sizeof(th08::multiplayer::LiveBulletJournal::CopyRegion)==12);
EM_JS(void,th08_live_bullet_copy_batch,(void* destination,const void* source,const void* regions,std::size_t count),{
    const dst=destination>>>0,src=source>>>0,u8=HEAPU8,u32=HEAPU32;
    let entry=(regions>>>0)/4;
    for(let i=0;i<count;++i,entry+=3){
        const from=src+u32[entry],to=dst+u32[entry+1],bytes=u32[entry+2];
        if(from+bytes>u8.length||to+bytes>u8.length)throw new RangeError('live Bullet copy outside WASM heap');
        u8.copyWithin(to,from,from+bytes);
    }
});
#endif

namespace th08::multiplayer {
namespace {
bool capture_executor(Netplay::RollbackJournal& j,AnmExecutor& e){
    return touch(j,e.timing)&&touch(j,e.executed)&&touch(j,e.invalid);
}
u32 hash_bytes(const void* data,std::size_t size,u32 hash=2166136261u){
    const auto* p=static_cast<const u8*>(data);
    for(std::size_t i=0;i<size;++i){hash^=p[i];hash*=16777619u;}return hash;
}
template<class T>u32 hash_value(const T& value,u32 hash){return hash_bytes(&value,sizeof(value),hash);}
}
bool PoolsJournal::Bind(BulletSystem& b,ItemSystem& i,EffectSystem& e,Rng& random){
    if(bullets||b.creation.rollback_journal||b.creation.live_bullets||b.lasers.rollback_journal||i.pool.rollback_journal||e.rollback_journal)return false;
    Netplay::RollbackJournalConfig config;config.maxFrames=History;
    config.maxBytesPerFrame=16*1024*1024;config.maxBlocksPerFrame=10000;
    config.fastBulkCopy=true;config.coalesceRestore=true;
    if(!bytes.Reset(config))return false;
    LiveBulletJournal::CopyFunction copy=nullptr;
#if defined(__EMSCRIPTEN__) && !defined(__wasm64__)
    copy=th08_live_bullet_copy_batch;
#endif
    if(!live.Reset(b.state.bullets,LiveBulletParts(),History,copy))return false;
    bullets=&b;items=&i;effects=&e;rng=&random;failed=false;
    b.creation.rollback_journal=b.lasers.rollback_journal=i.pool.rollback_journal=e.rollback_journal=&bytes;
    b.creation.live_bullets=use_live?&live:nullptr;
    return true;
}
bool PoolsJournal::BeginFrame(u32 frame,bool extend){
    if(!bullets||Failed()||bytes.IsFrameOpen()||(!extend&&bytes.FrameCount()>=History))return Fail();
    if(!bytes.BeginFrame(frame,extend)||(use_live&&!live.BeginFrame(frame,extend)))return Fail();
    auto& b=*bullets;auto& i=*items;auto& e=*effects;
#ifdef TH_MULTIPLAYER_FIXTURES
    if(audit_bullets&&extend){
        if(bullet_audits.empty())return Fail();
        bullet_audits.back().end=frame;
    }else if(audit_bullets){
        BulletAudit audit{frame,frame,std::vector<u8>(sizeof(b.state.bullets))};
        std::memcpy(audit.bytes.data(),b.state.bullets,audit.bytes.size());
        bullet_audits.push_back(std::move(audit));
    }
#endif
    // Birth/reuse/cold-write hooks preserve the first value for the interval.
    if(extend)return true;
    if(!touch(bytes,*rng))return Fail();
    // Templates are constructed before native frame zero and then read-only.
    // Capture the small linked-layer/cursor/timer tail and only live bullets,
    // plus the sentinel. New/reused slots are captured by their native writers.
    constexpr auto bulletTail=offsetof(BulletManagerState,active_count);
    if(!bytes.Touch(reinterpret_cast<u8*>(&b.state)+bulletTail,sizeof(b.state)-bulletTail))return Fail();
    if(use_live){
        std::array<u32,1537> masks{};
        for(u32 slot=0;slot<masks.size();++slot)if(b.state.bullets[slot].state)masks[slot]=LiveBulletMask(b.state.bullets[slot].state);
        if(!live.CaptureMasks(masks))return Fail();
    }else for(auto& bullet:b.state.bullets)if(bullet.state&&!touch(bytes,bullet))return Fail();
    for(auto& laser:b.state.lasers)if(laser.in_use&&!touch(bytes,laser))return Fail();
    if(!touch(bytes,b.arcade)||!touch(bytes,b.failed)||!touch(bytes,b.ready)||
       !touch(bytes,b.creation.timing)||!touch(bytes,b.creation.failure)||
       !touch(bytes,b.updater.timing)||!touch(bytes,b.updater.player)||
       !touch(bytes,b.updater.paused)||!touch(bytes,b.updater.cancel_item)||
       !capture_executor(bytes,b.updater.animation)||
       !touch(bytes,b.lasers.timing)||!touch(bytes,b.lasers.player)||!touch(bytes,b.lasers.invalid)||
       !capture_executor(bytes,b.lasers.animation))return Fail();

    constexpr auto itemTail=offsetof(ItemPoolState,next_index);
    if(!bytes.Touch(reinterpret_cast<u8*>(i.state.get())+itemTail,sizeof(*i.state)-itemTail))return Fail();
    for(u32 slot=0;slot<=ItemPoolState::capacity;++slot)
        if((i.state->items[slot].active||slot==ItemPoolState::capacity)&&!touch(bytes,i.state->items[slot]))return Fail();
    if(!touch(bytes,i.item_owners)||!touch(bytes,i.gift_recipients)||!touch(bytes,i.failed)||
       !touch(bytes,i.pool.rollback_failed)||!touch(bytes,i.difficulty)||!touch(bytes,i.updater.timing)||
       !touch(bytes,i.updater.players)||!touch(bytes,i.updater.player_count))return Fail();
    for(auto& owner:i.owners){
        if(!touch(bytes,owner.input)||!touch(bytes,owner.reward_input)||!touch(bytes,owner.available))return Fail();
        if(owner.rewards&&!touch(bytes,owner.rewards->failed))return Fail();
    }

    if(!bytes.Touch(&e.state,offsetof(EffectPoolState,objects))||
       !bytes.Touch(&e.state.sentinels,sizeof(e.state)-offsetof(EffectPoolState,sentinels))||
       !touch(bytes,e.environment)||!touch(bytes,e.arcade)||!touch(bytes,e.paused)||
       !touch(bytes,e.invalid)||!touch(bytes,e.quality)||!touch(bytes,e.geometry.arcade)||
       !touch(bytes,e.geometry.invalid)||!touch(bytes,e.replay_flags)||
       !capture_executor(bytes,e.anm))return Fail();
    for(u32 slot=0;slot<effect_pool_layout::object_count;++slot){
        auto& effect=e.state.objects[slot];
        if(effect.active||effect.vertices||slot==effect_pool_layout::dummy_index)
            if(!e.capture_slot(effect))return Fail();
    }
    return true;
}
bool PoolsJournal::EndFrame(){return !Failed()&&bytes.EndFrame()&&(!use_live||live.EndFrame());}
bool PoolsJournal::UndoTo(u32 frame){
    if(!bullets||Failed()||!bytes.UndoTo(frame)||(use_live&&!live.UndoTo(frame)))return false;
#ifdef TH_MULTIPLAYER_FIXTURES
    if(audit_bullets){
        const auto found=std::find_if(bullet_audits.begin(),bullet_audits.end(),[&](const auto& value){return value.frame==frame;});
        if(found==bullet_audits.end()||std::memcmp(found->bytes.data(),bullets->state.bullets,found->bytes.size()))return Fail();
        ++bullet_audit_restores;
    }
    while(!bullet_audits.empty()&&bullet_audits.back().frame>=frame)bullet_audits.pop_back();
#endif
    bullets->presentation_prepared=false;
    bullets->drawing.presentation_marker={};bullets->drawing.snapshot();
    items->pool.presentation_marker={};items->pool.snapshot();
    effects->presentation_marker={};effects->snapshot_presentation();
    return true;
}
void PoolsJournal::DiscardBefore(u32 frame){
    bytes.DiscardBefore(frame);live.DiscardBefore(frame);
#ifdef TH_MULTIPLAYER_FIXTURES
    while(!bullet_audits.empty()&&bullet_audits.front().end<frame)bullet_audits.pop_front();
#endif
}
#ifdef TH_MULTIPLAYER_FIXTURES
bool PoolsJournal::SetLiveBullets(bool enabled){
    if(bytes.IsFrameOpen()||HasHistory()||Failed())return false;
    use_live=enabled;if(bullets)bullets->creation.live_bullets=enabled?&live:nullptr;return true;
}
#endif
void PoolsJournal::Clear(){
    if(bullets){if(bullets->creation.rollback_journal==&bytes)bullets->creation.rollback_journal=nullptr;
        if(bullets->creation.live_bullets==&live)bullets->creation.live_bullets=nullptr;
        if(bullets->lasers.rollback_journal==&bytes)bullets->lasers.rollback_journal=nullptr;}
    if(items&&items->pool.rollback_journal==&bytes)items->pool.rollback_journal=nullptr;
    if(effects&&effects->rollback_journal==&bytes)effects->rollback_journal=nullptr;
    bullets=nullptr;items=nullptr;effects=nullptr;rng=nullptr;bytes.Clear();live.Clear();failed=false;
#ifdef TH_MULTIPLAYER_FIXTURES
    bullet_audits.clear();
#endif
}
u32 PoolsJournal::AuditHash()const{
    if(!bullets)return 0;
    u32 hash=hash_bytes(&bullets->state,sizeof(bullets->state));
    hash=hash_bytes(items->state.get(),sizeof(*items->state),hash);
    hash=hash_value(items->item_owners,hash);hash=hash_value(items->gift_recipients,hash);
    hash=hash_value(effects->state,hash);hash=hash_value(*rng,hash);
    for(const auto& e:effects->state.objects)if(e.vertices)
        hash=hash_bytes(e.vertices,258*sizeof(SpriteVertex),hash);
    return hash;
}
}
