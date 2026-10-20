// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421cd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421cd0_20 {
    char padding_0[1424];
    unsigned int field_590;
} st_421cd0_20;

typedef struct st_421cd0_15 {
    char padding_0[24];
    unsigned int field_18;
    char padding_1c[1];
    char field_1d;
} st_421cd0_15;

typedef struct st_421cd0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_421cd0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_421cd0_1 *field_20;
} st_421cd0_0;

typedef struct st_421cd0_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_421cd0_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_421cd0_1 *field_20;
} st_421cd0_2;

typedef struct st_421cd0_16 {
    char padding_0[96];
    unsigned int field_60;
} st_421cd0_16;

typedef struct st_421cd0_1 {
    char padding_0[8];
    struct st_421cd0_0 *field_8;
    struct st_421cd0_2 *field_c;
    char padding_10[20];
    unsigned int field_24;
    char field_28;
    char padding_29[55];
    unsigned int field_60;
    char padding_64[16];
    unsigned int field_74;
} st_421cd0_1;

typedef struct st_421cd0_17 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[108];
    int field_74;
} st_421cd0_17;

typedef struct st_421cd0_13 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_421cd0_12 *field_494;
} st_421cd0_13;

typedef struct st_421cd0_12 {
    int field_0;
} st_421cd0_12;

typedef struct st_421cd0_19 {
    unsigned int field_0;
    int field_4;
    char padding_8[4];
    struct st_421cd0_16 *field_c;
    int field_10;
    int field_14;
} st_421cd0_19;

typedef struct st_4385b0_0 {
    char padding_0[2440];
    unsigned int field_988;
    unsigned int field_98c;
    char padding_990[30928];
    unsigned int field_8260;
    char padding_8264[88];
    unsigned int field_82bc;
    unsigned int field_82c0;
    char padding_82c4[76];
    unsigned int field_8310;
    char padding_8314[28];
    int field_8330;
    unsigned int field_8334;
    char padding_8338[224];
    unsigned int field_8418;
    char padding_841c[224];
    unsigned int field_84fc;
    char padding_8500[224];
    unsigned int field_85e0;
    char padding_85e4[224];
    unsigned int field_86c4;
    char padding_86c8[224];
    unsigned int field_87a8;
    char padding_87ac[224];
    unsigned int field_888c;
    char padding_8890[224];
    unsigned int field_8970;
    char padding_8974[15016];
    unsigned int field_c41c;
    char padding_c420[108];
    unsigned int field_c48c;
} st_4385b0_0;

typedef struct st_421cd0_21 {
    char padding_0[36];
    unsigned long long field_24;
    unsigned long long field_2c;
} st_421cd0_21;

extern int g_4b0c40;
extern unsigned int g_4b0c44;
extern unsigned int g_4b0c48;
extern unsigned int g_4b0c4c;
extern unsigned int g_4b0c50;
extern unsigned int g_4b0c54;
extern unsigned int g_4b0c58;
extern unsigned int g_4b0c5c;
extern unsigned int g_4b0c78;
extern unsigned int g_4b0c7c;
extern unsigned int g_4b0c80;
extern unsigned int g_4b0c84;
extern unsigned int g_4b0c88;
extern void g_4b0c8c;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0c98;
extern unsigned int g_4b0c9c;
extern unsigned int g_4b0ca0;
extern unsigned int g_4b0ca4;
extern unsigned int g_4b0ca8;
extern char g_4b0cb0;
extern unsigned int g_4b0cb8;
extern unsigned int g_4b0cbc;
extern unsigned int g_4b0cc0;
extern unsigned int g_4b0cc4;
extern unsigned int g_4b0cc8;
extern unsigned int g_4b0ccc;
extern int g_4b0cd0;
extern int g_4b0cd4;
extern unsigned int g_4b0cd8;
extern unsigned int g_4b0cdc;
extern void g_4b0ce0;
extern unsigned int g_4b0cec;
extern unsigned int g_4b0cf0;
extern unsigned int g_4b0cf4;
extern unsigned int g_4b2ed0;
extern unsigned int g_4b30e8[4];
extern unsigned int g_4b30fc[4];
extern st_421cd0_21 *g_4b43e0;
extern st_421cd0_16 *g_4b43e4;
extern st_421cd0_1 *g_4b44e8;
extern unsigned int g_4b4508;
extern unsigned int g_4b450c;
extern st_4385b0_0 *g_4b4514;
extern unsigned int g_4b451c;
extern st_421cd0_19 *g_4b452c;
extern unsigned int g_4ce55c;
extern st_421cd0_12 *g_4ce8cc;
extern st_421cd0_13 *g_4ceaa4;
extern unsigned int g_4ceab0;
extern unsigned int g_4cee4c;
extern unsigned int g_4cee78;
extern unsigned int g_4cf0e4;
extern unsigned int g_4cf0e8;
extern unsigned int g_4d14d4;

