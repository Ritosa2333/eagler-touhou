// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462310; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462310_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_462310_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
} st_462310_0;

int sub_462310(void)
{
    st_462310_0 *idx;  // eax
    unsigned int v2;  // ecx

    idx->field_4 = idx->field_4 & 0xfffffffe;
    idx->field_8 = v2;
    idx->field_c = 0;
    idx->field_10 = 0;
    idx->field_0 = 0;
    idx->field_14 = idx;
    idx->field_18 = 0;
    idx->field_1c = 0;
    return;
}
