// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402520; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_402520_0 {
    char padding_0[4];
    struct st_402520_0 *field_4;
    unsigned int field_8;
    unsigned int field_c;
    struct st_402520_0 *field_10;
    unsigned int field_14;
    unsigned int field_18;
    char padding_1c[4];
    int field_20;
    char padding_24[28];
    unsigned int field_40;
    unsigned int field_44;
    char padding_48[8];
    unsigned int field_50;
    unsigned int field_54;
    char padding_58[16];
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[108];
    unsigned int field_e0;
    char padding_e4[72];
    unsigned int field_12c;
    char padding_130[40];
    unsigned int field_158;
    char padding_15c[72];
    unsigned int field_1a4;
    char padding_1a8[56];
    unsigned int field_1e0;
    char padding_1e4[56];
    unsigned int field_21c;
    char padding_220[72];
    unsigned int field_268;
    char padding_26c[40];
    unsigned int field_294;
    char padding_298[40];
    unsigned int field_2c0;
    char padding_2c4[40];
    unsigned int field_2ec;
    char padding_2f0[12];
    unsigned int field_2fc;
    unsigned int field_300;
    unsigned int field_304;
    unsigned int field_308;
    unsigned int field_30c;
    unsigned int field_310;
    unsigned int field_314;
    unsigned int field_318;
    unsigned int field_31c;
    unsigned int field_320;
    unsigned int field_324;
    unsigned int field_328;
    unsigned int field_32c;
    unsigned int field_330;
    unsigned int field_334;
    unsigned int field_338;
    char padding_33c[128];
    unsigned int field_3bc;
    char padding_3c0[112];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[64];
    unsigned short field_47c;
} st_402520_0;

void sub_402520(void)
{
    st_402520_0 *idx;  // esi
    unsigned int v6;  // ebx
    unsigned int v7;  // ebp
    int v0;  // [bp-0x1c]
    int v1;  // [bp-0x18]
    int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x4]

    v6 = idx->field_430;
    v7 = idx->field_434;
    v3 = idx->field_438;
    sub_477420(idx, 0, 1204, v0, v1, v2, idx->field_20);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_40 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_44 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_50 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_430 = v6;
    idx->field_54 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_434 = v7;
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_20 = idx->field_20;
    idx->field_438 = v3;
    idx->field_3bc = 0xffffffff;
    idx->field_334 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_330 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_32c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_328 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_320 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_31c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_318 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_314 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_30c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_308 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_304 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_300 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_338 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_324 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_310 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_2fc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_47c = 7;
    idx->field_6c = 0;
    idx->field_70 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_68 = 0xfff0bdc1;
    idx->field_e0 = 0;
    idx->field_12c = 0;
    idx->field_158 = 0;
    idx->field_1a4 = 0;
    idx->field_1e0 = 0;
    idx->field_21c = 0;
    idx->field_268 = 0;
    idx->field_294 = 0;
    idx->field_2c0 = 0;
    idx->field_2ec = 0;
    idx->field_8 = 0;
    idx->field_c = 0;
    idx->field_4 = idx;
    idx->field_14 = 0;
    idx->field_18 = 0;
    idx->field_10 = idx;
    return;
}
