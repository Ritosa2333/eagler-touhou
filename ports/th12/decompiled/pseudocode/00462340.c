// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462340; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462340_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_462340_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[4];
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    unsigned int field_30;
    unsigned int field_34;
    struct st_462340_0 *field_38;
    unsigned int field_3c;
    unsigned int field_40;
    char padding_44[4];
    unsigned int field_48;
} st_462340_0;


int sub_462340(void)
{
    st_462340_0 *idx;  // eax
    st_462340_0 *index;  // ecx

    idx->field_4 = idx->field_4 & 0xfffffffe;
    idx->field_8 = 0;
    idx->field_c = 0;
    idx->field_10 = 0;
    idx->field_0 = 0;
    idx->field_14 = idx;
    idx->field_18 = 0;
    idx->field_1c = 0;
    idx->field_28 = idx->field_28 & 0xfffffffe;
    index = &idx->field_24;
    index->field_8 = 0;
    index->field_c = 0;
    index->field_10 = 0;
    index->field_0 = 0;
    index->field_14 = index;
    index->field_18 = 0;
    index->field_1c = 0;
    idx->field_48 = 0;
    return;
}
