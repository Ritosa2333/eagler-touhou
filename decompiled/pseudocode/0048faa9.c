// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48faa9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

unsigned int sub_48faa9(unsigned int a0)
{
    unsigned short v3;  // ftop
    unsigned int v4;  // fc3210
    char v5;  // al
    unsigned int v6;  // sseround
    unsigned int v7;  // 4120
    char v8;  // cl
    unsigned int v9;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8], Other Possible Types: unsigned short

    v1 = a0;
    v0 = a0;
    v1 = (v3 & 7) * 0x800 | (unsigned short)v4 & 0x4700;
    v5 = v1;
    v9 = 0;
    if ((char)v1 & 63)
    {
        if (v5 & 1)
            v9 = 16;
        if (v5 & 4)
            v9 |= 8;
        if (v5 & 8)
            v9 |= 4;
        if (v5 & 16)
            v9 |= 2;
        if (v5 & 32)
            v9 |= 1;
        if (v5 & 2)
            v9 |= 0x80000;
    }
    if (!g_4d52dc)
        return v9;
    v7 = _ccall(v6 & 3);
    v0 = v7;
    v8 = v0;
    v9 = 0;
    if ((char)v0 & 63)
    {
        if (v8 & 1)
            v9 = 16;
        if (v8 & 4)
            v9 |= 8;
        if (v8 & 8)
            v9 |= 4;
        if (v8 & 16)
            v9 |= 2;
        if (v8 & 32)
            v9 |= 1;
        if (v8 & 2)
            v9 |= 0x80000;
    }
    return v9 | v9;
}
