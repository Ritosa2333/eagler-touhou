// Generated from rueee/thprac master_2, 1778005 (MIT).
// TH17 source SHA-256 3db9c33b1f4760d5082eb25dc117cf06a95a83f759e3e4ca3dc4d0c2334bcbd0
    void ECLStdExec(ECLHelper& ecl, unsigned int start, int std_id, int ecl_time = 0)
    {
        if (start)
            ecl.SetPos(start);
        ecl << ecl_time << 0x00140276 << 0x01ff0000 << 0x00000000 << std_id;
    }
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        if (start)
            ecl.SetPos(start);
        else
            start = ecl.GetPos();
        ecl.Jump(start, dest);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x78c4, 0x7c3c, 60, 90); // 0x7cd8
                ECLJump(ecl, 0x4678, 0x46e0, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x78c4, 0x7cd8, 60, 90);
                break;
            case 4:
                ECLJump(ecl, 0x78c4, 0x7cd8, 60, 90);
                ECLJump(ecl, 0x47d0, 0x4814, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x99fc, 0x9dec, 60, 90); // 0x9e88
                ECLJump(ecl, 0x4654, 0x4688, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x99fc, 0x9e88, 60, 90);
                break;
            case 4:
                ECLJump(ecl, 0x99fc, 0x9e88, 60, 90);
                ECLJump(ecl, 0x488c, 0x48c0, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x99fc, 0x9e88, 60, 90);
                ECLJump(ecl, 0x488c, 0x4954, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8cec, 0x9010, 60, 90); // 0x90c0
                ECLJump(ecl, 0x44b8, 0x44ec, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8cec, 0x9010, 60, 90); // 0x90c0
                ECLJump(ecl, 0x44b8, 0x45e8, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x8cec, 0x90c0, 60, 90);
                ecl << pair{0x6b04, 0};
                break;
            case 5:
                ECLJump(ecl, 0x8cec, 0x90c0, 60, 90);
                ECLJump(ecl, 0x46f8, 0x4758, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x8cec, 0x90c0, 60, 90);
                ECLJump(ecl, 0x46f8, 0x47c0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xc374, 0xc698, 60, 90); // 0xc748
                ECLJump(ecl, 0x4d28, 0x4d88, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xc374, 0xc698, 60, 90); // 0xc748
                ECLJump(ecl, 0x4d28, 0x4df0, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0xc374, 0xc698, 60, 90); // 0xc748
                ECLJump(ecl, 0x4d28, 0x4e58, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0xc374, 0xc748, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0xc374, 0xc748, 60, 90);
                ECLJump(ecl, 0x4f20, 0x4f94, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xae0c, 0xb154, 60, 90); // 0xb1f0
                ECLJump(ecl, 0x4ba4, 0x4bec, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xae0c, 0xb154, 60, 90); // 0xb1f0
                ECLJump(ecl, 0x4ba4, 0x4c4c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0xae0c, 0xb154, 60, 90); // 0xb1f0
                ECLJump(ecl, 0x4ba4, 0x4c80, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0xae0c, 0xb1f0, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0xae0c, 0xb1f0, 60, 90);
                ECLJump(ecl, 0x4d14, 0x4d88, 0, 0);
                ecl << pair{0x6a98, 0};
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x71bc, 0x7504, 60, 90); // 0x75b4
                ECLJump(ecl, 0x35ac, 0x3620, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x71bc, 0x75b4, 60, 90);
                ecl << pair{0x36c4, 0};
                break;
            case 4:
                ECLJump(ecl, 0x71bc, 0x75b4, 60, 90);
                ECLJump(ecl, 0x36b4, 0x3728, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xa208, 0xa6a8, 60, 90); // 0xa758
                ECLJump(ecl, 0x4960, 0x49a8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xa208, 0xa6a8, 60, 90); // 0xa758
                ECLJump(ecl, 0x4960, 0x49dc, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0xa208, 0xa6a8, 60, 90); // 0xa758
                ECLJump(ecl, 0x4960, 0x4a10, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0xa208, 0xa758, 60, 90);
                ecl << pair{0x4ac4, 0};
                break;
            case 6:
                ECLJump(ecl, 0xa208, 0xa758, 60, 90);
                ECLJump(ecl, 0x4ab4, 0x4b28, 0, 0);
                break;
            case 7:
                ECLJump(ecl, 0xa208, 0xa758, 60, 90);
                ECLJump(ecl, 0x4ab4, 0x4b88, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0xa208, 0xa758, 60, 90);
                ECLJump(ecl, 0x4ab4, 0x4bbc, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    void THPatch(ECLHelper& ecl, i32 section)
    {
        constexpr uint32_t st6BossCall = 0x7634;
        constexpr uint32_t st6bsSTDCallOp = 0x450 + 04;
        constexpr uint32_t st6bsPrePushSpellID = 0x548;
        constexpr uint32_t st6bsPostNotSpellPracCheck = 0x630;
        constexpr uint32_t st6bsHealthVal = 0x630 + 0x10;
        constexpr uint32_t st6bsSpellSubCallOrd = 0x644 + 0x1c;
        constexpr uint32_t st6bsNonSubCallOrd = 0xd8c + 0x18;

        switch (section) {
        case TH17_ST1_MID1:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            ECLJump(ecl, 0, 0x7c94, 60);
            break;
        case TH17_ST1_BOSS1:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            if (thPracParam.dialogue)
                ECLJump(ecl, 0, 0x7d70, 60);
            else
                ECLJump(ecl, 0, 0x7d84, 60);
            break;
        case TH17_ST1_BOSS2:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            ECLJump(ecl, 0, 0x7d84, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x39c, 0x484, 1); // Utilize Spell Practice Jump
            ecl << pair{0x494, 2400}; // Set Health
            ecl << pair{0x4b4, (int8_t)0x31}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x1a44, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST1_BOSS3:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            ECLJump(ecl, 0, 0x7d84, 60);
            ecl.SetFile(2);
            ecl << pair{0x5d4, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0xf7c, (int16_t)0 } << pair { 0xfb4, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x10e0, (int16_t)0 }; // Disable Item Drops & SE
            }
            ECLJump(ecl, 0x1234, 0x12b4, 0);
            ecl << pair{0xe9c, 0};
            break;
        case TH17_ST1_BOSS4:
            ECLStdExec(ecl, 0x78c4, 1, 1);
            ECLJump(ecl, 0, 0x7d84, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x39c, 0x484, 1); // Utilize Spell Practice Jump
            ecl << pair{0x494, 2400}; // Set Health
            ecl << pair{0x4b4, (int8_t)0x32}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x2b2c, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST2_MID1:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9e44, 60);
            break;
        case TH17_ST2_BOSS1:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            if (thPracParam.dialogue)
                ECLJump(ecl, 0, 0x9ee0, 60);
            else
                ECLJump(ecl, 0, 0x9ef4, 60);
            break;
        case TH17_ST2_BOSS2:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9ef4, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 2200}; // Set Health
            ecl << pair{0x514, (int8_t)0x31}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x26e4, (int16_t)0 } << pair { 0x271c, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST2_BOSS3:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9ef4, 60);
            ecl.SetFile(2);
            ecl << pair{0x738, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x1360, (int16_t)0 } << pair { 0x1398, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x14c4, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x1654, 60} << pair{0x1280, 90};
            break;
        case TH17_ST2_BOSS4:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9ef4, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4f4, 2200}; // Set Health
            ecl << pair{0x514, (int8_t)0x32}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x3058, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST2_BOSS5:
            ECLStdExec(ecl, 0x99fc, 1, 1);
            ECLJump(ecl, 0, 0x9ef4, 60);
            ecl.SetFile(2);
            if (thPracParam.keep_drops) {
                ecl << pair { 0x738, (int8_t)0x33 }; // Change Nonspell

            } else {
                ECLJump(ecl, 0x3fc, 0x4e4, 1); // Utilize Spell Practice Jump
                ecl << pair { 0x4f4, 3000 }; // Set Health
                ecl << pair { 0x514, (int8_t)0x33 }; // Set Spell Ordinal
            }
            break;
        case TH17_ST3_MID1:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x907c, 60);
            break;
        case TH17_ST3_BOSS1:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            if (thPracParam.dialogue)
                ECLJump(ecl, 0, 0x912c, 60);
            else
                ECLJump(ecl, 0, 0x9140, 60);
            break;
        case TH17_ST3_BOSS2:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x470, 0x558, 1); // Utilize Spell Practice Jump
            ecl << pair{0x568, 2700}; // Set Health
            ecl << pair{0x588, (int8_t)0x31}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x2d70, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST3_BOSS3:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ecl << pair{0x7cc, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x1330, (int16_t)0 } << pair { 0x1368, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x1494, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x15f8, 0} << pair{0x1250, 30};
            break;
        case TH17_ST3_BOSS4:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x470, 0x558, 1); // Utilize Spell Practice Jump
            ecl << pair{0x568, 2700}; // Set Health
            ecl << pair{0x588, (int8_t)0x32}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x3b2c, (int16_t)0 } << pair { 0x3b64, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST3_BOSS5:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ecl << pair{0x7cc, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x2070, (int16_t)0 } << pair { 0x20a8, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x21d4, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x2338, 0} << pair{0x1f90, 30};
            break;
        case TH17_ST3_BOSS6:
            ECLStdExec(ecl, 0x8cec, 1, 1);
            ECLJump(ecl, 0, 0x9140, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x470, 0x558, 1); // Utilize Spell Practice Jump
            ecl << pair{0x568, 2600}; // Set Health
            ecl << pair{0x588, (int8_t)0x33}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x4ff0, (int16_t)0 } << pair { 0x5028, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST4_MID1:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc704, 60);
            break;
        case TH17_ST4_BOSS1:
            ECLStdExec(ecl, 0xc374, 1, 1);
            if (thPracParam.dialogue)
                ECLJump(ecl, 0, 0xc7b4, 60);
            else
                ECLJump(ecl, 0, 0xc7c8, 60);
            break;
        case TH17_ST4_BOSS2:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3d4, 0x4bc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4cc, 2400}; // Set Health
            ecl << pair{0x4ec, (int8_t)0x31}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x33dc, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST4_BOSS3:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ecl << pair{0x730, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x1454, (int16_t)0 } << pair { 0x148c, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x15b8, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x171c, 0} << pair{0x1374, 30};
            break;
        case TH17_ST4_BOSS4:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3d4, 0x4bc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4cc, 2400}; // Set Health
            ecl << pair{0x4ec, (int8_t)0x32}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x44f0, (int16_t)0 } << pair { 0x4528, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST4_BOSS5:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ecl << pair{0x730, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x24a0, (int16_t)0 } << pair { 0x24d8, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x2604, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x2768, 0} << pair{0x23c0, 30};
            break;
        case TH17_ST4_BOSS6:
            ECLStdExec(ecl, 0xc374, 1, 1);
            ECLJump(ecl, 0, 0xc7c8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3d4, 0x4bc, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4cc, 4000}; // Set Health
            ecl << pair{0x4ec, (int8_t)0x33}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x5264, (int16_t)0 } << pair { 0x529c, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST5_MID1:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb1ac, 60);
            break;
        case TH17_ST5_BOSS1:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            if (thPracParam.dialogue)
                ECLJump(ecl, 0, 0xb25c, 60);
            else
                ECLJump(ecl, 0, 0xb270, 60);
            break;
        case TH17_ST5_BOSS2:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x484, 0x56c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x57c, 2500}; // Set Health
            ecl << pair{0x59c, (int8_t)0x31}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x3854, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST5_BOSS3:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ecl << pair{0x8e4, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x1508, (int16_t)0 } << pair { 0x1540, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x166c, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x17d0, 0} << pair{0x1428, 30};
            break;
        case TH17_ST5_BOSS4:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x484, 0x56c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x57c, 2300}; // Set Health
            ecl << pair{0x59c, (int8_t)0x32}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x5260, (int16_t)0 } << pair { 0x5298, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST5_BOSS5:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ecl << pair{0x8e4, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x2408, (int16_t)0 } << pair { 0x2440, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x256c, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x26d0, 0} << pair{0x2328, 30};
            break;
        case TH17_ST5_BOSS6:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x484, 0x56c, 1); // Utilize Spell Practice Jump
            ecl << pair{0x57c, 2300}; // Set Health
            ecl << pair{0x59c, (int8_t)0x33}; // Set Spell Ordinal
            if (!thPracParam.keep_drops)
                ecl << pair { 0x70ac, (int16_t)0 } << pair { 0x70e4, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST5_BOSS7:
            ECLStdExec(ecl, 0xae0c, 1, 1);
            ECLJump(ecl, 0, 0xb270, 60);
            ecl.SetFile(2);
            if (thPracParam.keep_drops) {
                ecl << pair { 0x8e4, (int8_t)0x34 }; // Change Nonspell

            } else {
                ECLJump(ecl, 0x484, 0x56c, 1); // Utilize Spell Practice Jump
                ecl << pair { 0x57c, 3000 }; // Set Health
                ecl << pair { 0x59c, (int8_t)0x34 }; // Set Spell Ordinal
            }
            break;
        case TH17_ST6_MID1:
            ECLStdExec(ecl, 0x71bc, 1, 1);
            ECLJump(ecl, 0, 0x7570, 60);
            break;
        case TH17_ST6_BOSS1:
            ECLStdExec(ecl, 0x71bc, 1, 1);
            ECLStdExec(ecl, 0, 2, 2);
            if (thPracParam.dialogue)
                ECLJump(ecl, 0, 0x7620, 60);
            else
                ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };
            break;
        case TH17_ST6_BOSS2:
            ECLStdExec(ecl, 0x71bc, 1, 1);
            ECLStdExec(ecl, 0, 2, 2);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 2); // Utilize Spell Practice Jump
            ecl << pair { st6bsHealthVal, 2600 }; // Set Health
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x31 }; // Set Spell Ordinal (1)
            if (!thPracParam.keep_drops)
                ecl << pair { 0x66c4, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST6_BOSS3:
            ECLStdExec(ecl, 0x71bc, 3, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x32 }; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x2038, (int16_t)0 } << pair { 0x2070, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x219c, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair { 0x2300, 0 } << pair { 0x1f44, 30 };
            ecl << pair { 0x2004, (int16_t)0 };
            break;
        case TH17_ST6_BOSS4:
            ECLStdExec(ecl, 0x71bc, 3, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 2); // Utilize Spell Practice Jump
            ecl << pair { st6bsHealthVal, 3800 }; // Set Health
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x32 }; // Set Spell Ordinal (2)
            if (!thPracParam.keep_drops)
                ecl << pair { 0x78d8, (int16_t)0 } << pair { 0x7910, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST6_BOSS5:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x33 }; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x32c8, (int16_t)0 } << pair { 0x3300, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x3444, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair { 0x31d4, 0 };
            ECLJump(ecl, 0x3598, 0x35e8, 0);
            ecl << pair { 0x3294, (int16_t)0 };
            break;
        case TH17_ST6_BOSS6:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 2); // Utilize Spell Practice Jump
            ecl << pair { st6bsHealthVal, 3000 }; // Set Health
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x33 }; // Set Spell Ordinal (3)
            if (!thPracParam.keep_drops)
                ecl << pair { 0x9bc0, (int16_t)0 } << pair { 0x9bf8, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST6_BOSS7:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            ecl << pair { st6bsNonSubCallOrd, (int8_t)0x34 }; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x4888, (int16_t)0 } << pair { 0x48c0, (int16_t)0 } << pair { 0x48f8, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x4a3c, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair { 0x4ba0, 0 } << pair { 0x4794, 30 };
            ecl << pair { 0x4854, (int16_t)0 };
            break;
        case TH17_ST6_BOSS8:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 2); // Utilize Spell Practice Jump
            ecl << pair { st6bsHealthVal, 2700 }; // Set Health
            ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x34 }; // Set Spell Ordinal (4)
            if (!thPracParam.keep_drops)
                ecl << pair { 0xac30, (int16_t)0 } << pair { 0xac68, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST6_BOSS9:
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            if (thPracParam.keep_drops) {
                ecl << pair { st6bsNonSubCallOrd, (int8_t)0x35 }; // Change Nonspell

            } else {
                ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 2); // Utilize Spell Practice Jump
                ecl << pair { st6bsHealthVal, 3500 }; // Set Health
                ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x35 }; // Set Spell Ordinal (5)
            }
            break;

        case TH17_ST6_BOSS10: {
            constexpr uint32_t st6bsInvulnVal = 0x2c4 + 0x10;
            constexpr uint32_t st6bsNon6InvulnVal = 0x5c3c + 0x10;
            constexpr uint32_t st6bsNon6ItemDrop1Enable = 0x5d14 + 0x4;
            constexpr uint32_t st6bsNon6ItemDrop2Enable = 0x5d4c + 0x4;
            constexpr uint32_t st6bsNon6ItemDropSFXEnable = 0x5e8c + 0x4;
            constexpr uint32_t st6bsNon6DialogWaitEnable = 0x5fec + 0x4;
            constexpr uint32_t st6bsNon6DialogStart = 0x6000;
            constexpr uint32_t st6bsNon6PostDialog = 0x6024;

            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            if (thPracParam.dialogue || thPracParam.keep_drops) {
                ecl << pair { st6bsNonSubCallOrd, (int8_t)0x36 } // Change Nonspell (6)
                    << pair { st6bsNon6InvulnVal, 180 - 120 } // Invuln timer: account for waits (60f)
                    << pair { st6bsNon6DialogWaitEnable, (int16_t)0 }; // Disable main wait (120f)

                if (!thPracParam.dialogue)
                    ECLJump(ecl, st6bsNon6DialogStart, st6bsNon6PostDialog, 0);

                if (!thPracParam.keep_drops) {
                    ecl << pair { st6bsNon6ItemDrop1Enable, (int16_t)0 }
                        << pair { st6bsNon6ItemDrop2Enable, (int16_t)0 } // Disable Item Drops
                        << pair { st6bsNon6ItemDropSFXEnable, (int16_t)0 }; // Disable Item Drops SFX
                }
            } else {
                ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 2); // Utilize Spell Practice Jump
                ecl << pair { st6bsInvulnVal, 180 - 120 - 40 } // Invuln timer: account for waits (20f)
                    << pair { st6bsHealthVal, 5000 } // Set Health
                    << pair { st6bsSpellSubCallOrd, (int8_t)0x36 }; // Set Spell Ordinal (6)
            }
            break;
        }

        case TH17_ST6_BOSS11: {
            ECLStdExec(ecl, 0x71bc, 5, 1);
            ECLJump(ecl, 0, st6BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st6bsSTDCallOp, (int16_t)0 };

            if (thPracParam.keep_drops) {
                ecl << pair { st6bsNonSubCallOrd, (int8_t)0x37 }; // Change Nonspell (7)

            } else {
                ECLJump(ecl, st6bsPrePushSpellID, st6bsPostNotSpellPracCheck, 2); // Utilize Spell Practice Jump
                ecl << pair { st6bsHealthVal, 10000 }; // Set Health
                ecl << pair { st6bsSpellSubCallOrd, (int8_t)0x37 }; // Set Spell Ordinal (7)
            }

            const uint32_t tgtPhase = (uint32_t)thPracParam.phase;
            if (!tgtPhase || tgtPhase > 3)
                break;
            constexpr float shtAngleDt = 3.883222f;
            constexpr float shtOffsetDt = 0.017453292f;

            // patch ID's attack subs to use args to initialize its 3 main variables (instead of as their delta vals, which are always the same)
            // initializing shoot angle to arg2 + incrementing it without argument
            ecl << pair { 0xf86c + 0x8, (uint16_t)1 } // init pushf -> first param is an argument
                << pair { 0xf86c + 0x10, 8.0f }; // (argument 2)
            ecl << pair { 0xf880 + 0x4, (int16_t)0 } << pair { 0xf894 + 0x4, (int16_t)0 }; // nop out multiplication for init
            ecl << pair { 0xfbb4 + 0x8, (uint16_t)0 } // increment pushf: first param is a constant
                << pair { 0xfbb4 + 0x10, shtAngleDt }; // (same delta value passed for all 4 shooters)

            // initializing offset angle to arg3 + incrementing it without argument
            ecl << pair { 0xf8b8 + 0x8, (uint16_t)1 } // init pushf -> first param is an argument
                << pair { 0xf8b8 + 0x10, 12.0f }; // (argument 3)
            ecl << pair { 0xf8cc + 0x4, (int16_t)0 } << pair { 0xf8e0 + 0x4, (int16_t)0 }; // nop out multiplication for init
            ecl << pair { 0xfb38 + 0x8, (uint16_t)0 } // increment pushf: first param is a constant
                << pair { 0xfb38 + 0x10, -shtOffsetDt }; // (same delta value passed for all 4 shooters)

            // initializing offset radius to arg4 + incrementing it without argument
            ecl << pair { 0xf904 + 0x4, (int16_t)0x2c } // init pushi -> pushf
                << pair { 0xf904 + 0x8, (uint16_t)1 } // first param is an argument
                << pair { 0xf904 + 0x10, 16.0f }; // (argument 4)
            ecl << pair { 0xfaa0 + 0x8, (uint16_t)0 } // increment pushf: first param is a constant
                << pair { 0xfaa0 + 0x10, shtOffsetDt }; // (same delta value passed for all 4 shooters)

            // changing attack sub calls to pass new args (+ adding jumps & setting health)
            constexpr uint32_t phaseHealthVals[4] = { 10000, 7000, 4500, 2500 };
            constexpr uint32_t phaseAttackCalls[4] = { 0xf03c, 0xf284, 0xf4bc, 0xf6cc };
            constexpr uint32_t st6bsSp7PhaseStart[4] = { 0xeff0, 0xf238, 0xf448, 0xf680 };
            constexpr uint32_t st6bsSp7PhaseLoop[4] = { 0xf0c0, 0xf2d0, 0xf508, 0xf718 };
            constexpr float st6bsSp7DiffAtkTickRate[4] = { 3.0f, 1.0f, 0.8f, 0.75f };
            const int32_t offsets[4] = { 0, thPracParam.keiki_offsets[0], thPracParam.keiki_offsets[1], thPracParam.keiki_offsets[2] };
            const float tickRate = st6bsSp7DiffAtkTickRate[difficulty];

            // this is to reproduce the float imprecision that happens for spawners who've been looping for a while
            auto loopIncAngle = [](float dt, uint32_t loopCnt) -> float {
                volatile float ang = 0.0f;

                for (volatile size_t i = 0; i < loopCnt; i++)
                    ang = NormRad(ang + dt);

                return ang;
            };

            for (size_t p = 0; p < 4; p++) {
                float initOffDt = 0.0f;
                float initAngDt = 0.0f;

                if (p <= tgtPhase && offsets[p] <= offsets[tgtPhase]) {
                    const uint32_t phaseLoopCnt = (uint32_t)((offsets[tgtPhase] - offsets[p]) / tickRate); // this being accurate for Hard/Luna is a miracle
                    initOffDt = loopIncAngle(shtOffsetDt, phaseLoopCnt);
                    initAngDt = loopIncAngle(shtAngleDt, phaseLoopCnt);
                }

                ecl << pair { phaseAttackCalls[p] + 0x38, 1.0471976f * (p + 1) + initAngDt } // arg2 (now shoot ang cycle offset)
                    << pair { phaseAttackCalls[p] + 0x40, 1.5707964f * (p + 1) - initOffDt } // arg3 (now polar ang cycle offset)
                    << pair { phaseAttackCalls[p] + 0x48, initOffDt }; // arg4 (now polar rad cycle offset)

                if (p < tgtPhase)
                    ECLJump(ecl, st6bsSp7PhaseLoop[p], st6bsSp7PhaseStart[p + 1], 120);
                else if (p == tgtPhase)
                    ecl << pair { st6bsHealthVal, phaseHealthVals[p] };
            }
            break;
        }

        case TH17_ST7_MID1:
            ECLStdExec(ecl, 0xa208, 1, 1);
            if (thPracParam.dialogue)
                ECLJump(ecl, 0, 0xa6ec, 60);
            else {
                ECLJump(ecl, 0, 0xa714, 60);
                ecl.SetFile(3);
                ecl.SetPos(0x410);
                ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 100;
                ECLJump(ecl, 0, 0x55c, 0);
                ecl << pair{0x5a4, 100};
                ECLJump(ecl, 0x5d8, 0x678, 0);
            }
            break;
        case TH17_ST7_MID2:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa714, 60);
            ecl.SetFile(3);
            ecl.SetPos(0x410);
            ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 100;
            ECLJump(ecl, 0, 0x55c, 0);
            ecl << pair { 0x5a4, 100 };

            if (thPracParam.keep_drops) {
                ecl << pair { 0x7fd, (int8_t)0x32 }; // Change Nonspell (2)
            } else {
                ECLJump(ecl, 0x5d8, 0x714, 0);
            }
            break;
        case TH17_ST7_MID3:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa714, 60);
            ecl.SetFile(3);
            ecl.SetPos(0x410);
            ecl << 0 << 0x00140203 << 0x01ff0000 << 0 << 100;
            ECLJump(ecl, 0, 0x55c, 0);
            ecl << pair{0x5a4, 100};
            if (thPracParam.keep_drops) {
                ecl << pair { 0x7fd, (int8_t)0x33 }; // Change Nonspell (3)
            } else {
                ECLJump(ecl, 0x5d8, 0x7b0, 0);
            }
            break;
        case TH17_ST7_END_NS1:
            ECLStdExec(ecl, 0xa208, 1, 1);
            if (thPracParam.dialogue)
                ECLJump(ecl, 0, 0xa7c4, 60);
            else
                ECLJump(ecl, 0, 0xa7d8, 60);
            break;
        case TH17_ST7_END_S1:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0x8a0, 3); // Utilize Spell Practice Jump
            if (!thPracParam.keep_drops)
                ecl << pair { 0x6354, (int16_t)0 }; // Disable Item Drops
            break;
        case TH17_ST7_END_NS2: {
            constexpr unsigned int st7BossCall = 0xa7d8;
            constexpr unsigned int st7BossNonSubCallOrd = 0x1168 + 0x18;
            constexpr unsigned int st7BossSpell1MoveLimit = 0x6600;
            constexpr unsigned int st7BossSpell1PostMoveLimit = 0x6620;

            constexpr unsigned int st7BossNon2ItemDrop1Enable = 0x1cec + 0x4;
            constexpr unsigned int st7BossNon2ItemDrop2Enable = 0x1d24 + 0x4;
            constexpr unsigned int st7BossNon2ItemDrop3Enable = 0x1d5c + 0x4;
            constexpr unsigned int st7BossNon2ItemDrop4Enable = 0x1d94 + 0x4;
            constexpr unsigned int st7BossNon2ItemDrop5Enable = 0x1dcc + 0x4;
            constexpr unsigned int st7BossNon2ItemDropSFXEnable = 0x1ef8 + 0x4;
            constexpr unsigned int st7BossNon2PreKillAll = 0x1e14;
            constexpr unsigned int st7BossNon2PostKillAll = 0x1e34;

            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, st7BossCall, 60);
            ecl.SetFile(2);
            ecl << pair { st7BossNonSubCallOrd, (int8_t)0x32 }; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { st7BossNon2ItemDrop1Enable, (int16_t)0 }
                    << pair { st7BossNon2ItemDrop2Enable, (int16_t)0 }
                    << pair { st7BossNon2ItemDrop3Enable, (int16_t)0 }
                    << pair { st7BossNon2ItemDrop4Enable, (int16_t)0 }
                    << pair { st7BossNon2ItemDrop5Enable, (int16_t)0 } // Disable Item Drops
                    << pair { st7BossNon2ItemDropSFXEnable, (int16_t)0 }; // Disable Item Drops & SE
            }

            // skipping kill all isnt necessary but it allows this jump pattern
            ECLJump(ecl, st7BossNon2PreKillAll, st7BossSpell1MoveLimit, 0);
            ECLJump(ecl, st7BossSpell1PostMoveLimit, st7BossNon2PostKillAll, 0);
            break;
        }
        case TH17_ST7_END_S2:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0x93c, 3); // Utilize Spell Practice Jump
            break;
        case TH17_ST7_END_NS3:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x25b0, (int16_t)0 } << pair { 0x25e8, (int16_t)0 } << pair { 0x2620, (int16_t)0 }
                    << pair { 0x2658, (int16_t)0 } << pair { 0x2690, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x27bc, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x24d8, 0} << pair{0x2954, 0};
            break;
        case TH17_ST7_END_S3:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0x9d8, 3); // Utilize Spell Practice Jump
            break;
        case TH17_ST7_END_NS4:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x34}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x2eac, (int16_t)0 } << pair { 0x2ee4, (int16_t)0 } << pair { 0x2f1c, (int16_t)0 }
                    << pair { 0x2f54, (int16_t)0 } << pair { 0x2f8c, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x30b8, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x2dd4, 0} << pair{0x3260, 0};
            break;
        case TH17_ST7_END_S4:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xa74, 3); // Utilize Spell Practice Jump
            break;
        case TH17_ST7_END_NS5:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x35}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x37e4, (int16_t)0 } << pair { 0x381c, (int16_t)0 } << pair { 0x3854, (int16_t)0 }
                    << pair { 0x388c, (int16_t)0 } << pair { 0x38c4, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x39f0, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x370c, 0} << pair{0x3b88, 0};
            break;
        case TH17_ST7_END_S5:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xb10, 3); // Utilize Spell Practice Jump
            break;
        case TH17_ST7_END_NS6:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x36}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x4110, (int16_t)0 } << pair { 0x4148, (int16_t)0 } << pair { 0x4180, (int16_t)0 }
                    << pair { 0x41b8, (int16_t)0 } << pair { 0x41f0, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x431c, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x4008, 0} << pair{0x44c4, 0};
            break;
        case TH17_ST7_END_S6:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xbac, 3); // Utilize Spell Practice Jump
            break;
        case TH17_ST7_END_NS7:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x37}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x4a48, (int16_t)0 } << pair { 0x4a80, (int16_t)0 } << pair { 0x4ab8, (int16_t)0 }
                    << pair { 0x4af0, (int16_t)0 } << pair { 0x4b28, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x4c54, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x4970, 0} << pair{0x4dfc, 0};
            break;
        case TH17_ST7_END_S7:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xc48, 3); // Utilize Spell Practice Jump
            break;
        case TH17_ST7_END_NS8:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ecl << pair{0x1180, (int8_t)0x38}; // Change Nonspell
            if (!thPracParam.keep_drops) {
                ecl << pair { 0x5380, (int16_t)0 } << pair { 0x53b8, (int16_t)0 } << pair { 0x53f0, (int16_t)0 }
                    << pair { 0x5428, (int16_t)0 } << pair { 0x5460, (int16_t)0 }; // Disable Item Drops
                ecl << pair { 0x558c, (int16_t)0 }; // Disable Item Drops & SE
            }
            ecl << pair{0x52a8, 0} << pair{0x5734, 0};
            break;
        case TH17_ST7_END_S8:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x800, 0xce4, 3); // Utilize Spell Practice Jump
            break;
        case TH17_ST7_END_S9:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            if (thPracParam.keep_drops) {
                ecl << pair { 0x1180, (int8_t)0x39 }; // Change Nonspell (9)
            } else {
                ECLJump(ecl, 0x800, 0xd80, 3); // Utilize Spell Practice Jump
            }
            break;
        case TH17_ST7_END_S10:
            ECLStdExec(ecl, 0xa208, 1, 1);
            ECLJump(ecl, 0, 0xa7d8, 60);
            ecl.SetFile(2);
            if (thPracParam.keep_drops) {
                ecl << pair { 0x1180, (int8_t)0x31 } << (int8_t)0x30; // Change Nonspell (10)
            } else {
                ECLJump(ecl, 0x800, 0xe1c, 3); // Utilize Spell Practice Jump
            }

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe2c, 7000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                break;
            case 2:
                ecl << pair{0xe2c, 4500};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                break;
            case 3:
                ecl << pair{0xe2c, 2000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                break;
            case 4:
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                ECLJump(ecl, 0x6ec8, 0x6f10, 90);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    void THPatchSP(ECLHelper& ecl, i32 spell_id)
    {
        switch (spell_id) {
        case 84:
        case 85:
        case 86:
        case 87:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xb98, 7000};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                break;
            case 2:
                ecl << pair{0xb98, 4500};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                ECLJump(ecl, 0xf2d0, 0xf448, 120);
                break;
            case 3:
                ecl << pair{0xb98, 2500};
                ECLJump(ecl, 0xf0c0, 0xf238, 120);
                ECLJump(ecl, 0xf2d0, 0xf448, 120);
                ECLJump(ecl, 0xf508, 0xf680, 120);
                break;
            default:
                break;
            }
            break;
        case 100:
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe2c, 7000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                break;
            case 2:
                ecl << pair{0xe2c, 4500};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                break;
            case 3:
                ecl << pair{0xe2c, 2000};
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                break;
            case 4:
                ECLJump(ecl, 0x6d3c, 0x6d84, 90);
                ECLJump(ecl, 0x6dcc, 0x6e14, 90);
                ECLJump(ecl, 0x6e38, 0x6e80, 90);
                ECLJump(ecl, 0x6ec8, 0x6f10, 90);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
