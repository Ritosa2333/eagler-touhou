// Original result score lists and score.dat construction; MIT reference:
// GensokyoClub/th08. All persisted records are owned C++ values.
#pragma once
#include "GameplaySession.hpp"
#include <array>
namespace th08 {
struct ScoreSaveIdentity {u32 size=840704,checksum=2724749753u;};
class ScoreStore {
    GameplaySession& session;
    std::array<std::array<std::vector<HighScore>,12>,5> scores;
public:
    ScoreHeader header;
    ScoreChapter file_header;
    bool last_name_saved=false;
    u32& previous_clock;
    explicit ScoreStore(GameplaySession& session):session(session),previous_clock(session.total_clock){reset();}
    void reset();
    bool load(const u8* bytes,u32 size,bool load_game_records);
    i32 link(const HighScore&,i32 difficulty,i32 character);
    std::vector<HighScore>& list(i32 difficulty,i32 character){return scores[difficulty][character];}
    const std::vector<HighScore>& list(i32 difficulty,i32 character)const{return scores[difficulty][character];}
    std::vector<u8> save(u32 now,ScoreSaveIdentity identity={});
    void update_time(u32 now);
};
}
