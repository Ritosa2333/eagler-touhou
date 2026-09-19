// Generated from thprac (MIT), sha256 230c3aacf25e1fdb350dd3be3f5974923c28a5f82325a7382c97516c6910cc3e.
// Regenerate with portable/generate-thprac.mjs; included inside PracticePatcher.
    void THStageWarp([[maybe_unused]] ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0x9c38, 20, 0xad18);
                break;
            case 2:
                ECLWarp(2875, 0xaba4, 20, 0xad18);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                ECLWarp(400, 0xa194, 20, 0xc0cc);
                break;
            case 2:
                ECLWarp(990, 0xa8d8, 20, 0xc0cc);
                break;
            case 3:
                ECLWarp(2350, 0xb418, 20, 0xc0cc);
                break;
            case 4:
                ECLWarp(3350, 0xba78, 20, 0xc0cc);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0xb258, 20, 0xb91c, 20, 0xbb74, 20, 0xc0ac);
                break;
            case 2:
                ECLWarp(1220, 0xb368, 200, 0xba14, 20, 0xbb74, 20, 0xc0ac);
                break;
            case 3:
                ECLWarp(3103, 0xb664, 380, 0xbacc, 20, 0xbb74, 160, 0xc304);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0xa2d4, 20, 0xbafc);
                break;
            case 2:
                ECLWarp(1180, 0xa834, 20, 0xbafc);
                break;
            case 3:
                ECLWarp(2240, 0xa8ac, 360, 0xbed4);
                break;
            case 4:
                ECLWarp(3180, 0xabcc, 360, 0xbed4);
                break;
            case 5:
                ECLWarp(4903, 0xb2d8, 360, 0xbed4);
                STDJump(0xf48, 56, 6558);
                break;
            case 6:
                ECLWarp(5363, 0xb598, 360, 0xbed4);
                STDJump(0xf48, 56, 6558);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0xc538, 20, 0xdd80);
                break;
            case 2:
                ECLWarp(1180, 0xcab8, 20, 0xdd80);
                break;
            case 3:
                ECLWarp(2240, 0xcb30, 360, 0xe158);
                break;
            case 4:
                ECLWarp(3180, 0xce50, 360, 0xe158);
                break;
            case 5:
                ECLWarp(4903, 0xd55c, 360, 0xe158);
                STDJump(0xf48, 56, 6558);
                break;
            case 6:
                ECLWarp(5363, 0xd81c, 360, 0xe158);
                STDJump(0xf48, 56, 6558);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                ECLWarp(200, 0xa674);
                break;
            case 2:
                ECLWarp(1040, 0xa694);
                break;
            case 3:
                ECLWarp(2990, 0xafdc);
                break;
            case 4:
                ECLWarp(4651, 0xb0e8);
                break;
            case 5:
                ECLWarp(5971, 0xb648);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                ECLWarp(200, 0xc6f4);
                break;
            case 2:
                ECLWarp(1140, 0xc714);
                break;
            default:
                break;
            }
        } else if (stage == 8) {
            switch (portion) {
            case 1:
                ECLWarp(200, 0x10324);
                break;
            case 2:
                ECLWarp(1140, 0x10364);
                break;
            default:
                break;
            }
        } else if (stage == 9) {
            switch (portion) {
            case 1:
                ECLWarp(340, 0x12544, 20, 0x1535c);
                break;
            case 2:
                ECLWarp(1010, 0x127a8, 210, 0x155c0);
                break;
            case 3:
                ECLWarp(2900, 0x12b6c, 390, 0x15824);
                break;
            case 4:
                ECLWarp(5237, 0x132b8, 810, 0x15d60);
                break;
            case 5:
                ECLWarp(5947, 0x136b4, 20, 0x1535c);
                break;
            case 6:
                ECLWarp(6757, 0x13774, 20, 0x1535c);
                break;
            case 7:
                ECLWarp(8337, 0x151f4, 20, 0x1535c);
                break;
            default:
                break;
            }
        }
    }
    void THPatch(ECLHelper& ecl, int section)
    {
        int32_t diff = scene.globals.difficulty;
        int32_t sub_id;

        switch (section) {
        case TH08_ST1_MID1:
            ECLWarp(2935, 0xab78, 20, 0xad18);
            break;
        case TH08_ST1_MID2:
            ECLWarp(2935, 0xab78, 20, 0xad18);
            if (diff == 1 || diff == 2)
                break;
            ECLSetTime(ecl, 0x178c, 0, 22, 60);
            break;
        case TH08_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(4175, 0xac70, 0, -1);
            else
                ECLWarp(4175, 0xac84, 0, -1);
            break;
        case TH08_ST1_BOSS2:
            ECLWarp(4175, 0xac84, 0, -1);
            ECLSetTime(ecl, 0x3d30, 0, 38, 60);
            break;
        case TH08_ST1_BOSS3:
            ECLWarp(4175, 0xac84, 0, -1);
            ECLTimeFix(0x3c68, 0, 6);
            ECLJump(ecl, 0x3d08, 0x4e0c, 0, 30);
            break;
        case TH08_ST1_BOSS4:
            ECLWarp(4175, 0xac84, 0, -1);
            ECLTimeFix(0x3c68, 0, 6);
            ECLCallSub(ecl, 0x3d08, 33, 60);
            ECLTimeFix(0x4ed4, 0, 2);
            ECLSetTime(ecl, 0x4ef0, 0, 44);
            break;
        case TH08_ST1_LS:
            ECLWarp(4176, 0xacd0, 0, -1);
            ECLTimeFix(0x58d4, 0, 5);
            ECLTimeFix(0x5944, 30, 6);
            ecl << ECLX(0x5948, (int16_t)0);
            ECLCheckTime(3000);
            break;
        case TH08_ST2_MID1:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 60, 3);
            break;
        case TH08_ST2_MID2:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 60, 3);

            ECLSetTime(ecl, 0x22b0, 0, 23);
            break;
        case TH08_ST2_BOSS1:
            ECLWarp(4870, 0xc020, 0, -1);
            if (thPracParam.dlg) {
                // I spent about an hour trying to do this
                // I am not exactly sure why this works I just took it from the SpoilerAL SSG
                // But I at least documented what it does for anyone curious
                ecl.SetPos(0x1e6c);
                ecl << 0 << 22; // Sub17: timerThreshold(1980, 23) -> timerThreshold(0, 22)
                ecl.SetPos(0x3784);
                ecl << 0 << 29; // Sub27: timerThreshold(1920, 38) -> timerThreshold(0, 29)
            } else {
                ECLJump(ecl, 0x1e34, 0x1ea8);
                ECLTimeFix(0x1ed8, 0, 7);
                ECLCallSub(ecl, 0x1f98, 27, 60, true);
                ECLJump(ecl, 0x37c0, 0x3880, 60);
                ecl << ECLX(0xc026, (int8_t)0x50);
            }
            break;
        case TH08_ST2_BOSS2:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLJump(ecl, 0x1e34, 0x1ea8);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLCallSub(ecl, 0x1f98, 27, 60, true);
            ECLJump(ecl, 0x37c0, 0x3880, 60);
            ecl << ECLX(0xc026, (int8_t)0x50);
            ECLSetTime(ecl, 0x389c, 0, (diff <= 1) ? 33 : 38, 60);
            break;
        case TH08_ST2_BOSS3:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 0, 3);
            ECLCallSub(ecl, 0x1ed8, 27, 0);
            ecl << ECLX(0xc026, (int8_t)0x50) << ECLX(0x1f9c, (int16_t)0);

            ECLTimeFix(0x37e0, 0, 5);
            ECLCallSub(ecl, 0x389c, 29, 60);
            break;
        case TH08_ST2_BOSS4:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 0, 3);
            ECLCallSub(ecl, 0x1ed8, 27, 0);
            ecl << ECLX(0xc026, (int8_t)0x50) << ECLX(0x1f9c, (int16_t)0);

            ECLTimeFix(0x37e0, 0, 5);
            ECLCallSub(ecl, 0x389c, 29, 60);

            ECLTimeFix(0x3e14, 0, 1);
            ECLSetTime(ecl, 0x3e30, 0, 44, 0);
            break;
        case TH08_ST2_BOSS5:
            ECLWarp(4870, 0xc020, 0, -1);
            ECLTimeFix(0x1ed8, 0, 7);
            ECLTimeFix(0x1f98, 0, 3);
            ECLCallSub(ecl, 0x1ed8, 27, 0);
            ecl << ECLX(0xc026, (int8_t)0x50) << ECLX(0x1f9c, (int16_t)0);

            ECLTimeFix(0x37e0, 0, 5);
            ECLCallSub(ecl, 0x389c, 29, 60);

            ECLTimeFix(0x3e14, 0, 1);
            ECLSetTime(ecl, 0x3e30, 0, 51, 0);
            ecl << ECLX(0x3db0, (int16_t)0) << ECLX(0x3da8, 51) << ECLX(0x7524, 0)
                << ECLX(0x7508, (int16_t)0) << ECLX(0x7518, (int16_t)0);
            break;
        case TH08_ST2_LS:
            ECLWarp(4871, 0xc07c, 0, -1);
            ECLTimeFix(0x49b0, 0, 5);
            ECLTimeFix(0x4a20, 30, 6);
            ecl << ECLX(0x4a24, (int16_t)0);
            ECLCheckTime(7200);
            break;
        case TH08_ST3_MID1:
            ECLWarp(3080, 0xb5d8, 380, 0xbacc, 0, 0xbb74, 160, 0xc304);
            ECLTimeFix(0x2380, 0, 6);
            ECLTimeFix(0x2440, 60, 3);
            break;
        case TH08_ST3_MID2:
            ECLWarp(3080, 0xb5d8, 380, 0xbacc, 0, 0xbb74, 160, 0xc304);
            ECLTimeFix(0x2380, 0, 6);
            ECLTimeFix(0x2440, 60, 3);

            ECLSetTime(ecl, 0x245c, 0, 30, 60);
            ecl << ECLX(0xb5de, (int8_t)0x50);
            break;
        case TH08_ST3_BOSS1:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            if (!thPracParam.dlg)
                ecl << ECLX(0xb882, (int8_t)0x2c);
            break;
        case TH08_ST3_BOSS2:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);

            if (!diff)
                break;
            ECLSetTime(ecl, 0x3f04, 0, 44, 60);
            break;
        case TH08_ST3_BOSS3:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);

            ECLSetTime(ecl, 0x3f04, 0, 47, 60);
            ecl << ECLX(0x3ddc, (int16_t)0);
            break;
        case TH08_ST3_BOSS4:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);
            ECLCallSub(ecl, 0x3f04, 38, 60);
            break;
        case TH08_ST3_BOSS5:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);
            ECLCallSub(ecl, 0x3f04, 38, 60);

            switch (diff) {
            case 0:
                sub_id = 50;
                break;
            case 1:
                sub_id = 54;
                break;
            case 2:
                sub_id = 58;
                break;
            case 3:
                sub_id = 62;
                break;
            default:
                sub_id = 75;
            }
            ECLSetTime(ecl, 0x4748, 0, sub_id);
            ecl << ECLX(0x472c, 0);
            break;
        case TH08_ST3_BOSS6:
            ECLWarp(4663, 0xb87c, 0, -1, 0, -1, 0, -1);
            ecl << ECLX(0xb882, (int8_t)0x2c);
            ECLCallSub(ecl, 0x3f04, 38, 60);

            ECLSetTime(ecl, 0x4748, 0, 67);
            ECLJump(ecl, 0x462c, 0x46dc, 0, 0);
            ECLJump(ecl, 0x3d80, 0x3df0, 0, 0);
            ecl << ECLX(0x472c, 0);
            break;
        case TH08_ST3_LS:
            ECLWarp(4663, 0xb8cc, 0, -1, 0, -1, 0, -1);
            ECLTimeFix(0x54bc, 0, 5);
            ECLTimeFix(0x552c, 30, 6);
            ecl << ECLX(0x5530, (int16_t)0);
            ECLCheckTime(8800);
            break;
        case TH08_ST4A_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(4962, 0xb26c, 360, 0xbed4);
            else
                ECLWarp(4962, 0xb280, 360, 0xbed4);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();
            break;
        case TH08_ST4A_BOSS2:
            ECLWarp(4962, 0xb280, 360, 0xbed4);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();

            ECLSetTime(ecl, 0x188c, 0, 28, 60);
            break;
        case TH08_ST4A_BOSS3:
            ECLWarp(4962, 0xb280, 360, 0xbed4);
            ECLTimeFix(0x17d0, 0, 6);
            ECLCallSub(ecl, 0x1870, 19, 0);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();
            break;
        case TH08_ST4A_BOSS4:
            ECLWarp(4962, 0xb280, 360, 0xbed4);
            ECLCallSub(ecl, 0x1870, 19, 60);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();

            ECLSetTime(ecl, 0x22f0, 0, 30, 0);
            break;
        case TH08_ST4A_BOSS5:
            ECLWarp(6923, 0xba50, 0, -1);
            ecl << pair{0x2e10, 0};
            STDJump(0xf48, 56, 6558);
            break;
        case TH08_ST4A_BOSS6:
            ECLWarp(6923, 0xba50, 0, -1);
            STDJump(0xf48, 56, 6558);

            ECLSetTime(ecl, 0x2fa8, 0, 33, 0);
            ecl << pair{0xba56, (int8_t)0x50};
            break;
        case TH08_ST4A_BOSS7:
            ECLWarp(6923, 0xba50, 0, -1);
            ecl << pair{0x2e10, 0} << pair{0x2e1c, 26};
            STDStage4Fix();
            ecl << pair{0xba56, (int8_t)0x50};
            break;
        case TH08_ST4A_BOSS8:
            ECLWarp(6923, 0xba50, 0, -1);
            ecl << pair{0x2e10, 0} << pair{0x2e1c, 26};
            STDStage4Fix();
            ecl << pair{0xba56, (int8_t)0x50};

            ECLSetTime(ecl, 0x3850, 0, 41, 60);
            ecl << pair{0x3764, (int16_t)0};
            ecl << pair{0x3818, 60} << pair{0x3834, 60};
            break;
        case TH08_ST4A_BOSS9:
            ECLWarp(6923, 0xba50, 0, -1);
            ecl << pair{0x2e10, 0} << pair{0x2e1c, 26};
            STDStage4Fix();
            ecl << pair{0xba56, (int8_t)0x50};

            ECLCallSub(ecl, 0x3850, 45, 60);
            ecl << pair{0x3818, 60} << pair{0x3834, 60};
            ecl << pair{0x3764, (int16_t)0} << pair{0x37d4, (int16_t)0}
                << pair{0x7d04, 0} << pair{0x7d10, 0}
                << pair{0x7c68, (int16_t)0} << pair{0x7c78, (int16_t)0};
            break;
        case TH08_ST4A_LS:
            STDStage4Fix();
            ECLWarp(6923, 0xbaac, 0, -1);
            ECLTimeFix(0x4168, 0, 5);
            ECLTimeFix(0x41d8, 30, 6);
            ecl << ECLX(0x41dc, (int16_t)0);
            ECLCheckTime(9999);

            ecl.SetPos(0x41c0);
            ecl << 0 << 0x00180093 << 0x0000ff00 << 2;
            break;
        case TH08_ST4B_BOSS1:
            if (thPracParam.dlg)
                ECLWarp(4962, 0xd4f0, 360, 0xe158);
            else
                ECLWarp(4962, 0xd504, 360, 0xe158);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();
            break;
        case TH08_ST4B_BOSS2:
            ECLWarp(4962, 0xd504, 360, 0xe158);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();

            ECLSetTime(ecl, 0x18d0, 0, 36, 60);
            break;
        case TH08_ST4B_BOSS3:
            ECLWarp(4962, 0xd504, 360, 0xe158);
            ECLTimeFix(0x1814, 0, 5);
            ECLCallSub(ecl, 0x18b4, 22, 0);
            ecl << pair{0x2554, 120} << pair{0x2570, 120};
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();
            break;
        case TH08_ST4B_BOSS4:
            ECLWarp(4962, 0xd504, 360, 0xe158);
            ECLTimeFix(0x1814, 0, 5);
            ECLCallSub(ecl, 0x18b4, 22, 60);
            STDJump(0xf48, 38, 4562);
            STDStage4Fix();

            ECLSetTime(ecl, 0x2570, 0, 41, 0);
            break;
        case TH08_ST4B_BOSS5:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair{0x2f14, 0} << pair{0x3030, 120} << pair{0x304c, 120};
            STDJump(0xf48, 56, 6558);
            break;
        case TH08_ST4B_BOSS6:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair{0x2f14, 0} << pair{0x3030, 60};
            STDJump(0xf48, 56, 6558);

            ecl << pair{0xdcda, (int8_t)0x50};
            ECLSetTime(ecl, 0x304c, 0, 47, 60);
            break;
        case TH08_ST4B_BOSS7:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair{0xdcda, (int8_t)0x50};
            ecl << pair{0x2f14, 0} << pair{0x2f20, 31}
                << pair{0x38b8, 120} << pair{0x38d4, 120};
            STDStage4Fix();
            break;
        case TH08_ST4B_BOSS8:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair{0xdcda, (int8_t)0x50};
            ecl << pair{0x2f14, 0} << pair{0x2f20, 31}
                << pair{0x38b8, 120} << pair{0x38d4, 120};
            STDStage4Fix();

            ECLSetTime(ecl, 0x38d4, 0, 55, 60);
            ecl << pair{0x38b8, 60};
            break;
        case TH08_ST4B_BOSS9:
            ECLWarp(6923, 0xdcd4, 0, -1);
            ecl << pair{0xdcda, (int8_t)0x50};
            ecl << pair{0x2f14, 0} << pair{0x2f20, 31};
            STDStage4Fix();

            ECLCallSub(ecl, 0x38d4, 61, 60);
            ecl << pair{0x38b8, 60}
                << pair{0x8b48, 0} << pair{0x8b54, 0} << pair{0x8b68, 0}
                << pair{0x8aac, (int16_t)0} << pair{0x8abc, (int16_t)0}
                << pair{0x3888, (int16_t)0};
            break;
        case TH08_ST4B_LS:
            STDStage4Fix();
            ECLWarp(6923, 0xdd30, 0, -1);
            ECLTimeFix(0x4374, 0, 5);
            ECLTimeFix(0x43e4, 30, 6);
            ecl << ECLX(0x43e8, (int16_t)0);
            ECLCheckTime(8500);

            ecl.SetPos(0x43cc);
            ecl << 0 << 0x00180093 << 0x0000ff00 << 2;
            break;
        case TH08_ST5_MID1:
            ECLWarp(4530, 0xb0bc);
            ECLTimeFix(0x3704, 0, 6);
            ECLTimeFix(0x37c4, 60, 3);
            break;
        case TH08_ST5_MID2:
            ECLWarp(4530, 0xb0bc);
            ECLTimeFix(0x3704, 0, 9);
            ecl << pair{0x37c8, (int16_t)0} << pair{0x37ec, 45};
            ECLJump(ecl, 0x4360, 0x437c, 119, 0);
            ECLTimeFix(0x437c, 120, 4);
            ECLSetHealth(ecl, 0x37e0, 0, 1);
            break;
        case TH08_ST5_BOSS1:
            MSGNameFix();
            if (thPracParam.dlg)
                ECLWarp(7481, 0xb788);
            else {
                ECLWarp(7481, 0xb79c);
                ecl << pair{0xb7a2, (int8_t)0x78};
                ECLCallSub(ecl, 0x4c6c, 51);
            }
            break;
        case TH08_ST5_BOSS2:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair{0xb7a2, (int8_t)0x78};
            ECLCallSub(ecl, 0x4c6c, 51);

            ECLSetTime(ecl, 0x4f78, 0, 62, 60);
            break;
        case TH08_ST5_BOSS3:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair{0xb7a2, (int8_t)0x78};
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 53, 0);
            ECLTimeFix(0x5214, 60, 9);
            ecl << pair{0x52cc, (int16_t)0};
            break;
        case TH08_ST5_BOSS4:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair{0xb7a2, (int8_t)0x78};
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 53, 0);
            ECLTimeFix(0x5214, 60, 9);
            ecl << pair{0x52cc, (int16_t)0};

            ECLSetTime(ecl, 0x52d8, 0, 66, 60);
            break;
        case TH08_ST5_BOSS5:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair{0xb7a2, (int8_t)0x78};
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 56, 0);
            ECLTimeFix(0x5744, 60, 11);
            ecl << pair{0x5830, (int16_t)0};
            break;
        case TH08_ST5_BOSS6:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair{0xb7a2, (int8_t)0x78};
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 56, 0);
            ECLTimeFix(0x5744, 60, 11);
            ecl << pair{0x5830, (int16_t)0};

            ECLSetTime(ecl, 0x583c, 0, 63, 60);
            break;
        case TH08_ST5_BOSS7:
            MSGNameFix();
            ECLWarp(7481, 0xb79c);
            ecl << pair{0xb7a2, (int8_t)0x78};
            ECLCallSub(ecl, 0x4c6c, 51);
            ECLTimeFix(0x4ebc, 0, 5);
            ECLCallSub(ecl, 0x4f5c, 56, 0);
            ECLTimeFix(0x5744, 60, 11);
            ecl << pair{0x5830, (int16_t)0};

            ECLSetTime(ecl, 0x583c, 0, 75, 60);
            ecl << pair{0x5798, (int16_t)0} << pair{0x57b8, (int16_t)0};
            break;
        case TH08_ST5_LS:
            MSGNameFix();
            ECLWarp(7484, 0xb820);
            ECLTimeFix(0x5e24, 0, 5);
            ECLTimeFix(0x5e94, 30, 6);
            ecl << ECLX(0x5e98, (int16_t)0);
            ECLCheckTime(9999);
            break;
        case TH08_ST6A_MID1:
            ECLWarp(3400, 0xc7d4);
            if (!thPracParam.dlg) {
                ecl << pair{0xc7da, (int8_t)0x44};
                ECLTimeFix(0x1de4, 0, 6);
                ecl << pair{0x1ea4, 60} << pair{0x1dd4, (int16_t)0};
                ECLJump(ecl, 0x1ec0, 0x1ee0, 0, 60);
            }
            break;
        case TH08_ST6A_MID2:
            ECLWarp(3400, 0xc7d4);
            ecl << pair{0xc7da, (int8_t)0x44};
            ECLTimeFix(0x1de4, 0, 6);
            ecl << pair{0x1ea4, 60} << pair{0x1dd4, (int16_t)0};
            ECLJump(ecl, 0x1ec0, 0x1ee0, 0, 60);

            ECLSetTime(ecl, 0x1fdc, 0, 21);
            break;
        case TH08_ST6A_BOSS1:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            if (thPracParam.dlg)
                ECLWarp(4022, 0xc824);
            else {
                ECLWarp(4022, 0xc838);
                ecl << pair{0xc83e, (int8_t)0x38};
                ECLCallSub(ecl, 0x367c, 33);
            }
            break;
        case TH08_ST6A_BOSS2:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair{0xc83e, (int8_t)0x38};
            ECLCallSub(ecl, 0x367c, 33);

            ECLSetTime(ecl, 0x3de4, 0, 52, 60);
            break;
        case TH08_ST6A_BOSS3:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair{0xc83e, (int8_t)0x38};
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 36);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x454c, 60, 8);
            break;
        case TH08_ST6A_BOSS4:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair{0xc83e, (int8_t)0x38};
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 36);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x454c, 60, 8);

            ECLSetTime(ecl, 0x4600, 0, 56, 60);
            break;
        case TH08_ST6A_BOSS5:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair{0xc83e, (int8_t)0x38};
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 40);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x4c98, 60, 8);
            break;
        case TH08_ST6A_BOSS6:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair{0xc83e, (int8_t)0x38};
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 40);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x4c98, 60, 8);

            ECLSetTime(ecl, 0x4d4c, 0, 63, 60);
            break;
        case TH08_ST6A_BOSS7:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair{0xc83e, (int8_t)0x38};
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 44);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x5534, 60, 8);
            break;
        case TH08_ST6A_BOSS8:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4022, 0xc838);
            ecl << pair{0xc83e, (int8_t)0x38};
            ECLCallSub(ecl, 0x367c, 33);
            ECLCallSub(ecl, 0x3dc8, 44);
            ECLTimeFix(0x3d28, 0, 6);
            ECLTimeFix(0x5534, 60, 8);

            ECLSetTime(ecl, 0x55e8, 0, 68, 60);
            break;
        case TH08_ST6A_BOSS9: {
            constexpr unsigned int st6AFirstBossCreateTime = 4022;
            constexpr unsigned int st6AFirstBossCreate = 0xc838;
            constexpr unsigned int st6AFirstBossCreateNextInstOffset = 0xc838 + 0x6;
            constexpr unsigned int st6AFirstBossFirstSubEnd = 0x367c;
            constexpr unsigned int st6AFirstBossSecondSubSetLifeThresholdIns = 0x3cd8;
            constexpr unsigned int st6AFirstBossSecondSubMoveLimits = 0x3dc8;
            constexpr unsigned int st6AFirstBossSecondSubSecondCardEffect = 0x3d28; //why
            constexpr unsigned int st6ABossPreSp9SubMoveLimits = 0x9a44;
            constexpr unsigned int st6ABossPreSp9DropItemsIns = 0x99f8;
            constexpr unsigned int st6ABossPreSp9DropPointItemsIns = 0x9a08;

            MSGNameFix();
            ECLWarp(st6AFirstBossCreateTime, st6AFirstBossCreate); // main timeline warp (NOTE: using SecondBossCreateTime here leads to LS warp + dialogue)
            ecl << pair { st6AFirstBossCreateNextInstOffset, (int8_t)(0x20 + 0x18) }; // skips ahead 24 bytes more than usual, skipping dialogue wait & event

            ECLCallSub(ecl, st6AFirstBossFirstSubEnd, 33);         // forces the next sub to trigger without waiting for ecl time (i think?)
            ECLCallSub(ecl, st6AFirstBossSecondSubMoveLimits, 72); // call pre-Spell 9 sub
            ECLTimeFix(st6AFirstBossSecondSubSecondCardEffect, 0, 6); // removes time offsets set in next 6 instructions
            ECLTimeFix(st6ABossPreSp9SubMoveLimits, 60, 6);           // adds the removed time offset to the next 6 instructions?

            ecl << pair { st6AFirstBossSecondSubSetLifeThresholdIns, (int16_t)0 } // NOPs setting the life threshold
                << pair { st6ABossPreSp9DropItemsIns, (int16_t)0 }                // NOPs dropping items from (fake) non
                << pair { st6ABossPreSp9DropPointItemsIns, (int16_t)0 };          // NOPs dropping point items from (fake) non
            break;

        } case TH08_ST6A_LS: {
            constexpr unsigned int st6ATLPreKaguyaDlgTime = 4023;
            constexpr unsigned int st6ATLPreKaguyaDlg = 0xc87c;
            constexpr unsigned int st6ATLKaguyaCreateTime = 4083;
            constexpr unsigned int st6ATLKaguyaCreate = 0xc89c;

            constexpr unsigned int st6ATLEirinCreate = 0xc8e0;
            constexpr unsigned int st6ATLPostEirinCreateWaitEnemy = 0xc900;
            constexpr unsigned int st6ATLPostEirinCreateReadMsg = 0xc90c;
            constexpr unsigned int st6ATLPostEirinCreateWaitMsg = 0xc918;

            //note: Kaguya prepares stuff in Sub29/30
            constexpr unsigned int st6AKaguyaFirstSubEffectCall = 0x375c;
            constexpr unsigned int st6AKaguyaSecondSubEffectCall = 0x3914;
            constexpr unsigned int st6AKaguyaPreEirinCreationTimedInstr = 0x3844;
            constexpr unsigned int st6AKaguyaScreenFlashFuncCall = 0x3934;
            constexpr unsigned int st6AKaguyaPostEirinCreationTimedInstr = 0x3948;

            //note: HE is Sub78
            constexpr unsigned int st6BossSp10TimerThreshold = 0xac60;
            constexpr unsigned int st6BossSp10FirstAttackNOP = 0xb244;
            constexpr unsigned int st6BossSp10FirstAttack = 0xb250;
            constexpr unsigned int st6BossSp10PostSpin = 0xb2bc;
            constexpr unsigned int st6BossSp10PostSpinEtCancel = 0xb2f8;

            //note: HE post-spin is async Sub84
            constexpr unsigned int st6BossSp10FinalPhasePostSetup = 0xbf68;
            constexpr unsigned int st6BossSp10FinalPhaseEtEx1 = 0xc078;
            constexpr unsigned int st6BossSp10FinalPhasePostEtEx1 = 0xc0a0;
            constexpr unsigned int st6BossSp10FinalPhaseSlowdownStart = 0xc228;

            MSGNameFix();
            scene.globals.stage_interrupt = 5; // manually set correct STD interrupt
            ecl << pair { st6AKaguyaFirstSubEffectCall + ECL_INS_OPCODE, (int16_t)0 }; // NOPs a pre-existing effect sub call (when Kaguya spawns)

            if (thPracParam.dlg) {
                ECLWarp(st6ATLPreKaguyaDlgTime, st6ATLPreKaguyaDlg); // main timeline warp (to pre-Kaguya dialogue)
            } else {
                ECLWarp(st6ATLKaguyaCreateTime, st6ATLKaguyaCreate); // main timeline warp (to Kaguya creation)
                ECLTimeFix(st6AKaguyaPreEirinCreationTimedInstr, 0, 10); // all these need to happen before Eirin is created, but we skip timeline wait that ensures it
                ECLTimeFix(st6AKaguyaPostEirinCreationTimedInstr, 1, 2); // done to prevent a crash if the spell is cancelled while Kaguya isn't boss

                ecl << pair { st6ATLKaguyaCreate + ECL_TL_OFFNEXT, (int8_t)(0x20 + 0x8) } // skips ahead 0xc bytes more than usual, skipping dialogue wait
                    << pair { st6AKaguyaSecondSubEffectCall + 0x4, (int16_t)0 } // NOPs a pre-existing effect sub call (timed w/ music start)
                    << pair { st6AKaguyaScreenFlashFuncCall + 0x4, (int16_t)0 } // NOPs calling hardcoded function 17 which does a white flash
                    << pair { st6ATLEirinCreate + ECL_TL_TIME,              st6ATLKaguyaCreateTime + 1 } // lowers timeline instruction's time offset to run immediately
                    << pair { st6ATLPostEirinCreateWaitEnemy + ECL_TL_TIME, st6ATLKaguyaCreateTime + 1 }  // lowers timeline instruction's time offset to run immediately
                    << pair { st6ATLPostEirinCreateReadMsg + ECL_TL_TIME,   st6ATLKaguyaCreateTime + 1 }  // lowers timeline instruction's time offset to run immediately
                    << pair { st6ATLPostEirinCreateWaitMsg + ECL_TL_TIME,   st6ATLKaguyaCreateTime + 1 }; // lowers timeline instruction's time offset to run immediately
            }

            if (thPracParam.phase) {
                // remove skipped time from spell duration: 5940 (original) minus 2920-200 (skipped time in Sub78) = 3220
                ecl << pair { st6BossSp10TimerThreshold + ECL_INS_ARG1, (int16_t)3220 };
                ECLTimeFix(st6BossSp10FirstAttackNOP, 200, 1); // make NOP run without waiting in order to trigger jump
                ECLJump(ecl, st6BossSp10FirstAttack, st6BossSp10PostSpin, 2920, 200); // jump to post-spin logic

                if (thPracParam.phase == 2) {
                    // remove skipped time & waits from spell duration: 5940 - (2920 - 200) = 3220 (cf. above), minus...
                    // post spin parts force a repeated wait: wait = sum_{i=0}^{cnt-1} (start - i * sub) = cnt*start - sub*(cnt*(cnt-1))/2
                    // first post spin part has cnt = 13 (12 + 1 loops), start = 60, sub = 2   => wait1 = 624
                    // second post spin part has cnt = 11 (10 + 1 loops), start = 80, sub = 6  => wait2 = 550
                    // we also have instruction time waits between each part which amount to 460 - 36 - 20 (considering the spell time value which is reset on each loop, then incremented by wait)
                    // spell duration accounting for skips: 5940 - (2920 - 200) - (550 + 624) - (460 - 36 - 20) = 3220 - 1578 = 1642 frames
                    ecl << pair { st6BossSp10TimerThreshold + ECL_INS_ARG1, (int16_t)1642 }
                        << pair { st6BossSp10PostSpinEtCancel + ECL_INS_OPCODE, (int16_t)0 }; // NOP etCancel
                    ECLJump(ecl, st6BossSp10FinalPhasePostSetup, st6BossSp10FinalPhaseEtEx1, 260); // jump to otherwise missed etEx
                    ECLJump(ecl, st6BossSp10FinalPhasePostEtEx1, st6BossSp10FinalPhaseSlowdownStart, 460, 260); // jump to slowdown phase
                }
            }
            break;

        } case TH08_ST6B_MID1:
            ECLWarp(3490, 0x104e4);
            if (!thPracParam.dlg) {
                ecl << pair{0x104ea, (int8_t)0x44};
                ECLTimeFix(0x1bfc, 0, 6);
                ecl << pair{0x1cbc, 60} << pair{0x1bec, (int16_t)0};
                ECLCallSub(ecl, 0x1cd8, 15, 60);
            }
            break;
        case TH08_ST6B_MID2:
            ECLWarp(3490, 0x104e4);
            ecl << pair{0x104ea, (int8_t)0x44};
            ECLTimeFix(0x1bfc, 0, 6);
            ecl << pair{0x1cbc, 60} << pair{0x1bec, (int16_t)0};
            ECLCallSub(ecl, 0x1cd8, 15, 60);

            ECLSetTime(ecl, 0x1df4, 0, 19);
            break;
        case TH08_ST6B_BOSS1:
            scene.globals.stage_interrupt = 1;
            if (thPracParam.dlg) {
                ECLWarp(4112, 0x10534);
            } else {
                MSGNameFix();
                ECLWarp(4112, 0x10548);
                ecl << pair{0x1054e, (int8_t)0x38};
                ECLCallSub(ecl, 0x3a28, 27);
            }
            break;
        case TH08_ST6B_BOSS2:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair{0x1054e, (int8_t)0x38};
            ECLCallSub(ecl, 0x3a28, 27);

            ECLSetTime(ecl, 0x3c24, 0, 50, 60);
            break;
        case TH08_ST6B_BOSS3:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair{0x1054e, (int8_t)0x38};
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 31);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x4370, 60, 8);
            break;
        case TH08_ST6B_BOSS4:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair{0x1054e, (int8_t)0x38};
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 31);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x4370, 60, 8);

            ECLSetTime(ecl, 0x4424, 0, 55, 60);
            break;
        case TH08_ST6B_BOSS5:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair{0x1054e, (int8_t)0x38};
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 34);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x4c14, 60, 9);
            break;
        case TH08_ST6B_BOSS6:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair{0x1054e, (int8_t)0x38};
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 34);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x4c14, 60, 9);

            ECLSetTime(ecl, 0x4cec, 0, 61, 60);
            break;
        case TH08_ST6B_BOSS7:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair{0x1054e, (int8_t)0x38};
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 38);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x5570, 60, 8);
            break;
        case TH08_ST6B_BOSS8:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair{0x1054e, (int8_t)0x38};
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 38);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0x5570, 60, 8);

            ECLSetTime(ecl, 0x5624, 0, 67, 60);
            break;
        case TH08_ST6B_BOSS9:
            MSGNameFix();
            ECLWarp(4112, 0x10548);
            ecl << pair{0x1054e, (int8_t)0x38};
            ECLCallSub(ecl, 0x3a28, 27);

            ECLCallSub(ecl, 0x3c08, 70);
            ECLTimeFix(0x3b68, 0, 6);
            ECLTimeFix(0xa920, 60, 7);
            ecl << pair{0x3b18, (int16_t)0}
                << pair{0xa8e4, (int16_t)0} << pair{0xa8d4, (int16_t)0};
            break;
        case TH08_ST6B_LS1:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4234, 0x105ac);
            break;
        case TH08_ST6B_LS2:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4235, 0x105ec);
            break;
        case TH08_ST6B_LS3:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4236, 0x1062c);
            break;
        case TH08_ST6B_LS4:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4237, 0x1066c);
            break;
        case TH08_ST6B_LS5:
            scene.globals.stage_interrupt = 1;
            MSGNameFix();
            ECLWarp(4238, 0x106ac);
            break;
        case TH08_ST7_MID1:
            ECLWarp(5155, 0x13268, 810, 0x15d60);
            if (!thPracParam.dlg) {
                ecl << pair{0x1326e, (int8_t)0x44} << pair{0x3e10, 60};
                ECLTimeFix(0x3d50, 0, 6);
                ECLCallSub(ecl, 0x3e2c, 51, 60);
            }
            break;
        case TH08_ST7_MID2:
            ECLWarp(5155, 0x13268, 810, 0x15d60);
            ecl << pair{0x1326e, (int8_t)0x44};
            ECLTimeFix(0x3d50, 0, 6);
            ECLCallSub(ecl, 0x3e10, 55);
            ECLTimeFix(0x4b34, 60, 6);
            break;
        case TH08_ST7_MID3:
            ECLWarp(5155, 0x13268, 810, 0x15d60);
            ecl << pair{0x1326e, (int8_t)0x44};
            ECLTimeFix(0x3d50, 0, 6);
            ECLCallSub(ecl, 0x3e10, 59);
            ECLTimeFix(0x5418, 60, 6);
            break;
        case TH08_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLWarp(9497, 0x152b4, 0, -1);
            else {
                ECLWarp(9497, 0x152c8, 0, -1);
                MSGNameFix();
                ecl << pair{0x152ce, (int8_t)0x38};
                ECLCallSub(ecl, 0x65c4, 66);
            }
            break;
        case TH08_ST7_END_S1:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);

            ECLSetTime(ecl, 0x67b0, 0, 93, 60);
            break;
        case TH08_ST7_END_NS2:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 68, 60);
            ecl << pair{0x6aac, (int16_t)0};
            break;
        case TH08_ST7_END_S2:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 68, 60);
            ecl << pair{0x6aac, (int16_t)0};

            ECLSetTime(ecl, 0x6b9c, 0, 97);
            break;
        case TH08_ST7_END_NS3:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 70, 60);
            ecl << pair{0x6e74, (int16_t)0};
            break;
        case TH08_ST7_END_S3:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 70, 60);
            ecl << pair{0x6e74, (int16_t)0};

            ECLSetTime(ecl, 0x6f64, 0, 100);
            break;
        case TH08_ST7_END_NS4:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 72, 60);
            ecl << pair{0x724c, (int16_t)0};
            ecl << pair{0x71dc, (int16_t)0};
            break;
        case TH08_ST7_END_S4:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 72, 60);
            ecl << pair{0x724c, (int16_t)0};
            ecl << pair{0x71dc, (int16_t)0};

            ECLSetTime(ecl, 0x733c, 0, 103);
            break;
        case TH08_ST7_END_NS5:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 74, 60);
            ecl << pair{0x7548, (int16_t)0};
            break;
        case TH08_ST7_END_S5:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 74, 60);
            ecl << pair{0x7548, (int16_t)0};

            ECLSetTime(ecl, 0x7638, 0, 108);
            break;
        case TH08_ST7_END_NS6:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x6794, 76, 0);
            ecl << pair{0x7844, (int16_t)0};
            ECLTimeFix(0x7810, 60, 15);
            break;
        case TH08_ST7_END_S6:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x6794, 76, 0);
            ecl << pair{0x7844, (int16_t)0};
            ECLTimeFix(0x7810, 60, 15);

            ECLSetTime(ecl, 0x7944, 0, 113, 60);
            break;
        case TH08_ST7_END_NS7:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x6794, 78, 0);
            ecl << pair{0x7bcc, (int16_t)0};
            ECLTimeFix(0x7b98, 60, 15);
            break;
        case TH08_ST7_END_S7:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x6794, 78, 0);
            ecl << pair{0x7bcc, (int16_t)0};
            ECLTimeFix(0x7b98, 60, 15);

            ECLSetTime(ecl, 0x7ccc, 0, 118, 60);
            break;
        case TH08_ST7_END_NS8:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x6794, 80, 0);
            ecl << pair{0x7f44, (int16_t)0};
            ECLTimeFix(0x7f10, 60, 15);
            break;
        case TH08_ST7_END_S8:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ECLTimeFix(0x66f4, 0, 5);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x6794, 80, 0);
            ecl << pair{0x7f44, (int16_t)0};
            ECLTimeFix(0x7f10, 60, 15);

            ECLSetTime(ecl, 0x8044, 0, 126, 60);
            break;
        case TH08_ST7_END_S9:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ECLCallSub(ecl, 0x67b0, 82, 60);

            ECLJump(ecl, 0x95f4, 0x98c4, 10);
            ECLJump(ecl, 0x98dc, 0x9910, 130, 10);
            break;
        case TH08_ST7_END_S10:
            MSGNameFix();
            ECLWarp(9497, 0x152c8, 0, -1);
            ecl << pair{0x152ce, (int8_t)0x38};
            ECLCallSub(ecl, 0x65c4, 66);
            ecl << pair{0x66a4, (int16_t)0};
            ecl.SetPos(0x67b0);
            ecl << 60 << 0x0014004e << 0x0000ff00 << 0x43800000 << 0x42000000;
            ECLCallSub(ecl, 0x67c4, 83, 60);
            ecl << pair{0x8408, (int16_t)0};
            break;

        case TH08_ST7_END_LS: { // Imperishable Shooting
            constexpr unsigned int st7TLPreLSTime = 9678;
            constexpr unsigned int st7TLPreLS = 0x1530c;
            constexpr unsigned int st7BossSp11PreTimeCheck = 0x851c;
            constexpr unsigned int st7BossSp11PostTimeCheck = 0x8544;

            constexpr unsigned int st7BossSp11TimerThreshold = 0x0fc88;
            constexpr unsigned int st7BossSp11Wave1Particles = 0x0fe74;
            constexpr unsigned int st7BossSp11Wave1ShotColor = 0x0fec0;
            constexpr unsigned int st7BossSp11Wave1PostColor = 0x0fed4;

            constexpr unsigned int st7BossSp11Wave2Start = 0xff0c;
            constexpr unsigned int st7BossSp11Wave3Start = 0x10050;
            constexpr unsigned int st7BossSp11Wave4Start = 0x101f0;
            constexpr unsigned int st7BossSp11Wave5Start = 0x104fc;
            constexpr unsigned int st7BossSp11Wave6Start = 0x10a10;
            constexpr unsigned int st7BossSp11Wave7Start = 0x10d1c;

            MSGNameFix();
            ECLWarp(st7TLPreLSTime, st7TLPreLS, 0, -1);
            ECLJump(ecl, st7BossSp11PreTimeCheck, st7BossSp11PostTimeCheck);
            if (!thPracParam.phase) break;
            ecl << pair{ st7BossSp11Wave1Particles + ECL_INS_OPCODE, (int16_t)0 }; // NOP wave1 particles
            //Note: each particles call (sub152) is 32f counted for seconds timer but not ECL time; we deduct skipped ones from threshold

            switch (thPracParam.phase) {
            case 1: // two circles (69s left)
                ECLJump(ecl, st7BossSp11Wave1PostColor, st7BossSp11Wave2Start, 540, 90); // (skip 540-90 = 450f)
                ecl << pair{ st7BossSp11TimerThreshold + ECL_INS_ARG1, (int16_t)4258 };  // (base 4740f - 450f - 1 * 32f)
                break;

            case 2: // triangular circles (61s left)
                ECLJump(ecl, st7BossSp11Wave1PostColor, st7BossSp11Wave3Start, 1000, 90); // (skip 1000-90 = 910f)
                ecl << pair{ st7BossSp11TimerThreshold + ECL_INS_ARG1, (int16_t)3798 }    // (base 4740f - 910f - 1 * 32f)
                    << pair{ st7BossSp11Wave1ShotColor + ECL_INS_ARG2, 2}; // (red)
                break;

            case 3: // line of circles (52s left)
                ECLJump(ecl, st7BossSp11Wave1PostColor, st7BossSp11Wave4Start, 1460, 90); // (skip 1460-90 = 1370f)
                ecl << pair{ st7BossSp11TimerThreshold + ECL_INS_ARG1, (int16_t)3306 };   // (base 4740f - 1370f - 2 * 32f)
                break;

            case 4: // thick circles (36s left)
                ECLJump(ecl, st7BossSp11Wave1PostColor, st7BossSp11Wave5Start, 2280, 90); // (skip 2280-90 = 2190f)
                ecl << pair{ st7BossSp11TimerThreshold + ECL_INS_ARG1, (int16_t)2326 }    // (base 4740f - 2190f - 7 * 32f)
                    << pair{ st7BossSp11Wave1ShotColor + ECL_INS_ARG2, 2 }; // (red)
                break;

            case 5: // green rice circle (22s left)
                ECLJump(ecl, st7BossSp11Wave1PostColor, st7BossSp11Wave6Start, 3040, 90); // (skip 3040-90 = 2950f)
                ecl << pair{ st7BossSp11TimerThreshold + ECL_INS_ARG1, (int16_t)1502 };   // (base 4740f - 2950f - 9 * 32f)
                break;

            case 6: // finale (14s left)
                ECLJump(ecl, st7BossSp11Wave1PostColor, st7BossSp11Wave7Start, 3410, 90); // (skip 3410-90 = 3320f)
                ecl << pair{ st7BossSp11TimerThreshold + ECL_INS_ARG1, (int16_t)1020 };   // (base 4740f - 3320f - 10 * 32f - 5 * 16f) (phase 6 green shooter subs cost 16f)
                break;
            }

            break;
        }

        default:
            break;
        }
    }
