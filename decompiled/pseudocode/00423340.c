// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423340; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423340_0 {
    char padding_0[12];
    struct st_423340_0 *field_c;
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[84];
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[8];
    unsigned int field_7c;
    char padding_80[4];
    unsigned int field_84;
} st_423340_0;

st_423340_0 * sub_423340(void)
{
    st_423340_0 *idx;  // esi

    sub_477420(idx, 0, 0x88);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_c = idx;
    idx->field_10 = 0;
    idx->field_14 = 0;
    idx->field_7c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_84 = 0xffffffff;
    idx->field_70 = 0xffffffff;
    idx->field_6c = 300;
    return idx;
}
