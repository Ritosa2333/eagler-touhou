// Generated from purple thprac_games_def.json (MIT). Enum order is authoritative.
#pragma once
namespace th15 {
enum PracticeSection {PracticeNone=0,
 TH15_ST1_MID1=1,
 TH15_ST1_MID2=2,
 TH15_ST1_MID3=3,
 TH15_ST1_BOSS1=4,
 TH15_ST1_BOSS2=5,
 TH15_ST1_BOSS3=6,
 TH15_ST1_BOSS4=7,
 TH15_ST2_MID1=8,
 TH15_ST2_BOSS1=9,
 TH15_ST2_BOSS2=10,
 TH15_ST2_BOSS3=11,
 TH15_ST2_BOSS4=12,
 TH15_ST2_BOSS5=13,
 TH15_ST2_BOSS6=14,
 TH15_ST3_MID1=15,
 TH15_ST3_MID2=16,
 TH15_ST3_BOSS1=17,
 TH15_ST3_BOSS2=18,
 TH15_ST3_BOSS3=19,
 TH15_ST3_BOSS4=20,
 TH15_ST3_BOSS5=21,
 TH15_ST3_BOSS6=22,
 TH15_ST3_BOSS7=23,
 TH15_ST4_MID1=24,
 TH15_ST4_BOSS1=25,
 TH15_ST4_BOSS2=26,
 TH15_ST4_BOSS3=27,
 TH15_ST4_BOSS4=28,
 TH15_ST4_BOSS5=29,
 TH15_ST4_BOSS6=30,
 TH15_ST4_BOSS7=31,
 TH15_ST5_MID1=32,
 TH15_ST5_BOSS1=33,
 TH15_ST5_BOSS2=34,
 TH15_ST5_BOSS3=35,
 TH15_ST5_BOSS4=36,
 TH15_ST5_BOSS5=37,
 TH15_ST5_BOSS6=38,
 TH15_ST5_BOSS7=39,
 TH15_ST5_BOSS8=40,
 TH15_ST6_STARS=41,
 TH15_ST6_BOSS1=42,
 TH15_ST6_BOSS2=43,
 TH15_ST6_BOSS3=44,
 TH15_ST6_BOSS4=45,
 TH15_ST6_BOSS5=46,
 TH15_ST6_BOSS6=47,
 TH15_ST6_BOSS7=48,
 TH15_ST6_BOSS8=49,
 TH15_ST6_BOSS9=50,
 TH15_ST6_BOSS10=51,
 TH15_ST6_BOSS11=52,
 TH15_ST7_MID1=53,
 TH15_ST7_MID2=54,
 TH15_ST7_MID3=55,
 TH15_ST7_END_NS1=56,
 TH15_ST7_END_S1=57,
 TH15_ST7_END_NS2=58,
 TH15_ST7_END_S2=59,
 TH15_ST7_END_NS3=60,
 TH15_ST7_END_S3=61,
 TH15_ST7_END_NS4=62,
 TH15_ST7_END_S4=63,
 TH15_ST7_END_NS5=64,
 TH15_ST7_END_S5=65,
 TH15_ST7_END_NS6=66,
 TH15_ST7_END_S6=67,
 TH15_ST7_END_NS7=68,
 TH15_ST7_END_S7=69,
 TH15_ST7_END_NS8=70,
 TH15_ST7_END_S8=71,
 TH15_ST7_END_S9=72,
 TH15_ST7_END_S10=73,
 TH15_ST8_AB_TEST=74,
};
struct PracticeSectionInfo {int appearance,group,bgm;bool spell;};
inline constexpr PracticeSectionInfo practice_sections[]{{0,0,0,false},
 {1,1,0,false},
 {1,1,0,false},
 {1,1,0,true},
 {1,2,1,false},
 {1,2,1,true},
 {1,2,1,false},
 {1,2,1,true},
 {2,1,0,false},
 {2,2,1,false},
 {2,2,1,true},
 {2,2,1,false},
 {2,2,1,true},
 {2,2,1,false},
 {2,2,1,true},
 {3,1,0,false},
 {3,1,0,true},
 {3,2,1,false},
 {3,2,1,true},
 {3,2,1,false},
 {3,2,1,true},
 {3,2,1,false},
 {3,2,1,true},
 {3,2,1,true},
 {4,1,0,false},
 {4,2,1,false},
 {4,2,1,true},
 {4,2,1,false},
 {4,2,1,true},
 {4,2,1,false},
 {4,2,1,true},
 {4,2,1,true},
 {5,1,0,false},
 {5,2,1,false},
 {5,2,1,true},
 {5,2,1,false},
 {5,2,1,true},
 {5,2,1,false},
 {5,2,1,true},
 {5,2,1,true},
 {5,2,1,true},
 {6,1,0,false},
 {6,2,1,false},
 {6,2,1,true},
 {6,2,1,false},
 {6,2,1,true},
 {6,2,1,false},
 {6,2,1,true},
 {6,2,1,false},
 {6,2,1,true},
 {6,2,1,true},
 {6,2,1,true},
 {6,2,1,true},
 {7,1,0,true},
 {7,1,0,true},
 {7,1,0,true},
 {7,2,1,false},
 {7,2,1,true},
 {7,2,1,false},
 {7,2,1,true},
 {7,2,1,false},
 {7,2,1,true},
 {7,2,1,false},
 {7,2,1,true},
 {7,2,1,false},
 {7,2,1,true},
 {7,2,1,false},
 {7,2,1,true},
 {7,2,1,false},
 {7,2,1,true},
 {7,2,1,false},
 {7,2,1,true},
 {7,2,1,true},
 {7,2,1,true},
 {3,2,1,false},
};
        inline int practice_phase_count(int section, int difficulty)
        {
            switch (section)
            {
            default:
                break;
            case TH15_ST6_BOSS11:
                return 4;
            case TH15_ST7_END_S9:
                return 3;
            case TH15_ST7_END_S10:
                return 4;
            case TH15_ST5_MID1:
                return 7;
            case TH15_ST3_BOSS1:
                return 4;
            case TH15_ST8_AB_TEST:
                return 5;
            case 10000 + 5 * 100 + 8:
                return 4;
            case 10000 + 5 * 100 + 10:
                return 2;
            case TH15_ST6_STARS:
                return 2;
            case TH15_ST6_BOSS4:
                return 2;
            case TH15_ST6_BOSS6:
                if (difficulty>=2)
                    return 2;
                return 1;
            case TH15_ST5_BOSS4:
                return 2;
            case 10000 + 3 * 100 + 3:
                return 2;
            case 10000 + 4 * 100 + 2:
                return 2;
            }
            return 1;
        }

}
