// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x409400; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_409400_0 {
    char padding_0[520];
    unsigned int field_208;
} st_409400_0;

st_409400_0 * sub_409400(st_409400_0 *a0)
{
    int v0;  // [bp-0x4]

    sub_477420(a0, 0, 532, v0);
    a0->field_208 = 0xffffffff;
    return a0;
}
