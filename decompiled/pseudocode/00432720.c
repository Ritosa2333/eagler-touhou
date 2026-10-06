// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x432720; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_432720_1 {
    unsigned int field_0;
} st_432720_1;

typedef struct st_432720_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[8];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    struct st_432720_1 *field_1c;
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    char padding_30[4];
    unsigned int field_34;
    char padding_38[432];
    int field_1e8;
    int field_1ec;
    char padding_1f0[16];
    unsigned int field_200;
    char padding_204[212];
    unsigned int field_2d8;
    unsigned int field_2dc;
} st_432720_0;

typedef struct st_432720_3 {
    char padding_0[96];
    unsigned int field_60;
} st_432720_3;

typedef struct st_432720_2 {
    char padding_0[27972];
    unsigned int field_6d44;
} st_432720_2;

extern unsigned int g_4b2ed0;
extern st_432720_2 *g_4b43e4;
extern st_432720_3 *g_4b44e8;
extern unsigned int g_4cf468;

void sub_432720(unsigned int a0)
{
    unsigned int v7;  // ebx
    st_432720_0 *idx;  // esi
    unsigned int v9;  // eax
    unsigned int v10;  // eax
    int *v11;  // eax
    int v12;  // eax
    unsigned int v0;  // [bp-0x24]
    int v1;  // [bp-0x20]
    char *v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    v5 = a0;
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = v7;
    idx->field_4 = 1;
    v9 = idx->field_20;
    if (!((char)v9 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_18 = /* unsupported instruction */;
        else
            idx->field_18 = nan;
        idx->field_14 = 0;
        idx->field_10 = 0xfff0bdc1;
        idx->field_1c = &g_4b2ed0;
        idx->field_20 = v9 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_18 = /* unsupported instruction */;
    else
        idx->field_18 = nan;
    idx->field_14 = 0;
    idx->field_10 = 0xffffffff;
    v10 = idx->field_34;
    if (!((char)idx->field_34 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_2c = /* unsupported instruction */;
        else
            idx->field_2c = nan;
        idx->field_28 = 0;
        idx->field_24 = 0xfff0bdc1;
        *((unsigned int **)&idx->padding_30[0]) = &g_4b2ed0;
        idx->field_34 = v10 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_2c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_2c = nan;
        /* unsupported instruction */
    }
    idx->field_28 = 0;
    idx->field_24 = 0xffffffff;
    g_4b44e8->field_60 = g_4b44e8->field_60 | 16;
    v3 = 75;
    v2 = &v5;
    v11 = sub_4357c0();
    v12 = *(v11);
    idx->field_1ec = *(v11);
    sub_435750(v12);
    idx->field_2dc = g_4b43e4->field_6d44;
    if (*((int *)&g_4b44e8[1].padding_0[16]))
    {
        v1 = 103;
        v0 = &edi;
        idx->field_1e8 = *((int *)sub_4357c0(&edi, 103));
    }
    else
    {
        v1 = 0x66;
        v0 = &edi;
        idx->field_1e8 = *((int *)sub_4357c0(&edi, 0x66));
    }
    sub_461970(idx->field_1e8, v0);
    sub_423020(v1);
    sub_453d90();
    sub_454960(6, 0);
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
        idx->field_2d8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_2d8 = nan;
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_200 = g_4cf468;
    g_4cf468 = 0;
    return;
}
