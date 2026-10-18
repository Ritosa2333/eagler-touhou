// Adapted from TH07's live-bullet-snapshot-test: compare ALL bytes, including
// dormant VMs, pointer representations, matrices and padding, after rewind.
#include "../th08_web/cpp/multiplayer/LiveBulletSnapshot.hpp"
#include <cassert>
#include <cstdio>
#include <memory>
using namespace th08;
using namespace th08::multiplayer;
int main(){
    constexpr unsigned Count=1537;
    auto pool=std::make_unique<BulletState[]>(Count);
    auto before=std::make_unique<u8[]>(sizeof(BulletState)*Count);
    const auto parts=LiveBulletParts();
    std::memset(pool.get(),0x5a,sizeof(BulletState)*Count);
    for(unsigned i=0;i<Count;++i)pool[i].state=1+i%5;
    std::memcpy(before.get(),pool.get(),sizeof(BulletState)*Count);
    LiveBulletJournal journal;assert(journal.Reset(pool.get(),parts,8));
    assert(journal.BeginFrame(0));
    std::array<u32,Count> masks{};
    for(unsigned i=0;i<Count;++i)masks[i]=LiveBulletMask(pool[i].state);
    assert(journal.CaptureMasks(masks));
    for(unsigned i=0;i<Count;++i)for(unsigned part=0;part<6;++part)
        if(masks[i]&(1u<<part))std::memset(reinterpret_cast<u8*>(&pool[i])+parts[part].offset,i&255,parts[part].size);
    const auto active=journal.BytesForFrame(0);assert(active<sizeof(BulletState)*Count);
    assert(journal.EndFrame()&&journal.BeginFrame(1,true));
    for(unsigned i=0;i<Count;i+=3){assert(journal.Touch(i,LiveBulletJournal::AllParts));std::memset(&pool[i],0,sizeof(BulletState));}
    assert(journal.EndFrame()&&journal.BeginFrame(2));
    for(unsigned i=0;i<Count;++i){assert(journal.Touch(i,LiveBulletJournal::AllParts));std::memset(&pool[i],0xac,sizeof(BulletState));}
    assert(journal.EndFrame());u32 restored=99;
    assert(journal.UndoTo(1,&restored)&&restored==0);
    assert(std::memcmp(pool.get(),before.get(),sizeof(BulletState)*Count)==0);
    std::printf("PASS TH08 live Bullet complete-byte restore: active=%zu full=%zu\n",active,sizeof(BulletState)*Count);
}
