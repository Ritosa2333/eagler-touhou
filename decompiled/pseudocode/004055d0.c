// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4055d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4055d0_0 {
    int field_0;
    unsigned short field_4;
    short field_6;
    unsigned int field_8;
} st_4055d0_0;

typedef struct st_4055d0_2 {
    unsigned int field_0;
} st_4055d0_2;

typedef struct st_4055d0_1 {
    char padding_0[28];
    struct st_4055d0_0 *field_1c;
    char padding_20[24];
    unsigned int field_38;
    unsigned int field_3c;
    unsigned int field_40;
    char padding_44[4];
    unsigned int field_48;
    struct st_4055d0_2 *field_4c;
} st_4055d0_1;

extern char g_4b2ed0;
extern st_4055d0_1 *g_4b43c0;

void sub_4055d0(unsigned int a0, unsigned int a1)
{
    st_4055d0_0 *v3;  // eax
    unsigned int v4;  // edi
    unsigned int *v5;  // eax
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = a0;
    v3 = g_4b43c0->field_1c;
    if (g_4b43c0->field_1c->field_0 < 0)
        return;
    v0 = v4;
    while (v3->field_4 != 16 || v3->field_8 != a1)
    {
        v3 = (char *)v3 + v3->field_6;
        if (v3->field_0 < 0)
            return;
    }
    v5 = (char *)v3 + v3->field_6;
    g_4b43c0->field_4c = v5;
    v6 = g_4b43c0->field_48;
    v1 = *(v5);
    if (!((char)v6 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b43c0->field_40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4b43c0->field_3c = 0;
        g_4b43c0->field_38 = 0xfff0bdc1;
        *((char **)&g_4b43c0->padding_44[0]) = &g_4b2ed0;
        g_4b43c0->field_48 = v6 | 1;
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
    g_4b43c0->field_3c = v1;
    g_4b43c0->field_38 = v1 - 1;
    if (/* unsupported instruction */)
    {
        g_4b43c0->field_40 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b43c0->field_40 = nan;
        /* unsupported instruction */
    }
    return;
}
