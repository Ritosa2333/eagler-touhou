// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44f0a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_49cd00;
extern int g_4ab5d0;

int sub_44f0a0(unsigned int a0)
{
    unsigned int v1;  // ecx
    unsigned int v0;  // [bp-0x10]

    v1 = a0;
    if (!v1)
    {
        v1 = 0;
    }
    else if ((unsigned int)(0xffffffff / v1) < 1)
    {
        v0 = &a0;
        a0 = 0;
        sub_46df5e(&a0);
        sub_46ff8f(&v0, &g_4ab5d0, &g_49cd00);
        __debugbreak();
    }
    return sub_46c9ea(v1);
}
