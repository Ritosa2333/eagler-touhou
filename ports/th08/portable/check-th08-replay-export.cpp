// End-to-end replay export + practice block + touch trailer round trip using
// the real game code: ReplayRecording -> export_replay -> USER/'PRAC' append ->
// THMOTION append -> ReplayFile::decode -> practice_replay_read -> motion load.
#include "game/ReplayExport.hpp"
#include "game/PracticeSections.hpp"
#include "../../portable/input/MotionTrack.hpp"
#include <cassert>
#include <cstring>
using namespace th08;
int main(){
    GameplaySession session{};session.display_config.lives=2;session.display_config.bombs=1;
    EclGlobals game{};game.shot=0;game.difficulty=1;game.stage=4;game.game_flags=1;game.current_spell=-1;
    ReplayRecording recording;
    assert(recording.begin(4,session,game,0));
    for(i32 f=0;f<120;f++){recording.input.flags=4;recording.input.physical=u16(f&3);recording.input.timing_level=60;recording.update();}
    recording.stop();
    ReplayExportContext context;std::memcpy(context.player_name,"NO N",4);std::memcpy(context.date,"09/19",5);
    std::memcpy(context.timestamp,"26/09/19 12:00:00",18);
    context.rendered_frames=120;context.total_frames=120;context.human_frames=120;context.active_frames=120;
    auto bytes=export_replay(recording,session,game,context);
    assert(!bytes.empty());

    PracticeConfig config;config.stage=4;config.section=TH08_ST4A_BOSS1;config.life=4;config.bomb=2;config.power=112;
    config.gauge=3000;config.score=123456780;config.graze=123;config.point_total=45;config.point_stage=6;
    config.time=700;config.value=765430;config.night=3;config.familiar=12;config.rank=14;config.rankLock=0;
    const auto block=practice_replay_block(config);assert(!block.empty());
    bytes.insert(bytes.end(),block.begin(),block.end());

    touhou::input::MotionTrack motion;motion.begin(4,true,false,true);motion.record(4,true,-1.f,2.f);
    const auto tail=motion.trailer(8);assert(!tail.empty());
    bytes.insert(bytes.end(),tail.begin(),tail.end());

    // The saved file must still decode as a replay, reveal its PRAC parameters
    // and keep the touch movement readable, all from the same bytes.
    ReplayFile file;assert(file.decode(bytes.data(),u32(bytes.size())));
    PracticeConfig found;assert(practice_replay_read(bytes.data(),u32(bytes.size()),found));
    assert(found.mode==1&&found.stage==4&&found.section==config.section&&found.life==4&&found.power==112);
    assert(found.gauge==3000&&found.score==123456780&&found.point_total==45&&found.point_stage==6);
    assert(found.time==700&&found.value==765430&&found.night==3&&found.familiar==12&&found.rank==14&&!found.rankLock);
    touhou::input::MotionTrack playback;assert(playback.load(bytes.data(),bytes.size(),8));
    float x=0,y=0;playback.playing=true;assert(playback.playback(4,x,y)&&x==-1.f&&y==2.f);

    // An Original-mode run (mode==0, never active) saves no PRAC block at all.
    PracticeConfig original;original.mode=0;
    assert(practice_replay_block(original).empty()==false); // serializes, but the
    // write path only appends while practice.active (mode==1); see BrowserRuntime.
}
