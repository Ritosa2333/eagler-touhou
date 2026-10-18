#include "thprac/portable/session.hpp"
#include "thprac/portable/ecl_interpreter.hpp"
#include "PurpleRandom.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>
using namespace thprac::portable;
using std::pair;
#include "th06_ecl_helpers.inc"
#include "th06_full_patch.inc"
#include "th06_legacy_patch.inc"
template<class T> T at(const std::vector<std::uint8_t>& b,std::size_t pos) {
    T value; std::memcpy(&value,b.data()+pos,sizeof(T)); return value;
}
int main() {
    Session s; s.active=true; s.mode=1; s.stage=3; s.section=22; s.phase=4;
    s.ResetPurpleMenuWarp(); s.bF=0b101011; s.bX1=-150; s.bY1=70;
    s.delay_st6bs9=250; s.snipeN=45; s.snipeF=90; s.wall_prac_st6=true;
    Session roundtrip;
    assert(DeserializeReplaySession(SerializeReplaySession(s),Game::Th06,roundtrip));
    assert(roundtrip.PurpleJsonFields()==s.PurpleJsonFields());
    assert(roundtrip.bF==43 && roundtrip.bX1==-150 && roundtrip.bY1==70);
    Session old;
    assert(DeserializeReplaySession("{\"version\":\"2.3.0.3\",\"game\":\"th06\",\"section\":22,\"phase\":4}",Game::Th06,old));
    assert(!old.purpleSections && old.PurpleJsonFields().empty());
    std::vector<std::uint8_t> bytes(0x10000,0x55);
    PatchContext legacyCtx;
    ECLHelper legacyPatch(bytes.data(),bytes.size());
    ApplyLegacySection(legacyPatch,legacyCtx,old,22);
    assert(legacyPatch.Valid() && legacyCtx.warpTime==0xd40);
    assert(at<std::int16_t>(bytes,0xcdd8)==0x5555);
    assert(at<std::int32_t>(bytes,0x1324)==0x55555555);
    PatchContext ctx; ECLHelper ecl(bytes.data(),bytes.size());
    ApplySection(ecl,ctx,s,22); assert(ecl.Valid()); assert(ctx.warpTime==0xd40);
    assert(at<std::int16_t>(bytes,0xcdd8)==0);
    assert(at<float>(bytes,0xcddc)==42.0f && at<float>(bytes,0xcde0)==70.0f);
    assert(at<float>(bytes,0xcddc+2*0x1c)==at<float>(std::vector<std::uint8_t>(0x10000,0x55),0));
    assert(at<std::int16_t>(bytes,0xce7c)==-1 && at<std::int32_t>(bytes,0x1324)==99999999);
    for(int section=1;section<=106;++section) for(int phase=0;phase<=5;++phase) {
        std::fill(bytes.begin(),bytes.end(),0x55); PatchContext c;
        ECLHelper patch(bytes.data(),bytes.size()); s.phase=phase;
        ApplySection(patch,c,s,section); assert(patch.Valid());
    }
    s.phase=3; s.delay_st6bs9=250;
    std::fill(bytes.begin(),bytes.end(),0x55); ECLHelper final(bytes.data(),bytes.size());
    ApplySection(final,ctx,s,49); assert(final.Valid());
    assert(at<std::int32_t>(bytes,0x6592)==430);
    assert(at<std::int16_t>(bytes,0x6596)==2);
    assert(at<std::int32_t>(bytes,0x65a2)==0x6756-0x6592);
    std::vector<std::uint8_t> shortBytes(4); ECLHelper invalid(shortBytes.data(),shortBytes.size());
    ApplySection(invalid,ctx,s,49); assert(!invalid.Valid());
    // Explicitly a patch/serialization test, not a retail-game/replay oracle.
    std::cout << "PASS: 636 purple section/phase patches, native book and final-wave bytes, params roundtrip and bounds\n";
}
