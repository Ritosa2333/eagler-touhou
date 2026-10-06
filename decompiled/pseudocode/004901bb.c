// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4901bb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

unsigned int sub_4901bb(unsigned int a0, unsigned int a1)
{
    unsigned int v7;  // edi
    unsigned int v8;  // fpround
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v17;  // sseround
    unsigned int v18;  // 4123
    unsigned int v19;  // eax
    unsigned int v20;  // ecx
    unsigned int v21;  // ecx
    unsigned int v9;  // 4141
    unsigned int v22;  // eax
    unsigned int v23;  // eax
    unsigned int v24;  // 4122
    unsigned int v25;  // ecx
    unsigned int v26;  // eax
    unsigned int v27;  // eax
    unsigned int v28;  // ecx
    unsigned int v29;  // ecx
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned long long v12;  // 4139
    unsigned int v13;  // 4142
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x14]
    int v3;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]
    unsigned int v6;  // [bp+0x8]

    v0 = v7;
    v9 = _ccall(v8);
    *((unsigned short *)&v4) = v9;
    v16 = 0;
    if ((char)v4 & 1)
        v16 = 16;
    if ((char)v4 & 4)
        v16 |= 8;
    if ((char)v4 & 8)
        v16 |= 4;
    if ((char)v4 & 16)
        v16 |= 2;
    if ((char)v4 & 32)
        v16 |= 1;
    if ((char)v4 & 2)
        v16 |= 0x80000;
    v10 = (unsigned short)v4;
    v11 = v10 & 0xc00;
    if (v10 & 0xc00)
    {
        if (v11 == 0x400)
        {
            v16 |= 0x100;
        }
        else if (v11 == 0x800)
        {
            v16 |= 0x200;
        }
        else if (v11 == 0xc00)
        {
            v16 |= 0x300;
        }
    }
    if (!(v10 & 0x300))
    {
        v16 |= 0x20000;
    }
    else if ((v10 >> 8 & 3) == 2)
    {
        v16 |= 0x10000;
    }
    if (v4 & 0x1000)
        v16 |= 0x40000;
    v16 = ~(a1) & v16 | a0 & a1;
    v6 = v16;
    if (v16 != v16)
    {
        v5 = sub_48f849() & 0xffff;
        v12 = _ccall((unsigned short)v5);
        v13 = _ccall((unsigned int)v12);
        *((unsigned short *)&v5) = v13;
        v16 = 0;
        if ((char)v5 & 1)
            v16 = 16;
        if ((char)v5 & 4)
            v16 |= 8;
        if ((char)v5 & 8)
            v16 |= 4;
        if ((char)v5 & 16)
            v16 |= 2;
        if ((char)v5 & 32)
            v16 |= 1;
        if ((char)v5 & 2)
            v16 |= 0x80000;
        v14 = (unsigned short)v5;
        v15 = v14 & 0xc00;
        if (v14 & 0xc00)
        {
            if (v15 == 0x400)
            {
                v16 |= 0x100;
            }
            else if (v15 == 0x800)
            {
                v16 |= 0x200;
            }
            else if (v15 == 0xc00)
            {
                v16 |= 0x300;
            }
        }
        if (!(v14 & 0x300))
        {
            v16 |= 0x20000;
        }
        else if ((v14 >> 8 & 3) == 2)
        {
            v16 |= 0x10000;
        }
        if (v5 & 0x1000)
            v16 |= 0x40000;
        v6 = v16;
    }
    v16 = 0;
    if (!g_4d52dc)
        return v16;
    v1 = a1 & 50856735;
    v18 = _ccall(v17 & 3);
    v2 = v18;
    v19 = v2;
    if ((char)v19 < 0)
        v16 = 16;
    if (v19 & 0x200)
        v16 |= 8;
    if (v19 & 0x400)
        v16 |= 4;
    if (v19 & 0x800)
        v16 |= 2;
    if (v19 & 0x1000)
        v16 |= 1;
    if (v19 & 0x100)
        v16 |= 0x80000;
    v20 = v19;
    v21 = v20 & 0x6000;
    if (v20 & 0x6000)
    {
        if (v21 == 0x2000)
        {
            v16 |= 0x100;
        }
        else if (v21 == 0x4000)
        {
            v16 |= 0x200;
        }
        else if (v21 == 0x6000)
        {
            v16 |= 0x300;
        }
    }
    v22 = v19 & 32832;
    v23 = v22 - 64;
    if (v22 == 64)
    {
        v16 |= 0x2000000;
    }
    else if (v23 == 0x7fc0)
    {
        v16 |= 0x3000000;
    }
    else if (v23 == 0x8000)
    {
        v16 |= 0x1000000;
    }
    if ((~(v1) & v16 | v1 & a0) != v16)
    {
        v3 = sub_48f9ba();
        sub_491220(v3);
        v24 = _ccall(v17 & 3);
        v3 = v24;
        v25 = v3;
        v16 = 0;
        if ((char)v25 < 0)
            v16 = 16;
        if (v25 & 0x200)
            v16 |= 8;
        if (v25 & 0x400)
            v16 |= 4;
        if (v25 & 0x800)
            v16 |= 2;
        if (v25 & 0x1000)
            v16 |= 1;
        if (v25 & 0x100)
            v16 |= 0x80000;
        v26 = v25;
        v27 = v26 & 0x6000;
        if (v26 & 0x6000)
        {
            if (v27 == 0x2000)
            {
                v16 |= 0x100;
            }
            else if (v27 == 0x4000)
            {
                v16 |= 0x200;
            }
            else if (v27 == 0x6000)
            {
                v16 |= 0x300;
            }
        }
        v28 = v25 & 32832;
        v29 = v28 - 64;
        if (v28 == 64)
        {
            v16 |= 0x2000000;
        }
        else if (v29 == 0x7fc0)
        {
            v16 |= 0x3000000;
        }
        else if (v29 == 0x8000)
        {
            v16 |= 0x1000000;
        }
    }
    return v16 | v6;
}
