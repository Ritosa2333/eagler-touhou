// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x432850; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_432850_1 {
    unsigned int field_0;
} st_432850_1;

typedef struct st_432850_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[8];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    struct st_432850_1 *field_1c;
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
} st_432850_0;

typedef struct st_432850_3 {
    char padding_0[96];
    unsigned int field_60;
} st_432850_3;

typedef struct st_432850_2 {
    char padding_0[27972];
    unsigned int field_6d44;
} st_432850_2;

extern unsigned int g_4b2ed0;
extern st_432850_2 *g_4b43e4;
extern st_432850_3 *g_4b44e8;
extern st_432850_0 *g_4b4510;
extern unsigned int g_4cf468;

void sub_432850(unsigned int a0)
{
    unsigned int v6;  // ebx
    unsigned int v7;  // eax
    unsigned int v8;  // edi
    unsigned int v9;  // eax
    int *v10;  // eax
    int v11;  // eax
    int *v12;  // eax
    int v13;  // eax
    char *v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    v4 = a0;
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = v6;
    g_4b4510->field_4 = 2;
    v7 = g_4b4510->field_20;
    v2 = v8;
    if (!((char)v7 & 1))
    {
        if (/* unsupported instruction */)
            g_4b4510->field_18 = /* unsupported instruction */;
        else
            g_4b4510->field_18 = nan;
        g_4b4510->field_14 = 0;
        g_4b4510->field_10 = 0xfff0bdc1;
        g_4b4510->field_1c = &g_4b2ed0;
        g_4b4510->field_20 = v7 | 1;
    }
    if (/* unsupported instruction */)
        g_4b4510->field_18 = /* unsupported instruction */;
    else
        g_4b4510->field_18 = nan;
    g_4b4510->field_14 = 0;
    g_4b4510->field_10 = 0xffffffff;
    v9 = g_4b4510->field_34;
    if (!((char)g_4b4510->field_34 & 1))
    {
        if (/* unsupported instruction */)
            g_4b4510->field_2c = /* unsupported instruction */;
        else
            g_4b4510->field_2c = nan;
        g_4b4510->field_28 = 0;
        g_4b4510->field_24 = 0xfff0bdc1;
        *((unsigned int **)&g_4b4510->padding_30[0]) = &g_4b2ed0;
        g_4b4510->field_34 = v9 | 1;
    }
    if (/* unsupported instruction */)
    {
        g_4b4510->field_2c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b4510->field_2c = nan;
        /* unsupported instruction */
    }
    g_4b4510->field_28 = 0;
    g_4b4510->field_24 = 0xffffffff;
    g_4b44e8->field_60 = g_4b44e8->field_60 | 16;
    v1 = 75;
    v0 = &v4;
    v10 = sub_4357c0();
    v11 = *(v10);
    g_4b4510->field_1ec = *(v10);
    sub_435750(v11);
    g_4b4510->field_2dc = g_4b43e4->field_6d44;
    v12 = sub_4357c0(&esi, 104);
    v13 = *(v12);
    g_4b4510->field_1e8 = *(v12);
    sub_461970(v13);
    sub_423020(v13);
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
        g_4b4510->field_2d8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b4510->field_2d8 = nan;
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4b4510->field_200 = g_4cf468;
    g_4cf468 = 0;
    return;
}
