// Native round-trip checks for the th08 thprac replay parameter block.
// Mirrors upstream thprac ReplaySaveParam/ReplayLoadParam for th08 ('USER' /
// 'PRAC' block holding THPracParam::GetJson()) and the THGuiRep State(1/2/3)
// candidate lifecycle used by the title replay menu.
#include "../th08_web/cpp/game/PracticeConfig.hpp"
#include "../th08_web/cpp/game/PracticeSections.hpp"
#include "../th08_web/cpp/game/PracticeVersion.hpp"
#include "../portable/input/MotionTrack.hpp"
#include <cassert>
#include <cstring>
#include <string>
using namespace th08;
namespace {
PracticeConfig sample(){
    PracticeConfig original;original.stage=5;original.section=TH08_ST5_BOSS1;original.score=9876543210LL;
    original.gauge=-5000;original.time=7200;original.value=123450;original.night=7;original.familiar=250;original.rank=40;original.rankLock=1;
    return original;
}
// Minimal encoded replay: 0x68-byte header, then the plain USER block area.
std::vector<u8> fake_replay(){
    std::vector<u8> bytes(0x68,0);
    std::memcpy(bytes.data(),"T8RP",4);bytes[4]=6;
    const u32 file_size=0x68;std::memcpy(bytes.data()+0xc,&file_size,4);
    // The vanilla information USER block, as export_replay appends it.
    const char info[4]={'i','n','f','o'};const u32 length=12+sizeof(info);
    const u32 at=u32(bytes.size());bytes.resize(at+length,0);
    std::memcpy(bytes.data()+at,"USER",4);std::memcpy(bytes.data()+at+4,&length,4);
    std::memcpy(bytes.data()+at+12,info,sizeof(info));
    return bytes;
}
touhou::input::MotionTrack touched(){
    touhou::input::MotionTrack motion;
    motion.begin(0,true,false,true);
    motion.record(0,true,1.5f,-2.5f);
    return motion;
}
}
int main(){
    const PracticeConfig original=sample();
    assert(original.valid());
    double words[PracticeConfig::word_count]{};original.encode(words);
    PracticeConfig decoded;assert(decoded.decode(words,PracticeConfig::word_count));
    assert(decoded.stage==5&&decoded.section==original.section&&decoded.score==original.score&&decoded.gauge==-5000);
    assert(words[22]==1); // Keep the original bridge's schema slot stable.
    PracticeConfig legacy;assert(legacy.decode(words,23));
    assert(legacy.stage==5&&legacy.sp1_pts==0);
    auto point_words=original;point_words.sp1_pts=1;point_words.bsX=-160;point_words.bsY=128;point_words.encode(words);
    assert(decoded.decode(words,26)&&decoded.sp1_pts==1&&decoded.bsX==-160&&decoded.bsY==128);
    words[25]=129;assert(!decoded.decode(words,26));
    original.encode(words);
    words[3]=19999;assert(!decoded.decode(words,PracticeConfig::word_count));
    constexpr int lw_stages[]{0,1,2,5,6,7,8,5,8,3,4,3,3,3,3,3,3};
    for(int i=0;i<17;++i){auto lw=original;lw.stage=9;lw.section=TH08_LW_1+i;
        assert(lw.valid()&&practice_runtime_stage(lw)==lw_stages[i]);
        assert(practice_last_word_matches(lw,lw_stages[i],205+i));
        assert(!practice_last_word_matches(lw,lw_stages[i]+1,205+i));
        assert(!practice_last_word_matches(lw,lw_stages[i],204+i));
        auto lw_json=practice_replay_json(lw);PracticeConfig copy;
        assert(practice_replay_parse(lw_json.data(),u32(lw_json.size()),copy)&&copy.stage==9&&copy.section==lw.section);
    }
    auto invalid_lw=original;invalid_lw.stage=9;assert(!invalid_lw.valid());invalid_lw.section=11001;assert(!invalid_lw.valid());

    // THPracParam::GetJson() byte layout: compact, optional keys omitted.
    const std::string json=practice_replay_json(original);
    assert(json.find(std::string("{\"version\":\"")+practice_source_version+"\",\"game\":\"th08\",\"mode\":1,\"stage\":5,\"section\":")==0);
    auto points=original;points.stage=1;points.section=TH08_ST2_BOSS3;points.sp1_pts=1;points.bsX=48;points.bsY=96;
    const auto points_json=practice_replay_json(points);PracticeConfig points_copy;
    assert(practice_replay_parse(points_json.data(),u32(points_json.size()),points_copy));
    assert(points_copy.sp1_pts==1&&points_copy.bsX==48&&points_copy.bsY==96);
    points.bsY=129;assert(!points.valid());
    assert(json.find("\"phase\"")==std::string::npos&&json.find("\"frame\"")==std::string::npos&&json.find("\"dlg\"")==std::string::npos);
    assert(json.find("\"score\":9876543210")!=std::string::npos&&json.size()>=16&&json.compare(json.size()-16,16,"\"rankLock\":true}")==0);
    PracticeConfig parsed;assert(practice_replay_parse(json.data(),u32(json.size()),parsed));
    assert(parsed.mode==1&&parsed.stage==5&&parsed.section==original.section&&parsed.score==original.score);
    assert(parsed.gauge==-5000&&parsed.time==7200&&parsed.value==123450&&parsed.night==7&&parsed.familiar==250);
    assert(parsed.rank==40&&parsed.rankLock==1&&parsed.warp==0&&parsed.point==0);
    // THPracParam::ReadJson() Reset()s first: missing keys stay zero, never menu defaults.
    PracticeConfig sparse;const std::string partial="{\"version\":\"2.3.0.3\",\"game\":\"th08\",\"mode\":1,\"stage\":5,\"life\":3,\"bomb\":2,\"power\":100,\"gauge\":0,\"score\":100,\"graze\":0,\"point_total\":0,\"point_stage\":0,\"time\":0,\"value\":60000,\"night\":0,\"familiar\":0,\"rank\":12,\"rankLock\":false}";
    assert(practice_replay_parse(partial.data(),u32(partial.size()),sparse)&&sparse.section==0&&sparse.life==3&&sparse.value==60000);
    assert(sparse.legacy_blue_replay);
    const auto blue_json=practice_replay_json(sparse);assert(blue_json.find("\"version\":\"2.3.0.3\"")!=std::string::npos);
    assert(!parsed.legacy_blue_replay);
    sparse.encode(words);assert(decoded.decode(words,26)&&!decoded.legacy_blue_replay);
    // Wrong game / missing schema / invalid values all degrade to Original.
    PracticeConfig rejected;
    assert(!practice_replay_parse("{\"version\":\"2.3.0.3\",\"game\":\"th07\",\"mode\":1}",35,rejected));
    assert(!practice_replay_parse("{\"game\":\"th08\",\"mode\":1}",25,rejected));
    assert(!practice_replay_parse("not json",8,rejected));
    const std::string bad_rank="{\"version\":\"2.3.0.3\",\"game\":\"th08\",\"mode\":1,\"stage\":5,\"life\":3,\"bomb\":2,\"power\":100,\"gauge\":0,\"score\":0,\"graze\":0,\"point_total\":0,\"point_stage\":0,\"time\":0,\"value\":60000,\"night\":0,\"familiar\":0,\"rank\":2,\"rankLock\":false}";
    assert(!practice_replay_parse(bad_rank.data(),u32(bad_rank.size()),rejected));

    // ReplaySaveParam block layout: 'USER', 4-aligned total size, 'PRAC', payload.
    const auto block=practice_replay_block(original);
    assert(!block.empty()&&(block.size()&3)==0);
    u32 block_size=0;std::memcpy(&block_size,block.data()+4,4);
    assert(!std::memcmp(block.data(),"USER",4)&&block_size==block.size()&&!std::memcmp(block.data()+8,"PRAC",4));
    assert(!std::memcmp(block.data()+12,json.data(),json.size()));

    // ReplayLoadParam walks the plain USER area after the encoded file_size,
    // skips the vanilla information block and finds 'PRAC'.
    auto replay=fake_replay();
    replay.insert(replay.end(),block.begin(),block.end());
    PracticeConfig found;assert(practice_replay_read(replay.data(),u32(replay.size()),found));
    assert(found.stage==5&&found.section==original.section&&found.score==original.score&&found.familiar==250);
    // Touch movement stays last: both extensions coexist and parse independently.
    auto motion=touched();const auto tail=motion.trailer(8);assert(!tail.empty());
    replay.insert(replay.end(),tail.begin(),tail.end());
    assert(practice_replay_read(replay.data(),u32(replay.size()),found)&&found.stage==5);
    touhou::input::MotionTrack playback;assert(playback.load(replay.data(),replay.size(),8));
    float x=0,y=0;playback.playing=true;assert(playback.playback(0,x,y)&&x==1.5f&&y==-2.5f);
    // Vanilla replay (information block only) and corrupt files report "no parameters".
    auto vanilla=fake_replay();PracticeConfig none;
    assert(!practice_replay_read(vanilla.data(),u32(vanilla.size()),none));
    auto corrupt=replay;corrupt[0x68+5]=0xff;assert(!practice_replay_read(corrupt.data(),u32(corrupt.size()),none));
    auto corrupt_json=replay;corrupt_json[0x68+16+12+2]='X';assert(!practice_replay_read(corrupt_json.data(),u32(corrupt_json.size()),none));
    assert(!practice_replay_read(nullptr,0,none));

    // THGuiRep State(1/2/3) ownership lifecycle.
    PracticeState state;state.run=sample();
    practice_replay_menu_reset(state);
    assert(!state.replay_candidate_valid&&state.run.mode==0&&state.run.life==0&&state.run.bomb==0&&state.run.power==0&&state.run.value==0&&state.run.rank==0);
    assert(practice_replay_menu_check(state,replay.data(),u32(replay.size())));
    assert(state.replay_candidate_valid&&state.replay_candidate.stage==5&&state.run.mode==0&&state.run.rank==0);
    practice_replay_menu_activate(state);
    assert(state.run.mode==1&&state.run.stage==5&&state.run.section==original.section);
    // A vanilla replay Reset()s the candidate but keeps mParamStatus sticky;
    // State(3) then restores Original mode, exactly like upstream.
    assert(!practice_replay_menu_check(state,vanilla.data(),u32(vanilla.size())));
    assert(state.replay_candidate_valid&&state.replay_candidate.mode==0&&state.replay_candidate.life==0&&state.replay_candidate.value==0&&state.replay_candidate.rank==0);
    practice_replay_menu_activate(state);
    assert(state.run.mode==0&&state.run.life==0&&state.run.value==0&&state.run.rank==0);
    state.run=sample();
    practice_replay_menu_reset(state);
    assert(!state.replay_candidate_valid&&state.run.mode==0&&state.run.rank==0);
    practice_replay_menu_activate(state);
    assert(state.run.mode==0&&state.run.rank==0);
}
