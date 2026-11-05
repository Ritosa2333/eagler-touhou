// TH06Save's native data layout and counters (purple THPrac, MIT).
// File/clock adapters live at the title boundary; replay is never counted.
#pragma once
#include <cstdint>
#include <limits>
namespace PracticeRuntime::Purple {
struct PurpleSave {
    enum TH06SpellState {Capture=0,Attempt=1,Timeout=2};
    struct Save {
        std::int32_t SC_History[65][5][4][3]{};
        std::int64_t timePlayer[5][4]{};
    } current,total;
    static constexpr int spellid_books=64;
    static bool Valid(int spell,int diff,int player,int state) {
        return spell>=0&&spell<65&&diff>=0&&diff<5&&player>=0&&player<4&&state>=0&&state<3;
    }
    bool Add(int spell,int diff,int player,int state) {
        if(!Valid(spell,diff,player,state))return false;
        auto& a=total.SC_History[spell][diff][player][state];
        auto& b=current.SC_History[spell][diff][player][state];
        if(a<std::numeric_limits<std::int32_t>::max())++a;
        if(b<std::numeric_limits<std::int32_t>::max())++b;
        return true;
    }
    int GetTotalSpellCardCount(int s,int d,int p,TH06SpellState k) const {return Valid(s,d,p,k)?total.SC_History[s][d][p][k]:0;}
    int GetCurrentSpellCardCount(int s,int d,int p,TH06SpellState k) const {return Valid(s,d,p,k)?current.SC_History[s][d][p][k]:0;}
    std::int64_t GetTotalGameTime(int d,int p) const {return Valid(0,d,p,0)?total.timePlayer[d][p]:0;}
    std::int64_t GetCurrentGameTime(int d,int p) const {return Valid(0,d,p,0)?current.timePlayer[d][p]:0;}
};
static_assert(sizeof(PurpleSave::Save)==15760);
}
