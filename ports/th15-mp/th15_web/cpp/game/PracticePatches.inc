// Generated from thprac_purple TH15 (MIT), LF sha256 2b412168cd8d71dd7ef8e1f79a1a5d33cf64f983792cbc9d0f4ccbc56e60ce03.
// Only platform-hook ownership and bounds observations are adapted.
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.Jump(start, dest);
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }

    static constexpr unsigned int st7Start = 0x8f6c;
    void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x7248, 0x7494, 60, 90); // 0x7544
                ECLJump(ecl, 0x3f4c, 0x3fa8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x7248, 0x7544, 60, 90);
                ECLSetChapter(2);
                break;
            case 4:
                ECLJump(ecl, 0x7248, 0x7544, 60, 90);
                ECLJump(ecl, 0x4098, 0x40fc, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x7248, 0x75f8, 60, 90);
                ECLSetChapter(5);
                break;
            case 6:
                ECLJump(ecl, 0x7248, 0x75f8, 60, 90);
                ECLJump(ecl, 0x4150, 0x41b8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x74a0, 0x76ec, 60, 90); // 0x778c
                ECLJump(ecl, 0x42b4, 0x4354, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x74a0, 0x76ec, 60, 90); // 0x778c
                ECLJump(ecl, 0x42b4, 0x43d4, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x74a0, 0x778c, 60, 29);
                ECLSetChapter(4);
                break;
            case 5:
                ECLJump(ecl, 0x74a0, 0x778c, 60, 0);
                ECLJump(ecl, 0x443c, 0x448c, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x74a0, 0x778c, 60, 90);
                ECLJump(ecl, 0x443c, 0x450c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0xa280, 0xa4cc, 60, 90); // 0xa56c
                ECLJump(ecl, 0x5b6c, 0x5c00, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0xa280, 0xa4cc, 60, 30); // 0xa56c
                ECLJump(ecl, 0x5b6c, 0x5c6c, 0, 0);
                if (thPracParam.phase == 1) {
                    ecl << pair { 0x71D8, (int32_t)30 };
                    ECLJump(ecl, 0x71DC, 0x7468, 0, 0);
                }
                break;
            case 4:
                ECLJump(ecl, 0xa280, 0xa56c, 60, 50);
                ECLSetChapter(4);
                break;
            case 5:
                ECLJump(ecl, 0xa280, 0xa56c, 60, 20);
                ECLJump(ecl, 0x5d0c, 0x5d70, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0xa280, 0xa56c, 60, 90);
                ECLJump(ecl, 0x5d0c, 0x5df0, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8634, 0x8880, 60, 90); // 0x8930
                ECLJump(ecl, 0x406c, 0x40ec, 0, 0);
                if (thPracParam.phase == 1) {
                    ecl << pair { 0x4DD0, (int32_t)0 };
                    ECLJump(ecl, 0x485C, 0x4DC0, 0, 0);
                }
                break;
            case 3:
                ECLJump(ecl, 0x8634, 0x8880, 60, 90); // 0x8930
                ECLJump(ecl, 0x406c, 0x4158, 0, 0);
                ecl << pair{0x5cc4, 0};
                break;
            case 4:
                ECLJump(ecl, 0x8634, 0x8930, 60, 90);
                ecl << pair{0x6bc0, 0};
                break;
            case 5:
                ECLJump(ecl, 0x8634, 0x8930, 60, 90);
                ECLJump(ecl, 0x41e0, 0x4244, 0, 0);
                ecl << pair{0x6e74, 0};
                break;
            case 6:
                ECLJump(ecl, 0x8634, 0x8930, 60, 90);
                ECLJump(ecl, 0x41e0, 0x42c4, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x9f34, 0xa180, 60, 90); // 0xa1a0
                ECLJump(ecl, 0x6ed8, 0x6f58, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x9f34, 0xa180, 60, 90); // 0xa1a0
                ECLJump(ecl, 0x6ed8, 0x7040, 0, 0);
                ecl << pair{0x7ba0, 0};
                break;
            case 4:
                ECLJump(ecl, 0x9f34, 0xa180, 60, 90); // 0xa1a0
                ECLJump(ecl, 0x6ed8, 0x7098, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ecl << pair{0x70f0, (int16_t)0};
                break;
            case 6:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x716c, 0, 0);
                ecl << pair{0x81c0, 0};
                break;
            case 7:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x71ec, 0, 0);
                ecl << pair{0x845c, 0};
                break;
            case 8:
                switch (thPracParam.phase)
                {
                default:
                case 0:
                    break;
                case 1:
                    ECLJump(ecl, 0x89C0, 0x8824, 0, 0);
                    break;
                case 2:
                    ECLJump(ecl, 0x880C, 0x8A00, 0, 0);
                    ECLJump(ecl, 0x8C78, 0x8A40, 0, 0);
                    break;
                case 3:
                    ECLJump(ecl, 0x880C, 0x8A00, 0, 0);
                    break;
                }
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x726c, 0, 0);
                ecl << pair{0x87e0, 0};
                break;
            case 9:
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x72ec, 0, 0);
                ecl << pair{0x8ce8, 0};
                break;
            case 10:
                if (thPracParam.phase == 1) {
                    ECLJump(ecl, 0x9CE0, 0x9ABC, 0, 0);
                }
                ECLJump(ecl, 0x9f34, 0xa1a0, 60, 90);
                ECLJump(ecl, 0x70ec, 0x736c, 0, 0);
                ecl << pair{0x9a50, 0};
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x91f8, 0x93e0, 60, 90);
                ECLJump(ecl, 0x2b34, 0x2be8, 0, 0);
                ecl << pair{0x38d0, 0};
                break;
            case 3:
                ECLJump(ecl, 0x91f8, 0x93e0, 60, 90);
                ECLJump(ecl, 0x2b34, 0x2c68, 0, 0);
                ecl << pair{0x40ac, 0};
                break;
            case 4:
                ECLJump(ecl, 0x91f8, 0x93e0, 60, 90);
                ECLJump(ecl, 0x2b34, 0x2ce8, 0, 0);
                ecl << pair{0x43b0, 0};
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8f94, 0x9208, 60, 90); // 0x9338
                ECLJump(ecl, 0x5d08, 0x5d88, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8f94, 0x9208, 60, 90); // 0x9338
                ECLJump(ecl, 0x5d08, 0x5e08, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x8f94, 0x9208, 60, 90); // 0x9338
                ECLJump(ecl, 0x5d08, 0x5e9c, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x8f94, 0x9208, 60, 90); // 0x9338
                ECLJump(ecl, 0x5d08, 0x5f30, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x8f94, 0x9338, 60, 90);
                break;
            case 7:
                ECLJump(ecl, 0x8f94, 0x9338, 60, 10);
                ECLJump(ecl, 0x5f84, 0x6018, 0, 0);
                break;
            case 8:
                ECLJump(ecl, 0x8f94, 0x9338, 60, 90);
                ECLJump(ecl, 0x5f84, 0x60c0, 0, 0);
                break;
            case 9:
                ECLJump(ecl, 0x8f94, 0x9338, 60, 90);
                ECLJump(ecl, 0x5f84, 0x6154, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    void THPatch(ECLHelper& ecl, int section)
    {
        auto st7_hide_subboss = [&]() {
            ecl.SetFile(4);
            // Void Particle Effect & Wait
            ecl << pair{0x190, (int16_t)0} << pair{0x1dc, (int16_t)0} << pair{0x228, (int16_t)0};
            // Jump Through Dialogue
            ECLJump(ecl, 0x2a0, 0x2f4, 5);
            // Remove Movement Restriction & Move Directly
            ecl.SetPos(0x480);
            ecl << 0 << 0x001001f9 << 0x00ff0000 << 0
                << 0 << 0x00340190 << 0x02ff0000 << 0 << 0x43400000 << 0xc2800000;
        };
        auto st7_enter_spell = [&](int ordinal, int health, bool is_junko = false) {
            ecl.SetFile(3);
            ecl.SetPos(0x6d0);
            ecl << 0 << 0x001401ff << 0x01ff0000 << 0 << health;
            if (is_junko) {
                ecl << 0 << 0x0014012e << 0x01ff0000 << 0 << 5
                    << 0 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
                ecl << 0 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                    << 0x73736f42 << 0x6f705f34 << 0x00000073; // "Boss", "4_po", "s"
            }
            ecl << 0 << 0x0020000b << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x64726143 << ordinal;
        };
        constexpr unsigned int st7BossCreateCall = 0x93cc;
        constexpr unsigned int st7bsMovePosX = 0x498 + 0x10; // 0x4a8
        constexpr unsigned int st7bsMovePosY = 0x498 + 0x14; // 0x4ac
        constexpr unsigned int st7bsANMSelect = 0x56c + 0x10; // 0x57c
        constexpr unsigned int st7bsANMSetMainArg2 = 0x580 + 0x14; // 0x594
        constexpr unsigned int st7bsSetSpriteIns = 0x5d4 + 0x4; // 0x5d8
        constexpr unsigned int st7bsANMSetSprite1Arg2 = 0x674 + 0x14; // 0x688
        constexpr unsigned int st7bsANMSetSprite2Arg2 = 0x68c + 0x14; // 0x6a0
        constexpr unsigned int st7bsMoveLimitIns = 0x640 + 0x4; // 0x644

        switch (section) {
        case TH15_ST1_MID1:
            ECLJump(ecl, 0x7220, 0x74d8, 60);
            ECLSkipChapter(1);
            break;
        case TH15_ST1_MID2:
            ECLJump(ecl, 0x7220, 0x758c, 60);
            ECLSkipChapter(1);
            break;
        case TH15_ST1_MID3:
            ECLJump(ecl, 0x7220, 0x75b4, 60);
            ecl.SetFile(4);
            ECLSkipChapter(1);
            ECLJump(ecl, 0x444, 0x560, 0); // Utilize Spell Practice Jump
            ecl << pair{0x434, 60} << pair{0x570, 60}; // Move Speed
            ecl << pair{0x588, 9999}; // Do not use spellcard
            ecl.SetPos(0x384);
            ecl << 0 << 0x001c0203 << 0x01ff0000 << 0 << 60; // Muteki
            break;
        case TH15_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x7220, 0x7678, 60);
            else
                ECLJump(ecl, 0x7220, 0x768c, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            break;
        case TH15_ST1_BOSS2:
            ECLJump(ecl, 0x7220, 0x768c, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x3b8, 0x4a0, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4b0, 1700}; // Set Health
            ecl << pair{0x4d0, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case TH15_ST1_BOSS3:
            ECLJump(ecl, 0x7220, 0x768c, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x5d0, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x13ec, (int16_t)0} << pair{0x151c, (int16_t)0}; // Disable Item Drops & SE
            break;
        case TH15_ST1_BOSS4:
            ECLJump(ecl, 0x7220, 0x768c, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x3b8, 0x4a0, 1); // Utilize Spell Practice Jump
            ecl << pair{0x4b0, 1900}; // Set Health
            ecl << pair{0x4d0, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case TH15_ST2_MID1:
            ECLJump(ecl, 0x7444, 0x7720, 60);
            ECLSkipChapter(1);
            break;
        case TH15_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x7444, 0x7820, 60);
            else
                ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            break;
        case TH15_ST2_BOSS2:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2200}; // Set Health
            ecl << pair{0x524, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case TH15_ST2_BOSS3:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x644, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1214, (int16_t)0} << pair{0x1344, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x14bc, 59} << pair{0x1508, 0} << pair{0x1120, 60}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH15_ST2_BOSS4:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2400}; // Set Health
            ecl << pair{0x524, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case TH15_ST2_BOSS5:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x644, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x1ea8, (int16_t)0} << pair{0x1fd8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2150, 59} << pair{0x219c, 0} << pair{0x1db4, 60}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH15_ST2_BOSS6:
            ECLJump(ecl, 0x7444, 0x7834, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x40c, 0x4f4, 1); // Utilize Spell Practice Jump
            ecl << pair{0x504, 2500}; // Set Health
            ecl << pair{0x524, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case TH15_ST3_MID1:
            ECLJump(ecl, 0xa258, 0xa500, 60);
            ECLSkipChapter(1);
            break;
        case TH15_ST3_MID2:
            ECLJump(ecl, 0xa258, 0xa528, 60);
            ecl.SetFile(3);
            ECLSkipChapter(1);
            ECLJump(ecl, 0x360, 0x47c, 0); // Utilize Spell Practice Jump
            ecl << pair{0x350, 60} << pair{0x48c, 60}; // Move Speed
            ecl << pair{0x4a4, 9999}; // Do not use spellcard
            ecl.SetPos(0x2a4);
            ecl << 0 << 0x001c0203 << 0x01ff0000 << 0 << 60; // Muteki
            break;
        case TH15_ST3_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0xa258, 0xa600, 60);
            else
                ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            // wave2
            switch (thPracParam.phase) {
            case 0:
                break;
            case 1:
                ECLJump(ecl, 0x7D4, 0x898, 0);
                break;
            case 2:
                ecl << pair { 0xD3C, thPracParam.doremy_normal_1_phase };
                ecl << pair { 0xD34, (uint8_t)0x00 };
                break;
            case 3:
                ecl << pair { 0xD3C, thPracParam.doremy_normal_1_phase };
                ecl << pair { 0xD34, (uint8_t)0x00 };
                ECLJump(ecl, 0x7D4, 0x898, 0);
                break;
            default:
                break;
            }
            //---
            break;
        case TH15_ST8_AB_TEST:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            ecl << pair { 0x23C, (int8_t)0 };
            ecl << pair { 0x288, (int8_t)0 };
            ecl << pair { 0x2D4, (int8_t)0 };
            for (int i = 0; i < sizeof(th15_abtest_ecl_file);i++)
                ecl << pair { 0x694 + i, th15_abtest_ecl_file[i] };
            ecl << pair { 0x694 + (0x66C - 0x5E8), (int32_t)thPracParam.phase + 1 };
            //---
            break;
        case TH15_ST3_BOSS2:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 2000}; // Set Health
            ecl << pair{0x560, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case TH15_ST3_BOSS3:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x680, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x15d8, (int16_t)0} << pair{0x1708, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x18ac, 0} << pair{0x14e4, 60}; // Change Wait Time & Inv. Time
            break;
        case TH15_ST3_BOSS4:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 2100}; // Set Health
            ecl << pair{0x560, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case TH15_ST3_BOSS5:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x680, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x27b0, (int16_t)0} << pair{0x28e0, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x2a98, 0} << pair{0x2a58, 60}; // Change Wait Time & Inv. Time
            break;
        case TH15_ST3_BOSS6:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 2500}; // Set Health
            ecl << pair{0x560, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case TH15_ST3_BOSS7:
            ECLJump(ecl, 0xa258, 0xa614, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x448, 0x530, 1); // Utilize Spell Practice Jump
            ecl << pair{0x540, 3000}; // Set Health
            ecl << pair{0x560, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case TH15_ST4_MID1:
            ECLJump(ecl, 0x860c, 0x88ec, 60);
            ECLSkipChapter(1);
            break;
        case TH15_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x860c, 0x899c, 60);
            else
                ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(1);
            break;
        case TH15_ST4_BOSS2:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x41e0, 0x42c8, 1); // Utilize Spell Practice Jump
            ecl << pair{0x42d8, 2300}; // Set Health
            ecl << pair{0x42f8, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case TH15_ST4_BOSS3:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x4418, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x4f24, (int16_t)0} << pair{0x5054, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x51cc, 20} << pair{0x4e30, 0}; // Change Wait Time & Inv. Time
            break;
        case TH15_ST4_BOSS4:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x41e0, 0x42c8, 1); // Utilize Spell Practice Jump
            ecl << pair{0x42d8, 3100}; // Set Health
            ecl << pair{0x42f8, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case TH15_ST4_BOSS5:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x4418, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x5e70, (int16_t)0} << pair{0x5d40, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x5ffc, 20} << pair{0x5fe8, 0}; // Change Wait Time & Inv. Time
            break;
        case TH15_ST4_BOSS6:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x41e0, 0x42c8, 1); // Utilize Spell Practice Jump
            ecl << pair{0x42d8, 2500}; // Set Health
            ecl << pair{0x42f8, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case TH15_ST4_BOSS7:
            ECLJump(ecl, 0x860c, 0x89b0, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x41e0, 0x42b4, 1); // Utilize Spell Practice Jump
            ecl.SetPos(0x42b4);
            ecl << 1 << 0x00140279 << 0x01ff0000 << 0 << 1;
            ecl << pair{0x42d8, 3400}; // Set Health
            ecl << pair{0x42f8, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case TH15_ST5_MID1:
            switch (thPracParam.phase) {
            case 1:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x7100, 0);
                break;
            case 2:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x716c, 0);
                break;
            case 3:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x71ec, 0);
                break;
            case 4:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x726c, 0);
                break;
            case 5:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x72ec, 0);
                break;
            case 6:
                ECLJump(ecl, 0x9f0c, 0xa1a0, 60);
                ECLJump(ecl, 0x70ec, 0x736c, 0);
                break;

            default:
                ECLJump(ecl, 0x9f0c, 0xa180, 60);
                ECLJump(ecl, 0x6ed8, 0x7074, 0);
                break;
            }
            ECLSkipChapter(1);
            break;
        case TH15_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x9f0c, 0xa20c, 60);
            else
                ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(1);
            break;
        case TH15_ST5_BOSS2:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 1}; // Set Health
            ecl << pair{0x640, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case TH15_ST5_BOSS3:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ecl << pair{0x760, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1394, (int16_t)0} << pair{0x14c4, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x163c, 59} << pair{0x165c, 0} << pair{0x12ac, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH15_ST5_BOSS4:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 3400}; // Set Health
            ecl << pair{0x640, (int8_t)0x32}; // Set Spell Ordinal
            switch (thPracParam.phase) {
            default:
            case 0:
                break;
            case 1:
                ecl << pair { 0x5B20, (int16_t)542 };
                break;
            }
            break;
        case TH15_ST5_BOSS5:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ecl << pair{0x760, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x2744, (int16_t)0} << pair{0x2874, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x29ec, 59} << pair{0x2a0c, 0}; // Change Move Time & Wait Time
            break;
        case TH15_ST5_BOSS6:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 2320}; // Set Health
            ecl << pair{0x640, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case TH15_ST5_BOSS7:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 3001}; // Set Health
            ecl << pair{0x640, (int8_t)0x34}; // Set Spell Ordinal
            break;
        case TH15_ST5_BOSS8:
            ECLJump(ecl, 0x9f0c, 0xa220, 60);
            ecl.SetFile(3);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x528, 0x610, 0); // Utilize Spell Practice Jump
            ecl << pair{0x620, 1}; // Set Health
            ecl << pair{0x640, (int8_t)0x35}; // Set Spell Ordinal
            break;
        case TH15_ST6_STARS:
            if (thPracParam.phase == 1) {
                ECLJump(ecl, 0x91d0, 0x93e0, 0);
                ECLJump(ecl, 0x2b34, 0x2c68, 0);
                ecl << pair(0x436C, (int32_t)99999999) // endless
                    << pair(0x40AC, (int32_t)0)
                    // << pair(0x40F4, (int32_t)0)
                    // << pair(0x413C, (int32_t)0)
                    // << pair(0x4184, (int32_t)0)
                    // << pair(0x41CC, (int32_t)0)
                    << pair(0x4214, (int32_t)0)
                    //<< pair(0x425C, (int32_t)0)
                    //<< pair(0x42A4, (int32_t)0)
                    //<< pair(0x42EC, (int32_t)0)
                    //<< pair(0x4334, (int32_t)0)
                    << pair(0x79CC, (int32_t)99999999)
                    << pair(0x8FC4, (int32_t)99999999)
                    ;
            }else{
                ECLJump(ecl, 0x91d0, 0x93e0, 0);
                ECLJump(ecl, 0x2b34, 0x2c40, 60);
                ECLSkipChapter(1);
                ECLStarsBGMSync();
            }
            break;
        case TH15_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x91d0, 0x94ec, 60);
            else
                ECLJump(ecl, 0x91d0, 0x9500, 60);
            ECLSkipChapter(1);
            break;
        case TH15_ST6_BOSS2:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x688, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 4000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x31}; // Set Spell Ordinal
            break;
        case TH15_ST6_BOSS3:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x7d8, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1274, (int16_t)0} << pair{0x13a4, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x1528, 0} << pair{0x1178, 0}; // Change Wait Time & Inv. Time
            break;
        case TH15_ST6_BOSS4:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            if (thPracParam.phase == 1) {
                ECLJump(ecl, 0x5A94, 0x5BB8, 94);
                ECLJump(ecl, 0x390, 0x43C, 0);//disable waiting time
            }
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x688, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 2600}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x32}; // Set Spell Ordinal
            break;
        case TH15_ST6_BOSS5:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x7d8, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x1f84, (int16_t)0} << pair{0x20b4, (int16_t)0}; // Disable Item Drops & SE
            break;
        case TH15_ST6_BOSS6:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            if (thPracParam.phase == 1) {
                ECLJump(ecl, 0x390, 0x43C, 0);//disable waiting time
                ecl << pair { 0x94BC, -9999.0f};
                ecl << pair { 0x94D0, (2.0f - thPracParam.enhanced_para) * 1.570796f };
                
            }
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x63c, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 4000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x33}; // Set Spell Ordinal
            ecl.SetPos(0x63c); // Boss Effect
            ecl << 0 << 0x0014012e << 0x01ff0000 << 0 << 3
                << 0 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
            ecl << 0 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x6f705f33 << 0x00000073; // "Boss", "3_po", "s"
            break;
        case TH15_ST6_BOSS7:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ecl << pair{0x7d8, (int8_t)0x34}; // Change Nonspell
            ecl << pair{0x302c, (int16_t)0} << pair{0x315c, (int16_t)0}; // Disable Item Drops & SE
            break;
        case TH15_ST6_BOSS8:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x63c, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 3700}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x34}; // Set Spell Ordinal
            ecl.SetPos(0x63c); // Boss Effect
            ecl << 0 << 0x0014012e << 0x01ff0000 << 0 << 3
                << 0 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
            ecl << 0 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x6f705f34 << 0x00000073; // "Boss", "4_po", "s"
            break;
        case TH15_ST6_BOSS9:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x688, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 5000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x35}; // Set Spell Ordinal
            break;
        case TH15_ST6_BOSS10:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x688, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 6000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x36}; // Set Spell Ordinal
            break;
        case TH15_ST6_BOSS11:
            ECLJump(ecl, 0x91d0, 0x9500, 60);
            ecl.SetFile(2);
            ECLSkipChapter(2);
            ECLJump(ecl, 0x5a0, 0x63c, 0); // Utilize Spell Practice Jump
            ecl << pair{0x698, 8000}; // Set Health
            ecl << pair{0x6b8, (int8_t)0x37}; // Set Spell Ordinal
            ecl.SetPos(0x63c); // Boss Effect
            ecl << 0 << 0x0014012e << 0x01ff0000 << 0 << 3
                << 0 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
            ecl << 0 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x6f705f34 << 0x00000073; // "Boss", "4_po", "s"

            if (thPracParam.phase) {
                if (thPracParam.phase == 1)
                    ecl << pair{0x698, 5500};
                else if (thPracParam.phase == 2)
                    ecl << pair{0x698, 3500};
                else if (thPracParam.phase == 3)
                    ecl << pair{0x698, 1200};
            }
            break;
        case TH15_ST7_MID1:
            if (thPracParam.dlg)
                ECLJump(ecl, st7Start, 0x9270, 60);
            else {
                ECLJump(ecl, st7Start, 0x92a8, 60);
                ECLSkipChapter(1);
                ecl.SetFile(2);

                ecl.SetPos(0x34c); // Inv. Time
                ecl << 0 << 0x00200203 << 0x01ff0000 << 0 << 60;
                ecl.SetPos(0x37c); // Boss Movement Restriction
                ecl << 0 << 0x002c01f8 << 0x04ff0000 << 0
                    << 0x0 << 0x43000000 << 0x438c0000 << 0x43800000;
                ecl << pair{0x33c, (int16_t)0}; // Void Wait
                ecl << pair{0x36c, 60} << pair{0x328, 60}; // Wait Time & Move Time
                ECLJump(ecl, 0x48c, 0x508, 0); // Spell Jump
                ecl << pair{0x2fc, 2200} << pair{0x525, (int8_t)0x31}; // Spell Health & Spell Ordinal
            }
            break;
        case TH15_ST7_MID2:
            ECLJump(ecl, st7Start, 0x92a8, 60);
            ECLSkipChapter(1);
            ecl.SetFile(2);

            ecl.SetPos(0x34c); // Inv. Time
            ecl << 0 << 0x00200203 << 0x01ff0000 << 0 << 60;
            ecl.SetPos(0x37c); // Boss Movement Restriction
            ecl << 0 << 0x002c01f8 << 0x04ff0000 << 0
                << 0x0 << 0x43000000 << 0x438c0000 << 0x43800000;
            ecl << pair{0x33c, (int16_t)0}; // Void Wait
            ecl << pair{0x36c, 60} << pair{0x328, 60}; // Wait Time & Move Time
            ECLJump(ecl, 0x48c, 0x508, 0); // Spell Jump
            ecl << pair{0x2fc, 2200} << pair{0x525, (int8_t)0x32}; // Spell Health & Spell Ordinal
            break;
        case TH15_ST7_MID3:
            ECLJump(ecl, st7Start, 0x92a8, 60);
            ECLSkipChapter(1);
            ecl.SetFile(2);

            ecl.SetPos(0x34c); // Inv. Time
            ecl << 0 << 0x00200203 << 0x01ff0000 << 0 << 60;
            ecl.SetPos(0x37c); // Boss Movement Restriction
            ecl << 0 << 0x002c01f8 << 0x04ff0000 << 0
                << 0x0 << 0x43000000 << 0x438c0000 << 0x43800000;
            ecl << pair{0x33c, (int16_t)0}; // Void Wait
            ecl << pair{0x36c, 60} << pair{0x328, 60}; // Wait Time & Move Time
            ECLJump(ecl, 0x48c, 0x508, 0); // Spell Jump
            ecl << pair{0x2fc, 3000} << pair{0x525, (int8_t)0x33}; // Spell Health & Spell Ordinal
            break;
        case TH15_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLJump(ecl, st7Start, 0x93b8, 60);
            else {
                effects.extra_boss_chapter_bonus = true;
                ECLJump(ecl, st7Start, st7BossCreateCall, 60);
                st7_hide_subboss();
            }
            ECLSkipChapter(1);
            break;
        case TH15_ST7_END_S1:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x31, 3000);
            break;
        case TH15_ST7_END_NS2:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x32}; // Change Nonspell
            ecl << pair{0x16f4, (int16_t)0} << pair{0x1824, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x199c, 59} << pair{0x19bc, 0} << pair{0x15dc, 0}; // Change Move Time, Wait Time & Inv. Time

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0} << pair{0x191c, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{st7bsANMSetMainArg2, 14} << pair{st7bsANMSetSprite2Arg2, 120}; // Change 306-0 & 303-2
            ECLJump(ecl, 0x1a90, 0x1ae8, 24); // Jump Over Sprite Change
            break;
        case TH15_ST7_END_S2:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x32, 3400);

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0}; // Void 303-0
            ecl << pair{st7bsANMSetMainArg2, 14} << pair{st7bsANMSetSprite2Arg2, 120}; // Change 306-0 & 303-2
            break;
        case TH15_ST7_END_NS3:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x33}; // Change Nonspell
            ecl << pair{0x2430, (int16_t)0} << pair{0x2560, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x26d8, 59} << pair{0x26f8, 0} << pair{0x2318, 0}; // Change Move Time, Wait Time & Inv. Time

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0} << pair{0x2658, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{st7bsANMSetMainArg2, 7} << pair{st7bsANMSetSprite2Arg2, 132}; // Change 306-0 & 303-2
            ECLJump(ecl, 0x276c, 0x2798, 24); // Jump Over Sprite Change
            break;
        case TH15_ST7_END_S3:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x33, 3000);

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0}; // Void 303-0
            ecl << pair{st7bsANMSetMainArg2, 7} << pair{st7bsANMSetSprite2Arg2, 132}; // Change 306-0 & 303-2
            break;
        case TH15_ST7_END_NS4:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x34}; // Change Nonspell
            ecl << pair{0x32a0, (int16_t)0}; // Don't Activate Sub Boss
            ecl << pair{0x32e8, (int16_t)0} << pair{0x3418, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x3548, 0x35b0, 0); // Skip Move
            ecl << pair{0x31c4, 10}; // Inv. Time

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0} << pair{0x3510, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{st7bsANMSetMainArg2, 0} << pair{st7bsANMSetSprite2Arg2, 116}; // Change 306-0 & 303-2
            ecl << pair{st7bsANMSelect, 5}; // Change 302
            break;
        case TH15_ST7_END_S4:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x34, 2000, true);

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0}; // Void 303-0
            ecl << pair{st7bsANMSetMainArg2, 0} << pair{st7bsANMSetSprite2Arg2, 116}; // Change 306-0 & 303-2
            ecl << pair{st7bsANMSelect, 5}; // Change 302
            break;
        case TH15_ST7_END_NS5:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x35}; // Change Nonspell
            ecl << pair{0x4464, (int16_t)0}; // Don't Activate Sub Boss
            ecl << pair{0x44c0, (int16_t)0} << pair{0x45f0, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x4720, 0x4788, 0); // Skip Move
            ecl << pair{0x4388, 0}; // Inv. Time

            ecl.SetFile(3);
            ecl << pair{0x46e8, (int16_t)0}; // Void 306-0
            break;
        case TH15_ST7_END_S5:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x35, 3500);
            break;
        case TH15_ST7_END_NS6:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x36}; // Change Nonspell
            ecl << pair{0x5368, (int16_t)0} << pair{0x5498, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x5610, 59} << pair{0x5630, 0} << pair{0x5250, 0}; // Change Move Time, Wait Time & Inv. Time

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0} << pair{0x5590, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{st7bsANMSetMainArg2, 14} << pair{st7bsANMSetSprite2Arg2, 120}; // Change 306-0 & 303-2
            ECLJump(ecl, 0x5694, 0x56ec, 24); // Jump Over Sprite Change
            break;
        case TH15_ST7_END_S6:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x36, 3500);
            ecl << pair{0xd898, (int16_t)0}; // Void 316

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0}; // Void 303-0
            ecl << pair{st7bsANMSetMainArg2, 14} << pair{st7bsANMSetSprite2Arg2, 120}; // Change 306-0 & 303-2
            break;
        case TH15_ST7_END_NS7:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x37}; // Change Nonspell
            ecl << pair{0x6198, (int16_t)0} << pair{0x62c8, (int16_t)0}; // Disable Item Drops & SE
            ecl << pair{0x6440, 59} << pair{0x6460, 0} << pair{0x6080, 0}; // Change Move Time, Wait Time & Inv. Time

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0} << pair{0x63c0, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{st7bsANMSetMainArg2, 7} << pair{st7bsANMSetSprite2Arg2, 132}; // Change 306-0 & 303-2
            ECLJump(ecl, 0x64d4, 0x652c, 24); // Jump Over Sprite Change
            break;
        case TH15_ST7_END_S7:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x37, 3000);

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0}; // Void 303-0
            ecl << pair{st7bsANMSetMainArg2, 7} << pair{st7bsANMSetSprite2Arg2, 132}; // Change 306-0 & 303-2
            break;
        case TH15_ST7_END_NS8:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();

            ecl.SetFile(3);
            ecl << pair{0x708, 0x38}; // Change Nonspell
            ecl << pair{0x7004, (int16_t)0}; // Don't Activate Sub Boss
            ecl << pair{0x708c, (int16_t)0} << pair{0x71bc, (int16_t)0}; // Disable Item Drops & SE
            ECLJump(ecl, 0x72ec, 0x7354, 0); // Skip Move
            ecl << pair{0x6f54, 0}; // Inv. Time

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0} << pair{0x72b4, (int16_t)0}; // Void 303-0 & 306-0
            ecl << pair{st7bsANMSetMainArg2, 0} << pair{st7bsANMSetSprite2Arg2, 116}; // Change 306-0 & 303-2
            ecl << pair{st7bsANMSelect, 5}; // Change 302
            break;
        case TH15_ST7_END_S8:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            st7_enter_spell(0x38, 3000, true);

            ecl.SetFile(3);
            ecl << pair{st7bsSetSpriteIns, (int16_t)0}; // Void 303-0
            ecl << pair{st7bsANMSetMainArg2, 0} << pair{st7bsANMSetSprite2Arg2, 116}; // Change 306-0 & 303-2
            ecl << pair{st7bsANMSelect, 5}; // Change 302
            break;
        case TH15_ST7_END_S9:
        {
            constexpr unsigned int st7bsANMSelect2 = 0x6a4 + 0x10; // 0x6b4
            constexpr unsigned int st7bsANMSetMainIns = 0x580 + 0x4; // 0x584

            constexpr unsigned int st7BossSpell9Duration = 0x10408 + 0x18;
            constexpr unsigned int st7BossSpell9TRStart = 0x104b8;
            constexpr unsigned int st7BossSpell9TRBluePhaseTime = 120 * 3 + 180 * 5 + 280;

            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);
            st7_hide_subboss();
            ecl.SetFile(3);
            ecl << pair { st7bsMovePosX, 0 } << pair { st7bsMovePosY, 0x43600000 }; // Center pos
            ecl << pair { st7bsSetSpriteIns, (int16_t)0 }; // Cancel changing boss sprite
            ecl << pair { st7bsANMSelect, 5 } << pair { st7bsANMSelect2, 5 }; // Change selected anim
            ecl << pair { st7bsANMSetMainIns, (int16_t)0 }
                << pair(st7bsANMSetSprite1Arg2, -1)
                << pair(st7bsANMSetSprite2Arg2, -1); // More ANM fixing
            ecl << pair { st7bsMoveLimitIns, (int16_t)0 }; // Remove move limits
            ecl.SetPos(0x6d0);
            // 306
            ecl << 0 << 0x00180132 << 0x02ff0000 << 0 << 0 << 0;
            // Flag
            ecl << 0 << 0x001401f6 << 0x01ff0000 << 0 << 0x20;
            // Health
            ecl << 0 << 0x001401ff << 0x01ff0000 << 0 << 3000;
            // Card
            ecl << 0 << 0x0020000b << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x64726143 << 0x39;

            switch (thPracParam.phase) {
            case 1: {
                constexpr unsigned int st7BossSpell9TRGreenPhase = 0x10a50;

                ecl << pair { st7BossSpell9Duration, 5400 - st7BossSpell9TRBluePhaseTime + 90 }; // Adjust duration (removing waits)
                ecl << pair { 0x10bb0 + 0x10, 55 - (st7BossSpell9TRBluePhaseTime - 90) / 60 }; // Also adjust p2->p3 transition condition
                ECLJump(ecl, st7BossSpell9TRStart, st7BossSpell9TRGreenPhase, 0);
                break;
            }
            case 2: {
                constexpr unsigned int st7BossSpell9TRRedPhase = 0x10c7c;

                ecl << pair { st7BossSpell9Duration, 5400 - (55 * 60) - (90 + 18) }; // Adjust duration (hardcoded time threshold for p3 start, + wait)
                ECLJump(ecl, st7BossSpell9TRStart, st7BossSpell9TRRedPhase, 0);
                break;
            }
            default:
                break;
            }

        }
            break;
        case TH15_ST7_END_S10:
            ECLJump(ecl, st7Start, st7BossCreateCall, 60);
            ECLSkipChapter(2);

            // Hecatia
            ecl.SetFile(3);
            ecl << pair{st7bsMovePosX, 0xc2800000} << pair{st7bsMovePosY, 0x43000000}; // Pos
            ecl << pair{st7bsMoveLimitIns, (int16_t)0}; // Void 504
            ecl << pair(st7bsANMSetSprite1Arg2, -1) << pair(st7bsANMSetSprite2Arg2, -1); // Change 303-1 & 303-2
            ecl.SetPos(0x6d0);
            // Effect
            ecl << 5 << 0x0014012e << 0x01ff0000 << 0 << 5
                << 5 << 0x0018012f << 0x02ff0000 << 0 << 3 << 6;
            ecl << 5 << 0x0020000f << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x6f705f34 << 0x00000073; // "Boss", "4_po", "s"
            ecl << 5 << 0x0014012e << 0x01ff0000 << 0 << 3;
            // Health
            ecl << 5 << 0x001401ff << 0x01ff0000 << 0 << 7000;
            // Card
            ecl << 6 << 0x0020000b << 0x01ff0000 << 0 << 0xc
                << 0x73736f42 << 0x64726143 << 0x3031;

            // Junko
            ecl.SetFile(4);
            ecl << pair{0x184, 0x42800000} << pair{0x188, 0x43000000}; // Pos
            ecl << pair{0x328, (int16_t)0}; // Void 504
            ecl << pair(0x36c, -1) << pair(0x384, -1); // Change 303-1 & 303-2
            ecl << pair{0x2b4, (int16_t)0}; // Void 401
            ecl.SetPos(0x3b4);
            // Health
            ecl << 5 << 0x001401ff << 0x01ff0000 << 0 << 7000;
            // Inv.
            ecl << 5 << 0x00140203 << 0x01ff0000 << 0 << 14;
            // Stall
            ecl << 9999 << 0x00100000 << 0x00ff0000 << 0;

            switch (thPracParam.phase) {
            case 1:
                ecl.SetFile(3);
                ECLJump(ecl, 0x8f30, 0x8fc8, 94);
                ecl.SetFile(4);
                ECLJump(ecl, 0xb08, 0xba0, 0);
                break;
            case 2:
                ecl.SetFile(3);
                ECLJump(ecl, 0x8f30, 0x8fc8, 94);
                break;
            case 3:
                ecl.SetFile(4);
                ECLJump(ecl, 0xb08, 0xba0, 0);
                break;
            }
            break;
        default:
            break;
        }
    }
