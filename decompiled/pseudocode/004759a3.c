// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4759a3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adaa8;
extern unsigned int g_4b40dc;

unsigned int sub_4759a3(int a0, int a1)
{
    if (g_4b40dc)
        return sub_4758eb(a0, a1, 0);
    return *((short *)(g_4adaa8 + a0 * 2)) & a1;
}
