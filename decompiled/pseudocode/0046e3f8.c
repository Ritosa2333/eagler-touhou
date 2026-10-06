// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e3f8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b40dc;

unsigned int sub_46e3f8(int a0, int a1)
{
    if (g_4b40dc)
        return sub_46e323(a0, a1, 0);
    if (a0 && a1)
        return sub_46e2ea();
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 0x7fffffff;
}
