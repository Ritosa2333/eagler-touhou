// Generated native purple ECL write helpers (MIT).
    void ECLJump(ECLHelper& ecl,int32_t time,int32_t pos, int32_t time_jmp,int32_t target)
    {
        ecl << pair { pos, (int32_t)time } << pair { pos + 4, (int16_t)0x2 } << pair { pos + 6, (int16_t)0x14 } << pair { pos + 8, (int32_t)0x00FFFF00 };
        ecl << pair { pos + 0xC, (int32_t)time_jmp } << pair { pos + 0xC + 4, (int32_t)target - pos };
    }

    void ECLSetHealth(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t time)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x0010006f << 0x00ffff00 << time;
    }
    void ECLSetTime(ECLHelper& ecl, int offset, int32_t ecl_time, int32_t health)
    {
        ecl.SetPos(offset);
        ecl << ecl_time << 0x00100073 << 0x00ffff00 << health;
    }
    void ECLStall(ECLHelper& ecl, int offset)
    {
        ecl.SetPos(offset);
        ecl << 0x99999 << 0x000c0000 << 0x0000ff00;
    }
