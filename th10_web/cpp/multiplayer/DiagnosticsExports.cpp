#include "../platform/Application.hpp"
#include <algorithm>

using namespace th10;
// Byte-level owner descriptors for diagnosing strict same-layout scene-hash
// mismatches. Read-only, bounded, and called outside measured hot loops.
extern "C" __attribute__((export_name("multiplayer_scene_owners")))
const u32* multiplayer_scene_owners(browser::Application* app){
    static u32 words[41]{};std::fill(words,words+41,0u);
    if(!app||!app->world)return words;
    auto& w=*app->world;u32 count=0;
    const auto add=[&](const auto* value){
        words[1+count*2]=u32(reinterpret_cast<std::uintptr_t>(value));
        words[2+count*2]=value?u32(sizeof(*value)):0;++count;
    };
    add(w.actors.session);add(w.actors.spell);add(w.actors.gui);add(w.actors.results);
    add(w.actors.popups);add(w.actors.hints);add(w.actors.effects);
    for(const auto* stage:{w.backgrounds.current,w.backgrounds.previous}){
        add(stage);
        words[1+count*2]=stage?u32(reinterpret_cast<std::uintptr_t>(stage->object_animations)):0;
        words[2+count*2]=stage&&stage->file?u32(stage->file->primitive_count)*sizeof(AnmVm):0;
        ++count;
    }
    words[0]=count;return words;
}
extern "C" __attribute__((export_name("multiplayer_rollback_storage")))
const double* multiplayer_rollback_storage(browser::Application* app){
    static double values[9]{};std::fill(values,values+9,0.0);
    if(!app||!app->world)return values;
    const auto& rollback=app->world->rollback;
    values[0]=1;values[1]=double(rollback.LastBlocks());values[2]=double(rollback.ArenaGrowths());
    values[3]=double(rollback.RestoreCopiedBytes());values[4]=double(rollback.RestoreSkippedBytes());
    values[5]=double(rollback.ElidedFrames());values[6]=double(rollback.SnapshotFrames());
    values[7]=double(rollback.SnapshotBytes());values[8]=rollback.IsFrameOpen()?1:0;
    return values;
}
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
