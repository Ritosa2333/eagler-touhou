// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406bf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406bf0_1 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    char field_7c;
} st_406bf0_1;

typedef struct st_406bf0_0 {
    char padding_0[20];
    unsigned int field_14;
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[4];
    unsigned int field_24;
    char padding_28[20];
    unsigned int field_3c;
    char padding_40[1244];
    unsigned int field_51c;
} st_406bf0_0;

extern unsigned int g_406cc8[4];
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern char g_4b2ed0;
extern st_406bf0_0 *g_4b43c4;
extern st_406bf0_1 *g_4b43cc;

unsigned int sub_406bf0(void)
{
    unsigned int v2;  // eax
    unsigned int v0;  // [bp-0x8]

    if (g_4b43c4->field_3c)
        return 0xffffffff;
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b43c4->field_3c = 1;
    v2 = g_4b43c4->field_24;
    if (!((char)g_4b43c4->field_24 & 1))
    {
        if (/* unsupported instruction */)
            g_4b43c4->field_1c = /* unsupported instruction */;
        else
            g_4b43c4->field_1c = nan;
        g_4b43c4->field_18 = 0;
        g_4b43c4->field_14 = 0xfff0bdc1;
        *((char **)&g_4b43c4->padding_20[0]) = &g_4b2ed0;
        g_4b43c4->field_24 = v2 | 1;
    }
    if (/* unsupported instruction */)
    {
        g_4b43c4->field_1c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b43c4->field_1c = nan;
        /* unsupported instruction */
    }
    g_4b43c4->field_18 = 0;
    g_4b43c4->field_14 = 0xffffffff;
    if (g_4b43cc->field_7c & 1 && g_4b43cc->field_28 >= 60)
        g_4b43c4->field_51c = 1;
    else
        g_4b43c4->field_51c = 0;
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
    v0 = 1;
    if (/* unsupported instruction */)
    {
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    sub_453e20();
    if (g_4b0c94 + g_4b0c90 * 2 > 5)
        return 0;
    goto *((void *)(g_406cc8[2 * g_4b0c90 + g_4b0c94]));
}
