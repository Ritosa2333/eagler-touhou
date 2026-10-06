// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473e82; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d6434;

unsigned int sub_473e82(void)
{
    if (!g_4d6434)
    {
        sub_473ce8(-0x3);
        g_4d6434 = 1;
    }
    return 0;
}
