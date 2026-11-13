// Adapted from thprac_purple (MIT, copyright Ack). See PracticeLicenseTh14.txt.
// thprac_th14.cpp SHA-256 (LF): eacc80f94d1716dd2b2c26080ce54ec5871463de4318b1b2984e8c90585760a7
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.Jump(start, dest);
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    static constexpr unsigned int st7PostMaple = 0x7778;
    void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x689c, 0x6ae8, 60, 90); // 0x6b4c
                ECLJump(ecl, 0x40e4, 0x4118, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x689c, 0x6ae8, 60, 90); // 0x6b4c
                ECLJump(ecl, 0x40e4, 0x414c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x689c, 0x6b4c, 60, 90);
                break;
            case 5:
                ECLJump(ecl, 0x689c, 0x6b4c, 60, 90);
                ECLJump(ecl, 0x41e8, 0x4218, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x4f30, 0x51c4, 60, 90); // 0x51e4, 0x5218
                ECLJump(ecl, 0x3004, 0x3038, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x4f30, 0x51c4, 60, 90); // 0x51e4, 0x5218
                ECLJump(ecl, 0x3004, 0x306c, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x4f30, 0x51e4, 60, 90); // 0x5218
                ECLJump(ecl, 0x31c8, 0x3220, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x4f30, 0x5218, 60, 30);
                break;
            case 6:
                ECLJump(ecl, 0x4f30, 0x5218, 60, 90);
                ECLJump(ecl, 0x3118, 0x3160, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x648c, 0x6720, 60, 90); // 0x6740, 0x6774
                ECLJump(ecl, 0x21f4, 0x2228, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x648c, 0x6720, 60, 30); // 0x6740, 0x6774
                ECLJump(ecl, 0x21f4, 0x2290, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x648c, 0x6740, 60, 90); // 0x6774
                ECLJump(ecl, 0x23c4, 0x241c, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x648c, 0x6774, 60, 30);
                break;
            case 6:
                ECLJump(ecl, 0x648c, 0x6774, 60, 90);
                ECLJump(ecl, 0x22f8, 0x233c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x97f8, 0x9ab4, 60, 90); // 0x9ae4, 0x9b18
                ECLJump(ecl, 0x5090, 0x50c4, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x97f8, 0x9ab4, 60, 90); // 0x9ae4, 0x9b18
                ECLJump(ecl, 0x5090, 0x50f8, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x97f8, 0x9ab4, 60, 90); // 0x9ae4, 0x9b18
                ECLJump(ecl, 0x5090, 0x512c, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x97f8, 0x9ae4, 60, 90); // 0x9b18
                ECLJump(ecl, 0x5240, 0x53c0, 0, 0);
                break;
            case 6:
                ECLJump(ecl, 0x97f8, 0x9b18, 60, 30);
                break;
            case 7:
                ECLJump(ecl, 0x97f8, 0x9b18, 60, 90);
                ECLJump(ecl, 0x5194, 0x51d8, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x8654, 0x8910, 60, 90); // 0x8940, 0x8988
                ECLJump(ecl, 0x2784, 0x27b8, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x8654, 0x8910, 60, 90); // 0x8940, 0x8988
                ECLJump(ecl, 0x2784, 0x27ec, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x8654, 0x8910, 60, 90); // 0x8940, 0x8988
                ECLJump(ecl, 0x2784, 0x2820, 0, 0);
                break;
            case 5:
                ECLJump(ecl, 0x8654, 0x8988, 60, 90);
                break;
            case 6:
                ECLJump(ecl, 0x8654, 0x8988, 60, 90);
                ECLJump(ecl, 0x2888, 0x28dc, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, 0x79d0, 0x7bb8, 60, 90);
                ECLJump(ecl, 0x2bc0, 0x2bf4, 0, 0);
                break;
            case 3:
                ECLJump(ecl, 0x79d0, 0x7bb8, 60, 90);
                ECLJump(ecl, 0x2bc0, 0x2c28, 0, 0);
                break;
            case 4:
                ECLJump(ecl, 0x79d0, 0x7bb8, 60, 90);
                ECLJump(ecl, 0x2bc0, 0x2c5c, 0, 0);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJump(ecl, st7PostMaple, 0x7a34, 60, 90); // 0x7aac, 0x7acc
                ECLJump(ecl, 0x36ac, 0x36e0, 0, 0);
                break;
            case 3:
                ECLJump(ecl, st7PostMaple, 0x7a34, 60, 50); // 0x7aac, 0x7acc
                ECLJump(ecl, 0x36ac, 0x3714, 0, 0);
                break;
            case 4:
                ECLJump(ecl, st7PostMaple, 0x7a34, 60, 90); // 0x7aac, 0x7acc
                ECLJump(ecl, 0x36ac, 0x3748, 0, 0);
                break;
            case 5:
                ECLJump(ecl, st7PostMaple, 0x7aac, 60, 90); // 0x7acc
                ecl << pair{0x4cf8, 0};
                break;
            case 6:
                ECLJump(ecl, st7PostMaple, 0x7acc, 60, 90);
                break;
            case 7:
                ECLJump(ecl, st7PostMaple, 0x7acc, 60, 10);
                ECLJump(ecl, 0x37b0, 0x37f4, 0, 0);
                break;
            case 8:
                ECLJump(ecl, st7PostMaple, 0x7acc, 60, 90);
                ECLJump(ecl, 0x37b0, 0x3828, 0, 0);
                break;
            case 9:
                ECLJump(ecl, st7PostMaple, 0x7acc, 60, 90);
                ECLJump(ecl, 0x37b0, 0x385c, 0, 0);
                break;
            default:
                break;
            }
        }
    }
    void THPatch(ECLHelper& ecl, i32 section)
    {
        auto st6_boss = [&](int std_id) {
            ECLJump(ecl, 0x79d0, 0x2aa0, 0); // Jump to 630
            ecl << pair{0x2ab0, std_id}; // 630 Param
            ECLJump(ecl, 0x2ab4, 0x7c68, 60); // Jump to MainBoss
            ecl.SetFile(3);
            ecl << pair{0x4a0, (int16_t)0}; // Disable 630
        };

        constexpr unsigned int st7BossCreateCall = 0x7b38;
        constexpr unsigned int st7bsPrePushSpellID = 0x790;
        constexpr unsigned int st7bsPostNotSpellPracCheck = 0x830;
        constexpr unsigned int st7bsSpellHealthVal = 0x830 + 0x10;
        constexpr unsigned int st7bsSpellSubCallOrd = 0x844 + 0x1c;

        switch (section) {
        case TH14_ST1_MID1:
            ECLJump(ecl, 0x689c, 0x6b08, 60);
            break;
        case TH14_ST1_MID2:
            ECLJump(ecl, 0x689c, 0x6b08, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x478, 0x560, 0);
            ecl << pair{0x38c, (int16_t)0};
            break;
        case TH14_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x689c, 0x6b94, 60);
            else
                ECLJump(ecl, 0x689c, 0x6bbc, 60);
            break;
        case TH14_ST1_BOSS2:
            ECLJump(ecl, 0x689c, 0x6bbc, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x400, 0x4e8, 0); // Utilize Spell Practice Jump
            ecl << pair{0x4f8, 1500}; // Set Health
            ecl << pair{0x518, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x1aa4, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST1_BOSS3:
            ECLJump(ecl, 0x689c, 0x6bbc, 60);
            ecl.SetFile(2);
            ecl << pair{0x638, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x1028, (int16_t)0 } << pair { 0x1158, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x12e4, 0} << pair{0x12d0, 0}; // Change Wait Time & Inv. Time
            break;
        case TH14_ST1_BOSS4:
            ECLJump(ecl, 0x689c, 0x6bbc, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x400, 0x4e8, 0); // Utilize Spell Practice Jump
            ecl << pair{0x4f8, 1900}; // Set Health
            ecl << pair{0x518, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x2cbc, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST2_MID1:
            ECLJump(ecl, 0x4f30, 0x51e4, 60);
            break;
        case TH14_ST2_MID2:
            ECLJump(ecl, 0x4f30, 0x51e4, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3fc, 0x4e4, 0);
            ecl << pair{0xdf8, (int16_t)0};
            break;
        case TH14_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x4f30, 0x524c, 60);
            else
                ECLJump(ecl, 0x4f30, 0x5274, 60);
            break;
        case TH14_ST2_BOSS2:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x4e4, 0x5cc, 0); // Utilize Spell Practice Jump
            ecl << pair{0x5dc, 1400}; // Set Health
            ecl << pair{0x5fc, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x2678, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST2_BOSS3:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            ecl.SetFile(3);
            ecl << pair{0x800, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x13d4, (int16_t)0 } << pair { 0x1518, (int16_t)0 }; // Disable Item Drops & SE
            break;
        case TH14_ST2_BOSS4:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x4e4, 0x5cc, 0); // Utilize Spell Practice Jump
            ecl << pair{0x5dc, 1900}; // Set Health
            ecl << pair{0x5fc, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x3410, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST2_BOSS5:
            ECLJump(ecl, 0x4f30, 0x5274, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x4e4, 0x5cc, 0); // Utilize Spell Practice Jump
            ecl << pair{0x5dc, 2100}; // Set Health
            ecl << pair{0x5fc, (int8_t)0x33}; // Set Spell Ordinal
            break;
        case TH14_ST3_MID1:
            ECLJump(ecl, 0x648c, 0x6740, 60);
            break;
        case TH14_ST3_MID2_EN:
            ECLJump(ecl, 0x648c, 0x6740, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x520, 0x614, 0);
            ECLJump(ecl, 0x664, 0xc88, 0);
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0xcc8, (int16_t)0 } << pair { 0xe24, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0xf68, 0} << pair{0xf54, 0}; // Change Wait Time & Inv. Time
            break;
        case TH14_ST3_MID2_HL:
            ECLJump(ecl, 0x648c, 0x6740, 60);
            ecl.SetFile(2);
            switch (thPracParam.phase) {
            default:
            case 0:
                break;
            case 1:
                ecl << pair { 0x1718,(int16_t)542};
                break;
            }
            ECLJump(ecl, 0x404, 0x4ec, 0);
            break;
        case TH14_ST3_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x648c, 0x67a8, 60);
            else
                ECLJump(ecl, 0x648c, 0x67d0, 60);
            break;
        case TH14_ST3_BOSS2:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair{0x550, 1600}; // Set Health
            ecl << pair{0x570, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x20f0, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST3_BOSS3:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ecl << pair{0x774, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0xe7c, (int16_t)0 } << pair { 0xfc0, (int16_t)0 } << pair { 0x1134, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x10b0, 59} << pair{0x1154, 20} << pair{0x112c, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST3_BOSS4:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair{0x550, 2100}; // Set Health
            ecl << pair{0x570, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x38c4, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST3_BOSS5:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ecl << pair{0x774, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x1790, (int16_t)0 } << pair { 0x18d4, (int16_t)0 } << pair { 0x1a48, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x19c4, 59} << pair{0x1a68, 20} << pair{0x1a40, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST3_BOSS6:
            ECLJump(ecl, 0x648c, 0x67d0, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair{0x550, 1800}; // Set Health
            ecl << pair{0x570, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x47e4, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST4_MID1:
            ECLJump(ecl, 0x97f8, 0x9ae4, 60);
            break;
        case TH14_ST4_MID2:
            ECLJump(ecl, 0x97f8, 0x9ae4, 60);

            // BossA
            ecl.SetFile(2);
            ecl << pair{0x439, (int8_t)0x32};
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x143c, (int16_t)0 } << pair { 0x1598, (int16_t)0 };
            ecl << pair{0x16c8, 0} << pair{0x16dc, (int16_t)0};

            // BossB
            ecl.SetFile(3);
            ecl << pair{0x542, (int8_t)0x32};
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0xca8, (int16_t)0 } << pair { 0xe04, (int16_t)0 };
            ecl << pair{0xf34, 0} << pair{0xf48, (int16_t)0};
            break;
        case TH14_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x97f8, 0x9b4c, 60);
            else {
                ECLJump(ecl, 0x97f8, 0x9b74, 60);

                // BossA
                ecl.SetFile(4);
                ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
                ecl << pair{0x3e8, 0}; // Cancel Msg Invi.

                // BossB
                ecl.SetFile(5);
                ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
                ecl << pair{0x358, 0}; // Cancel Msg Invi.
            }
            break;
        case TH14_ST4_BOSS2:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            ECLJump(ecl, 0x4e8, 0x5d0, 0); // Utilize Spell Practice Jump
            ecl << pair{0x5e0, 2300}; // Set Health
            ecl << pair{0x600, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x6370, (int16_t)0}; // Disable Item Drops

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair{0x550, 2300}; // Set Health
            ecl << pair{0x571, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x1e98, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST4_BOSS3:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            ecl << pair{0x3e8, 0}; // Cancel Msg Invi.
            ecl << pair{0x804, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x24e4, (int16_t)0 } << pair { 0x2628, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x2718, 59} << pair{0x2794, 0}; // Change Move Time & Wait Time

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            ecl << pair{0x358, 0}; // Cancel Msg Invi.
            ecl << pair{0x775, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0xf0c, (int16_t)0 } << pair { 0x1050, (int16_t)0 } << pair { 0x11e4, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x1140, 59} << pair{0x1204, 0} << pair{0x11dc, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST4_BOSS4:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            ECLJump(ecl, 0x4e8, 0x5d0, 0); // Utilize Spell Practice Jump
            ecl << pair{0x5e0, 2300}; // Set Health
            ecl << pair{0x600, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x6c98, (int16_t)0}; // Disable Item Drops

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair{0x550, 2300}; // Set Health
            ecl << pair{0x571, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x2ddc, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST4_BOSS5:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            ecl << pair{0x3e8, 0}; // Cancel Msg Invi.
            ecl << pair{0x804, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x44dc, (int16_t)0 } << pair { 0x4634, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x4724, 59} << pair{0x478c, 0}; // Change Move Time & Wait Time

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            ecl << pair{0x358, 0}; // Cancel Msg Invi.
            ecl << pair{0x775, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x1678, (int16_t)0 } << pair { 0x17dc, (int16_t)0 } << pair { 0x1950, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x18cc, 59} << pair{0x1948, 0} << pair{0x1970, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST4_BOSS6:
            ECLJump(ecl, 0x97f8, 0x9b74, 60);

            // BossA
            ecl.SetFile(4);
            ECLJump(ecl, 0x9950, 0x9ab4, 0); // Skip Dummy Boss
            ECLJump(ecl, 0x4e8, 0x5d0, 0); // Utilize Spell Practice Jump
            ecl << pair{0x5e0, 2800}; // Set Health
            ecl << pair{0x600, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x7db4, (int16_t)0}; // Disable Item Drops

            // BossB
            ecl.SetFile(5);
            ECLJump(ecl, 0x4dcc, 0x4f30, 0); // Skip Dummy Boss
            ECLJump(ecl, 0x458, 0x540, 0); // Utilize Spell Practice Jump
            ecl << pair{0x550, 2500}; // Set Health
            ecl << pair{0x571, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x3598, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST5_MID1:
            ECLJump(ecl, 0x8654, 0x8940, 60);
            break;
        case TH14_ST5_MID2:
            ECLJump(ecl, 0x8654, 0x8940, 60);
            ecl.SetFile(2);
            ECLJump(ecl, 0x3c4, 0x4ac, 0);
            ecl << pair{0xd38, (int16_t)0};
            break;
        case TH14_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x8654, 0x89bc, 60);
            else
                ECLJump(ecl, 0x8654, 0x89e4, 60);
            break;
        case TH14_ST5_BOSS2:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x43c, 0x524, 0); // Utilize Spell Practice Jump
            ecl << pair{0x534, 2600}; // Set Health
            ecl << pair{0x554, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x334c, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST5_BOSS3:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ecl << pair{0x83c, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x14b8, (int16_t)0 } << pair { 0x1638, (int16_t)0 } << pair { 0x17ac, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x1728, 59} << pair{0x17cc, 20} << pair{0x17a4, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST5_BOSS4:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x43c, 0x524, 0); // Utilize Spell Practice Jump
            ecl << pair{0x534, 2100}; // Set Health
            ecl << pair{0x554, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x4350, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST5_BOSS5:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ecl << pair{0x83c, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x239c, (int16_t)0 } << pair { 0x2530, (int16_t)0 } << pair { 0x26c0, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x2620, 59} << pair{0x26e0, 20} << pair{0x26b8, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST5_BOSS6:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x43c, 0x524, 0); // Utilize Spell Practice Jump
            ecl << pair{0x534, 5700}; // Set Health
            ecl << pair{0x554, (int8_t)0x33}; // Set Spell Ordinal
            ecl << pair{0x59ec, (int16_t)0}; // Disable Item Drops

            ECLJump(ecl, 0x538, 0x265c, 0);
            ECLJump(ecl, 0x2694, 0x59d4, 0);
            break;
        case TH14_ST5_BOSS7:
            ECLJump(ecl, 0x8654, 0x89e4, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x43c, 0x524, 0); // Utilize Spell Practice Jump
            ecl << pair{0x534, 2700}; // Set Health
            ecl << pair{0x554, (int8_t)0x34}; // Set Spell Ordinal

            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x604c, (int16_t)0 } << pair { 0x5f3c, (int16_t)0 } << pair { 0x6168, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x6094, 59} << pair{0x60c8, 0} << pair{0x60b4, 0}; // Change Move Time, Wait Time & Inv. Time
            ecl << pair{0x3b0, (int16_t)0}; // Disable 504
            break;
        case TH14_ST6_MID1:
            ECLJump(ecl, 0x79d0, 0x7bd8, 60);
            break;
        case TH14_ST6_MID2:
            ECLJump(ecl, 0x79d0, 0x7bd8, 60);
            ecl.SetFile(2);
            ecl << pair{0x3d5, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0xda0, (int16_t)0 } << pair { 0xee4, (int16_t)0 } << pair { 0x103c, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0xfd4, 59} << pair{0x105c, 20} << pair{0x1034, 0}; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLJump(ecl, 0x7988, 0x7c40, 60);
            else
                st6_boss(2);
            break;
        case TH14_ST6_BOSS2:
            st6_boss(2);
            ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
            ecl << pair{0x638, 3000}; // Set Health
            ecl << pair{0x658, (int8_t)0x31}; // Set Spell Ordinal
            ecl << pair{0x4ac0, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST6_BOSS3:
            st6_boss(3);
            ecl << pair{0xb3c, (int8_t)0x32}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x166c, (int16_t)0 } << pair { 0x17e0, (int16_t)0 } << pair { 0x1940, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x18d0, 59} << pair{0x1960, 0} << pair{0x1938, 60}; // Change Move Time, Wait Time & Inv. Time
            ecl << pair{0x16c4, (int16_t)0}; // Disable 630
            break;
        case TH14_ST6_BOSS4:
            st6_boss(3);
            ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
            ecl << pair{0x638, 2800}; // Set Health
            ecl << pair{0x658, (int8_t)0x32}; // Set Spell Ordinal
            ecl << pair{0x58bc, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST6_BOSS5:
            st6_boss(4);
            ecl << pair{0xb3c, (int8_t)0x33}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x27cc, (int16_t)0 } << pair { 0x2924, (int16_t)0 } << pair { 0x2a98, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x2a14, 59} << pair{0x2ab8, 0} << pair{0x2a90, 60}; // Change Move Time, Wait Time & Inv. Time
            ecl << pair{0x2808, (int16_t)0}; // Disable 630
            break;
        case TH14_ST6_BOSS6:
            st6_boss(4);
            ECLJump(ecl, 0x540, 0x7f0, 0); // Utilize Spell Practice Jump
            ecl << pair{0x6928, (int16_t)0}; // Disable Item Drops
            break;
        case TH14_ST6_BOSS7:
            st6_boss(4);
            ecl << pair{0xb3c, (int8_t)0x34}; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x37f8, (int16_t)0 } << pair { 0x3970, (int16_t)0 } << pair { 0x3ae4, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair{0x3a60, 59} << pair{0x3b04, 0} << pair{0x3adc, 60}; // Change Move Time, Wait Time & Inv. Time
            ecl << pair{0x3834, (int16_t)0}; // Disable 630
            ecl << pair{0x385c, (int16_t)0}; // Disable BossCupEnd
            break;
        case TH14_ST6_BOSS8:
            st6_boss(4);
            ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
            ecl << pair{0x638, 1}; // Set Health
            ecl << pair{0x658, (int8_t)0x34}; // Set Spell Ordinal
            ecl << pair{0x75e0, (int16_t)0}; // Disable Item Drops
            if (thPracParam.phase)
            {
                ecl << pair { 0x7CBC, (int16_t)0 };
                ecl << pair { 0x7CE4, (int16_t)0 };
                ecl << pair { 0x7CDC, (float)0.0174533f * thPracParam.st6bs8_phase };

                ecl << pair { 0x7E00, (int16_t)0 };
                ecl << pair { 0x7E28, (int16_t)0 };
                ecl << pair { 0x7E20, (float)-0.0174533f * thPracParam.st6bs8_phase };
            }
            break;
        case TH14_ST6_BOSS9:
            st6_boss(4);

            if (thPracParam.keepSpellDrops) {
                ecl << pair { 0xb3c, (int8_t)0x35 }; // Change Nonspell

            } else {
                ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
                ecl << pair { 0x638, 3000 }; // Set Health
                ecl << pair { 0x658, (int8_t)0x35 }; // Set Spell Ordinal
            }
            if (thPracParam.phase == 1) {
                ecl.SetPos(0x8cdc);
                ecl << 30 << 30 << 30 << 30;
            }
            break;
        case TH14_ST6_BOSS10:
            st6_boss(4);
            if (thPracParam.keepSpellDrops) {
                ecl << pair { 0xb3c, (int8_t)0x36 }; // Change Nonspell

            } else {
                ECLJump(ecl, 0x540, 0x628, 0); // Utilize Spell Practice Jump
                ecl << pair { 0x638, 8000 }; // Set Health
                ecl << pair { 0x658, (int8_t)0x36 }; // Set Spell Ordinal
                ECLJump(ecl, 0x63c, 0x4790, 0);
                ECLJump(ecl, 0x47c8, 0x9c18, 0);
            }
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0x638, 6500};
                ecl << pair{0x9fb0, 0};
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                break;
            case 2:
                ecl << pair{0x638, 3300};
                ecl << pair{0x9fb0, 0};
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                ECLJump(ecl, 0xa4a8, 0xa5ec, 120);
                break;
            case 3:
                ecl << pair{0x638, 3300};
                ecl << pair{0x9fb0, 0};
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                ECLJump(ecl, 0xa4a8, 0xa5ec, 120);
                ECLJump(ecl, 0xa7b4, 0xa828, 120);
                break;
            default:
                break;
            }
            break;
        case TH14_ST7_MID1:
            ECLJump(ecl, st7PostMaple, 0x7a64, 60);
            if (!thPracParam.dlg)
                ecl << pair{0x390c, (int16_t)0};
            break;
        case TH14_ST7_MID2:
            ECLJump(ecl, st7PostMaple, 0x7a64, 60);
            ecl << pair{0x390c, (int16_t)0};
            ecl.SetFile(2);

            ECLJump(ecl, 0x528, 0x590, 0); // Disable Effect
            ecl << pair{0x3b4, -32.0f} << pair{0x3b8, 128.0f}; // Change Pos
            ecl << pair{0x801, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x1efc, (int16_t)0} << pair{0x2028, (int16_t)0}; // Disable SE
            ecl << pair{0x2048, 0} << pair{0x2020, 0}; // Change Wait Time & Inv. Time
            ecl << pair{0x1fe0, (int16_t)0}; // Void 401

            ECLJump(ecl, 0x2d80, 0x2de8, 0); // Disable Effect
            ecl << pair{0x2c0c, 32.0f} << pair{0x2c10, 128.0f}; // Change Pos
            ecl << pair{0x30b5, (int8_t)0x32}; // Change Nonspell
            ecl << pair{0x2260, (int16_t)0}; // Disable SE
            ecl << pair{0x2370, 0} << pair{0x2164, 0}; // Change Wait Time & Inv. Time
            ecl << pair{0x2344, (int16_t)0}; // Void 401
            break;
        case TH14_ST7_MID3:
            ECLJump(ecl, st7PostMaple, 0x7a64, 60);
            ecl << pair{0x390c, (int16_t)0};
            ecl.SetFile(2);

             if (thPracParam.keepSpellDrops) {
                ECLJump(ecl, 0x528, 0x590 - 0x3c, 0); // Disable Effect
                ecl.SetPos(0x590 - 0x3c); // write custom BossItemPhase call for drops
                ecl << 0 << 0x003c000b << 0x04ff0000 << 0
                    << 0x00000010 << 0x73736f42 << 0x6d657449 << 0x73616850
                    << 0x00000065 << 0x00006969 << 0x0000000f << 0x00006969
                    << 0x0000000f << 0x00006669 << 0x00000040;
                ecl << pair { 0x3b4, -32.0f } << pair { 0x3b8, 160.0f }; // Change Pos

            } else {
                ECLJump(ecl, 0x528, 0x590, 0); // Disable Effect
                ecl << pair { 0x3b4, -128.0f } << pair { 0x3b8, 160.0f }; // Change Pos
                ecl << pair { 0x26c0, 0 } << pair { 0x2698, 0 }; // Change Wait Time & Inv. Time
                ecl << pair { 0x2658, (int16_t)0 }; // Void 401
            }
            ecl << pair{0x801, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x2574, (int16_t)0} << pair{0x26a0, (int16_t)0}; // Disable SE
            if (thPracParam.keepSpellDrops) {
                ECLJump(ecl, 0x2d80, 0x2de8 - 0x3c, 0); // Disable Effect
                ecl.SetPos(0x2de8 - 0x3c); // write custom BossItemPhase call for drops
                ecl << 0 << 0x003c000b << 0x04ff0000 << 0
                    << 0x00000010 << 0x73736f42 << 0x6d657449 << 0x73616850
                    << 0x00000065 << 0x00006969 << 0x0000000f << 0x00006969
                    << 0x0000000f << 0x00006669 << 0x00000040;
                ecl << pair { 0x2c0c, 32.0f } << pair { 0x2c10, 160.0f }; // Change Pos

            } else {
                ECLJump(ecl, 0x2d80, 0x2de8, 0); // Disable Effect
                ecl << pair { 0x2c0c, 128.0f } << pair { 0x2c10, 160.0f }; // Change Pos
                ecl << pair { 0x29e8, 0 } << pair { 0x27dc, 0 }; // Change Wait Time & Inv. Time
                ecl << pair { 0x29bc, (int16_t)0 }; // Void 401
            }
            ecl << pair{0x30b5, (int8_t)0x33}; // Change Nonspell
            ecl << pair{0x28d8, (int16_t)0}; // Disable SE
            break;
        case TH14_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLJump(ecl, st7PostMaple, 0x7b10, 60);
            else
                ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            break;
        case TH14_ST7_END_S1:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellHealthVal, 2100 }; // Set Health
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x31 }; // Set Spell Ordinal
            ecl << pair { 0x691c, (int16_t)0 }; // Disable Item Drops
            break;
        case TH14_ST7_END_NS2:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ecl << pair { 0xf08, (int8_t)0x32 }; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x190c, (int16_t)0 } << pair { 0x1ab0, (int16_t)0 } << pair { 0x1c48, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair { 0x1ba0, 59 } << pair { 0x1c68, 0 } << pair { 0x1c40, 60 }; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST7_END_S2:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellHealthVal, 3000 }; // Set Health
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x32 }; // Set Spell Ordinal
            ecl << pair { 0x9520, (int16_t)0 }; // Disable Item Drops
            break;
        case TH14_ST7_END_NS3:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ecl << pair { 0xf08, (int8_t)0x33 }; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x22b4, (int16_t)0 } << pair { 0x2460, (int16_t)0 } << pair { 0x25f8, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair { 0x2550, 59 } << pair { 0x2618, 0 } << pair { 0x25f0, 60 }; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST7_END_S3:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellHealthVal, 2300 }; // Set Health
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x33 }; // Set Spell Ordinal
            ecl << pair { 0xa670, (int16_t)0 }; // Disable Item Drops
            break;
        case TH14_ST7_END_NS4:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ecl << pair { 0xf08, (int8_t)0x34 }; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x2c00, (int16_t)0 } << pair { 0x2da4, (int16_t)0 } << pair { 0x2f3c, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair { 0x2e94, 59 } << pair { 0x2f5c, 0 } << pair { 0x2f34, 60 }; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST7_END_S4:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellHealthVal, 3500 }; // Set Health
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x34 }; // Set Spell Ordinal
            ecl << pair { 0xae74, (int16_t)0 }; // Disable Item Drops
            break;
        case TH14_ST7_END_NS5:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ecl << pair { 0xf08, (int8_t)0x35 }; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x34ec, (int16_t)0 } << pair { 0x3690, (int16_t)0 } << pair { 0x3828, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair { 0x3780, 59 } << pair { 0x3848, 0 } << pair { 0x3820, 60 }; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST7_END_S5:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellHealthVal, 3300 }; // Set Health
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x35 }; // Set Spell Ordinal
            ecl << pair { 0xb558, (int16_t)0 }; // Disable Item Drops
            break;
        case TH14_ST7_END_NS6:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ecl << pair { 0xf08, (int8_t)0x36 }; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x4300, (int16_t)0 } << pair { 0x44ac, (int16_t)0 } << pair { 0x4644, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair { 0x459c, 59 } << pair { 0x4664, 0 } << pair { 0x463c, 60 }; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST7_END_S6:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellHealthVal, 3000 }; // Set Health
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x36 }; // Set Spell Ordinal
            ecl << pair { 0xc5f0, (int16_t)0 }; // Disable Item Drops
            break;
        case TH14_ST7_END_NS7:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ecl << pair { 0xf08, (int8_t)0x37 }; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x4c3c, (int16_t)0 } << pair { 0x4de0, (int16_t)0 } << pair { 0x4f78, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair { 0x4ed0, 59 } << pair { 0x4f98, 0 } << pair { 0x4f70, 60 }; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST7_END_S7:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellHealthVal, 3500 }; // Set Health
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x37 }; // Set Spell Ordinal
            ecl << pair { 0xd538, (int16_t)0 }; // Disable Item Drops
            break;
        case TH14_ST7_END_NS8:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ecl << pair { 0xf08, (int8_t)0x38 }; // Change Nonspell
            if (!thPracParam.keepSpellDrops)
                ecl << pair { 0x5938, (int16_t)0 } << pair { 0x5ae4, (int16_t)0 } << pair { 0x5c7c, (int16_t)0 }; // Disable Item Drops & SE
            ecl << pair { 0x5bd4, 59 } << pair { 0x5c9c, 0 } << pair { 0x5c74, 60 }; // Change Move Time, Wait Time & Inv. Time
            break;
        case TH14_ST7_END_S8:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair { st7bsSpellHealthVal, 3500 }; // Set Health
            ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x38 }; // Set Spell Ordinal
            ecl << pair { 0xe638, (int16_t)0 }; // Disable Item Drops
            break;
        case TH14_ST7_END_S9:
        {
            constexpr unsigned int st7BossSpell9Duration = 0xf4f8 + 0x10;
            constexpr unsigned int st7BossSpell9BLSAtk1 = 0xf924;
            constexpr unsigned int st7BossSpell9BLSAtk2 = 0xf984;
            constexpr unsigned int st7BossSpell9BLSAtk3 = 0xf9e4;
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            if (thPracParam.keepSpellDrops) {
                ecl << pair { 0xf08, (int8_t)0x39 }; // Change Nonspell

            } else {
                ECLJump(ecl, st7bsPrePushSpellID, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
                ecl << pair { st7bsSpellHealthVal, 4000 }; // Set Health
                ecl << pair { st7bsSpellSubCallOrd, (int8_t)0x39 }; // Set Spell ID in sub call to 9
            }

            switch (thPracParam.phase) {
            case 1:
                ecl << pair { st7BossSpell9Duration, 75 - 22 }; // Adjust duration (skipping 1200f atk 1 + 120f wait)
                ECLJump(ecl, st7BossSpell9BLSAtk1, st7BossSpell9BLSAtk2, 120);
                break;
            case 2:
                ecl << pair { st7BossSpell9Duration, 75 - 44 }; // Adjust duration (idem + 1200f atk2 + 120f wait)
                ECLJump(ecl, st7BossSpell9BLSAtk1, st7BossSpell9BLSAtk3, 120 * 2);
                break;
            default:
                break;
            }
        }

            break;
        case TH14_ST7_END_S10:
            ECLJump(ecl, st7PostMaple, st7BossCreateCall, 60);
            ecl.SetFile(3);
            ECLJump(ecl, 0x790, st7bsPostNotSpellPracCheck, 0); // Utilize Spell Practice Jump
            ecl << pair{st7bsSpellHealthVal, 8000}; // Set Health
            ecl << pair{st7bsSpellHealthVal, (int8_t)0x31} << pair{0x861, (int8_t)0x30}; // Set Spell Ordinal
            ecl << pair{0x6d64, (int16_t)0}; // Disable Item Drops

            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0x840, 6200};
                ECLJump(ecl, 0x70e4, 0x7344, 60);
                break;
            case 2:
                ecl << pair{0x840, 4400};
                ECLJump(ecl, 0x70e4, 0x75b8, 60);
                break;
            case 3:
                ecl << pair{0x840, 2600};
                ECLJump(ecl, 0x70e4, 0x78a0, 60);
                break;
            case 4:
                ecl << pair{0x840, 800};
                ECLJump(ecl, 0x70e4, 0x78a0, 60);
                ECLJump(ecl, 0x78f0, 0x7ba0, 60);
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
        case 100:
        case 101:
        case 102:
        case 103:
            ecl.SetFile(3);
            if (thPracParam.phase == 1) {
                ecl.SetPos(0x8cdc);
                ecl << 30 << 30 << 30 << 30;
            }
            break;
        case 104:
        case 105:
        case 106:
        case 107:
            ecl.SetFile(3);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xae0, 6500};
                ecl << pair{0x9fb0, 0};
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                break;
            case 2:
                ecl << pair{0xae0, 3300};
                ecl << pair{0x9fb0, 0};
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                ECLJump(ecl, 0xa4a8, 0xa5ec, 120);
                break;
            case 3:
                ecl << pair{0xae0, 3300};
                ecl << pair{0x9fb0, 0};
                ECLJump(ecl, 0xa188, 0xa2cc, 120);
                ECLJump(ecl, 0xa4a8, 0xa5ec, 120);
                ECLJump(ecl, 0xa7b4, 0xa828, 120);
                break;
            default:
                break;
            }
            break;
        case 119:
            ecl.SetFile(3);
            switch (thPracParam.phase) {
            case 1:
                ecl << pair{0xe94, 6200};
                ECLJump(ecl, 0x70e4, 0x7344, 60);
                break;
            case 2:
                ecl << pair{0xe94, 4400};
                ECLJump(ecl, 0x70e4, 0x75b8, 60);
                break;
            case 3:
                ecl << pair{0xe94, 2600};
                ECLJump(ecl, 0x70e4, 0x78a0, 60);
                break;
            case 4:
                ecl << pair{0xe94, 800};
                ECLJump(ecl, 0x70e4, 0x78a0, 60);
                ECLJump(ecl, 0x78f0, 0x7ba0, 60);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
