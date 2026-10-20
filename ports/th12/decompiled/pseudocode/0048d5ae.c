// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d5ae; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4ad9e4;
extern int g_4ad9f4;
extern int g_4adac0;
extern unsigned int g_4adea4;
extern unsigned int g_4b40dc;

unsigned int sub_48d5ae(int a0, int a1)
{
    if ((unsigned short)a0 == 0xffff)
    {
        return 0;
    }
    else if ((unsigned short)a0 < 0x100)
    {
        return *((short *)(g_4adea4 + (unsigned short)a0 * 2)) & (unsigned short)a1;
    }
    else
    {
        if (g_4b40dc)
            return sub_48d524(a0, a1, 0);
        sub_48ed72(&g_4adac0, 1, &a0, 1, &ecx, g_4ad9e4, g_4ad9f4);
        return sub_48d524(a0, a1, 0);
    }
}
