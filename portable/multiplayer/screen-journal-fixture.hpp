#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error Native screen ownership fixture must not enter production
#endif
#include "../../th08_web/cpp/game/ScreenEffects.hpp"

namespace th08::multiplayer::fixture {
inline const u32* screen_journal_probe(AnmRenderer& renderer){
    static u32 result[10]{};std::fill(result,result+10,0);result[0]=1;
    const auto require=[&](bool condition,u32 step){if(!condition)result[2]=step;return condition;};
    Chain chain;Rng rng{1234,1234,0};ScreenEffects screen(chain,renderer,rng);
    Netplay::RollbackJournal journal;Netplay::RollbackJournalConfig config;
    config.maxFrames=8;config.maxBytesPerFrame=256*1024;config.maxBlocksPerFrame=2048;
    if(!require(journal.Reset(config),1))return result;
    auto* first=screen.create(ScreenEffectType::FadeIn,0,0xffffff,0,0,8);
    auto* second=screen.create(ScreenEffectType::Flash,6,2,0xffffffff,0,10);
    if(!require(first&&second&&screen.active_count()==2,2))return result;
    const auto first_before=*first,second_before=*second;
    const auto root_before=chain.calculation.next,draw_before=chain.drawing.next;
    if(!require(journal.BeginFrame(0)&&screen.capture_rollback(journal),3))return result;
    if(!require(chain.run()>0&&screen.active_count()==1,4))return result;
    const auto after=*second;
    auto* replacement=screen.create(ScreenEffectType::Flash,8,1,0xff00ffff,0,4);
    if(!require(replacement==first&&journal.EndFrame(),5))return result;
    result[3]=u32(journal.BytesForFrame(0));
    if(!require(journal.UndoTo(0)&&screen.active_count()==2&&
        chain.calculation.next==root_before&&chain.drawing.next==draw_before&&
        !std::memcmp(first,&first_before,sizeof(*first))&&!std::memcmp(second,&second_before,sizeof(*second)),6))return result;
    screen.rebase_after_rollback();result[4]=1;
    if(!require(journal.BeginFrame(0)&&screen.capture_rollback(journal)&&chain.run()>0&&
        !std::memcmp(second,&after,sizeof(*second))&&journal.EndFrame(),7))return result;
    result[5]=1;journal.DiscardBefore(1);screen.clear();
    if(!require(journal.BeginFrame(1)&&screen.capture_rollback(journal),8))return result;
    if(!require(screen.create(ScreenEffectType::Flash,5,1,0xffffffff,0,8)&&journal.EndFrame(),9))return result;
    if(!require(journal.UndoTo(1)&&!screen.active_count()&&!chain.calculation.next&&!chain.drawing.next,10))return result;
    screen.rebase_after_rollback();result[6]=1;
    for(u32 i=0;i<ScreenEffects::MaxInstances;++i)
        if(!require(screen.create(ScreenEffectType::Flash,5,1,0xffffffff,0,8)!=nullptr,11))return result;
    if(!require(!screen.create(ScreenEffectType::Flash,5,1,0xffffffff,0,8)&&screen.invalid(),12))return result;
    result[7]=1;screen.clear();
    if(!require(!screen.invalid()&&!screen.active_count()&&!chain.calculation.next&&!chain.drawing.next,13))return result;
    result[8]=1;
    auto* unowned=Chain::create([](void*){return JobResult::Continue;});chain.add(unowned,7);
    if(!require(journal.BeginFrame(2)&&!screen.capture_rollback(journal),14))return result;
    journal.Clear();chain.cut(unowned);result[9]=1;result[1]=1;return result;
}
}
