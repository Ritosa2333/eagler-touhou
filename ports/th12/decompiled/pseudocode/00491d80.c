// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491d80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_491d80_0 {
    unsigned int field_0;
    struct st_491d80_0 *field_4;
} st_491d80_0;


unsigned int sub_491d80(unsigned int a0)
{
    st_491d80_0 *iter;  // eax
    char v0;  // [bp+0x0]

    for (iter = *((int *)(sub_475467(&v0) + 152)); iter; iter = iter->field_4)
    {
        if (iter->field_0 == a0)
            return 0;
    }
    return 1;
}
