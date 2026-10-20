// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x411e40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_411e40_0 {
    char padding_0[4];
    struct st_411e40_0 *field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[4104];
    unsigned int field_1018;
    struct st_411e40_0 *field_101c;
    unsigned int field_1020;
    char padding_1024[4];
    unsigned int field_1028;
    char padding_102c[4];
    struct st_411e40_0 *field_1030;
    unsigned int field_1034;
    unsigned int field_1038;
} st_411e40_0;

int sub_411e40(void)
{
    st_411e40_0 *idx;  // eax
    st_411e40_0 *v2;  // ecx

    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_1028 = idx->field_1028 & 0xfffffffe;
    idx->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v2 = &idx->field_8;
    idx->field_c = 0;
    idx->field_101c = idx;
    idx->field_1018 = 0xffffffff;
    idx->field_1020 = 0;
    idx->field_4 = v2;
    idx->field_1030 = v2;
    idx->field_1034 = 0;
    idx->field_1038 = 0;
    return;
}
