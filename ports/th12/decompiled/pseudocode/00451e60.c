// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451e60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_451e60_0 {
    char padding_0[64];
    struct st_451e60_1 *field_40;
    struct st_451e60_1 *field_44;
    char padding_48[100];
    struct st_451e60_1 *field_ac;
    char padding_b0[12];
    struct st_451e60_1 *field_bc;
} st_451e60_0;

typedef struct st_451e60_1 {
    unsigned int field_0;
} st_451e60_1;

typedef struct st_451e60_5 {
    struct st_451e60_0 *field_0;
} st_451e60_5;

extern unsigned int g_4ce8cc;
extern st_451e60_5 *g_4ce8f0;
extern unsigned int g_4ce9c4;
extern unsigned int g_4ce9c8;
extern unsigned int g_4ce9cc;
extern unsigned int g_4ce9d0;
extern unsigned int g_4ce9d4;
extern unsigned int g_4ce9d8;
extern char g_4ce9dc;

void sub_451e60(void)
{
    int v2;  // esi
    st_451e60_0 *v0;  // [bp-0x8]

    if (g_4ce8cc)
        sub_45a3c0(v2);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4ce9d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4ce9c4 = 0;
    /* unsupported instruction */
    /* unsupported instruction */
    g_4ce9c8 = 0;
    g_4ce9d8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4ce9cc = 640;
    g_4ce9d0 = 480;
    g_4ce8f0->field_0->field_bc(g_4ce8f0, &g_4ce9c4);
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = g_4ce8f0->field_0;
    /* unsupported instruction */
    g_4ce8f0->field_0->field_ac(g_4ce8f0, 0, 0, 3, 0xff000000, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0);
    if (g_4ce8f0->field_0->field_44(g_4ce8f0, 0, 0, 0, 0) < 0)
        g_4ce8f0->field_0->field_40(g_4ce8f0, &g_4ce9dc);
    /* unsupported instruction */
    /* unsupported instruction */
    v0 = g_4ce8f0->field_0;
    /* unsupported instruction */
    g_4ce8f0->field_0->field_ac(g_4ce8f0, 0, 0, 3, 0xff000000, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0);
    if (g_4ce8f0->field_0->field_44(g_4ce8f0, 0, 0, 0, 0) < 0)
        g_4ce8f0->field_0->field_40(g_4ce8f0, &g_4ce9dc);
    return;
}
