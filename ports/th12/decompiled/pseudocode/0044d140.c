// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d140; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44d140_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[4];
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[4];
    int field_18;
    char padding_1c[4];
    unsigned int field_20;
    char padding_24[40];
    unsigned int field_4c;
} st_44d140_0;

int sub_44d140(void)
{
    st_44d140_0 *idx;  // eax
    int v0;  // [bp-0x4]

    idx->field_20 = idx->field_20 & 0xfffffff8;
    idx->field_4c = 0;
    sub_464c40(v0);
    idx->field_18 = sub_44d250;
    idx->field_10 = 1;
    idx->field_c = 0;
    idx->field_4 = sub_46e54b(0, 0, sub_44d250, 0, 0, idx->padding_8);
    return;
}
