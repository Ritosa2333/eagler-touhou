// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451f30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_451f30_1 {
    unsigned int field_0;
} st_451f30_1;

typedef struct st_451f30_0 {
    char padding_0[228];
    struct st_451f30_1 *field_e4;
    char padding_e8[36];
    struct st_451f30_1 *field_10c;
    char padding_110[60];
    struct st_451f30_1 *field_14c;
    char padding_150[20];
    struct st_451f30_1 *field_164;
} st_451f30_0;

typedef struct st_451f30_5 {
    struct st_451f30_0 *field_0;
} st_451f30_5;

extern char g_4b563c;
extern char g_4b5640;
extern char g_4b5641;
extern char g_4b5642;
extern char g_4b5643;
extern char g_4b5648;
extern unsigned int g_4ce8cc;
extern st_451f30_5 *g_4ce8f0;

void sub_451f30(void)
{
    unsigned int v25;  // ebx
    int v0;  // [bp-0x60]
    unsigned int v1;  // [bp-0x5c]
    unsigned int v2;  // [bp-0x58]
    unsigned int v3;  // [bp-0x54]
    unsigned int v4;  // [bp-0x50]
    unsigned int v5;  // [bp-0x4c]
    unsigned int v6;  // [bp-0x48]
    unsigned int v7;  // [bp-0x44]
    unsigned int v8;  // [bp-0x40]
    unsigned int v9;  // [bp-0x3c]
    unsigned int v10;  // [bp-0x38]
    unsigned int v11;  // [bp-0x34]
    unsigned int v12;  // [bp-0x30]
    unsigned int v13;  // [bp-0x2c]
    unsigned int v14;  // [bp-0x28]
    unsigned int v15;  // [bp-0x24]
    unsigned int v16;  // [bp-0x20]
    unsigned int v17;  // [bp-0x1c]
    unsigned int v18;  // [bp-0x18]
    unsigned int v19;  // [bp-0x14]
    unsigned int v20;  // [bp-0x10]
    unsigned int v21;  // [bp-0xc]
    unsigned int v22;  // [bp-0x8]
    unsigned int v23;  // [bp-0x4]

    sub_45a3c0(v0);
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = v1;
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = v2;
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v6 = v3;
    /* unsupported instruction */
    /* unsupported instruction */
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v9 = v1;
    v10 = v2;
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    v11 = v3;
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v14 = v1;
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v15 = v2;
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    v16 = v3;
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v19 = v1;
    v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v21 = v3;
    v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v20 = v2;
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v23 = v25;
    v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v18 = v25;
    v13 = v25;
    v8 = v25;
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 5, 0);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 2, 0);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 20, 6);
    g_4ce8f0->field_0->field_164(g_4ce8f0, 0x44);
    g_4ce8f0->field_0->field_14c(g_4ce8f0, 5, 2, &v4, 20);
    (&g_4b5642)[g_4ce8cc] = 0xff;
    *((unsigned int *)&(&g_4b5648)[g_4ce8cc]) = 0;
    *((unsigned int *)&(&g_4b563c)[g_4ce8cc]) = 0;
    (&g_4b5641)[g_4ce8cc] = 0xff;
    (&g_4b5640)[g_4ce8cc] = 7;
    (&g_4b5643)[g_4ce8cc] = 0xff;
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 4);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 4);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 5, 2);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 2, 2);
    return;
}
