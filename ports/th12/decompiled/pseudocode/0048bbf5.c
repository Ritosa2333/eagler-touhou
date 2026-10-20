// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48bbf5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adaa8;
extern unsigned int g_4b40dc;

int sub_48bbf5(int a0)
{
    if (g_4b40dc)
        return sub_48bba4(a0, 0);
    return *((short *)(g_4adaa8 + a0 * 2)) & 16;
}
