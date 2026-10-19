#pragma once
#ifndef TH_MULTIPLAYER_FIXTURES
#error Native screen ownership fixture must not enter production
#endif
#include "../../web/cpp/game/ScreenEffects.hpp"
#include <cstring>

namespace th08::multiplayer::fixture {
// Two fixed owners share both native callback lists in the real application.
// Keep the oracle in public state and raw node images: copying ChainElement
// objects would run their deletion callbacks when the copies are destroyed.
inline bool screen_journal_pair_probe(AnmRenderer& renderer,u32& failed_step){
    const auto require=[&](bool condition,u32 step){if(!condition)failed_step=step;return condition;};
    Chain chain;ChainElement external_calc,external_draw;Rng rng{4321,4321,0};
    ScreenEffects first(chain,renderer,rng),second(chain,renderer,rng);
    Netplay::RollbackJournal journal;Netplay::RollbackJournalConfig config;
    config.maxFrames=8;config.maxBytesPerFrame=256*1024;config.maxBlocksPerFrame=64;
    config.fastBulkCopy=true;config.coalesceRestore=true;
    if(!require(journal.Reset(config),15))return false;
    auto* a=first.create(ScreenEffectType::Flash,100,3,0xffffffff,0,2);
    auto* b=second.create(ScreenEffectType::Flash,100,4,0xffabcdef,0,4);
    external_calc.set_callback([](void*){return JobResult::Continue;});
    external_draw.set_callback([](void*){return JobResult::Continue;});
    chain.add(&external_calc,3);chain.add(&external_draw,6,true);
    auto* c=first.create(ScreenEffectType::Flash,100,5,0xff123456,0,8);
    auto* d=second.create(ScreenEffectType::Flash,100,6,0xfffedcba,0,10);
    if(!require(a&&b&&c&&d&&first.active_count()==2&&second.active_count()==2,16))return false;
    if(!require(chain.calculation.next==a->calculation&&a->calculation->next==b->calculation&&
       b->calculation->next==&external_calc&&external_calc.next==c->calculation&&c->calculation->next==d->calculation&&
       chain.drawing.next==a->drawing&&a->drawing->next==b->drawing&&b->drawing->next==&external_draw&&
       external_draw.next==c->drawing&&c->drawing->next==d->drawing,17))return false;
    first.context.transition_state=7;second.context.retry=true;
    const std::array<ScreenEffectState*,4> states{a,b,c,d};
    const std::array<ChainElement*,12> nodes{
        &chain.calculation,&chain.drawing,&external_calc,&external_draw,
        a->calculation,a->drawing,b->calculation,b->drawing,c->calculation,c->drawing,d->calculation,d->drawing};
    std::array<std::array<u8,sizeof(ScreenEffectState)>,4> state_images{};
    std::array<std::array<u8,sizeof(ChainElement)>,12> node_images{};
    std::array<u8,sizeof(ScreenEffectContext)> first_context{},second_context{};
    for(size_t i=0;i<states.size();++i)std::memcpy(state_images[i].data(),states[i],sizeof(ScreenEffectState));
    for(size_t i=0;i<nodes.size();++i)std::memcpy(node_images[i].data(),nodes[i],sizeof(ChainElement));
    std::memcpy(first_context.data(),&first.context,sizeof(first.context));
    std::memcpy(second_context.data(),&second.context,sizeof(second.context));
    const auto original_restored=[&](){
        if(first.active_count()!=2||second.active_count()!=2||first.invalid()||second.invalid()||
           std::memcmp(first_context.data(),&first.context,sizeof(first.context))||
           std::memcmp(second_context.data(),&second.context,sizeof(second.context)))return false;
        for(size_t i=0;i<states.size();++i)if(std::memcmp(state_images[i].data(),states[i],sizeof(ScreenEffectState)))return false;
        for(size_t i=0;i<nodes.size();++i)if(std::memcmp(node_images[i].data(),nodes[i],sizeof(ChainElement)))return false;
        return true;
    };
    if(!require(journal.BeginFrame(0)&&first.capture_rollback(journal,&second),18))return false;
    // Reuse slots from opposite ends of the two interleaved lists, and move
    // external nodes. Undo must restore every affected neighbor, not just roots.
    first.remove(a);second.remove(d);
    auto* reused_a=first.create(ScreenEffectType::FadeIn,40,0x112233,0,0,12);
    auto* reused_d=second.create(ScreenEffectType::FadeOut,50,0x334455,0,0,1);
    b->alpha=71;c->unused=0xabcdef12;first.context.paused=true;second.context.transition_state=11;
    chain.cut(&external_calc);chain.add(&external_calc,2);external_calc.argument=&second;
    chain.cut(&external_draw);chain.add(&external_draw,13,true);external_draw.argument=&first;
    if(!require(reused_a==a&&reused_d==d&&journal.EndFrame(),19))return false;
    // Reverse entry/peer order after reuse. Extending frame zero must keep
    // its original whole-array before-image rather than replace it with reuse.
    if(!require(journal.BeginFrame(1,true)&&second.capture_rollback(journal,&first),20))return false;
    first.remove(c);second.remove(b);
    auto* reused_c=first.create(ScreenEffectType::Flash,60,2,0xff102030,0,3);
    auto* reused_b=second.create(ScreenEffectType::Flash,70,2,0xff405060,0,9);
    a->phase=3;d->timer.set(27);first.context.transition_state=15;second.context.retry=false;
    chain.cut(&external_calc);chain.add(&external_calc,5);
    chain.cut(&external_draw);chain.add(&external_draw,0,true);
    if(!require(reused_c==c&&reused_b==b&&journal.EndFrame(),21))return false;
    // The next checkpoint has overlapping whole arrays, so coalesced Undo
    // must skip newer bulk copies and still recover frame zero's full graph.
    if(!require(journal.BeginFrame(2)&&first.capture_rollback(journal,&second),22))return false;
    auto* born=first.create(ScreenEffectType::Flash,80,2,0xff778899,0,7);
    second.remove(d);a->alpha=19;b->unused=0x12345678;c->timer.set(53);
    external_calc.argument=born;external_draw.priority=14;second.context.paused=true;
    if(!require(born&&first.active_count()==3&&second.active_count()==1&&journal.EndFrame(),23))return false;
    const auto skipped=journal.RestoreSkippedBytes();u32 restored=~u32(0);
    if(!require(journal.UndoTo(1,&restored)&&restored==0&&journal.FrameCount()==0&&
       journal.RestoreSkippedBytes()>skipped&&original_restored(),24))return false;
    first.rebase_after_rollback();second.rebase_after_rollback();
    // Execute restored callbacks through both owners, then recover once more.
    // This checks callback arguments/owner lifetime, not merely equal pointers.
    if(!require(journal.BeginFrame(0)&&first.capture_rollback(journal,&second)&&chain.run()>0&&
       first.active_count()==2&&second.active_count()==2&&journal.EndFrame()&&
       journal.UndoTo(0)&&original_restored(),25))return false;
    first.rebase_after_rollback();second.rebase_after_rollback();
    Chain foreign_chain;ScreenEffects foreign(foreign_chain,renderer,rng);
    if(!require(journal.Reset(config)&&journal.BeginFrame(10),26))return false;
    const bool different_chain_rejected=!first.capture_rollback(journal,&foreign)&&
        !journal.Failed()&&journal.BlocksForFrame(10)==0;
    journal.Clear();
    if(!require(different_chain_rejected,27))return false;
    // A covered pool node still has to pass the heap-ownership guard.
    // Restore flags before any failure return, avoiding a delete of pool memory.
    if(!require(journal.Reset(config)&&journal.BeginFrame(11),28))return false;
    const auto saved_flags=a->calculation->flags;a->calculation->flags|=1;
    const bool covered_flag_rejected=!first.capture_rollback(journal,&second);
    a->calculation->flags=saved_flags;journal.Clear();
    if(!require(covered_flag_rejected,29))return false;
    // A peer's covered node must also count toward the walk's cycle bound.
    if(!require(journal.Reset(config)&&journal.BeginFrame(12),30))return false;
    auto* cycle_node=d->drawing;auto* saved_next=cycle_node->next;cycle_node->next=cycle_node;
    const bool covered_cycle_rejected=!first.capture_rollback(journal,&second);
    cycle_node->next=saved_next;journal.Clear();
    if(!require(covered_cycle_rejected&&original_restored(),31))return false;
    first.clear();second.clear();chain.cut(&external_calc);chain.cut(&external_draw);
    return require(!chain.calculation.next&&!chain.drawing.next,32);
}

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
    journal.Clear();chain.cut(unowned);
    if(!screen_journal_pair_probe(renderer,result[2]))return result;
    result[9]=1;result[1]=1;return result;
}
}
