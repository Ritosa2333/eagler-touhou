// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462970; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4627e0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_4627e0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
} st_4627e0_0;


int sub_462970(void)
{
    unsigned int v1;  // eax
    st_4627e0_0 *idx;  // esi
    unsigned int v0;  // [bp+0x4]

    idx = sub_4627e0(v1);
    idx->field_4 = idx->field_4 & 0xfffffffd;
    idx[1].field_0 = v0;
    sub_462380();
    return;
}
