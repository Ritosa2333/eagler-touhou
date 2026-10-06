// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ed39; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad2b0;
extern unsigned int g_4b3c04;

unsigned int sub_46ed39(unsigned int *a0)
{
    unsigned int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    char v2;  // [bp+0x0]

    if (a0 && g_4b3c04)
    {
        *(a0) = g_4ad2b0;
        return 0;
    }
    *((unsigned int *)sub_46e96f(v1, &v2)) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    v0 = 22;
    return 22;
}
