// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4381e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4381e0_0 {
    char padding_0[2600];
    unsigned int field_a28;
    char padding_a2c[4];
    unsigned int field_a30;
    unsigned int field_a34;
    unsigned int field_a38;
    char padding_a3c[4];
    unsigned int field_a40;
    char padding_a44[30924];
    int field_8310;
    int field_8314;
    char padding_8318[16616];
    unsigned int field_c400;
    unsigned int field_c404;
    unsigned int field_c408;
    char padding_c40c[4];
    unsigned int field_c410;
} st_4381e0_0;

typedef struct st_4381e0_1 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_4381e0_1;

typedef struct st_4381e0_2 {
    char padding_0[60];
    unsigned int field_3c;
} st_4381e0_2;

extern int g_4b0c98;
extern int g_4b0c9c;
extern int g_4b0ccc;
extern char g_4b2ed0;
extern st_4381e0_2 *g_4b43c4;
extern st_4381e0_1 *g_4b43cc;
extern int g_4b43e4;

void sub_4381e0(void)
{
    unsigned int v9;  // ecx
    unsigned int v10;  // ebx
    int v11;  // 4102
    unsigned int v12;  // edi
    st_4381e0_0 *idx;  // esi
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    st_4381e0_0 *iter;  // edi
    unsigned int v17;  // eax
    unsigned int v18;  // eax
    unsigned int v0;  // [bp-0x28]
    unsigned int i;  // [bp-0x28]
    st_4381e0_0 *v2;  // [bp-0x24]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x18]
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp-0x4]

    v7 = v9;
    v6 = v10;
    v11 = g_4b0c98;
    g_4b0c98 = g_4b0c98 - 1;
    v5 = v12;
    if (v11 - 1 >= 0)
        sub_41ce60(g_4b43e4, g_4b0c98, g_4b0c9c);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_a28 = 2;
    v14 = idx->field_a40;
    if (!((char)idx->field_a40 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_a38 = /* unsupported instruction */;
        else
            idx->field_a38 = nan;
        idx->field_a34 = 0;
        idx->field_a30 = 0xfff0bdc1;
        *((char **)&idx->padding_a3c[0]) = &g_4b2ed0;
        idx->field_a40 = v14 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_a38 = /* unsupported instruction */;
    else
        idx->field_a38 = nan;
    idx->field_a34 = 0;
    idx->field_a30 = 0xffffffff;
    v15 = idx->field_c410;
    if (!((char)idx->field_c410 & 1))
    {
        if (/* unsupported instruction */)
        {
            idx->field_c408 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_c408 = nan;
            /* unsupported instruction */
        }
        idx->field_c404 = 0;
        idx->field_c400 = 0xfff0bdc1;
        *((char **)&idx->padding_c40c[0]) = &g_4b2ed0;
        idx->field_c410 = v15 | 1;
    }
    else
    {
        /* unsupported instruction */
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
    v3 = 0;
    if (/* unsupported instruction */)
    {
        idx->field_c408 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_c408 = nan;
        /* unsupported instruction */
    }
    idx->field_c404 = 180;
    idx->field_c400 = 179;
    v2 = &idx->padding_0[20];
    sub_454d10();
    iter = &idx->field_8310;
    v4 = 8;
    do
    {
        i = v0;
        *((unsigned int *)(&iter->padding_0[0] - 176)) = 0;
        sub_461970(*((int *)&iter->padding_0[0]));
        sub_461970(*((int *)&iter->padding_0[4]));
        iter = &iter->padding_0[228];
        v0 = i - 1;
    } while (i != 1);
    *((unsigned int *)&idx[1].padding_0[8]) = 0;
    v17 = g_4b43cc->field_7c;
    if ((char)v17 & 1)
    {
        if (g_4b43cc->field_28 >= 60)
        {
            g_4b43cc->field_80 = 0;
            v18 = v17 & 0xffffffdd;
        }
        else
        {
            if (!g_4b43c4->field_3c)
                goto LABEL_438328;
            v18 = v17 | 32;
        }
        g_4b43cc->field_7c = v18;
    }
LABEL_438328:
    g_4b0ccc = g_4b0ccc - 0x400;
    if (g_4b0ccc > 0x400)
    {
        g_4b0ccc = 0x400;
        return;
    }
    if (g_4b0ccc >= -0x400)
        return;
    g_4b0ccc = 0xfffffc00;
    return;
}
