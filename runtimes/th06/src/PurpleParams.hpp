#pragma once
#include <algorithm>
#include <string>

namespace PracticeRuntime {
// Native TH06 THPracParam names are also the upstream PRAC trailer keys.
// Keep these independent of the platform/session envelope.
struct PurpleParams {
    bool purpleSections = true; // Internal dispatch, never a native replay key.
    int delay_st6bs9 = 0;
    bool wall_prac_st6 = false;
    int bF = 0;
    int bX1=0,bX2=0,bX3=0,bX4=0,bX5=0,bX6=0;
    int bY1=32,bY2=128,bY3=144,bY4=64,bY5=80,bY6=96;
    int snipeN=0,snipeF=0;

    void ResetPurpleMenuWarp() {
        // Native THGuiPrac::PracticeMenu, on mWarp change (not replay Reset).
        delay_st6bs9=120; wall_prac_st6=false; snipeF=30; snipeN=0;
        bF=63; bX1=-180; bX2=-116; bX3=-61; bX4=41; bX5=112; bX6=180;
        bY1=32; bY2=128; bY3=144; bY4=64; bY5=80; bY6=96;
    }
    template<class Number, class Boolean> void ReadPurple(Number number, Boolean boolean) {
        delay_st6bs9=int(number("delay_st6bs9",delay_st6bs9));
        wall_prac_st6=boolean("wall_prac_st6",wall_prac_st6);
#define TH06_READ_PARAM(k) k=int(number(#k,k));
        TH06_READ_PARAM(bF)
        TH06_READ_PARAM(bX1) TH06_READ_PARAM(bX2) TH06_READ_PARAM(bX3)
        TH06_READ_PARAM(bX4) TH06_READ_PARAM(bX5) TH06_READ_PARAM(bX6)
        TH06_READ_PARAM(bY1) TH06_READ_PARAM(bY2) TH06_READ_PARAM(bY3)
        TH06_READ_PARAM(bY4) TH06_READ_PARAM(bY5) TH06_READ_PARAM(bY6)
        TH06_READ_PARAM(snipeN) TH06_READ_PARAM(snipeF)
#undef TH06_READ_PARAM
    }
    void NormalizePurple() {
        delay_st6bs9=std::clamp(delay_st6bs9,0,600);
        bF=std::clamp(bF,0,63);
        for(auto p:{&bX1,&bX2,&bX3,&bX4,&bX5,&bX6}) *p=std::clamp(*p,-192,192);
        for(auto p:{&bY1,&bY2,&bY3,&bY4,&bY5,&bY6}) *p=std::clamp(*p,-50,448);
        snipeN=std::clamp(snipeN,0,100); snipeF=std::clamp(snipeF,0,100);
    }
    std::string PurpleJsonFields() const {
        if(!purpleSections) return {}; // Preserve blue replay patch semantics.
        std::string out=",\"delay_st6bs9\":"+std::to_string(delay_st6bs9)+
            ",\"wall_prac_st6\":"+(wall_prac_st6?"true":"false");
#define TH06_WRITE_PARAM(k) out+=",\"" #k "\":"+std::to_string(k);
        TH06_WRITE_PARAM(bF)
        TH06_WRITE_PARAM(bX1) TH06_WRITE_PARAM(bX2) TH06_WRITE_PARAM(bX3)
        TH06_WRITE_PARAM(bX4) TH06_WRITE_PARAM(bX5) TH06_WRITE_PARAM(bX6)
        TH06_WRITE_PARAM(bY1) TH06_WRITE_PARAM(bY2) TH06_WRITE_PARAM(bY3)
        TH06_WRITE_PARAM(bY4) TH06_WRITE_PARAM(bY5) TH06_WRITE_PARAM(bY6)
        TH06_WRITE_PARAM(snipeN) TH06_WRITE_PARAM(snipeF)
#undef TH06_WRITE_PARAM
        return out;
    }
};
}
