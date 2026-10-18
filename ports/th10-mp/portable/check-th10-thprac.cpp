// Native round-trip checks for the th10 thprac replay parameter block.
// Mirrors upstream thprac ReplaySaveParam/ReplayLoadParam for th10 ('USER' /
// 'PRAC' block holding THPracParam::GetJson()) and the THGuiRep State(1/2/3)
// candidate lifecycle used by the title replay menu.
#include "../th10_web/cpp/game/PracticeConfig.hpp"
#include "../th10_web/cpp/game/PracticeSections.hpp"
#include "../portable/input/MotionTrack.hpp"
#include <cassert>
#include <cstring>
#include <string>
using namespace th10;
namespace {
PracticeConfig sample(){
    PracticeConfig original;
    original.mode=1;original.stage=4;original.warp=0;original.section=TH10_ST5_BOSS1;original.phase=1;original.frame=0;original.dlg=1;
    original.score=9876543210LL;original.life=9;original.power=100;original.faith=250000;original.faith_bar=130;original.st6_boss9_spd=160;original.real_bullet_sprite=1;
    return original;
}
// Minimal encoded replay: 0x24-byte header inside the 0x68-byte plain area, then
// the vanilla information USER block. th10 stores user_offset at 0x0c.
std::vector<u8> fake_replay(){
    std::vector<u8> bytes(0x68,0);
    const u32 signature=0x72303174u;std::memcpy(bytes.data(),&signature,4);
    bytes[4]=5;
    const u32 user_offset=0x68;std::memcpy(bytes.data()+0xc,&user_offset,4);
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
    assert(decoded.stage==4&&decoded.section==original.section&&decoded.score==original.score&&decoded.faith==250000);
    words[3]=19999;assert(!decoded.decode(words,PracticeConfig::word_count));

    // THPracParam::GetJson() byte layout: compact, optional keys omitted.
    const std::string json=practice_replay_json(original);
    assert(json.find("{\"version\":\"2.3.0.3\",\"game\":\"th10\",\"mode\":1,\"stage\":4,\"section\":")==0);
    assert(json.find("\"frame\"")==std::string::npos);
    assert(json.find("\"faith\":250000")!=std::string::npos&&json.find("\"score\":9876543210")!=std::string::npos&&json.size()>26&&json.compare(json.size()-26,26,"\"real_bullet_sprite\":true}")==0);
    PracticeConfig parsed;assert(practice_replay_parse(json.data(),u32(json.size()),parsed));
    assert(parsed.mode==1&&parsed.stage==4&&parsed.section==original.section&&parsed.score==original.score);
    assert(parsed.phase==1&&parsed.dlg==1&&parsed.life==9&&parsed.power==100&&parsed.faith==250000);
    assert(parsed.faith_bar==130&&parsed.st6_boss9_spd==160&&parsed.real_bullet_sprite==1&&parsed.warp==0&&parsed.frame==0);
    // THPracParam::ReadJson() Reset()s first: missing keys stay zero, never menu
    // defaults. st6_boss9_spd is the documented -1 substitute when absent.
    PracticeConfig sparse;const std::string partial="{\"version\":\"2.3.0.3\",\"game\":\"th10\",\"mode\":1,\"stage\":0,\"life\":3,\"power\":50,\"faith\":60000,\"faith_bar\":100,\"score\":100,\"real_bullet_sprite\":false}";
    assert(practice_replay_parse(partial.data(),u32(partial.size()),sparse)&&sparse.section==0&&sparse.phase==0&&sparse.dlg==0);
    assert(sparse.life==3&&sparse.faith==60000&&sparse.st6_boss9_spd==-1&&sparse.warp==0&&sparse.frame==0);
    // Wrong game / missing schema / invalid values all degrade to Original.
    PracticeConfig rejected;
    const std::string wrong_game="{\"version\":\"2.3.0.3\",\"game\":\"th07\",\"mode\":1}";
    assert(!practice_replay_parse(wrong_game.data(),u32(wrong_game.size()),rejected));
    const std::string no_schema="{\"game\":\"th10\",\"mode\":1}";
    assert(!practice_replay_parse(no_schema.data(),u32(no_schema.size()),rejected));
    assert(!practice_replay_parse("not json",8,rejected));
    const std::string bad_life="{\"version\":\"2.3.0.3\",\"game\":\"th10\",\"mode\":1,\"stage\":0,\"life\":99,\"power\":50,\"faith\":0,\"faith_bar\":0,\"score\":0,\"real_bullet_sprite\":false}";
    assert(!practice_replay_parse(bad_life.data(),u32(bad_life.size()),rejected));

    // ReplaySaveParam block layout: 'USER', 4-aligned total size, 'PRAC', payload.
    const auto block=practice_replay_block(original);
    assert(!block.empty()&&(block.size()&3)==0);
    u32 block_size=0;std::memcpy(&block_size,block.data()+4,4);
    assert(!std::memcmp(block.data(),"USER",4)&&block_size==block.size()&&!std::memcmp(block.data()+8,"PRAC",4));
    assert(!std::memcmp(block.data()+12,json.data(),json.size()));

    // ReplayLoadParam walks the plain USER area after the encoded user_offset,
    // skips the vanilla information block and finds 'PRAC'.
    auto replay=fake_replay();
    replay.insert(replay.end(),block.begin(),block.end());
    PracticeConfig found;assert(practice_replay_read(replay.data(),u32(replay.size()),found));
    assert(found.stage==4&&found.section==original.section&&found.score==original.score&&found.faith==250000);
    // Touch movement stays last: both extensions coexist and parse independently.
    auto motion=touched();const auto tail=motion.trailer(10);assert(!tail.empty());
    replay.insert(replay.end(),tail.begin(),tail.end());
    assert(practice_replay_read(replay.data(),u32(replay.size()),found)&&found.stage==4);
    touhou::input::MotionTrack playback;assert(playback.load(replay.data(),replay.size(),10));
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
    assert(!state.replay_candidate_valid&&state.run.mode==0&&state.run.life==0&&state.run.power==0&&state.run.faith==0&&state.run.st6_boss9_spd==0);
    assert(practice_replay_menu_check(state,replay.data(),u32(replay.size())));
    assert(state.replay_candidate_valid&&state.replay_candidate.stage==4&&state.run.mode==0&&state.run.stage==0);
    practice_replay_menu_activate(state);
    assert(state.run.mode==1&&state.run.stage==4&&state.run.section==original.section&&state.run.faith==250000);
    // A vanilla replay Reset()s the candidate but keeps mParamStatus sticky;
    // State(3) then restores Original mode, exactly like upstream.
    assert(!practice_replay_menu_check(state,vanilla.data(),u32(vanilla.size())));
    assert(state.replay_candidate_valid&&state.replay_candidate.mode==0&&state.replay_candidate.life==0&&state.replay_candidate.faith==0);
    practice_replay_menu_activate(state);
    assert(state.run.mode==0&&state.run.life==0&&state.run.faith==0&&state.run.st6_boss9_spd==0);
    state.run=sample();
    practice_replay_menu_reset(state);
    assert(!state.replay_candidate_valid&&state.run.mode==0);
    practice_replay_menu_activate(state);
    assert(state.run.mode==0&&state.run.stage==0);
}
