// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4301a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ceacb;

unsigned int sub_4301a0(void)
{
    if (g_4ceacb == 2)
    {
        return 0;
    }
    else if (g_4ceacb == 1)
    {
        return 0;
    }
    else
    {
        return 0xffffffff;
    }
}
