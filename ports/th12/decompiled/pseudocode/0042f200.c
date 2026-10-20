// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f200; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42f200_0 {
    char padding_0[148];
    struct st_42f200_1 *field_94;
    char padding_98[20];
    struct st_42f200_1 *field_ac;
} st_42f200_0;

typedef struct st_42f200_1 {
    unsigned int field_0;
} st_42f200_1;

typedef struct st_42f200_3 {
    struct st_42f200_0 *field_0;
} st_42f200_3;

extern st_42f200_3 *g_4ce8f0;
extern unsigned int g_4cea94;
extern unsigned int g_4cea98;
extern unsigned int g_4cede8;
extern unsigned int g_4cedec;
extern unsigned int g_4cedf0;
extern unsigned int g_4cedf4;
extern unsigned int g_4cf2a8;

unsigned int sub_42f200(void)
{
    int v6;  // esi
    st_42f200_0 *v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp-0x4]

    if (!g_4cea94)
        return 1;
    sub_45a3c0(v6);
    g_4ce8f0->field_0->field_94(g_4ce8f0, 0, g_4cea98);
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = g_4cede8;
    v4 = g_4cedf4 + g_4cedec;
    v3 = g_4cedf0 + g_4cede8;
    v2 = g_4cedec;
    v0 = g_4ce8f0->field_0;
    /* unsupported instruction */
    g_4ce8f0->field_0->field_ac(g_4ce8f0, 1, &v1, 3, g_4cf2a8, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0);
    return 1;
}
