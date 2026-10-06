// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464cb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464cb0_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[4];
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[4];
    int field_18;
} st_464cb0_0;


int sub_464cb0(void)
{
    st_464cb0_0 *idx;  // eax
    int v3;  // edi
    int v0;  // [bp-0x4]
    int v1;  // [bp+0x4]

    sub_464c40(v0);
    idx->field_18 = v3;
    idx->field_10 = 1;
    idx->field_c = 0;
    idx->field_4 = sub_46e54b(0, 0, v3, v1, 0, idx->padding_8);
    return;
}
