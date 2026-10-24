// Generated from thprac TH11 (MIT), sha256 6236d13b9c62ac981fc5bb625a81d0e93553b67bf64a2095fd4d6bd1c7a4abc0.
// Only Win32 buffer ownership and bounds protection are adapted.
    void* THStage6STD()
    {
        void* buffer = std_data.data();

        VFile std;
        std.SetFile(buffer, std_data.size());
        std.SetPos(0x328);
        std << 0 << 0x0010000e << 0 << 2
            << 0 << 0x00140008 << 0xff000000 << 0x44480000 << 0x44960000
            << 0 << 0x000c0011 << 2
            << 0 << 0x00140002 << 0 << 0xc4e04000 << 0xc4960000
            << 0x8000 << 0x00100001 << 0x234 << 0
            << 0xffffffff << 0xffffffff << 0xffffffff << 0xffffffff << 0xffffffff;

        auxiliary_valid = auxiliary_valid && std.valid;
        return nullptr;
    }
    void* THStage6ANM()
    {
        void* buffer = anm_data.data();

        VFile anm;
        anm.SetFile(buffer, anm_data.size());
        anm.SetPos(0x602ac);
        anm << 0x3f800000 << 0x3f800000;
        anm.SetPos(0x602c4);
        anm << 0x3f800000 << 0x3f800000;

        auxiliary_valid = auxiliary_valid && anm.valid;
        return nullptr;
    }

    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int delay = 0, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << delay;
    }
    void ECLTimeFix(ECLHelper& ecl, size_t pos, int32_t delta_time, unsigned int ins_count = 0xFFFFFFFF)
    {
        ecl.SetPos(pos);
        int32_t ecl_time;
        int16_t ecl_ins;
        int16_t ecl_length;
        size_t p = pos;
        for (unsigned int i = 0; i < ins_count && ecl.valid; i++) {
            ecl >> ecl_time;
            ecl >> ecl_ins;
            ecl >> ecl_length;
            if (ecl_time == 0x484c4345)
                break;
            if ((ecl_time + delta_time) >= 0)
                ecl_time += delta_time;
            ecl.SetPos(p);
            ecl << ecl_time;
            if (ecl_length < 16 || ecl_length % 4) { ecl.valid = false; break; }
            p += ecl_length;
            ecl.SetPos(p);
        }
    }
    void ECLSetHealth(ECLHelper& ecl, size_t pos, int32_t health, int32_t ecl_time = 0, bool stall = true)
    {
        ecl.SetPos(pos);
        ecl << ecl_time << 0x0014014b << 0x01ff0000 << 0 << health;
        if (stall)
            ecl << 9999 << 0x00100000 << 0x00ff0000 << 0;
    }
    template<typename... Args>
    void ECLVoid(ECLHelper& ecl, size_t pos, Args... rest)
    {
        ecl.SetPos(pos + 4);
        ecl << (int16_t)0;
        if constexpr (sizeof...(Args) != 0) {
            ECLVoid(ecl, rest...);
        }
    }
    void ECLSatoriJump(ECLHelper& ecl, int shot_type)
    {
        ECLJump(ecl, 0xf764, 0xfa04);
        ecl << pair{0x1108, 0x10} << pair{0x1110, 0x64726143} << pair{0x1114, 0x67734d32} << pair{0x1118, 0};
        ecl << pair{0x10fe, (int16_t)0x2c};
        ECLVoid(ecl, 0x5568, 0x55b4, 0x5704, 0x5618, 0x5664);
        switch (shot_type) {
        case 0:
            ECLJump(ecl, 0x5820, 0x5870);
            break;
        case 1:
            ECLJump(ecl, 0x5820, 0x58fc);
            break;
        case 2:
            ECLJump(ecl, 0x5820, 0x5988);
            break;
        case 3:
            ECLJump(ecl, 0x5820, 0x5a14);
            break;
        case 4:
            ECLJump(ecl, 0x5820, 0x5aa0);
            break;
        case 5:
            ECLJump(ecl, 0x5820, 0x5b2c);
            break;
        default:
            break;
        }
    }
    void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x10a90, 0x10b28, 60, 90);
                break;
            case 3:
                ECLJump(ecl, 0x10a90, 0x10bf4, 60, 90);
                break;
            case 4:
                ECLJump(ecl, 0x10a90, 0x10c8c, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xf1b4, 0xf218, 160, 90);
                ecl << pair{0xb608, 0};
                break;
            case 3:
                ECLJump(ecl, 0xf1b4, 0xf2c4, 160, 90);
                break;
            case 4:
                ECLJump(ecl, 0xf1b4, 0xf308, 160, 90);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x14ae8, 0x14b18, 160, 90);
                ecl << pair{0xf370, 0};
                break;
            case 3:
                ECLJump(ecl, 0xb34c, 0xb3d8);
                ECLJump(ecl, 0xb458, 0xdf74, 66);
                ECLJump(ecl, 0xdfd0, 0xe124, 66);
                ECLJump(ecl, 0x14ae8, 0x14ba0, 160);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf7f4);
                break;
            case 3:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf848);
                break;
            case 4:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf8d0);
                ecl << pair{0xa988, 0};
                break;
            case 5:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf8f0);
                break;
            case 6:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf978);
                break;
            case 7:
                ecl << pair{0xf774, 90} << pair{0xf788, 0};
                ECLJump(ecl, 0xf78c, 0xf9ac);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x67dc);
                ecl << pair{0x67a4, 30};
                break;
            case 3:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x6810);
                ecl << pair{0x67a4, 30};
                break;
            case 4:
                if (thPracParam.phase == 1) {
                    ECLJump(ecl, 0x2BF8, 0x2AB0, 0, 0);
                }
                ecl << pair { 0x2A80, 40 - thPracParam.wave_passed };
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x6878);
                break;
            case 5:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x68ac);
                break;
            case 6:
                ecl << pair{0x6790, 0};
                ECLJump(ecl, 0x67a8, 0x68e0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x4114);
                break;
            case 3:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x4148);
                break;
            case 4:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x41f8);
                break;
            case 5:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x422c);
                break;
            case 6:
                ecl << pair{0x40c8, 0};
                ECLJump(ecl, 0x40e0, 0x4260);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4c48);
                break;
            case 3:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4c7c);
                break;
            case 4:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4ce4);
                break;
            case 5:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4d7c);
                break;
            case 6:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4db0);
                break;
            case 7:
                ecl << pair{0x4c10, 90};
                ECLJump(ecl, 0x4c14, 0x4de4);
                break;
            default:
                break;
            }
        }
    }
    void THPatch(ECLHelper& ecl, int section)
    {
        auto st3_boss = [&]() {
            constexpr unsigned int st3PreMainSub = 0x14ae8;
            constexpr unsigned int st3MidBossCreate = 0x14ba0;
            constexpr unsigned int st3MidBossDeathWait = 0x14bd0;
            constexpr unsigned int st3BossDeathWait = 0x14c8c;

            constexpr unsigned int st3MidBossTimeoutInterrupt = 0xb34c;
            constexpr unsigned int st3MidBossKillInterrupt = 0xb374;
            constexpr unsigned int st3MidBossLifeMarker1 = 0xb3a0;
            constexpr unsigned int st3MidBossLifeMarker2 = 0xb3bc;
            constexpr unsigned int st3MidBossMovePosTime = 0xb404;
            constexpr unsigned int st3BossStart = 0xf44;

            ECLJump(ecl, st3PreMainSub, st3MidBossCreate, 160);
            ECLJump(ecl, st3MidBossDeathWait, st3BossDeathWait);
            ECLJump(ecl, st3MidBossMovePosTime, st3BossStart);
            ECLVoid(ecl, st3MidBossTimeoutInterrupt, st3MidBossKillInterrupt);
            ECLVoid(ecl, st3MidBossLifeMarker1, st3MidBossLifeMarker2);
        };
        switch (section) {
        case TH11_ST1_MID1:
            ECLJump(ecl, 0x10a90, 0x10bb0);
            ecl << pair{0x10bb0, 0};
            break;
        case TH11_ST1_MID2:
            ECLJump(ecl, 0x10a90, 0x10bb0);
            ecl << pair{0x10bb0, 0}
                << pair{0x7214, 8600} << pair{0x7228, 5200};
            ECLVoid(ecl, 0x7714, 0x7818);
            break;
        case TH11_ST1_MID3_EN:
        case TH11_ST1_MID3_HL:
            ECLJump(ecl, 0x10a90, 0x10bb0);
            ECLJump(ecl, 0x10a90, 0x10bb0);
            ecl << pair{0x10bb0, 0}
                << pair{0x7214, 4300} << pair{0x7228, 900}
                << pair{0x72d0, 4300} << pair{0x7348, 900};
            ECLVoid(ecl, 0x7714, 0x7818);
            ECLVoid(ecl, 0x7714, 0x7e78, 0x8570, 0x7f54);
            break;
        case TH11_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x10a90, 0x10d68);
            else {
                ECLJump(ecl, 0x10a90, 0x10d8c);
                ecl << pair{0x10d8c, 0};
                ECLVoid(ecl, 0x9bc);
                stage_section = 24;
            }
            break;
        case TH11_ST1_BOSS2:
            ECLJump(ecl, 0x10a90, 0x10d8c);
            ecl << pair{0x10d8c, 0} << pair{0x7dc, 1500};
            ECLVoid(ecl, 0x1fac);
            stage_section = 24;
            break;
        case TH11_ST1_BOSS3:
            ECLJump(ecl, 0x10a90, 0x10d8c);
            ecl << pair{0x10d8c, 0} << pair{0x8e0, (int8_t)0x32};
            ECLTimeFix(ecl, 0x828, -60);
            ECLVoid(ecl, 0x838, 0x1584, 0x1594, 0x15f8);
            stage_section = 24;
            break;
        case TH11_ST1_BOSS4:
            ECLJump(ecl, 0x10a90, 0x10d8c);
            ecl << pair{0x10d8c, 0} << pair{0x8e0, (int8_t)0x32};
            ECLVoid(ecl, 0x838, 0x1584, 0x1594, 0x15f8);
            ecl << pair{0x1298, 1760} << pair{0x2d04, (int16_t)0};
            stage_section = 24;
            break;
        case TH11_ST2_MID1:
            ECLJump(ecl, 0xf1b4, 0xf280);
            ecl << pair{0xf280, 0};
            break;
        case TH11_ST2_MID2:
            ECLJump(ecl, 0xf1b4, 0xf280);
            ecl << pair{0xf280, 0}
                << pair{0x815c, 1000} << pair{0x84ac, (int16_t)0};
            break;
        case TH11_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0xf1b4, 0xf33c);
            else {
                ECLJump(ecl, 0xf1b4, 0xf360);
                ecl << pair{0xf360, 0};
            }
            break;
        case TH11_ST2_BOSS2:
            ECLJump(ecl, 0xf1b4, 0xf360);
            ecl << pair{0xf360, 0}
                << pair{0xc80, 1500} << pair{0xc94, 1500} << pair{0x24c0, (int16_t)0};
            break;
        case TH11_ST2_BOSS3:
            ECLJump(ecl, 0xf1b4, 0xf360);
            ecl << pair{0xf360, 0} << pair{0xd80, (int8_t)0x32};
            ECLTimeFix(ecl, 0xcf8, -60);
            ECLVoid(ecl, 0xd08, 0x1b10, 0x1b20);
            break;
        case TH11_ST2_BOSS4:
            ECLJump(ecl, 0xf1b4, 0xf360);
            ecl << pair{0xf360, 0} << pair{0xd80, (int8_t)0x32} << pair{0x18b8, 3050};
            ECLVoid(ecl, 0xd08, 0x1b10, 0x1b20, 0x3eb4);
            break;
        case TH11_ST2_BOSS5:
            ECLJump(ecl, 0xf1b4, 0xf360);
            ECLTimeFix(ecl, 0xcf8, -60);
            ecl << pair{0xf360, 0}
                << pair{0xd78, 12} << pair{0xd80, 0x64726143} << pair{0xd84, 0x33} // Str
                << pair{0xc80, 1500} << pair{0xc94, 1500};
            ECLVoid(ecl, 0xd08, 0x5538, 0x5548);
            break;
        case TH11_ST3_MID1:
            ECLJump(ecl, 0x14ae8, 0x14ba0);
            ecl << pair{0x14ba0, 0};
            break;
        case TH11_ST3_MID2:
            ECLJump(ecl, 0x14ae8, 0x14ba0);
            ecl << pair{0x14ba0, 0} << pair{0xb2fc, 9000};
            ecl.SetPos(0xb34c);
            ecl << 0 << 0x0028014f << 0x01ff0000 << 0 << 70; // Change Health Jump To Invicible
            ECLJump(ecl, 0xb458, 0xb6c4); // Jump From MBoss To MBoss2
            ECLTimeFix(ecl, 0xb7dc, -60); // Timefix MBoss2
            ecl << pair{0xb420, 0x43100000}; // Fix Boss Position
            ECLVoid(ecl, 0xb7a8); // Void Original Move
            ECLVoid(ecl, 0xb7c8, 0xb730, 0xb764);
            break;
        case TH11_ST3_MID3:
            ECLJump(ecl, 0x14ae8, 0x14ba0);
            ecl << pair{0x14ba0, 0} << pair{0xb2fc, 2000};
            ECLJump(ecl, 0xb404, 0xc0b4);
            ECLVoid(ecl, 0xb34c, 0xb374, 0xc214, 0xc1c8, 0xc10c);
            ecl << pair{0xc0f4, 152.0f};
            break;
        case TH11_ST3_BOSS1:
            st3_boss();
            if (thPracParam.dlg)
                ECLJump(ecl, 0x14bd0, 0x14c68, 0xA0);
            break;
        case TH11_ST3_BOSS2:
            st3_boss();
            ecl << pair{0x1000, 1200} << pair{0x3420, (int16_t)0};
            break;
        case TH11_ST3_BOSS3:
            st3_boss();
            ecl << pair{0x1114, 0x32};
            ECLTimeFix(ecl, 0x103c, -60);
            ECLVoid(ecl, 0x1074, 0x1c44, 0x1c54);
            break;
        case TH11_ST3_BOSS4:
            st3_boss();
            ecl << pair{0x1114, 0x32};
            ECLVoid(ecl, 0x1074, 0x1c44, 0x1c54);
            ecl << pair{0x1a88, 1800} << pair{0x4558, (int16_t)0};
            break;
        case TH11_ST3_BOSS5:
            st3_boss();
            ecl << pair{0x1114, 0x33};
            ECLTimeFix(ecl, 0x103c, -60);
            ECLVoid(ecl, 0x1074, 0x2db4, 0x2dc4);
            break;
        case TH11_ST3_BOSS6:
            st3_boss();
            ecl << pair{0x1114, 0x33};
            ECLVoid(ecl, 0x1074, 0x2db4, 0x2dc4);
            ecl << pair{0x2bf8, 2700} << pair{0x65cc, (int16_t)0};
            break;
        case TH11_ST4_RA_MID1:
        case TH11_ST4_RB_MID1:
        case TH11_ST4_RC_MID1:
        case TH11_ST4_MA_MID1:
        case TH11_ST4_MB_MID1:
        case TH11_ST4_MC_MID1:
        case TH11_ST4_MID1:
            ECLJump(ecl, 0xf764, 0xf7c0);
            ecl << pair{0x95f0, (int16_t)0} << pair{0x7b74, (int16_t)0} << pair{0x7af4, (int16_t)0}
                << pair{0xf7f0, 900};
            break;
        case TH11_ST4_RA_MID2:
        case TH11_ST4_RB_MID2:
        case TH11_ST4_RC_MID2:
        case TH11_ST4_MA_MID2:
        case TH11_ST4_MB_MID2:
        case TH11_ST4_MC_MID2:
        case TH11_ST4_MID2:
            ECLJump(ecl, 0xf764, 0xf8b0);
            ecl << pair{0x9668, (int16_t)0};
            break;
        case TH11_ST4_RA_BOSS1:
        case TH11_ST4_RB_BOSS1:
        case TH11_ST4_RC_BOSS1:
        case TH11_ST4_MA_BOSS1:
        case TH11_ST4_MB_BOSS1:
        case TH11_ST4_MC_BOSS1:
        case TH11_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0xf764, 0xf9e0);
            else
                ECLJump(ecl, 0xf764, 0xfa04);
            break;
        case TH11_ST4_RA_BOSS2:
        case TH11_ST4_RB_BOSS2:
        case TH11_ST4_RC_BOSS2:
        case TH11_ST4_MA_BOSS2:
        case TH11_ST4_MB_BOSS2:
        case TH11_ST4_MC_BOSS2:
        case TH11_ST4_BOSS2:
            ECLJump(ecl, 0xf764, 0xfa04);
            ecl << pair{0xfe4, 1900};
            ECLVoid(ecl, 0x48f0);
            break;
        case TH11_ST4_RA_BOSS3:
        case TH11_ST4_RB_BOSS3:
        case TH11_ST4_RC_BOSS3:
        case TH11_ST4_MA_BOSS3:
        case TH11_ST4_MB_BOSS3:
        case TH11_ST4_MC_BOSS3:
        case TH11_ST4_BOSS3:
            ECLJump(ecl, 0xf764, 0xfa04);
            ecl << pair{0x1110, (int8_t)0x32};
            ECLVoid(ecl, 0x2fa4, 0x1058, 0x2fb4);
            ECLTimeFix(ecl, 0x1020, -60);
            break;
        case TH11_ST4_RA_BOSS4:
        case TH11_ST4_RA1:
            ECLSatoriJump(ecl, 0);
            break;
        case TH11_ST4_RA_BOSS5:
        case TH11_ST4_RA2:
            switch (thPracParam.phase) {
            case 0:
                break;
            case 1:
                ecl << pair { 0x5438 + 0x8, thPracParam.boss_x };
                ecl << pair { 0x5438 + 0xC, thPracParam.boss_y };
                break;
            }
            ECLSatoriJump(ecl, 0);
            ecl.SetFile(2);
            ECLJump(ecl, 0x1ac, 0x3f4);
            ecl << pair{0x124, 6000};
            ECLVoid(ecl, 0x128, 0x644, 0x690, 0x654);
            break;
        case TH11_ST4_RA_BOSS6:
        case TH11_ST4_RA3:
            ECLSatoriJump(ecl, 0);
            ecl.SetFile(2);
            ECLJump(ecl, 0x1ac, 0x1aac);
            ecl << pair{0x124, 3500};
            ECLVoid(ecl, 0x128, 0x1cf8, 0x1d08);
            break;
        case TH11_ST4_RB_BOSS4:
        case TH11_ST4_RB1:
            ECLSatoriJump(ecl, 1);
            break;
        case TH11_ST4_RB_BOSS5:
        case TH11_ST4_RB2:
            ECLSatoriJump(ecl, 1);
            ecl.SetFile(3);
            ECLJump(ecl, 0x24c, 0x1bd0);
            ecl << pair { 0x170, 4000 };
            ECLVoid(ecl, 0x174, 0x1f8, 0x1e20, 0x1e58, 0x1e6c);
            break;
        case TH11_ST4_RB_BOSS6:
        case TH11_ST4_RB3:
            {
            constexpr unsigned int st4BossRB3InvulnVal = 0x30fc + 0x10;

            ECLSatoriJump(ecl, 1);
            ecl.SetFile(3);
            ECLJump(ecl, 0x24c, 0x30e8);
            ecl << pair { 0x170, 1500 };
            ECLVoid(ecl, 0x174, 0x1f8, 0x3334, 0x3344, 0x33b0);
            ecl << pair { 0x3390, 0x3c } << pair { 0x33c0, 0x3c }
                << pair { st4BossRB3InvulnVal, 0 }; // note: we skip a bunch of waits that normally elapse the invuln timer before the spell even begins
            break;
        }
        case TH11_ST4_RC_BOSS4:
        case TH11_ST4_RC1:
            ECLSatoriJump(ecl, 2);
            break;
        case TH11_ST4_RC_BOSS5:
        case TH11_ST4_RC2:
            ECLSatoriJump(ecl, 2);
            ecl.SetFile(4);
            ECLJump(ecl, 0x310, 0x1ac0);
            ecl << pair{0x180, 4400} << pair{0x230, 5100};
            ECLVoid(ecl, 0x184, 0x2b8, 0x234, 0x1d40, 0x1d78, 0x1d8c);
            break;
        case TH11_ST4_RC_BOSS6:
        case TH11_ST4_RC3:
            ECLSatoriJump(ecl, 2);
            ecl.SetFile(4);
            ECLJump(ecl, 0x310, 0x2bfc);
            ecl << pair{0x180, 2500} << pair{0x230, 3200};
            ECLVoid(ecl, 0x184, 0x2b8, 0x234, 0x2f1c, 0x2f2c, 0x2f98);
            break;
        case TH11_ST4_MA_BOSS4:
        case TH11_ST4_MA1:
            ECLSatoriJump(ecl, 3);
            break;
        case TH11_ST4_MA_BOSS5:
        case TH11_ST4_MA2:
            ECLSatoriJump(ecl, 3);
            ecl.SetFile(5);
            ECLJump(ecl, 0x288, 0x1008);
            ecl << pair{0x198, 3600};
            ECLVoid(ecl, 0x19c, 0x234, 0x1258, 0x1290, 0x12c4);
            break;
        case TH11_ST4_MA_BOSS6:
        case TH11_ST4_MA3:
            ECLSatoriJump(ecl, 3);
            ecl.SetFile(5);
            ECLJump(ecl, 0x288, 0x217c);
            ecl << pair{0x198, 2000};
            ECLVoid(ecl, 0x19c, 0x234, 0x23c8, 0x23d8, 0x2444);
            break;
        case TH11_ST4_MB_BOSS4:
        case TH11_ST4_MB1:
            ECLSatoriJump(ecl, 4);
            break;
        case TH11_ST4_MB_BOSS5:
        case TH11_ST4_MB2:
            ECLSatoriJump(ecl, 4);
            ecl.SetFile(6);
            ECLJump(ecl, 0x214, 0x7b8);
            ecl << pair{0x130, 4400};
            ECLVoid(ecl, 0x134, 0x1b8, 0xa08, 0xa40, 0xa94);
            break;
        case TH11_ST4_MB_BOSS6:
        case TH11_ST4_MB3:
            ECLSatoriJump(ecl, 4);
            ecl.SetFile(6);
            ECLJump(ecl, 0x214, 0x12ac);
            ecl << pair{0x130, 2400};
            ECLVoid(ecl, 0x134, 0x1b8, 0x14f8, 0x1508, 0x1574);
            break;
        case TH11_ST4_MC_BOSS4:
        case TH11_ST4_MC1:
            ECLSatoriJump(ecl, 5);
            break;
        case TH11_ST4_MC_BOSS5:
        case TH11_ST4_MC2:
            ECLSatoriJump(ecl, 5);
            ecl.SetFile(7);
            ECLJump(ecl, 0xdec, 0x1a0c);
            ecl << pair{0xd0c, 4400};
            ECLVoid(ecl, 0xd10, 0xd94, 0x1c5c, 0x1c94, 0x1ce8);
            break;
        case TH11_ST4_MC_BOSS6:
        case TH11_ST4_MC3:
            ECLSatoriJump(ecl, 5);
            ecl.SetFile(7);
            ECLJump(ecl, 0xdec, 0x2a38);
            ecl << pair{0xd0c, 2400};
            ECLVoid(ecl, 0xd10, 0xd94, 0x2cf4, 0x2d04, 0x2d60);
            break;
        case TH11_ST5_MID1:
            ECLJump(ecl, 0x674c, 0x6844);
            ECLVoid(ecl, 0x29f8);
            break;
        case TH11_ST5_MID2:
            ECLJump(ecl, 0x674c, 0x6844);
            ECLVoid(ecl, 0x29f8);
            ecl.SetFile(2);
            ecl << pair{0x1b8, 9000};
            ecl.SetPos(0x1284);
            ecl << 0 << 0x0014014f << 0x01ff0000 << 0 << 60; // Invicible
            ecl << pair{0x1354, 60};
            ECLVoid(ecl, 0x1354);
            break;
        case TH11_ST5_MID3:
            ECLJump(ecl, 0x674c, 0x6844);
            ECLVoid(ecl, 0x29f8);
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 0:
                break;
            case 1:
                ECLJump(ecl, 0x239C, 0x2A60, 90, 90);
                ECLJump(ecl, 0x3194, 0x3BC8, 90, 90);
                break;
            case 2:
                {
                const auto target = ecl.AppendWave(true);
                ECLJump(ecl, 0x20f0, target);
            }
                break;
            case 3:
            {
                const auto target = ecl.AppendWave(false);
                ECLJump(ecl, 0x20f0, target);
            }
            break;
            }
            ecl << pair{0x1b8, 1500} << pair{0x1f8, 1500};
            ecl.SetPos(0x1284);
            ecl << 0 << 0x0014014f << 0x01ff0000 << 0 << 60; // Invicible
            ecl << pair{0x1354, 60};
            ECLVoid(ecl, 0x1354, 0x22a8, 0x21ec);
            break;
        case TH11_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x674c, 0x6948);
            else
                ECLJump(ecl, 0x674c, 0x696c);
            break;
        case TH11_ST5_BOSS2:
            // A troublesome one. Ew.
            // Use a direct call instead of a thread creation,
            // Make the boss invincible in between.
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0xb94, 1500};
            ECLVoid(ecl, 0xc68);
            ecl << pair{0xd00, (int16_t)11};
            ecl.SetPos(0xcdc);
            ecl << 0 << 0x00200142 << 0x01ff0000 << 0 << 65;
            ecl.SetPos(0xd1c);
            ecl << 3 << 0x00140143 << 0x01ff0000 << 0 << 1;
            ecl << 0 << 0x002c014e << 0x04ff0000 << 0 << 0 << 1500 << 3000 << 0xc
                << 0x73736f42 << 0x64726143 << 0x31;
            ecl << 9999 << 0x00100000 << 0x00ff0000 << 0;
            ecl << pair{0x1dac, 0x43960000} << pair{0x1dc0, 2};
            ECLVoid(ecl, 0x15d0, 0x1570);
            break;
        case TH11_ST5_BOSS3:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0x0bf0, (int8_t)0x32};
            ECLVoid(ecl, 0xb64, 0x2d2c, 0x2d64);
            ECLTimeFix(ecl, 0xad8, -60);
            ECLTimeFix(ecl, 0xb28, -1);
            break;
        case TH11_ST5_BOSS4:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0x0bf0, (int8_t)0x32};
            ECLVoid(ecl, 0xb64, 0x2d2c, 0x2d64);
            ECLTimeFix(ecl, 0xad8, -60);
            ECLTimeFix(ecl, 0xb28, -1);
            ECLSetHealth(ecl, 0x2ddc, 2200);
            ECLVoid(ecl, 0x2d78, 0x5360, 0x2d8c);
            break;
        case TH11_ST5_BOSS5:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0x0bf0, (int8_t)0x33};
            ECLVoid(ecl, 0xb64, 0x3e18, 0x3e50);
            ECLTimeFix(ecl, 0xad8, -60);
            ECLTimeFix(ecl, 0xb28, -1);
            break;
        case TH11_ST5_BOSS6:
            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ecl << pair{0x0bf0, (int8_t)0x33};
            ECLVoid(ecl, 0xb64, 0x3e18, 0x3e50);
            ECLTimeFix(ecl, 0xad8, -60);
            ECLTimeFix(ecl, 0xb28, -1);
            ECLSetHealth(ecl, 0x3eb4, 2300);
            ECLVoid(ecl, 0x62f4);
            break;
        case TH11_ST5_BOSS7: {
            constexpr unsigned int st5BossSp4InvulnVal = 0x7478 + 0x10;

            ECLJump(ecl, 0x674c, 0x696c);
            ecl.SetFile(3);
            ECLVoid(ecl, 0xb64);
            ecl.SetPos(0xbd8);
            ecl << 0 << 0x0014015a << 0x01ff0000 << 0 << 48.0f;
            ECLJump(ecl, 0xbec, 0x72ac);
            ECLVoid(ecl, 0x7548, 0x7580);
            ECLTimeFix(ecl, 0x75e8, -60);
            ecl << pair { st5BossSp4InvulnVal, 0 }; // note: invuln timer normally elapses before the spell even begins
            break;
        }
        case TH11_ST6_MID1:
            THStage6STD();
            THStage6ANM();
            ECLJump(ecl, 0x40b8, 0x41a4);
            ecl.SetFile(2);
            ecl << pair{0x110, 65};
            ecl.SetPos(0x1d0);
            ecl << 60 << 0x00140143 << 0x01ff0000 << 0 << 1;
            break;
        case TH11_ST6_BOSS1:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            if (thPracParam.dlg)
                ECLJump(ecl, 0x40b8, 0x4294);
            else
                ECLJump(ecl, 0x40b8, 0x42b8);
            break;
        case TH11_ST6_BOSS2:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 3000);
            ECLVoid(ecl, 0x33d0);
            break;
        case TH11_ST6_BOSS3:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ecl << pair{0x518, (int8_t)0x32};
            ECLTimeFix(ecl, 0x450, -60);
            ECLVoid(ecl, 0x48c, 0xc4c, 0xc84);
            break;
        case TH11_ST6_BOSS4:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 2400);
            ecl << pair{0x5a4, (int8_t)0x32} << pair{0x590, 2400};
            ECLVoid(ecl, 0x4cf0);
            break;
        case TH11_ST6_BOSS5:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ecl << pair{0x518, (int8_t)0x33};
            ECLTimeFix(ecl, 0x450, -60);
            ECLVoid(ecl, 0x48c, 0x1b20, 0x1b58);
            break;
        case TH11_ST6_BOSS6:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 3000);
            ecl << pair{0x5a4, (int8_t)0x33} << pair{0x590, 3000};
            ECLVoid(ecl, 0x60fc);
            break;
        case TH11_ST6_BOSS7:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ecl << pair{0x518, (int8_t)0x34};
            ECLTimeFix(ecl, 0x450, -60);
            ECLVoid(ecl, 0x48c, 0x24f8, 0x2530);
            break;
        case TH11_ST6_BOSS8: {
            constexpr unsigned int st6BossSp4InvulnVal = 0x769c + 0x10;
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 3500);
            ecl << pair { 0x5a4, (int8_t)0x34 } << pair { 0x590, 3500 };
            ECLVoid(ecl, 0x77dc, 0x787c, 0x78d0, 0x78fc, 0x7928);
            ecl.SetPos(0x785c);
            ecl << 0 << 0x00200118 << 0x02ff0000 << 0 << 0 << 0x43600000;
            ecl << pair { st6BossSp4InvulnVal, 260 - 60 * 4 }; // accounting for skipped waits (=20f)
            break;
        }
        case TH11_ST6_BOSS9:
            THStage6STD();
            THStage6ANM();
            ECLVoid(ecl, 0x460);
            ECLJump(ecl, 0x40b8, 0x42b8);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x5f0, 3500);
            ecl << pair{0x5a4, (int8_t)0x35} << pair{0x590, 3500};
            ECLVoid(ecl, 0x8e48, 0x8e80, 0x8ee8);
            ecl.SetPos(0x8ec8);
            ecl << 0 << 0x00200118 << 0x02ff0000 << 0 << 0 << 0x43600000;

            switch (thPracParam.phase) {
            case 1:// 2
                ecl << pair{0x8cb0, 4999};
                ECLJump(ecl, 0x9260, 0x9318);
                ecl << pair{0x9344, (int16_t)0};
                break;
            case 2:// 3
                ecl << pair{0x8cb0, 2999};
                ECLJump(ecl, 0x9260, 0x9318);
                ECLJump(ecl, 0x9354, 0x940c);
                ecl << pair{0x9344, (int16_t)0} << pair{0x9438, (int16_t)0};
                break;
            case 3:// 4
                ecl << pair{0x8cb0, 1499};
                ECLJump(ecl, 0x9260, 0x9318);
                ECLJump(ecl, 0x9354, 0x940c);
                ECLJump(ecl, 0x946c, 0x9524);
                ecl << pair{0x9344, (int16_t)0} << pair{0x9438, (int16_t)0} << pair{0x9550, (int16_t)0};
                break;
            case 4://rage 4
                ecl << pair{0x8cb0, 1499};
                ECLJump(ecl, 0x9260, 0x9318);
                ECLJump(ecl, 0x9354, 0x940c);
                ECLJump(ecl, 0x946c, 0x9524);
                ecl << pair{0x9344, (int16_t)0} << pair{0x9438, (int16_t)0} << pair{0x9550, (int16_t)0};
                ecl << pair{0x9f70, 2181}; // 2180
                ecl << pair{0xa08c, (int16_t)0};
                break;
            case 5://rage 3
                ecl << pair { 0x8cb0, 2999 };
                ECLJump(ecl, 0x9260, 0x9318);
                ECLJump(ecl, 0x9354, 0x940c);
                ecl << pair { 0x9344, (int16_t)0 } << pair { 0x9438, (int16_t)0 };
                ecl << pair { 0x9f70, 2181 }; // 2180
                ecl << pair { 0xa08c, (int16_t)0 };
                break;
            case 6: // rage 2
                ecl << pair { 0x8cb0, 4999 };
                ECLJump(ecl, 0x9260, 0x9318);
                ecl << pair { 0x9344, (int16_t)0 };
                ecl << pair { 0x9f70, 2181 }; // 2180
                ecl << pair { 0xa08c, (int16_t)0 };
                break;
            case 7: // rage 1
                ecl << pair { 0x9f70, 2181 }; // 2180
                ecl << pair { 0xa08c, (int16_t)0 };
                break;
            default:
                break;
            }
            break;
        case TH11_ST7_MID1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x4c00, 0x4d18);
            else
                ECLJump(ecl, 0x4c00, 0x4d2c);
            ecl.SetFile(2);
            ECLVoid(ecl, 0x284);
            break;
        case TH11_ST7_MID2:
            ECLJump(ecl, 0x4c00, 0x4d2c);
            ecl.SetFile(2);
            ECLVoid(ecl, 0x284);
            ecl << pair{0x351, (int8_t)0x32};
            ECLTimeFix(ecl, 0x298, -60);
            ECLVoid(ecl, 0x17cc, 0x1994, 0x19a4);
            break;
        case TH11_ST7_MID3:
            ECLJump(ecl, 0x4c00, 0x4d2c);
            ecl.SetFile(2);
            ECLVoid(ecl, 0x284);
            ecl << pair{0x351, (int8_t)0x33};
            ECLTimeFix(ecl, 0x298, -60);
            ECLVoid(ecl, 0x2048, 0x2210, 0x2220);
            break;
        case TH11_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x4c00, 0x4e38);
            else
                ECLJump(ecl, 0x4c00, 0x4e5c);
            ECLVoid(ecl, 0x734);
            break;
        case TH11_ST7_END_S1:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ECLSetHealth(ecl, 0x734, 3000);
            ECLVoid(ecl, 0x4ab0, 0x4c98);
            break;
        case TH11_ST7_END_NS2:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x32};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ecl << pair{0x528, 0x43200000};
            ECLVoid(ecl, 0x1198, 0x11d0);
            break;
        case TH11_ST7_END_S2:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x32};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ecl << pair{0x528, 0x43200000};
            ECLVoid(ecl, 0x1198, 0x11d0);
            ECLSetHealth(ecl, 0x1214, 3300);
            ECLVoid(ecl, 0x775c);
            break;
        case TH11_ST7_END_NS3:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x33};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x1870, 0x18a8);
            break;
        case TH11_ST7_END_S3:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x33};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x1870, 0x18a8);
            ECLSetHealth(ecl, 0x190c, 2800);
            ECLVoid(ecl, 0x8278, 0x82b0);
            break;
        case TH11_ST7_END_NS4:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x34};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x1fd0, 0x2008);
            break;
        case TH11_ST7_END_S4:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x34};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x1fd0, 0x2008);
            ecl << pair{0x201c, 60} << pair{0x202c, 60};
            ECLSetHealth(ecl, 0x204c, 3200, 60);
            ECLVoid(ecl, 0x89a0, 0x89d8);
            break;
        case TH11_ST7_END_NS5:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x35};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x2a8c, 0x2ac4);
            break;
        case TH11_ST7_END_S5:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x35};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x2a8c, 0x2ac4);
            ecl << pair{0x2ad8, 60} << pair{0x2ae8, 60};
            ECLSetHealth(ecl, 0x2b08, 1);
            ECLVoid(ecl, 0x9374, 0x93ac);
            break;
        case TH11_ST7_END_NS6:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x36};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x342c, 0x3464);
            break;
        case TH11_ST7_END_S6:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x36};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x342c, 0x3464);
            ECLSetHealth(ecl, 0x34e0, 3200);
            ecl << pair{0x34b0, 60} << pair{0x34c0, 60};
            ECLVoid(ecl, 0xa764, 0xa79c);
            break;
        case TH11_ST7_END_NS7:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x37};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x3b68, 0x3ba0);
            break;
        case TH11_ST7_END_S7:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x37};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x3b68, 0x3ba0);
            ECLSetHealth(ecl, 0x3c1c, 3600);
            ecl << pair{0x3bec, 60} << pair{0x3bfc, 60};
            ECLVoid(ecl, 0xb858, 0xb890);
            break;
        case TH11_ST7_END_NS8:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x38};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x441c, 0x4454);
            break;
        case TH11_ST7_END_S8:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x38};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x441c, 0x4454);
            ECLSetHealth(ecl, 0x44d0, 2000);
            ecl << pair{0x44a0, 60} << pair{0x44b0, 60};
            ECLVoid(ecl, 0xc610, 0xc648);
            break;
        // Blue source SHA256 af2c0a29b2d92bb41fca2d595614a5ea4aa08b0b5b9c0f0c3c5f3789663f6ea7, replay only.
        case TH11_ST7_END_S9:
            if(thPracParam.legacy_blue) {
 // Philosophy of a Hated Person
            constexpr unsigned int st7PostMaple = 0x4c00;
            constexpr unsigned int st7BossCreateCall = 0x4e5c;
            constexpr unsigned int st7bsDialogWait = 0x568;
            constexpr unsigned int st7bsNon1MoveLimit = 0x5b8;
            constexpr unsigned int st7bsNonspellOrd = 0x65c;

            constexpr unsigned int st7bsNon8InterruptOrd = 0x42b4;
            constexpr unsigned int st7bsNon8ItemDrop = 0x441c;
            constexpr unsigned int st7bsNon8ItemDropSnd = 0x4454;
            constexpr unsigned int st7bsNon8PostWaitNop = 0x44a0;
            constexpr unsigned int st7bsNon8PostWaitMoveLimit = 0x44b0;
            constexpr unsigned int st7bsNon8Attacks = 0x44d0;

            constexpr unsigned int st7bsSpell9ItemDrop = 0xd9e8;
            constexpr unsigned int st7bsSpell9ItemDropSnd = 0xda20;
            constexpr unsigned int st7bsSpell9StartWait = 0xda4c;
            constexpr unsigned int st7bsSpell9InterruptTime = 0xd898 + 0x18;

            constexpr unsigned int st7bsSpell9Phase1Snd = 0xdabc;
            constexpr unsigned int st7bsSpell9Phase1InitialBubbles = 0xdc40;
            constexpr unsigned int st7bsSpell9Phase1Atk = 0xdcd0;
            constexpr unsigned int st7bsSpell9Phase1PreFogWait = 0xdcf4;
            constexpr unsigned int st7bsSpell9Phase1PostFog = 0xdd20;

            ECLJump(ecl, st7PostMaple, st7BossCreateCall);
            ecl.SetFile(3);
            ECLTimeFix(ecl, st7bsDialogWait, -60);
            ECLVoid(ecl, st7bsNon1MoveLimit);
            ecl << pair{ st7bsNonspellOrd, (int8_t)0x38 }; // non 8

            ecl << pair{ st7bsNon8InterruptOrd, 0x39 }; // spell 9
            ECLVoid(ecl, st7bsNon8ItemDrop, st7bsNon8ItemDropSnd);
            ecl << pair{ st7bsNon8PostWaitNop, 60 } // 1s sooner
                << pair{ st7bsNon8PostWaitMoveLimit, 60 };
            ECLSetHealth(ecl, st7bsNon8Attacks, 2000); // trigger dmg interrupt

            ECLVoid(ecl, st7bsSpell9ItemDrop, st7bsSpell9ItemDropSnd, st7bsSpell9StartWait);

            if (thPracParam.phase) {
                ECLVoid(ecl, st7bsSpell9Phase1Snd, st7bsSpell9Phase1InitialBubbles);
                ECLVoid(ecl, st7bsSpell9Phase1Atk, st7bsSpell9Phase1PreFogWait);
            }

            switch (thPracParam.phase) {
            case 1: {
                constexpr unsigned int st7bsSpell9Phase2Start = 0xde5c;

                ecl << pair{ st7bsSpell9InterruptTime, 2700 }; // 3540f - 840f of waits
                ECLJump(ecl, st7bsSpell9Phase1PostFog, st7bsSpell9Phase2Start);
                break;
            }
            case 2: {
                constexpr unsigned int st7bsSpell9Phase3Start = 0xe0b4;

                ecl << pair{ st7bsSpell9InterruptTime, 1220 }; // 3540f - 2320f of waits
                ECLJump(ecl, st7bsSpell9Phase1PostFog, st7bsSpell9Phase3Start);
                break;
            }
            default:
                break;
            }
            break;

            }
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x38};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x441c, 0x4454);
            ECLSetHealth(ecl, 0x44d0, 2000);
            ecl << pair{0x44a0, 60} << pair{0x44b0, 60};
            ECLVoid(ecl, 0xc610, 0xc648);
            ecl << pair{0x42b4, 0x39};
            ECLVoid(ecl, 0xd9e8, 0xda20, 0xda4c);
            break;
        case TH11_ST7_END_S10:
            ECLJump(ecl, 0x4c00, 0x4e5c);
            ecl.SetFile(3);
            ecl << pair{0x65c, (int8_t)0x38};
            ECLTimeFix(ecl, 0x568, -60);
            ECLVoid(ecl, 0x5b8);
            ECLVoid(ecl, 0x441c, 0x4454);
            ECLSetHealth(ecl, 0x44d0, 2000);
            ecl << pair{0x44a0, 60} << pair{0x44b0, 60};
            ECLVoid(ecl, 0xc610, 0xc648);
            ecl << pair{0x42b4, 0x00003031};
            ECLVoid(ecl, 0x5204, 0x523c, 0x5268, 0x5000, 0x504c);

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0x5100, 8399};
                ECLJump(ecl, 0x53b8, 0x5484, 60);
                break;
            case 2:
                ecl << pair{0x5100, 5599};
                ECLJump(ecl, 0x53b8, 0x55e8, 60);
                break;
            case 3:
                ecl << pair{0x5100, 2799};
                ECLJump(ecl, 0x53b8, 0x574c, 60);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
