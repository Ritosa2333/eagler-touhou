// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4921d6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4921d6_0 {
    unsigned int field_0;
    int field_4;
    unsigned int field_8;
} st_4921d6_0;


unsigned int sub_4921d6(unsigned int a0, st_4921d6_0 *idx)
{
    unsigned int v0;  // eax

    v0 = idx->field_0 + a0;
    if (idx->field_4 >= 0)
        v0 += *((int *)(*((int *)(idx->field_4 + a0)) + idx->field_8)) + idx->field_4;
    return v0;
}
