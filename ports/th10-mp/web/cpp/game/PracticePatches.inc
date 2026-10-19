// Generated from thprac (MIT), sha256 4d4907927992be0e094f0c253e2b6035d89911ee0812ff29eb11197da2491352.
// Regenerate with portable/generate-thprac.mjs; included inside PracticePatcher.
    void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        StdStatus st4_c2 {
            208, 0xffffffff, 0x43f00000, 0x44610000, // Fog Color
            208, 0x0, 0x42c80000, 0xc3e10000, // Cam Pos
            208, 0x0, 0x43700000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c3 {
            240, 0xffffffff, 0x43f00000, 0x44610000, // Fog Color
            1159, 0x0, 0x42c80000, 0xc3e10000, // Cam Pos
            440, 0x0, 0x43700000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c4 {
            240, 0xffffffff, 0x43f00000, 0x44610000, // Fog Color
            2678, 0x0, 0x42c80000, 0xc3e10000, // Cam Pos
            440, 0x0, 0x43700000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c5 {
            0, 0xff704030, 0x43960000, 0x44160000, // Fog Color
            99, 0xc3480000, 0x434a0000, 0xc3a50000, // Cam Pos
            0, 0x42f00000, 0xc3960000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c6 {
            0, 0xff704030, 0x43960000, 0x44160000, // Fog Color
            487, 0xc3480000, 0x43e5f287, 0xc3a50000, // Cam Pos
            0, 0x42f00000, 0xc3960000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c7 {
            0, 0xff500000, 0x43960000, 0x44480000, // Fog Color
            70, 0x0, 0x43f10000, 0xc3e10000, // Cam Pos
            0, 0x0, 0xc3960000, 0x43a50000, // Cam Dir
        };
        StdStatus st4_c8 {
            247, 0xff500000, 0x43960000, 0x44480000, // Fog Color
            247, 0x0, 0x43f10000, 0xc3e10000, // Cam Pos
            247, 0x0, 0xc3960000, 0x43a50000, // Cam Dir
        };
        auto st4_boss_timeskip = [&](uint32_t stdTime) {
            ecl << pair(0x14aa4, 8600 - stdTime) << pair(0x14ab4, 8600 - stdTime) << pair(0x14ae4, 8600 - stdTime)
                << pair(0x14af8, 8601 - stdTime) << pair(0x14b08, 8601 - stdTime) << pair(0x14b18, 8601 - stdTime)
                << pair(0x14b2c, 8602 - stdTime);
            ecl << pair(0xf900, 3600 - stdTime)
                << pair(0xf930, 4100 - stdTime) << pair(0xf950, 4100 - stdTime);
        };

        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x9cb8, 0x9f40, 390);
                break;
            case 3:
                ECLJump(ecl, 0x9cb8, 0xa5b8, 1190);
                break;
            case 4:
                ECLJump(ecl, 0x9cb8, 0xaf74, 2220);
                ECLJump(ecl, 0xafa8, 0xafd8, 2510);
                break;
            case 5:
                ECLJump(ecl, 0x9cb8, 0xaf74, 2220);
                ECLJump(ecl, 0xafa8, 0xb1f0, 3330);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xb764, 0xba7c, 470);
                break;
            case 3:
                ECLJump(ecl, 0xb764, 0xbfe4, 884);
                break;
            case 4:
                ECLJump(ecl, 0xb764, 0xc028, 2884);
                break;
            case 5:
                ECLJump(ecl, 0xb764, 0xcb54, 4024);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xca7c, 0xcb64, 990);
                break;
            case 3:
                ECLJump(ecl, 0xca7c, 0xcff4, 1970);
                break;
            case 4:
                ECLJump(ecl, 0xca7c, 0xd220, 2830);
                break;
            case 5:
                ECLJump(ecl, 0xca7c, 0xd384, 3190);
                break;
            case 6:
                ECLJump(ecl, 0xca7c, 0xd694, 3915);
                break;
            case 7:
                ECLJump(ecl, 0xca7c, 0xe1f4, 5195);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                //c2
                STDSt4Jump(0x94, 409, st4_c2);
                THStage4ANM(409);
                ECLJump(ecl, 0xdf30, 0xe048, 409);
                st4_boss_timeskip(409);
                break;
            case 3:
                //c3
                STDSt4Jump(0x94, 1360, st4_c3);
                THStage4ANM(1360);
                ECLJump(ecl, 0xdf30, 0xe918, 1360);
                st4_boss_timeskip(1360);
                break;
            case 4:
                //c4
                STDSt4Jump(0x94, 2879, st4_c4);
                THStage4ANM(2879);
                ECLJump(ecl, 0xdf30, 0xed44, 310);
                ECLJump(ecl, 0xed6c, 0xf30c, 1459);
                st4_boss_timeskip(2879);
                break;
            case 5:
                //c5
                STDSt4Jump(0x16c, 4040, st4_c5);
                THStage4ANM(4040);
                ECLJump(ecl, 0x14a64, 0x14a84, 0);
                ECLJump(ecl, 0xf900, 0xf930, 4040);
                st4_boss_timeskip(4040);
                break;
            case 6:
                //c6
                STDSt4Jump(0x1cc, 5080, st4_c6);
                THStage4ANM(5080);
                ECLJump(ecl, 0x14a64, 0x14a84, 0);
                ECLJump(ecl, 0xf900, 0x105a4, 980);
                st4_boss_timeskip(5080);
                break;
            case 7:
                //c7
                STDSt4Jump(0x280, 6199, st4_c7);
                THStage4ANM(6199);
                ECLJump(ecl, 0x14a64, 0x14a84, 0);
                ECLJump(ecl, 0xf900, 0x12d24, 409);
                st4_boss_timeskip(6199);
                break;
            case 8:
                //c8
                STDSt4Jump(0x2e0, 7400, st4_c8);
                THStage4ANM(7400);
                ECLJump(ecl, 0x14a64, 0x14a84, 0);
                ECLJump(ecl, 0xf900, 0x12e00, 1610);
                st4_boss_timeskip(7400);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xe39c, 0x119a4, 150); // -878
                ecl << pair(0x143e4, 3800 - 878) << pair(0x143f4, 3800 - 878) << pair(0x14424, 3800 - 878)
                    << pair(0x14434, 3800 - 878) << pair(0x14454, 3800 - 878);
                break;
            case 3:
                ECLJump(ecl, 0xe39c, 0x11e7c, 30); // -1373
                ecl << pair(0x143e4, 3800 - 1373) << pair(0x143f4, 3800 - 1373) << pair(0x14424, 3800 - 1373)
                    << pair(0x14434, 3800 - 1373) << pair(0x14454, 3800 - 1373);
                break;
            case 4:
                ECLJump(ecl, 0xe39c, 0x12e54, 30); // -2873
                ecl << pair(0x143e4, 3800 - 2873) << pair(0x143f4, 3800 - 2873) << pair(0x14424, 3800 - 2873)
                    << pair(0x14434, 3800 - 2873) << pair(0x14454, 3800 - 2873);
                break;
            case 5:
                ecl << pair{0x14ba4, (int16_t)0};
                ECLJump(ecl, 0x143e4, 0x132b8, 30);
                break;
            case 6:
                ecl << pair{0x14ba4, (int16_t)0};
                ECLJump(ecl, 0x143e4, 0x136e8, 930);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xe8ec, 0xed70, 210); // -715
                ECLJump(ecl, 0xfb20, 0xfb40, 3668, 3368 - 715 + 300);
                break;
            case 3:
                ECLJump(ecl, 0xe8ec, 0xf63c, 110); // -1260
                ECLJump(ecl, 0xfb20, 0xfb40, 3668, 3368 - 1260 + 300);
                break;
            case 4:
                ECLJump(ecl, 0xe8ec, 0xf8c4, 70); // -1965
                ECLJump(ecl, 0xfb20, 0xfb40, 3668, 3368 - 1965 + 300);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x1366c, 0x15744, 40);
                break;
            case 3:
                ECLJump(ecl, 0x1366c, 0x1610c, 110);
                break;
            case 4:
                ECLJump(ecl, 0x1366c, 0x16fe4, 110);
                break;
            case 5:
                ECLJump(ecl, 0x1366c, 0x1721c, 110);
                break;
            case 6:
                ECLJump(ecl, 0x1366c, 0x18024, 790);
                break;
            default:
                break;
            }
        }
    }
    void THPatch(ECLHelper& ecl, int section)
    {
        switch (section) {
        case TH10_ST1_MID1:
            ECLJump(ecl, 0x9cb8, 0xaf74, 2200);
            break;
        case TH10_ST1_MID2_EN:
            ECLJump(ecl, 0x9cb8, 0xaf74, 2200);
            ecl << pair{0x6e24, 4200} << pair{0x6e38, 1650};
            ECLVoid(ecl, 0x7134);
            break;
        case TH10_ST1_MID2_HL:
            ECLJump(ecl, 0x9cb8, 0xaf74, 2200);
            ecl << pair{0x6e24, 4200} << pair{0x6e38, 1650};
            ECLVoid(ecl, 0x7134);
            break;
        case TH10_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x9cb8, 0xba8c, 4370);
            else {
                ECLJump(ecl, 0x9cb8, 0xbab0, 4370);
                game.section = 24;
            }
            break;
        case TH10_ST1_BOSS2:
            ECLJump(ecl, 0x9cb8, 0xbab0, 4370);
            ecl << pair{0x7a4, 0x898};
            game.section = 24;
            break;
        case TH10_ST1_BOSS3:
            ECLJump(ecl, 0x9cb8, 0xbab0, 4370);
            ecl << pair{0x850, (int8_t)0x32};
            ECLJump(ecl, 0x15ec, 0x1640, 60);
            game.section = 24;
            break;
        case TH10_ST1_BOSS4:
            ECLJump(ecl, 0x9cb8, 0xbab0, 4370);
            ecl << pair{0x850, (int8_t)0x32};
            ECLJump(ecl, 0x15ec, 0x1640, 60);
            ecl << pair{0x143c, 2200};
            game.section = 24;
            break;
        case TH10_ST2_MID1:
            ECLJump(ecl, 0xb730, 0xbfe4, 2200);
            ECLJump(ecl, 0x8f50, 0x96f0, 1578);
            break;
        case TH10_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0xb730, 0xd23c, 5864);
            else
                ECLJump(ecl, 0xb730, 0xd260, 5864);
            break;
        case TH10_ST2_BOSS2:
            ECLJump(ecl, 0xb730, 0xd260, 5864);
            ecl << pair{0x87c, 2700};
            break;
        case TH10_ST2_BOSS3:
            ECLJump(ecl, 0xb730, 0xd260, 5864);
            ecl << pair{0x990, (int8_t)0x32};
            ECLJump(ecl, 0x1d8c, 0x1df8, 120);
            break;
        case TH10_ST2_BOSS4:
            ECLJump(ecl, 0xb730, 0xd260, 5864);
            ecl << pair{0x990, (int8_t)0x32};
            ECLJump(ecl, 0x1d8c, 0x1df8, 120);
            ecl << pair{0x1bc8, 5400};
            break;
        case TH10_ST2_BOSS5:
            ECLJump(ecl, 0xb730, 0xd260, 5864);
            ecl << pair{0x990, (int8_t)0x32};
            ECLJump(ecl, 0x1d8c, 0x1df8, 120);
            ecl << pair{0x1bf4, (int8_t)0x33}
                << pair{0x1bc8, 2700} << pair{0x1be0, 2700};
            break;
        case TH10_ST3_MID1:
            ECLJump(ecl, 0xca48, 0xd320, 3190);
            ecl << pair{0xd320, 3190} << pair{0xd350, 3190};
            break;
        case TH10_ST3_BOSS1:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            if (!thPracParam.dlg)
                ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            break;
        case TH10_ST3_BOSS2:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair{0x884, 2300};
            break;
        case TH10_ST3_BOSS3:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair{0x998, (int8_t)0x32};
            ECLJump(ecl, 0x1474, 0x14e0, 120);
            break;
        case TH10_ST3_BOSS4:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair{0x998, (int8_t)0x32};
            ECLJump(ecl, 0x1474, 0x14e0, 120);
            ecl << pair{0x12cc, 2100};
            break;
        case TH10_ST3_BOSS5:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair{0x998, (int8_t)0x33};
            ECLJump(ecl, 0x2e48, 0x2eb4, 120);
            break;
        case TH10_ST3_BOSS6:
            ECLJump(ecl, 0xca48, 0xe354, 5885);
            ECLJump(ecl, 0xe384, 0xe3a8, 5885);
            ecl << pair{0x998, (int8_t)0x33};
            ECLJump(ecl, 0x2e48, 0x2eb4, 120);
            ecl << pair{0x2c58, 2300};
            break;
        case TH10_ST4_MID1:
            THStage4ANM(3600);
            THStage4STD(3600);
            ECLJump(ecl, 0x14a64, 0x14a84, 3600);
            ECLJump(ecl, 0xf8ec, 0xf900, 3600);
            break;
        case TH10_ST4_BOSS1:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            if (!thPracParam.dlg)
                ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            break;
        case TH10_ST4_BOSS2:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair{0x7ec, 2500};
            break;
        case TH10_ST4_BOSS3:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair{0x808, 0.0f} << pair{0x80c, 128.0f};
            ecl << pair{0x928, (int8_t)0x32};
            ECLJump(ecl, 0x19c0, 0x1a44, 120);
            break;
        case TH10_ST4_BOSS4:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair{0x928, (int8_t)0x32};
            ECLJump(ecl, 0x19c0, 0x1a44, 120);
            ecl << pair{0x1804, 2500};
            break;
        case TH10_ST4_BOSS5:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair{0x808, 0.0f} << pair{0x80c, 128.0f};
            ecl << pair{0x928, (int8_t)0x33};
            ECLJump(ecl, 0x2628, 0x2694, 120);
            break;
        case TH10_ST4_BOSS6:
            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, 0x14a64, 0x14aa4, 8600);
            ECLJump(ecl, 0x14ae4, 0x14b08, 8601);
            ecl << pair{0x928, (int8_t)0x33};
            ECLJump(ecl, 0x2628, 0x2694, 120);
            ecl << pair{0x2440, 0};

            ecl << pair{0x47e8, 99999999};
            break;
        case TH10_ST4_BOSS7: {
            constexpr unsigned int st4PreMainSub = 0x14a64;
            constexpr unsigned int st4BossCreate = 0x14aa4;
            constexpr unsigned int st4BossDialogRead = 0x14ae4;
            constexpr unsigned int st4BossDeathWait = 0x14b08;

            constexpr unsigned int st4BossNonSubCallOrd = 0x928;
            constexpr unsigned int st4Boss3DropStart = 0x2628;
            constexpr unsigned int st4Boss3DropEnd = 0x2694;

            constexpr unsigned int st4Boss3LifeSetE = 0x242c;
            constexpr unsigned int st4Boss3LifeSetNHL = 0x2440;
            constexpr unsigned int st4Boss3InterruptNHL = 0x2498;

            constexpr unsigned int st4BossSp4MovePos = 0x6b28;
            constexpr unsigned int st4BossSp4PostMove = 0x6b74;

            THStage4ANM(8688);
            THStage4STD(8688);
            ECLJump(ecl, st4PreMainSub, st4BossCreate, 8600); //skip stage
            ECLJump(ecl, st4BossDialogRead, st4BossDeathWait, 8601); //skip dialogue
            ecl << pair { st4BossNonSubCallOrd, (int8_t)0x33 }; // set to non 3
            ECLJump(ecl, st4Boss3DropStart, st4Boss3DropEnd, 120); //skip non drops
            ecl << pair { st4Boss3LifeSetE, 2300 } // set health to spell transition amt
                << pair { st4Boss3LifeSetNHL, 0 } //set health to spell transition amt
                << pair { st4Boss3InterruptNHL, (int8_t)0x34 }; // set to sp4
            ECLJump(ecl, st4BossSp4MovePos, st4BossSp4PostMove, 0);
            break;
        } case TH10_ST5_MID1:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 3800);
            ECLJump(ecl, 0x143d0, 0x143e4, 3800);
            break;
        case TH10_ST5_MID2:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 3800);
            ECLJump(ecl, 0x143d0, 0x143e4, 3800);
            ecl << pair{0xb4e0, 2900};
            break;
        case TH10_ST5_BOSS1:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair{0x14bdb, (int8_t)0x42} << pair{0x14bdc, 0x0073736f};
            if (!thPracParam.dlg)
                ecl << pair{0x1432c, (int16_t)0} << pair{0x14370, (int16_t)0};
            break;
        case TH10_ST5_BOSS2:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair{0x14bdb, (int8_t)0x42} << pair{0x14bdc, 0x0073736f}
                << pair{0x1432c, (int16_t)0} << pair{0x14370, (int16_t)0};
            ecl << pair{0x9ec, 2300} << pair{0xa00, 2400};
            break;
        case TH10_ST5_BOSS3:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair{0x14bdb, (int8_t)0x42} << pair{0x14bdc, 0x0073736f}
                << pair{0x1432c, (int16_t)0} << pair{0x14370, (int16_t)0};
            ecl << pair{0xb14, (int8_t)0x32};
            ECLJump(ecl, 0x2280, 0x22ec, 120);
            break;
        case TH10_ST5_BOSS4:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair{0x14bdb, (int8_t)0x42} << pair{0x14bdc, 0x0073736f}
                << pair{0x1432c, (int16_t)0} << pair{0x14370, (int16_t)0};
            ecl << pair{0xb14, (int8_t)0x32};
            ECLJump(ecl, 0x2280, 0x22ec, 120);
            ecl << pair{0x20c8, 2400};
            break;
        case TH10_ST5_BOSS5:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair{0x14bdb, (int8_t)0x42} << pair{0x14bdc, 0x0073736f}
                << pair{0x1432c, (int16_t)0} << pair{0x14370, (int16_t)0};
            ecl << pair{0xb14, (int8_t)0x33};
            ECLJump(ecl, 0x31b0, 0x321c, 100);
            break;
        case TH10_ST5_BOSS6:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair{0x14bdb, (int8_t)0x42} << pair{0x14bdc, 0x0073736f}
                << pair{0x1432c, (int16_t)0} << pair{0x14370, (int16_t)0};
            ecl << pair{0xb14, (int8_t)0x33};
            ECLJump(ecl, 0x31b0, 0x321c, 100);
            ecl << pair{0x2ff8, 2400};
            break;
        case TH10_ST5_BOSS7:
            ECLJump(ecl, 0x14ba0, 0x14bc0, 0);
            ecl << pair{0x14bdb, (int8_t)0x42} << pair{0x14bdc, 0x0073736f}
                << pair{0x1432c, (int16_t)0} << pair{0x14370, (int16_t)0};
            ecl << pair{0xb14, (int8_t)0x33};
            ECLJump(ecl, 0x31b0, 0x321c, 100);
            ecl << pair{0x2ff8, 2400};

            ecl << pair{0x3024, (int8_t)0x34};
            ECLJump(ecl, 0x7b74, 0x7c04, 90);
            ecl << pair{0x7c34, 60};
            break;
        case TH10_ST6_BOSS1:
            if (thPracParam.dlg) {
                ECLJump(ecl, 0xfb0c, 0xfb40, 3668);
                ECLJump(ecl, 0x10100, 0x10120, 0);
            } else
                ECLSt6Boss(ecl);
            ECLJump(ecl, 0x9bc, 0x9e8, 80);
            ecl << pair{0x994, 0} << pair{0x9a8, 0} << pair{0x990, 40};
            break;
        case TH10_ST6_BOSS2:
            ECLSt6Boss(ecl);
            ECLJump(ecl, 0x9bc, 0x9e8, 80);
            ecl << pair{0x994, 0} << pair{0x9a8, 0} << pair{0x990, 40};
            ecl << pair{0x71c, 2500};
            break;
        case TH10_ST6_BOSS3:
            ECLSt6Boss(ecl);
            ecl << pair{0x8c8, (int8_t)0x32};
            ECLJump(ecl, 0xf9c, 0x1028, 60);
            ECLJump(ecl, 0x1058, 0x1098, 120);
            ecl << pair{0xf38, 60}; // Invi. time
            ecl << pair{0x730, 0} << pair{0x734, 0x42e00000}; // Pos
            break;
        case TH10_ST6_BOSS4:
            ECLSt6Boss(ecl);
            ecl << pair{0x8c8, (int8_t)0x32};
            ECLJump(ecl, 0xf9c, 0x1028, 60);
            ECLJump(ecl, 0x1058, 0x1098, 120);
            ecl << pair{0xf38, 60};
            ecl << pair{0x730, 0} << pair{0x734, 0x42e00000};

            ecl << pair{0x0d58, 2500};
            ecl.SetPos(0x1058);
            ecl << 0 << 0x00180103 << 0x02ff0000 << 0 << 1 << 13
                << 0 << 0x00180103 << 0x02ff0000 << 0 << 2 << 14
                << 0 << 0x00180103 << 0x02ff0000 << 0 << 3 << 18
                << 0 << 0x00180103 << 0x02ff0000 << 0 << 4 << 19
                << 9999 << 0x00100000 << 0x00ff0000 << 0;
            break;
        case TH10_ST6_BOSS5:
            ECLSt6Boss(ecl);
            ecl << pair{0x8c8, (int8_t)0x33};
            ECLJump(ecl, 0x2190, 0x21fc, 0);
            ECLJump(ecl, 0x225c, 0x227c, 90);
            ecl << pair{0x212c, 0}; // Invi. time
            ecl << pair{0x730, 0} << pair{0x734, 0x43000000}; // Pos
            break;
        case TH10_ST6_BOSS6:
            ECLSt6Boss(ecl);
            ecl << pair{0x8c8, (int8_t)0x33};
            ECLJump(ecl, 0x2190, 0x21fc, 0);
            ECLJump(ecl, 0x225c, 0x227c, 90);
            ecl << pair{0x212c, 0}; // Invi. time
            ecl << pair{0x730, 0} << pair{0x734, 0x43000000}; // Pos

            ecl << pair{0x22ac, 9999};
            ecl << pair{0x1ed8, 2900} << pair{0x1f4c, 2900};
            break;
        case TH10_ST6_BOSS7:
            ECLSt6Boss(ecl);
            ecl << pair{0x8c8, (int8_t)0x34};
            ECLJump(ecl, 0x2970, 0x29c8, 0);
            ECLJump(ecl, 0x2a28, 0x2a48, 90);
            ecl << pair{0x290c, 0}; // Invi. time
            ecl << pair{0x730, 0} << pair{0x734, 0x43000000}; // Pos
            break;
        case TH10_ST6_BOSS8:
            ECLSt6Boss(ecl);
            ecl << pair{0x8c8, (int8_t)0x34};
            ECLJump(ecl, 0x2970, 0x29c8, 0);
            ECLJump(ecl, 0x2a28, 0x2a48, 90);
            ecl << pair{0x290c, 0}; // Invi. time
            ecl << pair{0x730, 0} << pair{0x734, 0x43000000}; // Pos

            ecl << pair{0x2a78, 9999};
            ecl << pair{0x272c, 3200};
            break;
        case TH10_ST6_BOSS9:
            ECLSt6Boss(ecl);
            ecl << pair{0x8c8, (int8_t)0x34};
            ECLJump(ecl, 0x2970, 0x29c8, 0);
            ECLJump(ecl, 0x2a28, 0x2a48, 90);
            ecl << pair{0x290c, 0}; // Invi. time
            ecl << pair{0x730, 0} << pair{0x734, 0x43200000}; // Pos

            ecl << pair{0x272c, 3200};

            ecl << pair{0x2758, (int8_t)0x35};
            ECLJump(ecl, 0x8a4c, 0x8aa4, 90);
            ecl << pair{0x88bc, 0};
            ecl.SetPos(0x2a58);
            ecl << 0 << 0x00180103 << 0x02ff0000 << 0 << 3 << 18
                << 0 << 0x00180103 << 0x02ff0000 << 0 << 4 << 19
                << 9999 << 0x00100000 << 0x00ff0000 << 0;

            if (thPracParam.st6_boss9_spd >= 0) {
                ecl.SetPos(0x8d38 + game.difficulty * 4);
                ecl << thPracParam.st6_boss9_spd;
            }
            break;
        case TH10_ST7_MID1:
            ecl << pair{0x18983, (int8_t)0x37}
                << pair{0x10bb4, 60} << pair{0x10bc4, 60} << pair{0x10bd8, 60}
                << pair{0x10bec, 60} << pair{0x10c04, 60} << pair{0x10c18, 60}
                << pair{0x10c28, 60} << pair{0x10c48, 60} << pair{0x10c5c, 60}
                << pair{0x10c70, 60} << pair{0x10c88, 60} << pair{0x10c9c, 60}
                << pair{0x10cb4, 60} << pair{0x10cc8, 60} << pair{0x10cdc, 60}
                << pair{0x10afc, (int16_t)0};
            if (!thPracParam.dlg)
                ECLJump(ecl, 0x171a8, 0x171cc, 61);
            break;
        case TH10_ST7_MID2:
            ECLSt7MidBoss(ecl);
            ecl << pair{0x10ac4, 2300};
            break;
        case TH10_ST7_MID3:
            ECLSt7MidBoss(ecl);
            ecl << pair{0x10ac4, 2300};
            ecl << pair{0x10d65, (int8_t)0x32};
            ECLJump(ecl, 0x118d8, 0x11930, 120);
            break;
        case TH10_ST7_MID4:
            ECLSt7MidBoss(ecl);
            ecl << pair{0x10ac4, 2300};
            ecl << pair{0x10d65, (int8_t)0x33};
            ECLJump(ecl, 0x126cc, 0x12724, 90);
            break;
        case TH10_ST7_END_NS1:
            if (thPracParam.dlg)
                ecl << pair{0x18983, (int8_t)0x42} << pair{0x18984, (int8_t)0x6f}
                    << pair{0x18985, (int8_t)0x73} << pair{0x18986, (int8_t)0x73};
            else {
                ECLSt7Boss(ecl);
                game.section = 24;
            }
            break;
        case TH10_ST7_END_S1:
            ECLSt7Boss(ecl);
            ecl << pair{0xbe0, 2800};
            ECLJump(ecl, 0xd3c, 0xd68, 0);
            ecl.SetPos(0xde8);
            ecl << 0 << 0x002c014e << 0x04ff0000 << 0 << 0 << 0xaf0 << 0xe10
                << 0xc << 0x73736f42 << 0x64726143 << 0x31;
            game.section = 24;
            break;
        case TH10_ST7_END_NS2:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x32};
            ECLJump(ecl, 0x12b8, 0x1324, 110);
            game.section = 24;
            break;
        case TH10_ST7_END_S2:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x32};
            ECLJump(ecl, 0x12b8, 0x1324, 110);
            ecl << pair{0x10e8, 3600};
            game.section = 24;
            break;
        case TH10_ST7_END_NS3:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x33};
            ECLJump(ecl, 0x17b8, 0x1824, 120);
            game.section = 24;
            break;
        case TH10_ST7_END_S3:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x33};
            ECLJump(ecl, 0x17b8, 0x1824, 120);
            ecl << pair{0x15cc, 2800};
            game.section = 24;
            break;
        case TH10_ST7_END_NS4:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x34};
            ECLJump(ecl, 0x207c, 0x20e8, 120);
            game.section = 24;
            break;
        case TH10_ST7_END_S4:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x34};
            ECLJump(ecl, 0x207c, 0x20e8, 120);
            ecl << pair{0x1e70, 3600};
            game.section = 24;
            break;
        case TH10_ST7_END_NS5:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x35};
            ECLJump(ecl, 0x2628, 0x2694, 110);
            game.section = 24;
            break;
        case TH10_ST7_END_S5:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x35};
            ECLJump(ecl, 0x2628, 0x2694, 110);
            ecl << pair{0x2408, 3500};
            game.section = 24;
            break;
        case TH10_ST7_END_NS6:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x36};
            ECLJump(ecl, 0x2b48, 0x2bb4, 120);
            game.section = 24;
            break;
        case TH10_ST7_END_S6:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x36};
            ECLJump(ecl, 0x2b48, 0x2bb4, 120);
            ecl << pair{0x293c, 3500};
            game.section = 24;
            break;
        case TH10_ST7_END_NS7:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x37};
            ECLJump(ecl, 0x340c, 0x3478, 120);
            game.section = 24;
            break;
        case TH10_ST7_END_S7:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x37};
            ECLJump(ecl, 0x340c, 0x3478, 120);
            ecl << pair{0x3200, 4500};
            game.section = 24;
            break;
        case TH10_ST7_END_NS8:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x38};
            ECLJump(ecl, 0x3988, 0x39f4, 110);
            game.section = 24;
            break;
        case TH10_ST7_END_S8:
            ECLSt7Boss(ecl);
            ecl << pair{0xcf4, (int8_t)0x38};
            ECLJump(ecl, 0x3988, 0x39f4, 110);
            ecl << pair{0x3798, 1};
            game.section = 24;
            break;
        case TH10_ST7_END_S9:
            ECLSt7Boss(ecl);
            ecl << pair{0xcec, 0xc} << pair{0xcf4, 0x64726143}
                << pair{0xcf8, 0x39};
            ECLJump(ecl, 0xb3d8, 0xb444, 80);
            game.section = 24;
            break;
        case TH10_ST7_END_S10:
            ECLSt7Boss(ecl);
            ecl << pair{0xcec, 0xc} << pair{0xcf4, 0x64726143}
                << pair{0xcf8, 0x3031};
            ECLJump(ecl, 0x41d8, 0x4244, 80);
            ecl << pair{0x41b8, 20};

            if (thPracParam.phase == 1) {
                ecl << pair{0x4338, 5};
                ECLJump(ecl, 0x4af0, 0x5198, 0);
            }
            game.section = 24;
            break;
        default:
            break;
        }
    }
