// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48fea7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

unsigned int sub_48fea7(unsigned int a0, unsigned int a1, unsigned int *a2, unsigned int *a3)
{
    unsigned int v5;  // eax
    unsigned int v6;  // edx
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v16;  // esi
    unsigned int v17;  // sseround
    unsigned int v18;  // 4123
    unsigned int v19;  // eax
    unsigned int v20;  // ecx
    unsigned int v7;  // edi
    unsigned int v21;  // ecx
    unsigned int v22;  // eax
    unsigned int v23;  // eax
    unsigned int v24;  // eax
    unsigned int v25;  // 4122
    unsigned int v26;  // ecx
    unsigned int v27;  // edx
    unsigned int v28;  // eax
    unsigned int v29;  // eax
    unsigned int v8;  // fpround
    unsigned int v30;  // ecx
    unsigned int v31;  // ecx
    unsigned int v9;  // 4117
    unsigned int v10;  // ecx
    unsigned int v11;  // ecx
    unsigned long long v12;  // 4139
    unsigned int v13;  // 4142
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]

    v5 = a1;
    v6 = 0;
    v0 = v7;
    if (a2)
    {
        v9 = _ccall(v8);
        *((unsigned short *)&v3) = v9;
        if ((char)v3 & 1)
            v6 = 16;
        if ((char)v3 & 4)
            v6 |= 8;
        if ((char)v3 & 8)
            v6 |= 4;
        if ((char)v3 & 16)
            v6 |= 2;
        if ((char)v3 & 32)
            v6 |= 1;
        if ((char)v3 & 2)
            v6 |= 0x80000;
        v10 = (unsigned short)v3;
        v11 = v10 & 0xc00;
        if (v10 & 0xc00)
        {
            if (v11 == 0x400)
            {
                v6 |= 0x100;
            }
            else if (v11 == 0x800)
            {
                v6 |= 0x200;
            }
            else if (v11 == 0xc00)
            {
                v6 |= 0x300;
            }
        }
        if (!(v10 & 0x300))
        {
            v6 |= 0x20000;
        }
        else if ((v10 >> 8 & 3) == 2)
        {
            v6 |= 0x10000;
        }
        if (v3 & 0x1000)
            v6 |= 0x40000;
        v6 = ~(a1) & v6 | a0 & a1;
        if (v6 != v6)
        {
            v4 = sub_48f849() & 0xffff;
            v12 = _ccall((unsigned short)v4);
            v13 = _ccall((unsigned int)v12);
            *((unsigned short *)&v4) = v13;
            v6 = 0;
            if ((char)v4 & 1)
                v6 = 16;
            if ((char)v4 & 4)
                v6 |= 8;
            if ((char)v4 & 8)
                v6 |= 4;
            if ((char)v4 & 16)
                v6 |= 2;
            if ((char)v4 & 32)
                v6 |= 1;
            if ((char)v4 & 2)
                v6 |= 0x80000;
            v14 = (unsigned short)v4;
            v15 = v14 & 0xc00;
            if (v14 & 0xc00)
            {
                if (v15 == 0x400)
                {
                    v6 |= 0x100;
                }
                else if (v15 == 0x800)
                {
                    v6 |= 0x200;
                }
                else if (v15 == 0xc00)
                {
                    v6 |= 0x300;
                }
            }
            if (!(v14 & 0x300))
            {
                v6 |= 0x20000;
            }
            else if ((v14 >> 8 & 3) == 2)
            {
                v6 |= 0x10000;
            }
            if (v4 & 0x1000)
                v6 |= 0x40000;
        }
        *(a2) = v6;
        v5 = a1;
    }
    if (!a3)
        return 1;
    v16 = 0;
    if (!g_4d52dc)
    {
        *(a3) = 0;
        return 1;
    }
    v1 = v5 & 50856735;
    v18 = _ccall(v17 & 3);
    v2 = v18;
    v19 = v2;
    if ((char)v19 < 0)
    {
        v6 = 16;
        v16 = 16;
    }
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
    if ((~(v1) & v16 | v1 & a0) == v16)
    {
        v24 = v16;
    }
    else
    {
        a2 = sub_48f9ba();
        sub_491220(a2);
        v25 = _ccall(v17 & 3);
        a2 = v25;
        v26 = a2;
        v27 = 0;
        if ((char)v26 < 0)
        {
            v6 = 16;
            v27 = 16;
        }
        if (v26 & 0x200)
            v27 |= 8;
        if (v26 & 0x400)
            v27 |= 4;
        if (v26 & 0x800)
            v27 |= 2;
        if (v26 & 0x1000)
            v27 |= 1;
        if (v26 & 0x100)
            v27 |= 0x80000;
        v28 = v26;
        v29 = v28 & 0x6000;
        if (v28 & 0x6000)
        {
            if (v29 == 0x2000)
            {
                v27 |= 0x100;
            }
            else if (v29 == 0x4000)
            {
                v27 |= 0x200;
            }
            else if (v29 == 0x6000)
            {
                v27 |= 0x300;
            }
        }
        v30 = v26 & 32832;
        v31 = v30 - 64;
        if (v30 == 64)
        {
            v27 |= 0x2000000;
        }
        else if (v31 == 0x7fc0)
        {
            v27 |= 0x3000000;
        }
        else if (v31 == 0x8000)
        {
            v27 |= 0x1000000;
        }
        v24 = v27;
    }
    *(a3) = v24;
    return 1;
}
