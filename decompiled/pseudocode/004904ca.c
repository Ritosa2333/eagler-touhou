// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4904ca; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

unsigned int sub_4904ca(unsigned int a0, unsigned int a1)
{
    unsigned int v6;  // edi
    unsigned int v7;  // edi
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // eax
    unsigned int v17;  // sseround
    unsigned int v18;  // 4123
    unsigned int v19;  // eax
    unsigned int v20;  // ecx
    unsigned int v8;  // fpround
    unsigned int v21;  // ecx
    unsigned int v22;  // eax
    unsigned int v23;  // eax
    unsigned int v24;  // edx
    unsigned int v25;  // 4122
    unsigned int v26;  // edx
    unsigned int v27;  // ecx
    unsigned int v28;  // ecx
    unsigned int v29;  // edx
    unsigned int v9;  // 4148
    unsigned int v30;  // edx
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned long long v12;  // 4139
    unsigned int v13;  // 4142
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]

    v0 = v6;
    v7 = a1 & 0xfff7ffff;
    v9 = _ccall(v8);
    *((unsigned short *)&v5) = v9;
    v14 = 0;
    if ((char)v5 & 1)
        v14 = 16;
    if ((char)v5 & 4)
        v14 |= 8;
    if ((char)v5 & 8)
        v14 |= 4;
    if ((char)v5 & 16)
        v14 |= 2;
    if ((char)v5 & 32)
        v14 |= 1;
    if ((char)v5 & 2)
        v14 |= 0x80000;
    v10 = (unsigned short)v5;
    v11 = v10 & 0xc00;
    if (v10 & 0xc00)
    {
        if (v11 == 0x400)
        {
            v14 |= 0x100;
        }
        else if (v11 == 0x800)
        {
            v14 |= 0x200;
        }
        else if (v11 == 0xc00)
        {
            v14 |= 0x300;
        }
    }
    if (!(v10 & 0x300))
    {
        v14 |= 0x20000;
    }
    else if ((v10 >> 8 & 3) == 2)
    {
        v14 |= 0x10000;
    }
    if (v5 & 0x1000)
        v14 |= 0x40000;
    v14 = ~(v7) & v14 | v7 & a0;
    v2 = v14;
    if (v14 != v14)
    {
        a1 = sub_48f849() & 0xffff;
        v12 = _ccall((unsigned short)a1);
        v13 = _ccall((unsigned int)v12);
        *((unsigned short *)&a1) = v13;
        v14 = 0;
        if ((char)a1 & 1)
            v14 = 16;
        if ((char)a1 & 4)
            v14 |= 8;
        if ((char)a1 & 8)
            v14 |= 4;
        if ((char)a1 & 16)
            v14 |= 2;
        if ((char)a1 & 32)
            v14 |= 1;
        if ((char)a1 & 2)
            v14 |= 0x80000;
        v15 = (unsigned short)a1;
        v16 = v15 & 0xc00;
        if (v15 & 0xc00)
        {
            if (v16 == 0x400)
            {
                v14 |= 0x100;
            }
            else if (v16 == 0x800)
            {
                v14 |= 0x200;
            }
            else if (v16 == 0xc00)
            {
                v14 |= 0x300;
            }
        }
        if (!(v15 & 0x300))
        {
            v14 |= 0x20000;
        }
        else if ((v15 >> 8 & 3) == 2)
        {
            v14 |= 0x10000;
        }
        if (a1 & 0x1000)
            v14 |= 0x40000;
        v2 = v14;
    }
    v14 = 0;
    if (!g_4d52dc)
        return v14;
    v1 = v7 & 50856735;
    v18 = _ccall(v17 & 3);
    v3 = v18;
    v19 = v3;
    if ((char)v19 < 0)
        v14 = 16;
    if (v19 & 0x200)
        v14 |= 8;
    if (v19 & 0x400)
        v14 |= 4;
    if (v19 & 0x800)
        v14 |= 2;
    if (v19 & 0x1000)
        v14 |= 1;
    if (v19 & 0x100)
        v14 |= 0x80000;
    v20 = v19;
    v21 = v20 & 0x6000;
    if (v20 & 0x6000)
    {
        if (v21 == 0x2000)
        {
            v14 |= 0x100;
        }
        else if (v21 == 0x4000)
        {
            v14 |= 0x200;
        }
        else if (v21 == 0x6000)
        {
            v14 |= 0x300;
        }
    }
    v22 = v19 & 32832;
    v23 = v22 - 64;
    if (v22 == 64)
    {
        v14 |= 0x2000000;
    }
    else if (v23 == 0x7fc0)
    {
        v14 |= 0x3000000;
    }
    else if (v23 == 0x8000)
    {
        v14 |= 0x1000000;
    }
    v24 = ~(v1) & v14 | v1 & a0;
    if (v24 != v14)
    {
        v4 = sub_48f9ba(v21, v24);
        sub_491220(v4);
        v25 = _ccall(v17 & 3);
        v4 = v25;
        v26 = v4;
        v14 = 0;
        if ((char)v26 < 0)
            v14 = 16;
        if (v26 & 0x200)
            v14 |= 8;
        if (v26 & 0x400)
            v14 |= 4;
        if (v26 & 0x800)
            v14 |= 2;
        if (v26 & 0x1000)
            v14 |= 1;
        if (v26 & 0x100)
            v14 |= 0x80000;
        v27 = v26;
        v28 = v27 & 0x6000;
        if (v27 & 0x6000)
        {
            if (v28 == 0x2000)
            {
                v14 |= 0x100;
            }
            else if (v28 == 0x4000)
            {
                v14 |= 0x200;
            }
            else if (v28 == 0x6000)
            {
                v14 |= 0x300;
            }
        }
        v29 = v26 & 32832;
        v30 = v29 - 64;
        if (v29 == 64)
        {
            v14 |= 0x2000000;
        }
        else if (v30 == 0x7fc0)
        {
            v14 |= 0x3000000;
        }
        else if (v30 == 0x8000)
        {
            v14 |= 0x1000000;
        }
    }
    return v14 | v2 | 0x80000000;
}
