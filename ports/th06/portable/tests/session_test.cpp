#include "thprac/portable/session.hpp"

#include <cassert>

using namespace thprac::portable;

int main()
{
    Session source;
    source.game = Game::Th07;
    source.active = true;
    source.mode = 1;
    source.stage = 7;
    source.section = 1234;
    source.score = 9876543210LL;
    source.cherry = 345670;
    source.rank = 24;
    source.rankLock = true;
    const auto json = SerializeReplaySession(source);
    Session decoded;
    assert(DeserializeReplaySession(json, Game::Th07, decoded));
    assert(decoded.active && decoded.stage == 7 && decoded.section == 1234);
    assert(decoded.score == source.score && decoded.cherry == source.cherry);
    assert(decoded.rank == 24 && decoded.rankLock);
    assert(!DeserializeReplaySession(json, Game::Th06, decoded));

    const std::string legacy =
        "{\"schema\":\"eagler-touhou/thprac-replay/1\",\"game\":\"th06\","
        "\"params\":{\"mode\":1,\"stage\":99,\"life\":99,\"rank\":-5}}";
    assert(DeserializeReplaySession(legacy, Game::Th06, decoded));
    assert(decoded.stage == 6 && decoded.life == 8 && decoded.rank == 0);

    const std::string upstreamTh07 =
        "{\"version\":\"2.3.0.3\",\"game\":\"th07\",\"mode\":1,\"stage\":5,"
        "\"section\":60,\"phase\":2,\"frame\":23456,\"dlg\":true,\"score\":87654320,"
        "\"life\":4,\"bomb\":2,\"power\":112,\"graze\":2300,\"point_total\":460,"
        "\"point_stage\":47,\"cherry\":12000,\"cherryMax\":321000,\"cherryPlus\":8000,"
        "\"spellBonus\":6,\"rank\":28,\"rankLock\":true}";
    assert(DeserializeReplaySession(upstreamTh07, Game::Th07, decoded));
    assert(decoded.active && decoded.stage == 5 && decoded.section == 60 && decoded.phase == 2);
    assert(decoded.frame == 23456 && decoded.dialogue && decoded.score == 87654320);
    assert(decoded.life == 4 && decoded.bomb == 2 && decoded.power == 112 && decoded.graze == 2300);
    assert(decoded.pointTotal == 460 && decoded.pointStage == 47 && decoded.cherry == 12000);
    assert(decoded.cherryMax == 321000 && decoded.cherryPlus == 8000 && decoded.spellBonus == 6);
    assert(decoded.rank == 28 && decoded.rankLock);
    assert(!DeserializeReplaySession(upstreamTh07, Game::Th06, decoded));

    Session stagePractice;
    stagePractice.active = true;
    stagePractice.mode = 0;
    Normalize(stagePractice);
    assert(stagePractice.active);
}
