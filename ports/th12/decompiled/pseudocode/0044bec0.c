// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44bec0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44bec0_0 {
    char padding_0[8];
    unsigned int field_8;
} st_44bec0_0;

unsigned int sub_44bec0(st_44bec0_0 *a0, unsigned int a1, unsigned int a2, unsigned int a3)
{
    st_44bec0_0 *v0;  // [bp-0x4], Other Possible Types: unsigned int

    v0 = a0;
    v0 = 0;
    if (a0->field_8 == 0x80000000)
        return sub_44bee4(0);
    return 0;
}
