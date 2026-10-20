// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45c800; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45c800_1 {
    unsigned int field_0;
} st_45c800_1;

typedef struct st_45c800_9 {
    char padding_0[1012];
    struct st_45c800_7 *field_3f4;
} st_45c800_9;

typedef struct st_45c800_7 {
    char padding_0[8];
    struct st_45c800_1 *field_8;
} st_45c800_7;

typedef struct st_45c800_0 {
    char padding_0[228];
    struct st_45c800_1 *field_e4;
    char padding_e8[28];
    struct st_45c800_1 *field_104;
    char padding_108[4];
    struct st_45c800_1 *field_10c;
    char padding_110[60];
    struct st_45c800_1 *field_14c;
    char padding_150[20];
    struct st_45c800_1 *field_164;
} st_45c800_0;

typedef struct st_45c800_6 {
    struct st_45c800_0 *field_0;
} st_45c800_6;

extern char g_4b563c;
extern char g_4b5642;
extern char g_4b56a0;
extern st_45c800_6 *g_4ce8f0;

int sub_45c800(void)
{
    st_45c800_9 *v6;  // eax
    unsigned int *v7;  // eax
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    int v2;  // [bp-0x4]
    unsigned int idx;  // [bp+0x4]
    unsigned int v4;  // [bp+0x8]
    unsigned int v5;  // [bp+0xc]

    if (*((int *)&(&g_4b56a0)[idx]))
        sub_45a3c0(v0, v1, v2);
    if ((&g_4b5642)[idx] != 3)
    {
        g_4ce8f0->field_0->field_164(g_4ce8f0, 324);
        (&g_4b5642)[idx] = 3;
    }
    sub_459cf0();
    v7 = &v6->field_3f4->field_8->field_0;
    if (*((int *)&(&g_4b563c)[idx]) != v7)
    {
        *((unsigned int **)&(&g_4b563c)[idx]) = v7;
        g_4ce8f0->field_0->field_104(g_4ce8f0, 0, *(v7));
    }
    sub_45a3c0();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
    if ((&g_4b5642)[idx] != 1)
    {
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 6, 0);
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 3, 0);
        (&g_4b5642)[idx] = 1;
    }
    g_4ce8f0->field_0->field_14c(g_4ce8f0, 6, v5 - 2, v4, 28);
    return;
}