unsigned int sub_421cd0(void)
{
    unsigned int v6;  // edi
    st_421cd0_1 *idx;  // edi
    st_421cd0_20 *v16;  // eax
    unsigned int v17;  // ecx
    st_421cd0_2 *idx1;  // eax
    st_421cd0_2 *idx2;  // esi
    unsigned int v20;  // edx
    st_421cd0_2 *index;  // eax
    st_421cd0_2 *v22;  // esi
    unsigned int v23;  // eax
    unsigned int iter;  // edi
    unsigned int v25;  // ecx
    unsigned int v8;  // ecx
    unsigned int *i;  // esi
    st_421cd0_17 *v27;  // ebx
    unsigned int v28;  // eax
    st_421cd0_1 *v29;  // esi
    st_421cd0_2 *v30;  // eax
    st_421cd0_2 *v31;  // eax
    st_421cd0_15 *v9;  // eax
    unsigned int v10;  // 4143
    unsigned int *v11;  // eax
    unsigned int v12;  // eax
    unsigned int v13;  // 4111
    int v14;  // eax
    unsigned int v15;  // ecx
    st_421cd0_16 *v0;  // [bp-0x2c]
    unsigned int v1;  // [bp-0x1c]
    char *v2;  // [bp-0x14]
    st_421cd0_1 *v3;  // [bp-0xc], Other Possible Types: unsigned int
    char v4;  // [bp-0x4]

    v2 = &v4;
    v1 = v6;
    idx = g_4b44e8;
    g_4b44e8->field_60 = g_4b44e8->field_60 | 4;
    v3 = g_4b44e8;
    if (g_4ce8cc->field_0 >= 0)
    {
        do
        {
            if (g_4cee78 & 384)
                goto LABEL_42217b;
        } while ((Sleep(1), g_4ce8cc->field_0 >= 0));
    }
    if (!g_4cee4c)
        Sleep(60);
    if (g_4ceaa4->field_494)
        g_4ceaa4->field_494();
    g_4ceaa4->field_3c4 = 2;
    sub_455630(g_4ceaa4);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4b0cbc = 0;
    g_4b0cc0 = 0;
    if (g_4cee4c)
    {
        if (*((int *)&g_4b0cb0) == 7)
        {
            v8 = 4;
            g_4b0ca8 = 4;
        }
        else
        {
            v8 = g_4b0ca8;
        }
        v9 = v8 * 280 + (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c;
        v10 = g_4b0ce0 & 8;
        g_4b0c40 = v9->field_18;
        g_4b0cc8 = v9->field_1d;
        if (!(char)v10)
            g_4b0cc4 = 0;
        g_4b0cdc = 0;
        g_4b0c44 = 0;
        g_4b0ca0 = 2;
        if (g_4b43e4)
        {
            v0 = g_4b43e4;
            sub_41cf40(g_4b43e4, 2, g_4b0ca4);
            v8 = g_4b0ca8;
        }
        g_4b0cf0 = g_4b30e8[v8] * 100;
        v11 = g_4b30fc[v8];
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
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b0ca4 = 0;
        g_4b0c54 = 0;
        g_4b0c50 = 0;
        g_4b0c4c = 0;
        g_4b0c58 = 0;
        g_4b0c5c = 0;
        g_4b0cf4 = v11 * 100;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v12 = sub_4931e0();
        v13 = g_4b0ce0 & 16;
        g_4b0c78 = v12;
        g_4b0c9c = 0;
        if (!(char)v13)
        {
            g_4b0c98 = 2;
        }
        else
        {
            if (!g_4b0cec)
                g_4b0c98 = 9;
            else
                g_4b0c98 = g_4b0cec - 1;
        }
        if (!sub_436410())
            goto LABEL_422180;
        if (*((int *)&g_4b0cb0) > 1 && *((int *)&g_4b0cb0) != 7)
        {
            v14 = g_4b0cd0;
            if (!(g_4b0ce0 & 8))
            {
                g_4b0c48 = 200;
                if (g_4b0cd0 < 200 || (v14 = g_4b0cd4, g_4b0cd4 > 200))
                    goto LABEL_421f20;
            }
            else
            {
                g_4b0c48 = 400;
                if (g_4b0cd0 < 400 || (v14 = g_4b0cd4, g_4b0cd4 > 400))
                    goto LABEL_421f20;
            }
        }
        else
        {
            v14 = g_4b0cd0;
            g_4b0c48 = 0;
            if (g_4b0cd0 < 0 || (v14 = g_4b0cd4, g_4b0cd4 > 0))
            {
LABEL_421f20:
                g_4b0c48 = v14;
            }
        }
        sub_4385b0(g_4b4514);
        *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) & 0xfffffffb;
        if (!g_4b44e8->field_74)
        {
            v15 = (g_4b0c94 + g_4b0c90 * 2) * 17908;
            v16 = v15 + g_4b451c + 1424;
            if (*((int *)(v15 + g_4b451c + 1424)) < 99999)
                *((unsigned int *)&v16->padding_0[0]) = *((int *)&v16->padding_0[0]) + 1;
        }
        v17 = -(0 < (g_4b0ce0 & 8));
        g_4b0cd8 = 0;
        g_4b0ccc = v17 & 0xfffffe00;
    }
    else if (g_4b0c44 > g_4b0c40)
    {
        g_4b0c40 = g_4b0c44;
    }
    if (!(g_4b0c8c & 1))
    {
        *((unsigned int *)&g_4b0c8c) = *((int *)&g_4b0c8c) | 1;
        g_4b0c88 = &g_4b2ed0;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b0c84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4b0c80 = 0;
    g_4b0c7c = 0xffffffff;
    idx1 = sub_46c9ea(36);
    if (idx1)
    {
        idx1->field_4 = idx1->field_4 & 0xfffffffe;
        idx1->field_8 = 0;
        idx1->field_c = 0;
        idx1->field_10 = 0;
        idx1->field_0 = 0;
        idx1->field_14 = idx1;
        idx1->field_18 = 0;
        idx1->field_1c = 0;
        idx2 = idx1;
    }
    else
    {
        idx2 = NULL;
    }
    v20 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_422bd0;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = g_4b44e8;
    idx2->field_4 = v20 | 1;
    sub_462380();
    g_4b44e8->field_8 = idx2;
    index = sub_46c9ea(36);
    if (index)
    {
        index->field_4 = index->field_4 & 0xfffffffe;
        index->field_8 = 0;
        index->field_c = 0;
        index->field_10 = 0;
        index->field_0 = 0;
        index->field_14 = index;
        index->field_18 = 0;
        index->field_1c = 0;
        v22 = index;
    }
    else
    {
        v22 = NULL;
    }
    v23 = v22->field_4 & 0xfffffffd;
    v22->field_8 = sub_422be0;
    v22->field_c = 0;
    v22->field_10 = 0;
    v22->field_20 = g_4b44e8;
    v22->field_4 = v23 | 1;
    sub_462420();
    g_4b44e8->field_c = v22;
    iter = &g_4b44e8->field_24;
    v25 = 15;
    for (i = &g_4ceab0; v25; i += 1)
    {
        v25 -= 1;
        *((unsigned int *)iter) = *(i);
        iter += 4;
    }
    v27->field_4 = g_4b452c->field_0;
    if (!(g_4b0ce0 & 2))
    {
        if (!sub_43b600(v27->field_74) || !sub_402f40(g_4b452c->field_4) || !sub_41df40() || !sub_4097f0() || !sub_425b10() || !sub_4281c0() || !sub_431fb0() || !sub_43dd50())
            goto LABEL_422177;
        v28 = sub_44a230();
    }
    else
    {
        sub_43c730();
        sub_41d3b0(g_4b43e4);
        v28 = sub_402f40(g_4b452c->field_4);
    }
    if (v28)
    {
        if (!(g_4b0ce0 & 9))
        {
            v0 = g_4b452c->field_c;
            if (sub_4130e0(g_4b452c->field_c))
                goto LABEL_42215c;
        }
        else
        {
            sub_421c60();
LABEL_42215c:
            if (sub_40fb00() && sub_406b20() && sub_40daa0())
            {
                if (!(g_4b0ce0 & 32))
                {
                    sub_430240();
                    sub_4300d0(0, g_4b452c->field_10);
                    sub_4300d0(1, g_4b452c->field_14);
                }
                /* unsupported instruction */
                /* unsupported instruction */
                *((long long *)((char *)&g_4b43e0->field_24 + 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                *((long long *)&(g_4b43e0->padding_0)[1]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_4067e0(0);
                if (g_4d14d4)
                {
                    do
                    {
                        Sleep(16);
                    } while (g_4d14d4);
                }
                if (g_4b0cb8)
                    g_4b0cc0 = 0;
                g_4b0cb8 = 0;
                sub_4306c0();
                v0->field_60 = v0->field_60 & 0xfffffffb;
                *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) & 0xfffffff4;
                g_4cf0e8 = 0;
                g_4cf0e4 = 1;
                g_4ce55c = 0;
                g_4b450c = 0;
                g_4b4508 = 0;
                sub_40e810();
                return 0;
            }
        }
    }
LABEL_422177:
    idx = v29;
LABEL_42217b:
LABEL_422180:
    idx->field_60 = idx->field_60 | 8;
    sub_430840();
    g_4cf0e8 = 0;
    g_4cf0e4 = 1;
    v30 = idx->field_8;
    if (v30)
        v30->field_4 = v30->field_4 | 2;
    v31 = idx->field_c;
    if (v31)
        v31->field_4 = v31->field_4 | 2;
    return 0xffffffff;
}
