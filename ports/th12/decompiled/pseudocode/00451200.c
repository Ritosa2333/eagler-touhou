// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451200; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_451200_0 {
    char padding_0[228];
    struct st_451200_1 *field_e4;
    char padding_e8[36];
    struct st_451200_1 *field_10c;
    char padding_110[4];
    struct st_451200_1 *field_114;
} st_451200_0;

typedef struct st_451200_1 {
    unsigned int field_0;
} st_451200_1;

typedef struct st_451200_4 {
    struct st_451200_0 *field_0;
} st_451200_4;

extern char g_4b563c;
extern char g_4b5640;
extern char g_4b5641;
extern char g_4b5642;
extern char g_4b5644;
extern unsigned int g_4ce8cc;
extern st_451200_4 *g_4ce8f0;

void sub_451200(void)
{
    unsigned int v8;  // ecx
    st_451200_0 **v0;  // [bp-0xe8]
    unsigned int v1;  // [bp-0xe4]
    unsigned int v2;  // [bp-0xe0]
    st_451200_0 **v3;  // [bp-0xdc], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0xd8]
    unsigned int v5;  // [bp-0xd4]
    unsigned int v6;  // [bp-0xd0]

    g_4ce8f0->field_0->field_e4(g_4ce8f0, 7, 1, v8);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 137, 0);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 22, 1);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 27, 1);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 9, 2);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 19, 5);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 20, 6);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 23, 8);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 171, 1);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 15, 1);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 24, 1);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 25, 7);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 28, 1);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 38, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 35, 0);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 140, 3);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 0x22, 0xffa0a0a0);
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
        v6 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v6 = nan;
        /* unsupported instruction */
    }
    v5 = v6;
    v4 = 36;
    v3 = g_4ce8f0;
    g_4ce8f0->field_0->field_e4();
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
    v2 = v3;
    v1 = 37;
    v0 = g_4ce8f0;
    g_4ce8f0->field_0->field_e4();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 161, 0);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 4);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 5, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 6, 3);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 4);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 2, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 3, 3);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 24, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 11, 0);
    g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 7, 0);
    g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 5, 2);
    g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 6, 2);
    g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 3, 3);
    g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 1, 1);
    g_4ce8f0->field_0->field_114(g_4ce8f0, 0, 2, 1);
    if (g_4ce8cc)
    {
        (&g_4b5640)[g_4ce8cc] = 7;
        (&g_4b5641)[g_4ce8cc] = 0xff;
        (&g_4b5642)[g_4ce8cc] = 0xff;
        *((unsigned int *)&(&g_4b563c)[g_4ce8cc]) = 0;
        (&g_4b5644)[g_4ce8cc] = 0xff;
    }
    return;
}
