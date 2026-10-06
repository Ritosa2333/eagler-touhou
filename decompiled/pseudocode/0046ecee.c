// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ecee; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad2b0;
extern unsigned int g_4b3c04;

unsigned int sub_46ecee(unsigned int a0)
{
    unsigned int *v1;  // eax
    unsigned int v0;  // [bp-0xc]

    if (a0 - 1 > 0xfffffffe)
    {
        v1 = sub_46e96f();
    }
    else if (!g_4b3c04)
    {
        v1 = sub_46e96f();
    }
    else
    {
        g_4ad2b0 = a0;
        return 0;
    }
    *(v1) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    v0 = 22;
    return 22;
}
