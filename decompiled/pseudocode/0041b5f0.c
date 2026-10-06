// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41b5f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41b5f0_1 {
    char padding_0[572];
    unsigned int field_23c;
    int field_240;
} st_41b5f0_1;

typedef struct st_41b5f0_2 {
    char padding_0[4744];
    int field_1288;
} st_41b5f0_2;

typedef struct st_41b5f0_3 {
    char padding_0[28];
    struct st_41b5f0_2 *field_1c;
} st_41b5f0_3;

typedef struct st_41b5f0_0 {
    char padding_0[102272];
    unsigned int field_18f80;
    unsigned int field_18f84;
    unsigned int field_18f88;
    char padding_18f8c[12];
    unsigned int field_18f98;
    unsigned int field_18f9c;
    char padding_18fa0[4];
    unsigned int field_18fa4;
    unsigned int field_18fa8;
} st_41b5f0_0;

extern int g_4b0c44;
extern st_41b5f0_0 *g_4b43b8;
extern st_41b5f0_3 *g_4b43dc;

unsigned int sub_41b5f0(st_41b5f0_1 *idx)
{
    unsigned int v6;  // edx
    unsigned int v0;  // [bp-0x24]
    unsigned int *v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    g_4b43b8->field_18f98 = 5;
    if (idx->field_23c == 1)
    {
        idx->field_240 = g_4b43dc->field_1c->field_1288;
        g_4b43dc->field_1c->field_1288 = g_4b43dc->field_1c->field_1288 + 1;
        if (g_4b43dc->field_1c->field_1288 >= 15)
            g_4b43dc->field_1c->field_1288 = 0;
        v6 = 1717986919 * *((int *)(4 * idx->field_240 + 4927304)) >> 0x22;
        g_4b0c44 = g_4b0c44 + (v6 >> 31) + v6;
        if (g_4b0c44 >= 1000000000)
            g_4b0c44 = 0x3b9ac9ff;
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
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v2 = nan;
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
    g_4b43b8->field_18f80 = 0xd0ffffff;
    if (/* unsupported instruction */)
    {
        v4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v4 = nan;
        /* unsupported instruction */
    }
    g_4b43b8->field_18fa4 = 0;
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
    g_4b43b8->field_18fa8 = 0;
    if (/* unsupported instruction */)
        g_4b43b8->field_18f84 = /* unsupported instruction */;
    else
        g_4b43b8->field_18f84 = nan;
    g_4b43b8->field_18f9c = 1;
    if (/* unsupported instruction */)
    {
        g_4b43b8->field_18f88 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b43b8->field_18f88 = nan;
        /* unsupported instruction */
    }
    v1 = *((int *)(4 * idx->field_240 + 4927304));
    v0 = "%d";
    sub_4015c0();
    /* unsupported instruction */
    /* unsupported instruction */
    g_4b43b8->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4b43b8->field_18f88 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4b43b8->field_18fa4 = 1;
    g_4b43b8->field_18fa8 = 1;
    g_4b43b8->field_18f98 = 0;
    g_4b43b8->field_18f9c = 0;
    g_4b43b8->field_18f80 = 0xffffffff;
    return 0;
}
