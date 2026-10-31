// Exact TH12 THStageWarp/THPatch source extraction (MIT), normalized SHA256 95be43b0efd25d73931e1e71de624a5097f7c576f2f7ccd64ed94cdf59a8ee6b.
// Only injection annotations, namespaces and a multi-character literal are adapted.
    void ECLJump(ECLHelper& ecl, unsigned int start, unsigned int ecl_time, int length)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << length << 0;
    }
    void ECLJumpEx(ECLHelper& ecl, unsigned int start, unsigned int dest, int at_frame, int ecl_time = 0)
    {
        ecl.SetPos(start);
        ecl.CheckInstruction(dest);
        ecl << ecl_time << 0x0018000C << 0x02ff0000 << 0x00000000 << dest - start << at_frame;
    }
    void ECLSetHealth(ECLHelper& ecl, unsigned int start, unsigned int ecl_time, int health)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x0014019b << 0x01ff0000 << 0x00000000 << health;
    }
    void ECLCallSub(ECLHelper& ecl, unsigned int start, unsigned int ecl_time, unsigned int sub_length, char* sub)
    {
        ecl.SetPos(start);
        ecl << ecl_time << 0x000b << sub_length + 0x14 << 0x01ff0000 << 0x00000000 << sub_length;
        for (unsigned int i = 0; i < sub_length; i++)
            ecl << (int8_t)sub[i];
    }
    void THStageWarp(ECLHelper& ecl, int stage, int portion)
    {
        if (stage == 1) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ecl << pair{0x12f6c, (int16_t)0} << pair{0x12fb8, (int16_t)0};
                ECLJumpEx(ecl, 0x133d4, 0x1346c, 60, 90);
                break;
            case 3:
                ecl << pair{0x12f6c, (int16_t)0} << pair{0x12fb8, (int16_t)0};
                ECLJumpEx(ecl, 0x133d4, 0x13548, 60, 90);
                break;
            case 4:
                ecl << pair{0x12f6c, (int16_t)0} << pair{0x12fb8, (int16_t)0};
                ECLJumpEx(ecl, 0x133d4, 0x1369c, 60, 90);
                break;
            case 5:
                ecl << pair{0x12f6c, (int16_t)0} << pair{0x12fb8, (int16_t)0};
                ECLJumpEx(ecl, 0x133d4, 0x136ec, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 2) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x13108, 0x131b4, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x13108, 0x1323c, 60, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x13108, 0x132e8, 60, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x13108, 0x13360, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 3) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x16864, 0x168dc, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x16864, 0x16910, 60, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x16864, 0x16944, 60, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x16864, 0x169ac, 60);
                ECLJumpEx(ecl, 0x10548, 0x1226c, 0);
                ECLJumpEx(ecl, 0x1227c, 0x1241c, 0);
                ecl << pair{0x12470, (int16_t)0} << pair{0x124dc, (int16_t)0};
                ecl.SetPos(0x12654);
                ecl << 0 << 0x001001a3 << 0x00ff0000 << 0
                    << 0 << 0x0014019c << 0x01ff0000 << 0 << -1
                    << 0 << 0x00100001 << 0x00ff0000 << 0;
                break;
            case 6:
                ECLJumpEx(ecl, 0x16864, 0x16a30, 60, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x16864, 0x16a64, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 4) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x13800, 0x13868, 0, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x13800, 0x1389c, 0, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x13800, 0x138d0, 0, 0);
                break;
            case 5:
                ECLJumpEx(ecl, 0x13800, 0x13948, 0, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x13800, 0x1397c, 0, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x13800, 0x139b0, 0, 90);
                break;
            case 8:
                ECLJumpEx(ecl, 0x13800, 0x139e4, 0, 90);
                break;
            default:
                break;
            }
        } else if (stage == 5) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x158c0, 0x15938, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x158c0, 0x1596c, 60, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0x158c0, 0x159a0, 60, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0x158c0, 0x15a3c, 181, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0x158c0, 0x15a80, 181, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0x158c0, 0x15ab4, 181, 90);
                break;
            default:
                break;
            }
        } else if (stage == 6) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0x1a3c0, 0x1a438, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0x1a3c0, 0x1a46c, 60, 90);
                break;
            default:
                break;
            }
        } else if (stage == 7) {
            switch (portion) {
            case 1:
                break;
            case 2:
                ECLJumpEx(ecl, 0xdb14, 0xdb8c, 60, 90);
                break;
            case 3:
                ECLJumpEx(ecl, 0xdb14, 0xdbc0, 60, 90);
                break;
            case 4:
                ECLJumpEx(ecl, 0xdb14, 0xdbf4, 60, 90);
                break;
            case 5:
                ECLJumpEx(ecl, 0xdb14, 0xdc8c, 60, 90);
                break;
            case 6:
                ECLJumpEx(ecl, 0xdb14, 0xdcc0, 60, 90);
                break;
            case 7:
                ECLJumpEx(ecl, 0xdb14, 0xdcf4, 60, 90);
                break;
            case 8:
                ECLJumpEx(ecl, 0xdb14, 0xdd28, 60, 90);
                break;
            default:
                break;
            }
        }
    }
    void THPatch(ECLHelper& ecl, int section)
    {
        auto nue = [&]() {
            ECLJumpEx(ecl, 0xdb14, 0xdd80, 71);
            ecl.SetFile(2);
            ecl << pair{0x774, 60} << pair{0x7e0, 60}; // Invincible Frame & Wait Time
            ecl << pair{0x790, (int16_t)0x0}; // Skip Teleportation
            ECLJumpEx(ecl, 0x7fc, 0x8b8, 2); // Skip Animation
            ecl.SetPos(0x6f0);
            ecl << 0 << 0x00300106 << 0x02ff0000 << 0 << 0 << 0; // Change Sprite
            ecl.SetPos(0x7a4);
            ecl << 0 << 0x002c012d << 0x04ff0000 << 0 << 60 << 4 << 0 << 0x42c00000; // Move 301
        };

        switch (section) {
        case TH12_ST1_MID1:
            ECLJumpEx(ecl, 0x133b0, 0x13658, 59);
            break;
        case TH12_ST1_MID2:
            ECLJumpEx(ecl, 0x133b0, 0x13658, 59);
            ecl << pair{0x0c820, 0x0fa0};
            ecl << pair{0x0c8bc, 0x14b4};
            ECLJumpEx(ecl, 0xcde8, 0xce40, 0);
            ecl << pair{0x0ccfc, (int16_t)0x0};
            ecl << pair{0x0ce74, 60};
            break;
        case TH12_ST1_MID3:
            ECLJumpEx(ecl, 0x133b0, 0x13658, 59);
            ecl << pair{0x0c820, 0x0fa0};
            ecl << pair{0x0c8bc, 0x14b4};
            ECLJumpEx(ecl, 0xcde8, 0xce40, 0);
            ecl << pair{0x0ccfc, (int16_t)0x0};
            ecl << pair{0x0ce74, 60};

            ECLSetHealth(ecl, 0x0ce74, 60, 0x514);
            ecl << pair{0x0ce7a, (int16_t)0x20};
            ecl << pair{0xce94, 9999};
            break;
        case TH12_ST1_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x133b0, 0x13788, 59);
            else
                ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            break;
        case TH12_ST1_BOSS2:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair{0x0ae0, 0x5dc};
            ecl << pair{0xcec, 60};
            break;
        case TH12_ST1_BOSS3:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair{0x0be4, (int8_t)0x32};
            ECLJumpEx(ecl, 0x190c, 0x1978, 0);
            ecl << pair{0x17e4, (int16_t)0x0} << pair{0x197c, (int16_t)0x0};
            ecl.SetPos(0xb4c);
            ecl << 0.0f << 128.0f << 280.0f << 256.0f;
            break;
        case TH12_ST1_BOSS4:
            ECLJumpEx(ecl, 0x133b0, 0x137ac, 59);
            ecl << pair{0x0be4, (int8_t)0x32};
            ECLJumpEx(ecl, 0x190c, 0x1978, 0);
            ecl << pair{0x17e4, (int16_t)0x0};
            ecl << pair{0x162c, 0x4b0};
            ecl << pair{0x1640, 0x3e8};
            ecl.SetPos(0xb4c);
            ecl << 0.0f << 128.0f << 280.0f << 256.0f;
            break;
        case TH12_ST2_MID1:
            ECLJumpEx(ecl, 0x13108, 0x132a4, 59);
            break;
        case TH12_ST2_MID2:
            ECLJumpEx(ecl, 0x13108, 0x132a4, 59);
            ecl.SetPos(0x09ea4);
            ecl << 0 << 0x0014019f << 0x01ff0000 << 0 << 60;
            ECLSetHealth(ecl, 0x09ed8, 60, 1350);
            ecl << pair{0x9eb8, 60};
            break;
        case TH12_ST2_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x13108, 0x134d8, 59);
            else
                ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            break;
        case TH12_ST2_BOSS2:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair{0x0d18, 0x44c};
            ecl << pair{0xeac, 60};
            break;
        case TH12_ST2_BOSS3:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair{0x0df4, (int8_t)0x32};
            ECLJumpEx(ecl, 0x1268, 0x12d4, 0);
            ecl << pair{0x1188, (int16_t)0x0} << pair{0x12d8, (int16_t)0x0};
            break;
        case TH12_ST2_BOSS4:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair{0x0df4, (int8_t)0x32};
            ECLJumpEx(ecl, 0x1268, 0x12d4, 0);
            ecl << pair{0x1188, (int16_t)0x0};
            ecl << pair{0x0fe4, 0x6a4};
            break;
        case TH12_ST2_BOSS5:
            ECLJumpEx(ecl, 0x13108, 0x134fc, 59);
            ecl << pair{0xdec, 0xc} << pair(0xdf4, 0x64726143) << pair{0xdf8, 0x00000033};
            ECLJumpEx(ecl, 0x32e8, 0x3340, 0);
            ECLJumpEx(ecl, 0x34dc, 0x3518, 0);
            ecl << pair{0x35b4, (int16_t)0x0};
            break;
        case TH12_ST3_MID1:
            ECLJumpEx(ecl, 0x16864, 0x169ac, 59);
            break;
        case TH12_ST3_MID2:
            ECLJumpEx(ecl, 0x16864, 0x169ac, 59);
            ecl << pair{0x1047c, 0x6a0};
            ECLJumpEx(ecl, 0x10930, 0x10988, 0);
            ECLJumpEx(ecl, 0x1099c, 0x109c0, 0);
            ecl << pair{0x10998, 60};
            ecl << pair{0x108a0, 60};
            ecl << pair{0x108b4, (int16_t)0x0};
            ecl << pair{0x1086c, (int16_t)0x0} << pair{0x10a5c, (int16_t)0x0};
            break;
        case TH12_ST3_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x16864, 0x16a98, 59);
            else
                ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            break;
        case TH12_ST3_BOSS2:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0bec, 0x6a4};
            ecl << pair{0xdac, 60};
            break;
        case TH12_ST3_BOSS3:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0cc8, (int8_t)0x32};
            ECLJumpEx(ecl, 0x27f0, 0x285c, 0);
            ecl << pair{0x2710, (int16_t)0x0} << pair{0x28a0, (int16_t)0x0};
            break;
        case TH12_ST3_BOSS4:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0cc8, (int8_t)0x32};
            ECLJumpEx(ecl, 0x27f0, 0x285c, 0);
            ecl << pair{0x2710, (int16_t)0x0};
            ecl << pair{0x256c, 0x7d0};
            break;
        case TH12_ST3_BOSS5:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0cc8, (int8_t)0x33};
            ECLJumpEx(ecl, 0x40b0, 0x411c, 0);
            ecl << pair{0x3fd0, (int16_t)0x0} << pair{0x4140, (int16_t)0x0};
            break;
        case TH12_ST3_BOSS6:
            ECLJumpEx(ecl, 0x16864, 0x16abc, 59);
            ecl << pair{0x0cc8, (int8_t)0x33};
            ECLJumpEx(ecl, 0x40b0, 0x411c, 0);
            ecl << pair{0x3fd0, (int16_t)0x0};
            ecl << pair{0x3e2c, 0x9c4};
            break;
        case TH12_ST4_MID1:
            ECLJumpEx(ecl, 0x13800, 0x13904, 0);
            break;
        case TH12_ST4_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x13800, 0x13a4c, 119);
            else
                ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            break;
        case TH12_ST4_BOSS2:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x34d4, 0x708};
            ecl << pair{0x3694, 60};
            break;
        case TH12_ST4_BOSS3:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35b0, (int8_t)0x32};
            ECLJumpEx(ecl, 0x4020, 0x408c, 0);
            ecl << pair{0x3f40, (int16_t)0x0} << pair{0x40d0, (int16_t)0x0};
            break;
        case TH12_ST4_BOSS4:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35b0, (int8_t)0x32};
            ECLJumpEx(ecl, 0x4020, 0x408c, 0);
            ecl << pair{0x3f40, (int16_t)0x0};
            ecl << pair{0x3d9c, 0x834};
            break;
        case TH12_ST4_BOSS5:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35b0, (int8_t)0x33};
            ECLJumpEx(ecl, 0x5178, 0x51e4, 0);
            ecl << pair{0x507c, (int16_t)0x0} << pair{0x5228, (int16_t)0x0};
            break;
        case TH12_ST4_BOSS6:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35b0, (int8_t)0x33};
            ECLJumpEx(ecl, 0x5178, 0x51e4, 0);
            ecl << pair{0x507c, (int16_t)0x0};
            ecl << pair{0x4ec4, 0x835};
            ecl << pair{0x4ed8, 0x835};
            break;
        case TH12_ST4_BOSS7:
            ECLJumpEx(ecl, 0x13800, 0x13a70, 119);
            ecl << pair{0x35a8, 0xc} << pair(0x35b0, 0x64726143) << pair{0x35b4, 0x00000034};
            ECLJumpEx(ecl, 0x86c4, 0x8730, 0);
            ECLJumpEx(ecl, 0x8798, 0x87d8, 0);
            ecl << pair{0x8760, 20};
            ecl << pair{0x8614, (int16_t)0x0} << pair{0x8774, (int16_t)0x0};
            break;
        case TH12_ST5_MID1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x158c0, 0x159d4, 179);
            else {
                ECLJumpEx(ecl, 0x158c0, 0x159e8, 179);
                ecl << pair{0xd6ac, (int16_t)0x0} << pair{0xd8b8, (int16_t)0x0};
                ecl << pair{0xd6f0, 60};
            }
            break;
        case TH12_ST5_MID2:
            ECLJumpEx(ecl, 0x158c0, 0x159e8, 179);
            ecl << pair{0xd6ac, (int16_t)0x0};
            ecl << pair{0xd6f0, 60};
            ecl << pair{0x0d7d4, 0x7d0};
            break;
        case TH12_ST5_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x158c0, 0x15ae8, 190);
            else
                ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            break;
        case TH12_ST5_BOSS2:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0b00, 0x7d0};
            ecl << pair{0xcec, 60};
            break;
        case TH12_ST5_BOSS3:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x32};
            ECLJumpEx(ecl, 0x1fbc, 0x2028, 0);
            ecl << pair{0x1edc, (int16_t)0x0} << pair{0x206c, (int16_t)0x0};
            ecl << pair{0xb20, 0x43000000};

            ecl << pair(0x1f04, 2640 - 120);
            break;
        case TH12_ST5_BOSS4:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x32};
            ECLJumpEx(ecl, 0x1fbc, 0x2028, 0);
            ecl << pair{0x1edc, (int16_t)0x0};
            ecl << pair{0x1d38, 0x7d0};
            break;
        case TH12_ST5_BOSS5:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x33};
            ECLJumpEx(ecl, 0x35c0, 0x362c, 0);
            ecl << pair{0x3498, (int16_t)0x0} << pair{0x3670, (int16_t)0x0};
            ecl << pair{0xb20, 0x43000000};

            ecl << pair(0x34c0, 2400 - 90) << pair(0x34ec, 2400 - 90);
            break;
        case TH12_ST5_BOSS6:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x33};
            ECLJumpEx(ecl, 0x35c0, 0x362c, 0);
            ecl << pair{0x3498, (int16_t)0x0};
            ecl << pair{0x32e0, 0x76c};
            ecl << pair{0x32f4, 0x514};
            break;
        case TH12_ST5_BOSS7:
            ECLJumpEx(ecl, 0x158c0, 0x15b0c, 190);
            ecl << pair{0x0bf0, (int8_t)0x34};
            ECLJumpEx(ecl, 0x412c, 0x4198, 0);
            ecl << pair{0x4078, (int16_t)0x0} << pair{0x41bc, (int16_t)0x0};
            break;
        case TH12_ST6_MID1:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a4a0, 60);
            break;
        case TH12_ST6_BOSS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0x1a3c0, 0x1a4e4, 59);
            else
                ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            break;
        case TH12_ST6_BOSS2:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x968, 0x898};
            ecl << pair{0xb10, 60};
            break;
        case TH12_ST6_BOSS3:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x32};
            ECLJumpEx(ecl, 0x29f0, 0x2a5c, 0);
            ecl << pair{0x2910, (int16_t)0x0};
            ecl << pair{0x298c, 60};
            ECLJumpEx(ecl, 0x2a9c, 0x2af0, 0);
            break;
        case TH12_ST6_BOSS4:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x32};
            ECLJumpEx(ecl, 0x29f0, 0x2a5c, 0);
            ecl << pair{0x2910, (int16_t)0x0};
            ecl << pair{0x298c, 60};
            ECLJumpEx(ecl, 0x2a9c, 0x2b04, 0); // Changed

            ECLSetHealth(ecl, 0x2d04, 0x0, 0x960);
            ecl << pair{0x2d18, 9999};
            break;
        case TH12_ST6_BOSS5:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x33};
            ECLJumpEx(ecl, 0x42e0, 0x4798, 0);
            ecl << pair{0x41e8, (int16_t)0x0} << pair{0x420c, (int16_t)0x0};
            ecl << pair{0x428c, 0};
            break;
        case TH12_ST6_BOSS6:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x33};
            ECLJumpEx(ecl, 0x42e0, 0x47ac, 0); // Changed
            ecl << pair{0x41e8, (int16_t)0x0} << pair{0x420c, (int16_t)0x0};
            ecl << pair{0x428c, 0};

            ecl << pair{0x3f54, 0x9c4};
            break;
        case TH12_ST6_BOSS7:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x34};
            ECLJumpEx(ecl, 0x6378, 0x6830, 0);
            ecl << pair{0x6280, (int16_t)0x0} << pair{0x62a4, (int16_t)0x0};
            ecl << pair{0x6324, 0};
            break;
        case TH12_ST6_BOSS8:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x34};
            ECLJumpEx(ecl, 0x6378, 0x6844, 0); // Changed
            ecl << pair{0x6280, (int16_t)0x0} << pair{0x62a4, (int16_t)0x0};
            ecl << pair{0x6324, 0};

            ecl << pair{0x5fd8, 0x1388};

            if (thPracParam.phase == 1) {
                ECLJumpEx(ecl, 0xd264, 0xd294, 120);
                ecl << pair{0xd508, (int16_t)0x0};

                ecl << pair{0x0ec0c, 0x0};
                ecl << pair{0x0f468, 0x0};
                ecl << pair{0x0f650, 0x3d};
                ecl << pair{0x0fa90, 0x0};
                ecl << pair{0x0ffdc, 0x0};
                ecl << pair{0x10188, 0x0};
            }
            break;
        case TH12_ST6_BOSS9:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x35};
            ECLJumpEx(ecl, 0x9030, 0x9508, 0);
            ecl << pair{0x8f44, (int16_t)0x0};
            ecl << pair{0x8fdc, 10};
            break;
        case TH12_ST6_BOSS10:
            ECLJumpEx(ecl, 0x1a3c0, 0x1a508, 59);
            ecl << pair{0x0a44, (int8_t)0x36};
            ECLJumpEx(ecl, 0x98f8, 0x99ac, 0);
            ecl << pair{0x9834, (int16_t)0x0};
            ecl << pair{0x9894, 10};

            if (thPracParam.phase == 1) {
                ecl << pair{0x12a28, 0x118c};
            }
            break;
        case TH12_ST7_MID1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0xdb14, 0xdc28, 59);
            else
                ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            break;
        case TH12_ST7_MID2:
            ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            ecl << pair{0x4099, (int8_t)0x32};
            ECLJumpEx(ecl, 0x52a0, 0x530c, 0);
            ECLJumpEx(ecl, 0x5374, 0x53b4, 0);
            ecl << pair{0x5194, (int16_t)0x0} << pair{0x5350, (int16_t)0x0};
            break;
        case TH12_ST7_MID3:
            ECLJumpEx(ecl, 0xdb14, 0xdc3c, 59);
            ecl << pair{0x4099, (int8_t)0x33};
            ECLJumpEx(ecl, 0x5fd4, 0x6040, 0);
            ECLJumpEx(ecl, 0x60a8, 0x60e8, 0);
            ecl << pair{0x5ec8, (int16_t)0x0} << pair{0x6084, (int16_t)0x0};
            break;
        case TH12_ST7_END_NS1:
            if (thPracParam.dlg)
                ECLJumpEx(ecl, 0xdb14, 0xdd5c, 71);
            else
                nue();
            break;
        case TH12_ST7_END_S1:
            nue();
            ecl << pair{0x788, 0x8fc};
            ecl << pair{0xab0, 9999};
            break;
        case TH12_ST7_END_NS2:
            nue();
            ecl << pair{0x98c, (int8_t)0x32};
            ECLJumpEx(ecl, 0x185c, 0x18dc, 0);
            ecl << pair{0x1768, (int16_t)0x0} << pair{0x1920, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case TH12_ST7_END_S2:
            nue();
            ecl << pair{0x98c, (int8_t)0x32};
            ECLJumpEx(ecl, 0x185c, 0x18dc, 0);
            ecl << pair{0x1768, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x15c4, 0x514};
            break;
        case TH12_ST7_END_NS3:
            nue();
            ecl << pair{0x98c, (int8_t)0x33};
            ECLJumpEx(ecl, 0x2240, 0x22c0, 0);
            ecl << pair{0x2160, (int16_t)0x0} << pair{0x2304, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case TH12_ST7_END_S3:
            nue();
            ecl << pair{0x98c, (int8_t)0x33};
            ECLJumpEx(ecl, 0x2240, 0x22c0, 0);
            ecl << pair{0x2160, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x1fbc, 0x0a8c};
            break;
        case TH12_ST7_END_NS4:
            nue();
            ecl << pair{0x98c, (int8_t)0x34};
            ECLJumpEx(ecl, 0x2dcc, 0x2e4c, 0);
            ecl << pair{0x2cd8, (int16_t)0x0} << pair{0x2e90, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case TH12_ST7_END_S4:
            nue();
            ecl << pair{0x98c, (int8_t)0x34};
            ECLJumpEx(ecl, 0x2dcc, 0x2e4c, 0);
            ecl << pair{0x2cd8, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x2b34, 0x6A4};
            break;
        case TH12_ST7_END_NS5:
            nue();
            ecl << pair{0x98c, (int8_t)0x35};
            ECLJumpEx(ecl, 0x3a2c, 0x3aac, 0);
            ecl << pair{0x3938, (int16_t)0x0} << pair{0x3af0, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case TH12_ST7_END_S5:
            nue();
            ecl << pair{0x98c, (int8_t)0x35};
            ECLJumpEx(ecl, 0x3a2c, 0x3aac, 0);
            ecl << pair{0x3938, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x3794, 0x0e10};
            break;
        case TH12_ST7_END_NS6:
            nue();
            ecl << pair{0x98c, (int8_t)0x36};
            ECLJumpEx(ecl, 0x44cc, 0x454c, 0);
            ecl << pair{0x43d8, (int16_t)0x0} << pair{0x4590, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case TH12_ST7_END_S6:
            nue();
            ecl << pair{0x98c, (int8_t)0x36};
            ECLJumpEx(ecl, 0x44cc, 0x454c, 0);
            ecl << pair{0x43d8, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x4234, 0x0a8c};
            break;
        case TH12_ST7_END_NS7:
            nue();
            ecl << pair{0x98c, (int8_t)0x37};
            ECLJumpEx(ecl, 0x51e8, 0x5268, 0);
            ecl << pair{0x50f4, (int16_t)0x0} << pair{0x52ac, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case TH12_ST7_END_S7:
            nue();
            ecl << pair{0x98c, (int8_t)0x37};
            ECLJumpEx(ecl, 0x51e8, 0x5268, 0);
            ecl << pair{0x50f4, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x4f50, 0x898};
            break;
        case TH12_ST7_END_NS8:
            nue();
            ecl << pair{0x98c, (int8_t)0x38};
            ECLJumpEx(ecl, 0x5f04, 0x5f84, 0);
            ecl << pair{0x5e10, (int16_t)0x0} << pair{0x5fa8, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case TH12_ST7_END_S8:
            nue();
            ecl << pair{0x98c, (int8_t)0x38};
            ECLJumpEx(ecl, 0x5f04, 0x5f84, 0);
            ecl << pair{0x5e10, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            ecl << pair{0x5c6c, 0x898};
            break;
        case TH12_ST7_END_S9:
            nue();
            ecl << pair{0x98c, (int8_t)0x39};
            ECLJumpEx(ecl, 0x71dc, 0x725c, 0);
            ecl << pair{0x7114, (int16_t)0x0} << pair{0x72a0, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};
            break;
        case TH12_ST7_END_S10:
            nue();
            ecl << pair{0x98c, (int8_t)0x31} << pair{0x98d, (int8_t)0x30};
            ECLJumpEx(ecl, 0xfc0, 0x106c, 0);
            ecl << pair{0xe6c, (int16_t)0x0} << pair{0xed8, (int16_t)0x0} << pair{0xeec, (int16_t)0x0};
            ecl << pair{0x7bc, 0} << pair{0x7c0, 0x43000000};

            switch (thPracParam.phase) {
                case 1:
                    ecl << pair{0x7a74, 180};
                    ECLSetHealth(ecl, 0x7b00, 0, 4600);
                    ECLJumpEx(ecl, 0x7b14, 0x7e8c, 0);
                    break;
                case 2:
                    ecl << pair{0x7a74, 180};
                    ECLSetHealth(ecl, 0x7b00, 0, 3000);
                    ECLJumpEx(ecl, 0x7b14, 0x823c, 0);
                    break;
                case 3:
                    ecl << pair{0x7a74, 180};
                    ECLSetHealth(ecl, 0x7b00, 0, 1150);
                    ECLJumpEx(ecl, 0x7b14, 0x8610, 0);
                    break;
                case 4:
                    ecl << pair(0x7b48, 0x0e1c); // EXTRA
                    ecl << pair(0x8958, (int16_t)0x15); // EXTRA
                    ecl << pair(0x895e, (int16_t)0x0ff); // EXTRA
            }
            break;
        default:
            break;
        }
    }
