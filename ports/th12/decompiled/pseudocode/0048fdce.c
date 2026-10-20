// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48fdce; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

unsigned int sub_48fdce(unsigned int a0)
{
    unsigned short v4;  // ftop
    unsigned int v5;  // fc3210
    unsigned int v11;  // eax
    char v12;  // cl
    unsigned int v13;  // eax
    char v6;  // al
    unsigned int v7;  // esi
    unsigned int v8;  // sseround
    unsigned int v9;  // 4136
    char v10;  // cl
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8], Other Possible Types: unsigned short

    v2 = a0;
    v1 = a0;
    v2 = (v4 & 7) * 0x800 | (unsigned short)v5 & 0x4700;
    if (g_4d52dc)
    {
        v6 = v2;
        v11 = 0;
        v0 = v7;
        if ((char)v2 & 63)
        {
            if (v6 & 1)
                v11 = 16;
            if (v6 & 4)
                v11 |= 8;
            if (v6 & 8)
                v11 |= 4;
            if (v6 & 16)
                v11 |= 2;
            if (v6 & 32)
                v11 |= 1;
            if (v6 & 2)
                v11 |= 0x80000;
        }
        v9 = _ccall(v8 & 3);
        v1 = v9;
        v1 &= 0xffffffc0;
        v10 = v1;
        v11 = 0;
        if ((char)v1 & 63)
        {
            if (v10 & 1)
                v11 = 16;
            if (v10 & 4)
                v11 |= 8;
            if (v10 & 8)
                v11 |= 4;
            if (v10 & 16)
                v11 |= 2;
            if (v10 & 32)
                v11 |= 1;
            if (v10 & 2)
                v11 |= 0x80000;
        }
        return v11 | v11;
    }
    else
    {
        v12 = v2;
        v13 = 0;
        if (!((char)v2 & 63))
            return 0;
        if (v12 & 1)
            v13 = 16;
        if (v12 & 4)
            v13 |= 8;
        if (v12 & 8)
            v13 |= 4;
        if (v12 & 16)
            v13 |= 2;
        if (v12 & 32)
            v13 |= 1;
        return v13;
    }
}
