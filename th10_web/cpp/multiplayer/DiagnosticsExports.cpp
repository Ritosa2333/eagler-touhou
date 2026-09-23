#include "../platform/Application.hpp"
#include <algorithm>

using namespace th10;
extern "C" __attribute__((export_name("multiplayer_memory_status")))
const u32* multiplayer_memory_status(browser::Application* app){
    static u32 words[12]{};std::fill(words,words+12,0);if(!app||!app->world)return words;
    const auto& world=*app->world;const auto& rollback=world.rollback;
    words[0]=1;words[1]=u32(rollback.LastBytes());words[2]=u32(rollback.PeakBytes());
    words[3]=u32(rollback.LiveFrames());words[4]=rollback.SnapshotFrames();
    words[5]=u32(rollback.SnapshotBytes());words[6]=u32(rollback.SnapshotBytes()>>32);
    words[7]=world.replay_memory.count;
    if(world.actors.bullets)for(u32 i=0;i<2000;++i)words[8]+=world.actors.bullets->pool[i].state?1u:0u;
    words[9]=world.actors.lasers?u32(world.actors.lasers->count):0;
    words[10]=app->multiplayer_rollbacks;words[11]=app->multiplayer_resimulated_frames;
    return words;
}
