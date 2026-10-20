// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423120; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423120_0 {
    char padding_0[160];
    unsigned int field_a0;
} st_423120_0;

int sub_423120(void)
{
    st_423120_0 *idx;  // eax
    unsigned int v0;  // [bp+0x4]

    idx->field_a0 = idx->field_a0 ^ (idx->field_a0 ^ v0) & 1;
    return;
}
