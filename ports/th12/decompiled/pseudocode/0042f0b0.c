// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f0b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42f0b0_7 {
    char padding_0[8];
    struct st_42f0b0_3 *field_8;
    char padding_c[1344];
    struct st_42f0b0_7 *field_54c;
    unsigned int field_550;
} st_42f0b0_7;

typedef struct st_42f0b0_0 {
    char padding_0[148];
    struct st_42f0b0_1 *field_94;
    char padding_98[20];
    struct st_42f0b0_1 *field_ac;
} st_42f0b0_0;

typedef struct st_42f0b0_1 {
    unsigned int field_0;
} st_42f0b0_1;

typedef struct st_42f0b0_3 {
    struct st_42f0b0_0 *field_0;
} st_42f0b0_3;

typedef struct st_42f0b0_4 {
    char padding_0[176];
    unsigned int field_b0;
    unsigned int field_b4;
} st_42f0b0_4;

extern char g_4b563c;
extern char g_4b5640;
extern char g_4b5641;
extern char g_4b5642;
extern char g_4b5643;
extern char g_4b5644;
extern char g_4b5646;
extern char g_4b5647;
extern char g_4b5648;
extern st_42f0b0_4 *g_4ce8cc;
extern st_42f0b0_3 *g_4ce8f0;
extern unsigned int g_4cea94;
extern unsigned int g_4cede8;
extern unsigned int g_4cedec;
extern unsigned int g_4cedf0;
extern unsigned int g_4cedf4;
extern unsigned int g_4cf2a8;

unsigned int sub_42f0b0(st_42f0b0_7 *idx)
{
    st_42f0b0_0 **v17;  // eax
    unsigned int v18;  // esi
    unsigned int v19;  // edi
    st_42f0b0_0 *v20;  // ecx
    unsigned int v0;  // [bp-0x58]
    char *v1;  // [bp-0x54]
    unsigned int v2;  // [bp-0x50]
    unsigned int v3;  // [bp-0x4c]
    st_42f0b0_0 *v4;  // [bp-0x48], Other Possible Types: unsigned int
    unsigned int v5;  // [bp-0x44]
    unsigned int v6;  // [bp-0x38]
    unsigned int v7;  // [bp-0x34]
    unsigned int v8;  // [bp-0x30]
    unsigned int v9;  // [bp-0x2c]
    unsigned int v10;  // [bp-0x28]
    unsigned int v11;  // [bp-0x24]
    st_42f0b0_0 *v12;  // [bp-0x20], Other Possible Types: unsigned int
    unsigned int v13;  // [bp-0x1c]
    unsigned int v14;  // [bp-0x18]
    unsigned int v15;  // [bp-0x14]

    v17 = g_4ce8f0;
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = v18;
    v14 = v19;
    v20 = g_4ce8f0->field_0;
    v13 = 0;
    v12 = v20;
    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if (g_4cea94)
    {
        v20->field_ac(g_4ce8f0, 0, 0, 3, 0xffffffff);
        g_4ce8f0->field_0->field_94(g_4ce8f0, 0, g_4cea94);
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = g_4cede8;
        v9 = g_4cedf4 + g_4cedec;
        v17 = g_4ce8f0;
        v5 = 0;
        v8 = g_4cedf0 + g_4cede8;
        v7 = g_4cedec;
        v20 = g_4ce8f0->field_0;
        v4 = g_4ce8f0->field_0;
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v3 = g_4cf2a8;
        v2 = 3;
        v1 = &v6;
        v0 = 1;
    }
    else
    {
        v11 = g_4cf2a8;
        v10 = 3;
        v9 = 0;
        v8 = 0;
    }
    v20->field_ac(v17);
    /* unsupported instruction */
    /* unsupported instruction */
    g_4ce8cc->field_b4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    g_4ce8cc->field_b0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned int *)(g_4ce8cc + &g_4b5648)) = 0;
    *((unsigned int *)(g_4ce8cc + &g_4b563c)) = 0;
    *((unsigned int *)&g_4ce8cc[48769].padding_0[152]) = 0;
    *((char *)(g_4ce8cc + &g_4b5641)) = 0xff;
    *((char *)(g_4ce8cc + &g_4b5640)) = 7;
    *((char *)(g_4ce8cc + &g_4b5643)) = 0xff;
    *((char *)(g_4ce8cc + &g_4b5644)) = 0xff;
    *((unsigned int *)&g_4ce8cc[48769].padding_0[148]) = 0x80808080;
    *((char *)(g_4ce8cc + &g_4b5646)) = 0xff;
    *((char *)(g_4ce8cc + &g_4b5647)) = 0xff;
    *((char *)(g_4ce8cc + &g_4b5642)) = 0xff;
    idx->field_54c = &idx->padding_c[784];
    sub_430a70(v17);
    (*((int *)&idx->field_8->field_0[1].padding_0[12]))(idx->field_8, &idx->field_54c->padding_c[192]);
    idx->field_550 = 1;
    return 1;
}
