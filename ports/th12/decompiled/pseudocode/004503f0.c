// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4503f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4503f0_0 {
    char padding_0[20];
    char field_14;
    char padding_15[51];
    unsigned long long field_48;
    unsigned long long field_50;
} st_4503f0_0;

typedef struct st_4503f0_2 {
    unsigned int field_0;
} st_4503f0_2;

typedef struct st_4503f0_1 {
    char padding_0[164];
    struct st_4503f0_2 *field_a4;
    struct st_4503f0_2 *field_a8;
    char padding_ac[16];
    struct st_4503f0_2 *field_bc;
    char padding_c0[68];
    struct st_4503f0_2 *field_104;
} st_4503f0_1;

typedef struct st_4503f0_6 {
    struct st_4503f0_1 *field_0;
} st_4503f0_6;

extern st_4503f0_6 *g_4ce8f0;
extern char g_4ceace;
extern char g_4cec04;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;
extern unsigned int g_4cf278;
extern unsigned long long g_4cf2a0;

unsigned int sub_4503f0(st_4503f0_0 *idx)
{
    unsigned int v5;  // ftop
    unsigned int v14;  // ftop
    unsigned int v15;  // fc3210
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // fc3210
    unsigned int v22;  // eax
    unsigned int v6;  // 4116
    unsigned int v7;  // fc3210
    unsigned int v8;  // ftop
    unsigned int v9;  // ftop
    unsigned int v10;  // ftop
    unsigned int v11;  // fc3210
    unsigned int v12;  // ftop
    unsigned int v13;  // ftop
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

    sub_4508b0(v0, v1, v2, v3);
    if (/* unsupported instruction */)
        *((long long *)&idx->padding_15[43]) = /* unsupported instruction */;
    else
        *((unsigned long long *)&idx->padding_15[43]) = nan;
    v6 = _ccall(10, 13, (char)((((unsigned short)v5 & 7) * 0x800 | (unsigned short)(/* unsupported instruction */ ? CmpF(/* unsupported instruction */, idx->field_48) * 0x100 & 0x4500 : CmpF(nan, idx->field_48) * 0x100 & 0x4500) & 0x4700) >> 8) & 5, 0, 0);
    if (!(v6 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_50 = /* unsupported instruction */;
        else
            idx->field_50 = nan;
    }
    if (/* unsupported instruction */)
        idx->field_48 = /* unsupported instruction */;
    else
        idx->field_48 = nan;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v7 = CmpF(/* unsupported instruction */, 0x3ff8000000000000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v8 = v5 + 1;
    }
    else
    {
        v7 = CmpF(nan, 0x3ff8000000000000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v8 = v5 + 1;
    }
    if (!((((unsigned short)v8 & 7) * 0x800 | (unsigned short)v7 & 0x4700) >> 8 & 1))
        Sleep(1);
    v9 = v8 - 1;
    if (/* unsupported instruction */)
    {
        v10 = v9 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v10 = v9 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v11 = CmpF(/* unsupported instruction */, idx->field_50) * 0x100 & 0x4500;
        /* unsupported instruction */
        v12 = v10 + 1;
    }
    else
    {
        v11 = CmpF(nan, idx->field_50) * 0x100 & 0x4500;
        /* unsupported instruction */
        v12 = v10 + 1;
    }
    if ((char)((((unsigned short)v12 & 7) * 0x800 | (unsigned short)v11 & 0x4700) >> 8) & 65)
        return 0;
    v13 = v12 - 1;
    if (/* unsupported instruction */)
    {
        v14 = v13 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v14 = v13 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        v15 = CmpF(/* unsupported instruction */, idx->field_50) * 0x100 & 0x4500;
        /* unsupported instruction */
        v16 = v14 + 1;
    }
    else
    {
        v15 = CmpF(nan, idx->field_50) * 0x100 & 0x4500;
        /* unsupported instruction */
        v16 = v14 + 1;
    }
    if (!((char)((((unsigned short)v16 & 7) * 0x800 | (unsigned short)v15 & 0x4700) >> 8) & 65))
    {
        v17 = v16 - 1;
        if (/* unsupported instruction */)
        {
            v18 = v17 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v18 = v17 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        do
        {
            v19 = v18 - 1;
            if (/* unsupported instruction */)
            {
                v20 = v19 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v20 = v19 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v21 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_50) * 0x100 & 0x4500;
            /* unsupported instruction */
            v18 = v20 + 1;
        } while (!((char)((((unsigned short)v18 & 7) * 0x800 | (unsigned short)v21 & 0x4700) >> 8) & 65));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    sub_45a3c0();
    g_4cee34 = &g_4cec04;
    sub_430910();
    g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
    g_4cee38 = 1;
    v22 = sub_4624c0(v0, v1);
    if (!v22)
    {
        sub_464c40();
        return 1;
    }
    else if (v22 == 0xffffffff)
    {
        sub_464c40();
        return 2;
    }
    else
    {
        idx->field_14 = idx->field_14 + 1;
        if (g_4ceace + 1 <= idx->field_14)
        {
            g_4ce8f0->field_0->field_a4(g_4ce8f0);
            sub_45a380(v0);
            g_4cf278 = 0xff;
            sub_430300();
            sub_462620();
            sub_45a3c0();
            g_4ce8f0->field_0->field_104(g_4ce8f0, 0, 0);
            g_4ce8f0->field_0->field_a8(g_4ce8f0);
            idx->field_14 = 0;
            sub_450580(v0);
        }
        sub_4508b0();
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (!/* unsupported instruction */)
        {
            g_4cf2a0 = nan;
            /* unsupported instruction */
            return 0;
        }
        g_4cf2a0 = /* unsupported instruction */;
        /* unsupported instruction */
        return 0;
    }
}
