// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f130; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_49cd00;
extern int g_4ab5d0;

int sub_44f130(void)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // ecx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0x10]

    if ((unsigned int)(0xffffffff / v3) < 1)
    {
        v0 = 0;
        sub_46df5e(&v0, 0);
        v1 = &g_49cd00;
        sub_46ff8f(&v1, &g_4ab5d0);
    }
    return sub_46c9ea(v4);
}
