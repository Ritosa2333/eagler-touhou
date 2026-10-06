// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450080; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_450080_0 {
    char padding_0[20];
    char field_14;
    char padding_15[59];
    unsigned long long field_50;
} st_450080_0;

typedef struct st_450080_2 {
    unsigned int field_0;
} st_450080_2;

typedef struct st_450080_1 {
    char padding_0[164];
    struct st_450080_2 *field_a4;
    struct st_450080_2 *field_a8;
    char padding_ac[16];
    struct st_450080_2 *field_bc;
    char padding_c0[68];
    struct st_450080_2 *field_104;
} st_450080_1;

typedef struct st_450080_6 {
    struct st_450080_1 *field_0;
} st_450080_6;

extern st_450080_6 *g_4ce8f0;
extern char g_4ceace;
extern char g_4cec04;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;
extern unsigned int g_4cf278;
extern unsigned long long g_4cf2a0;
extern char g_4cf428;

unsigned int sub_450080(st_450080_0 *idx)
{
    unsigned int v4;  // ftop
    unsigned int v5;  // ftop
    unsigned int v14;  // eax
    unsigned int v6;  // ftop
    unsigned int v7;  // fc3210
    unsigned int v8;  // ftop
    unsigned int v9;  // ftop
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    unsigned int v12;  // ftop
    unsigned int v13;  // fc3210
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

    if (g_4cf428 & 16)
    {
        v5 = v4 - 1;
        if (/* unsupported instruction */)
        {
            v6 = v5 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v6 = v5 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v7 = CmpF(/* unsupported instruction */, idx->field_50) * 0x100 & 0x4500;
            /* unsupported instruction */
            v8 = v6 + 1;
        }
        else
        {
            v7 = CmpF(nan, idx->field_50) * 0x100 & 0x4500;
            /* unsupported instruction */
            v8 = v6 + 1;
        }
        if (!((char)((((unsigned short)v8 & 7) * 0x800 | (unsigned short)v7 & 0x4700) >> 8) & 65))
        {
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
            do
            {
                v11 = v10 - 1;
                if (/* unsupported instruction */)
                {
                    v12 = v11 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v12 = v11 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v13 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), idx->field_50) * 0x100 & 0x4500;
                /* unsupported instruction */
                v10 = v12 + 1;
            } while (!((char)((((unsigned short)v10 & 7) * 0x800 | (unsigned short)v13 & 0x4700) >> 8) & 65));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
    }
    sub_45a3c0(v0, v1, v2, v3);
    g_4cee34 = &g_4cec04;
    sub_430910();
    g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
    g_4cee38 = 1;
    v14 = sub_4624c0(v0, v1);
    if (!v14)
    {
        sub_464c40();
        return 1;
    }
    else if (v14 == 0xffffffff)
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
            sub_4501e0(v0);
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
