// Generated from purple THPrac (MIT), source sha256 b5be6dc698f574992ab49696c5c0d8ad756af93bf0760be1504ee9e958bee1b0.
    static void ApplySection(ECLHelper& ecl, PatchContext& context, const Session& session, std::int32_t section)
    {
        auto lunasa = [&]() {
            context.fakeShot = 0;
            ecl << pair{0x4f08, 1} << pair{0x4f30, 1} << pair{0x4f74, 1} << pair{0x4f9c, 1};
        };
        auto merlin = [&]() {
            context.fakeShot = 4;
            ecl << pair{0x4f08, 2} << pair{0x4f30, 2} << pair{0x4f74, 2} << pair{0x4f9c, 2};
        };
        auto lyrica = [&]() {
            context.fakeShot = 2;
            ecl << pair{0x4f08, 3} << pair{0x4f30, 3} << pair{0x4f74, 3} << pair{0x4f9c, 3};
        };
        auto timesave = [&]() {
            ecl << pair{0xc550, 0} << pair{0xc588, 0} << pair{0xc598, 0} << pair{0xf048, 0}
                << pair{0xf080, 0} << pair{0xf090, 0} << pair{0x11670, 0} << pair{0x11684, 0}
                << pair{0x11694, 0};
        };
        auto yuyuko = [&]() {
            ecl << pair{0x425c, (int16_t)0} << pair{0x426c, (int16_t)0}
                << pair{0xe678, (int16_t)0x9d7} << pair{0x3330, 0x22}
                << pair{0x43a8, (int16_t)0} << pair{0x43f0, (int16_t)0}
                << pair{0x43fc, 0} << pair{0x4410, 0} << pair{0x4420, 0} << pair{0x4434, 0} << pair{0x4444, 0};
            SetWarp(context, 2, 0x9d7);
        };

        switch (section) {
        case 1:
            SetWarp(context, 2, 0xa60);
            break;
        case 2:
            SetWarp(context, 2, 0xa60);
            ECLSetTime(ecl, 0x2580, 0x3c, 0x0);
            break;
        case 3:
            if (session.dialogue)
                SetWarp(context, 2, 0x13b2);
            else {
                SetWarp(context, 2, 0x13b3);
                context.nameFix = true;
            }
            break;
        case 4:
            context.nameFix = true;
            SetWarp(context, 2, 0x13b3);
            ECLSetTime(ecl, 0x4c20, 0x3c, 0);
            break;
        case 5:
            context.nameFix = true;
            SetWarp(context, 2, 0x13b3);
            ecl << pair{0x4c2c, 0x27} << pair{0x4d7c, (int16_t)0} << pair{0x4d8c, (int16_t)0};
            break;
        case 6:
            context.nameFix = true;
            SetWarp(context, 2, 0x13b3);
            ecl << pair{0x4c2c, 0x27} << pair{0x4d7c, (int16_t)0} << pair{0x4d8c, (int16_t)0};
            ECLSetTime(ecl, 0x4f44, 0, 0);
            break;
        case 7:
            SetWarp(context, 3, 0xb0a);
            break;
        case 8:
            SetWarp(context, 3, 0xb0a);
            ECLSetHealth(ecl, 0x5444, 0, 0x833);
            ecl << pair{0x5388, 0} << pair{0x53a8, 0} << pair{0x53c8, 0}
                << pair{0x53e8, 0} << pair{0x5408, 0} << pair{0x5428, 0};
            break;
        case 9:
            if (session.dialogue)
                SetWarp(context, 3, 0x1dde);
            else
                SetWarp(context, 3, 0x1ddf);
            break;
        case 10:
            SetWarp(context, 3, 0x1ddf);
            ECLSetTime(ecl, 0x6ba8, 0x3c, 0);
            break;
        case 11:
            SetWarp(context, 3, 0x1ddf);
            ecl << pair{0x6bb4, 0x35} << pair{0x80d4, (int16_t)0} << pair{0x80e4, (int16_t)0};
            break;
        case 12:
            SetWarp(context, 3, 0x1ddf);
            ecl << pair{0x6bb4, 0x35} << pair{0x80d4, (int16_t)0} << pair{0x80e4, (int16_t)0};
            ECLSetTime(ecl, 0x81f4, 0, 0);
            break;
        case 13:
            SetWarp(context, 3, 0x1ddf);
            ecl << pair{0x6bb4, 0x35} << pair{0x80d4, (int16_t)0} << pair{0x80e4, (int16_t)0}
                << pair{0xc698, (int16_t)0} << pair{0xc6a8, (int16_t)0}
                << pair{0xd4ec, (int16_t)0} << pair{0xd4fc, (int16_t)0}
                << pair{0xcdd0, (int16_t)0} << pair{0xcde0, (int16_t)0};
            ECLForceSpell(ecl, 0x81f4, 0, 0x577, 0x578, 0x4444464b, 0xe10, 0x4444464b);
            break;
        case 14:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0x335);
            break;
        case 15:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0x742);
            break;
        case 16:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0x742);
            ECLSetHealth(ecl, 0x2e58, 0x3c, 0x4af);
            break;
        case 17:
            context.stage3BackgroundFix = true;
            if (session.dialogue)
                SetWarp(context, 2, 0xd41);
            else
                SetWarp(context, 2, 0xd42);
            break;
        case 18:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0xd42);
            ECLSetTime(ecl, 0x43d0, 0x3c, 0);
            break;
        case 19:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0xd42);
            ecl << pair{0x43dc, 36} << pair{0x4d30, (int16_t)0} << pair{0x4d40, (int16_t)0};
            ecl << pair{0x4248, (int16_t)0} << pair{0x4264, (int16_t)0}
                << pair{0x4274, (int16_t)0} << pair{0x4284, (int16_t)0}
                << pair{0x42a4, (int16_t)0} << pair{0x42b4, (int16_t)0};
            break;
        case 20:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0xd42);
            ecl << pair{0x42ac, 0x2e} << pair{0x42bc, 0x30} << pair{0x42dc, 0x25};
            ECLSetTime(ecl, 0x43bc, 0x3c, 0);
            break;
        case 21:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0xd42);
            ecl << pair{0x43dc, 0x25} << pair{0x4e50, (int16_t)0} << pair{0x4e60, (int16_t)0};
            break;
        case 22:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0xd42);
            ecl << pair{0x43dc, 0x25} << pair{0x4e50, (int16_t)0} << pair{0x4e60, (int16_t)0};
            ECLSetTime(ecl, 0x4fb0, 0, 0);
            break;
        case 23:
            context.stage3BackgroundFix = true;
            SetWarp(context, 2, 0xd42);
            ecl << pair{0x43dc, 0x25} << pair{0x4e50, (int16_t)0} << pair{0x4e60, (int16_t)0}
                << pair{0x9c94, (int16_t)0} << pair{0x9ca4, (int16_t)0};
            ECLForceSpell(ecl, 0x4fb0, 0, 0x7cf, 0x7d0, 0x38383838, 0xa8c, 0x38383838);
            break;
        case 24:
            SetWarp(context, 2, 0x1bd2);
            break;
        case 25:
            context.nameFix = true;
            if (session.dialogue)
                SetWarp(context, 2, 0x39ea);
            else
                SetWarp(context, 2, 0x39eb);
            context.fakeShot = 0;
            break;
        case 26:
            context.nameFix = true;
            if (session.dialogue)
                SetWarp(context, 2, 0x39ea);
            else
                SetWarp(context, 2, 0x39eb);
            context.fakeShot = 4;
            break;
        case 27:
            context.nameFix = true;
            if (session.dialogue)
                SetWarp(context, 2, 0x39ea);
            else
                SetWarp(context, 2, 0x39eb);
            context.fakeShot = 2;
            break;
        case 28:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            context.fakeShot = 0;
            ECLSetTime(ecl, 0x4934, 0x3c, 0);
            break;
        case 29:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            context.fakeShot = 4;
            ECLSetTime(ecl, 0x4934, 0x3c, 0);
            break;
        case 30:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            context.fakeShot = 2;
            ECLSetTime(ecl, 0x4934, 0x3c, 0);
            break;
        case 31:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ecl << pair{0x4880, 0} << pair{0x4adc, 0};
            break;
        case 32:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            lunasa();
            break;
        case 33:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            ECLSetTime(ecl, 0x51a0, 0x20, 0);
            lunasa();
            timesave();
            break;
        case 34:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            merlin();
            break;
        case 35:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            ECLSetTime(ecl, 0x51a0, 0x20, 0);
            merlin();
            timesave();
            break;
        case 36:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            lyrica();
            break;
        case 37:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x2f);
            ECLSetTime(ecl, 0x51a0, 0x20, 0);
            lyrica();
            timesave();
            break;
        case 38:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x31);
            break;
        case 39:
            context.nameFix = true;
            SetWarp(context, 2, 0x39eb);
            ECLCallSub(ecl, 0x4934, 0, 0x31);
            ECLForceSpell(ecl, 0x5488, 0, 0x95f, 0x960, 0x33333333, 0xe10, 0x33333333);
            break;
        case 40:
            SetWarp(context, 2, 0x12d4);
            if (!session.dialogue)
                ecl << pair{0xb74c, (uint16_t)13};
            break;
        case 41:
            SetWarp(context, 2, 0x12d4);
            ecl << pair{0xb748, (int16_t)0};
            ECLSetHealth(ecl, 0x38b4, 0x3c, 0x5db);
            break;
        case 42:
            if (session.dialogue)
                SetWarp(context, 2, 0x17e1);
            else
                SetWarp(context, 2, 0x17e2);
            break;
        case 43:
            SetWarp(context, 2, 0x17e2);
            ECLSetTime(ecl, 0x58f0, 0x3c, 0);
            break;
        case 44:
            SetWarp(context, 2, 0x17e2);
            ECLCallSub(ecl, 0x58f0, 0x3c, 0x34);
            ecl << pair{0x68dc, (int16_t)0} << pair{0x68ec, (int16_t)0};
            break;
        case 45:
            SetWarp(context, 2, 0x17e2);
            ECLCallSub(ecl, 0x58f0, 0x3c, 0x34);
            ecl << pair{0x68dc, (int16_t)0} << pair{0x68ec, (int16_t)0};
            ECLSetTime(ecl, 0x68f8, 0, 0);
            break;
        case 46:
            SetWarp(context, 2, 0x17e2);
            ECLCallSub(ecl, 0x58f0, 0x3c, 0x37);
            ecl << pair{0x7010, (int16_t)0} << pair{0x7020, (int16_t)0}
                << pair{0x7110, 0};
            break;
        case 47:
            ecl << pair{0x5820, 0} << pair{0x5840, 0} << pair{0x5860, 0} << pair{0x5880, 0}
                << pair{0x58a0, 0} << pair{0x58c0, 0} << pair{0x58dc, 0}
                << pair{0x7010, (int16_t)0} << pair{0x7020, (int16_t)0} << pair{0xa228, (int16_t)0} << pair{0xa238, (int16_t)0};
            SetWarp(context, 2, 0x17e2);
            ECLCallSub(ecl, 0x58f0, 0, 0x37);
            ECLSetHealth(ecl, 0x7110, 0, 0x7cf);
            break;
        case 48:
            SetWarp(context, 2, 0x49c);
            break;
        case 49:
            SetWarp(context, 2, 0x77a);
            if (!session.dialogue)
                ecl << pair{0xe630, (int16_t)0};
            break;
        case 50:
            SetWarp(context, 2, 0x77a);
            ecl << pair{0xe630, (int16_t)0};
            ECLSetHealth(ecl, 0x17cc, 0x3c, 0x5db);
            break;
        case 51:
            if (session.dialogue)
                SetWarp(context, 2, 2518);
            else {
                SetWarp(context, 2, 0x9d7);
                context.nameFix = true;
                ecl << pair{0xe678, (int16_t)0x9d7};
            }
            break;
        case 52:
            context.nameFix = true;
            SetWarp(context, 2, 0x9d7);
            ecl << pair{0xe678, (int16_t)0x9d7};
            ECLSetTime(ecl, 0x3324, 0x3c, 0);
            break;
        case 53:
            context.nameFix = true;
            yuyuko();
            break;
        case 54:
            context.nameFix = true;
            yuyuko();
            ECLSetTime(ecl, 0x445c, 0, 0);
            break;
        case 55:
            context.nameFix = true;
            yuyuko();
            ECLCallSub(ecl, 0x445c, 0, 0x24);
            ecl << pair{0x47a4, (int16_t)0} << pair{0x47b4, (int16_t)0};
            break;
        case 56:
            context.nameFix = true;
            yuyuko();
            ECLCallSub(ecl, 0x445c, 0, 0x24);
            ecl << pair{0x47a4, (int16_t)0} << pair{0x47b4, (int16_t)0};
            ECLSetTime(ecl, 0x48d4, 0, 0);
            break;
        case 57:
            context.nameFix = true;
            SetWarp(context, 2, 0x9d7);
            ecl << pair{0x5574, (int16_t)0} << pair{0x5584, (int16_t)0}
                << pair{0xe678, (int16_t)0x9d7} << pair{0x3330, 0x26};
            break;
        case 58:
            context.nameFix = true;
            SetWarp(context, 2, 0x9d7);
            ecl << pair{0x5574, (int16_t)0} << pair{0x5584, (int16_t)0}
                << pair{0xe678, (int16_t)0x9d7} << pair{0x3330, 0x26};
            ECLSetTime(ecl, 0x5694, 0, 0);
            break;
        case 59:
            context.nameFix = true;
            ecl << pair{0x3240, 0} << pair{0x3260, 0} << pair{0x3280, 0} << pair{0x32a0, 0}
                << pair{0x32c0, 0} << pair{0x32e0, 0} << pair{0x32fc, 0} << pair{0x3310, 0} << pair{0x3324, 0}
                << pair{0xa4b4, (int16_t)0} << pair{0xa4c4, (int16_t)0}
                << pair{0xe678, (int16_t)0x9d7} << pair{0x3330, 0x3a} << pair{0x3188, (int16_t)0};
            SetWarp(context, 2, 0x9d7);
            break;
        case 60:
            context.nameFix = true;
            context.ranBoundaryFix = true;
            SetWarp(context, 2, 0x9d7);
            ecl << pair{0xe678, (int16_t)0x9d7} << pair{0x30dc, 0x3e}
                << pair{0xb9f4, (int16_t)0} << pair{0xbab0, 0} << pair{0xc078, (int16_t)0};
            break;
        case 61:
            SetWarp(context, 2, 0xfe1);
            if (!session.dialogue)
                ecl << pair{0xee58, 0};
            break;
        case 62:
            SetWarp(context, 2, 0xfe1);
            ecl << pair{0xee58, 0};
            ECLSetTime(ecl, 0x3a70, 0x3c, 0);
            break;
        case 63:
            SetWarp(context, 2, 0xfe1);
            ecl << pair{0xee58, 0}
                << pair{0x3900, (int16_t)0} << pair{0x3a7c, 0x3d}
                << pair{0x39a0, 0} << pair{0x39c0, 0} << pair{0x39e0, 0} << pair{0x3a00, 0}
                << pair{0x3a20, 0} << pair{0x3a40, 0} << pair{0x3a5c, 0} << pair{0x3a70, 0};
            break;
        case 64:
            if (session.dialogue)
                SetWarp(context, 2, 8798);
            else {
                context.nameFix = true;
                SetWarp(context, 2, 0x225f);
            }
            break;
        case 65:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ECLSetTime(ecl, 0x4ce0, 0x3c, 0);
            break;
        case 66:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x4df8, (int16_t)0} << pair{0x4e08, (int16_t)0} << pair{0x4e18, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x46);
            break;
        case 67:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x4df8, (int16_t)0} << pair{0x4e08, (int16_t)0} << pair{0x4e18, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x46);
            ECLSetTime(ecl, 0x4f40, 0, 0);
            break;
        case 68:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x4fd4, (int16_t)0} << pair{0x4fe4, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x47);
            break;
        case 69:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x4fd4, (int16_t)0} << pair{0x4fe4, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x47);
            ECLSetTime(ecl, 0x510c, 0, 0);
            break;
        case 70:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x5210, (int16_t)0} << pair{0x5220, (int16_t)0} << pair{0x5230, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x49);
            break;
        case 71:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x5210, (int16_t)0} << pair{0x5220, (int16_t)0} << pair{0x5230, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x49);
            ECLSetTime(ecl, 0x5358, 0, 0);
            break;
        case 72:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x5430, (int16_t)0} << pair{0x5440, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4b);
            break;
        case 73:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x5430, (int16_t)0} << pair{0x5440, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4b);
            ECLSetTime(ecl, 0x5590, 0, 0);
            break;
        case 74:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x56e4, (int16_t)0} << pair{0x56f4, (int16_t)0} << pair{0x5704, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4d);
            break;
        case 75:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x56e4, (int16_t)0} << pair{0x56f4, (int16_t)0} << pair{0x5704, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4d);
            ECLSetTime(ecl, 0x5854, 0, 0);
            break;
        case 76:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x5964, (int16_t)0} << pair{0x5974, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4f);
            break;
        case 77:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x5964, (int16_t)0} << pair{0x5974, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x4f);
            ECLSetTime(ecl, 0x5ac4, 0, 0);
            break;
        case 78:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x5c28, (int16_t)0} << pair{0x5c38, (int16_t)0} << pair{0x5c48, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x51);
            break;
        case 79:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x5c28, (int16_t)0} << pair{0x5c38, (int16_t)0} << pair{0x5c48, (int16_t)0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x51);
            ECLSetTime(ecl, 0x5dc0, 0, 0);
            break;
        case 80:
            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair{0x4b5c, (int16_t)0} << pair{0x5ebc, (int16_t)0} << pair{0x5ecc, (int16_t)0}
                << pair{0x5fdc, 0} << pair{0x5ff0, 0};
            ECLCallSub(ecl, 0x4ce0, 0x3c, 0x53);
            break;
        case 81: {
            // Pre-spell non we warp to: Sub 84
            // Spell: Sub 123
            constexpr uint32_t st7BossLifeCallbackOp = 0x4b58 + 0x4;
            constexpr uint32_t st7BossLifeFirstCall = 0x4ce0;
            constexpr uint32_t st7BossNon10DropItemsOp = 0x602c + 0x4;
            constexpr uint32_t st7BossNon10DropPointItemsOp = 0x603c + 0x4;
            constexpr uint32_t st7BossNon10SetInt7Delay = 0x614c;
            constexpr uint32_t st7BossNon10SpellCallDelay = 0x6160;

            context.nameFix = true;
            SetWarp(context, 2, 0x225f);
            ecl << pair { st7BossLifeCallbackOp, (int16_t)0 }
                << pair { st7BossNon10DropItemsOp, (int16_t)0 }
                << pair { st7BossNon10DropPointItemsOp, (int16_t)0 }
                << pair { st7BossNon10SetInt7Delay, 0 }
                << pair { st7BossNon10SpellCallDelay, 0 };
            ECLCallSub(ecl, st7BossLifeFirstCall, 60, 84);

            switch (session.phase) {
            case 1: { // fully deployed
                constexpr uint32_t st7BossSpell10Shot1HealthCheckOp = 0xd3b8 + 0x4;
                constexpr uint32_t st7BossSpell10Shot2HealthCheckOp = 0xd480 + 0x4;
                constexpr uint32_t st7BossSpell10Shot3HealthCheckOp = 0xd548 + 0x4;
                constexpr uint32_t st7BossSpell10Shot4HealthCheckOp = 0xd610 + 0x4;
                constexpr uint32_t st7BossSpell10Shot5HealthCheckOp = 0xd6d8 + 0x4;

                ecl << pair { st7BossSpell10Shot1HealthCheckOp, (int16_t)0 } << pair { st7BossSpell10Shot2HealthCheckOp, (int16_t)0 }
                    << pair { st7BossSpell10Shot3HealthCheckOp, (int16_t)0 } << pair { st7BossSpell10Shot4HealthCheckOp, (int16_t)0 }
                    << pair { st7BossSpell10Shot5HealthCheckOp, (int16_t)0 };
                break;
            }

            case 2: { // rage phase
                constexpr uint32_t st7BossSpell10TimeCheckReq = 0xd39c + 0x10;
                ecl << pair { st7BossSpell10TimeCheckReq, (int16_t)0 };
                break;
            }

            default:
                break;
            }
            break;
        }
        case 82:
            SetWarp(context, 2, 0xf7d);
            if (!session.dialogue)
                ecl << pair{0x12674, 0};
            break;
        case 83:
            SetWarp(context, 2, 0xf7d);
            ecl << pair{0x12674, 0};
            ECLSetTime(ecl, 0x4350, 0x3c, 0);
            break;
        case 84:
            SetWarp(context, 2, 0xf7d);
            ECLCallSub(ecl, 0x4350, 0x0, 0x3f);
            ecl << pair{0x12674, 0}
                << pair{0x41e0, (int16_t)0}
                << pair{0x4280, 0} << pair{0x42a0, 0} << pair{0x42c0, 0} << pair{0x42e0, 0}
                << pair{0x4300, 0} << pair{0x4320, 0} << pair{0x433c, 0};
            break;
        case 85:
            if (session.dialogue)
                SetWarp(context, 2, 9258);
            else {
                context.nameFix = true;
                SetWarp(context, 2, 0x242b);
            }
            break;
        case 86:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ECLSetTime(ecl, 0x55ec, 0x3c, 0);
            break;
        case 87:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5704, (int16_t)0} << pair{0x5714, (int16_t)0} << pair{0x5724, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x48);
            break;
        case 88:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5704, (int16_t)0} << pair{0x5714, (int16_t)0} << pair{0x5724, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x48);
            ECLSetTime(ecl, 0x584c, 0, 0);
            break;
        case 89:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x58e0, (int16_t)0} << pair{0x58f0, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x49);
            break;
        case 90:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x58e0, (int16_t)0} << pair{0x58f0, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x49);
            ECLSetTime(ecl, 0x5a18, 0, 0);
            break;
        case 91:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5b1c, (int16_t)0} << pair{0x5b2c, (int16_t)0} << pair{0x5b3c, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4b);
            break;
        case 92:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5b1c, (int16_t)0} << pair{0x5b2c, (int16_t)0} << pair{0x5b3c, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4b);
            ECLSetTime(ecl, 0x5c64, 0, 0);
            break;
        case 93:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5d3c, (int16_t)0} << pair{0x5d4c, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4d);
            break;
        case 94:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5d3c, (int16_t)0} << pair{0x5d4c, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4d);
            ECLSetTime(ecl, 0x5e9c, 0, 0);
            break;
        case 95:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5ff0, (int16_t)0} << pair{0x6000, (int16_t)0} << pair{0x6010, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4f);
            break;
        case 96:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5ff0, (int16_t)0} << pair{0x6000, (int16_t)0} << pair{0x6010, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x4f);
            ECLSetTime(ecl, 0x6160, 0, 0);
            break;
        case 97:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x6270, (int16_t)0} << pair{0x6280, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x51);
            break;
        case 98:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x6270, (int16_t)0} << pair{0x6280, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x51);
            ECLSetTime(ecl, 0x63d0, 0, 0);
            break;
        case 99:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x6534, (int16_t)0} << pair{0x6544, (int16_t)0} << pair{0x6554, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x53);
            break;
        case 100:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x6534, (int16_t)0} << pair{0x6544, (int16_t)0} << pair{0x6554, (int16_t)0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x53);
            ECLSetTime(ecl, 0x66cc, 0, 0);
            break;
        case 101:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5468, (int16_t)0} << pair{0x67d0, (int16_t)0} << pair{0x67e0, (int16_t)0}
                << pair{0x68e8, 0} << pair{0x68fc, 0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x55);
            break;
        case 102:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5468, (int16_t)0} << pair{0x693c, (int16_t)0} << pair{0x694c, (int16_t)0} << pair{0x695c, (int16_t)0}
                << pair{0x6a68, 0} << pair{0x6a7c, 0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x56);

            if (session.phase == 1) {
                ecl << pair{0xe154, 0xa};
            }
            break;
        case 103:
            context.nameFix = true;
            SetWarp(context, 2, 0x242b);
            ecl << pair{0x5468, (int16_t)0} << pair{0x6abc, (int16_t)0} << pair{0x6acc, (int16_t)0}
                << pair{0x6bd8, 0} << pair{0x6bec, 0};
            ECLCallSub(ecl, 0x55ec, 0x3c, 0x57);
            break;
        default:
            break;
        }
    }
