// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ea5c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b3c04;
extern unsigned int g_4d6458;

unsigned int sub_46ea5c(unsigned int a0)
{
    g_4b3c04 = HeapCreate(!a0, 0x1000, NULL);
    if (g_4b3c04)
    {
        g_4d6458 = 1;
        return 1;
    }
    return 0;
}
