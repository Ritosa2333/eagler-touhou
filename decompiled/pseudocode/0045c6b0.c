// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45c6b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45c6b0_1 {
    unsigned int field_0;
} st_45c6b0_1;

typedef struct st_45c6b0_0 {
    char padding_0[228];
    struct st_45c6b0_1 *field_e4;
    char padding_e8[36];
    struct st_45c6b0_1 *field_10c;
    char padding_110[60];
    struct st_45c6b0_1 *field_14c;
    char padding_150[20];
    struct st_45c6b0_1 *field_164;
} st_45c6b0_0;

typedef struct st_45c6b0_5 {
    struct st_45c6b0_0 *field_0;
} st_45c6b0_5;

extern char g_4b5640;
extern char g_4b5641;
extern char g_4b5642;
extern char g_4b5643;
extern char g_4b56a0;
extern unsigned int g_4ce8cc;
extern st_45c6b0_5 *g_4ce8f0;

int sub_45c6b0(void)
{
    unsigned int v2;  // eax
    unsigned int v0;  // [bp+0x4]
    unsigned int v1;  // [bp+0x8]

    if (*((int *)&(&g_4b56a0)[v2]))
        sub_45a3c0();
    if ((&g_4b5642)[v2] != 4)
    {
        g_4ce8f0->field_0->field_164(g_4ce8f0, 0x44);
        (&g_4b5642)[v2] = 4;
    }
    sub_459cf0();
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 5, 0);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 2, 0);
    sub_45a3c0();
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
    g_4ce8f0->field_0->field_14c(g_4ce8f0, 6, v1 - 2, v0, 20);
    (&g_4b5642)[g_4ce8cc] = 0xff;
    (&g_4b5641)[g_4ce8cc] = 0xff;
    (&g_4b5640)[g_4ce8cc] = 7;
    (&g_4b5643)[g_4ce8cc] = 0xff;
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 4);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 4);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 5, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 2, 2);
    return;
}
