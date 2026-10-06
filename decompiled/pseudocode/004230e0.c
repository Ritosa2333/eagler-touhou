// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4230e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4230e0_0 {
    char padding_0[160];
    unsigned int field_a0;
} st_4230e0_0;

int sub_4230e0(void)
{
    st_4230e0_0 *idx;  // eax
    unsigned int v2;  // ecx

    idx->field_a0 = idx->field_a0 ^ (v2 * 8 ^ idx->field_a0) & 8;
    return;
}
