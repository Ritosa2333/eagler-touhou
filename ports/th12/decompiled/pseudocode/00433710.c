// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x433710; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_433710_2 {
    unsigned int field_0;
} st_433710_2;

typedef struct st_433710_4 {
    char padding_0[125419];
    char field_1e9eb;
} st_433710_4;

typedef struct st_433710_1 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[8];
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    struct st_433710_2 *field_1c;
    unsigned int field_20;
    char padding_24[8];
    unsigned int field_2c;
    char padding_30[4];
    unsigned int field_34;
    char padding_38[436];
    int field_1ec;
    char padding_1f0[4];
    unsigned int field_1f4;
    char padding_1f8[8];
    unsigned int field_200;
    char padding_204[212];
    unsigned int field_2d8;
    unsigned int field_2dc;
} st_433710_1;

typedef struct st_433710_0 {
    char padding_0[96];
    unsigned int field_60;
    char padding_64[16];
    unsigned int field_74;
} st_433710_0;

typedef struct st_433710_3 {
    char padding_0[27972];
    unsigned int field_6d44;
} st_433710_3;

extern char g_4b0ce0;
extern unsigned int g_4b2ed0;
extern st_433710_3 *g_4b43e4;
extern st_433710_0 *g_4b44e8;
extern st_433710_1 *g_4b4510;
extern st_433710_4 *g_4b451c;
extern char g_4ceae8;
extern char g_4cee40;
extern unsigned int g_4cee78;
extern unsigned int g_4cf468;

void sub_433710(void)
{
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    int *v6;  // eax
    int v7;  // eax
    char *v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    char v2;  // [bp-0x4]

    if (g_4b44e8->field_74 == 1)
    {
        *((unsigned int *)&g_4cee40) = (-(0 < (g_4cee78 & 0x2000)) & 0xfffffffe) + 4;
        return;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b4510->field_4 = (g_4b0ce0 & 16) * 8 + 13;
    v4 = g_4b4510->field_20;
    if (!((char)g_4b4510->field_20 & 1))
    {
        if (/* unsupported instruction */)
            g_4b4510->field_18 = /* unsupported instruction */;
        else
            g_4b4510->field_18 = nan;
        g_4b4510->field_14 = 0;
        g_4b4510->field_10 = 0xfff0bdc1;
        g_4b4510->field_1c = &g_4b2ed0;
        g_4b4510->field_20 = v4 | 1;
    }
    if (/* unsupported instruction */)
        g_4b4510->field_18 = /* unsupported instruction */;
    else
        g_4b4510->field_18 = nan;
    g_4b4510->field_14 = 0;
    g_4b4510->field_10 = 0xffffffff;
    v5 = g_4b4510->field_34;
    if (!((char)g_4b4510->field_34 & 1))
    {
        if (/* unsupported instruction */)
            g_4b4510->field_2c = /* unsupported instruction */;
        else
            g_4b4510->field_2c = nan;
        *((unsigned int *)&g_4b4510->padding_24[4]) = 0;
        *((unsigned int *)&g_4b4510->padding_24[0]) = 0xfff0bdc1;
        *((unsigned int **)&g_4b4510->padding_30[0]) = &g_4b2ed0;
        g_4b4510->field_34 = v5 | 1;
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
    *((unsigned int *)&g_4b4510->padding_24[4]) = 0;
    *((unsigned int *)&g_4b4510->padding_24[0]) = 0xffffffff;
    g_4b44e8->field_60 = g_4b44e8->field_60 | 16;
    v1 = 75;
    v0 = &v2;
    v6 = sub_4357c0();
    v7 = *(v6);
    g_4b4510->field_1ec = *(v6);
    sub_435750(v7);
    g_4b4510->field_2dc = g_4b43e4->field_6d44;
    sub_423020(g_4b43e4);
    sub_4300d0(0, "bgm/th10_17.wav");
    if (g_4ceae8 & 16)
        sub_454960(4, 0);
    sub_454960(2, 0);
    g_4b451c->field_1e9eb = 1;
    g_4b4510->field_1f4 = 0;
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
    g_4cf468 = 1;
    return;
}
