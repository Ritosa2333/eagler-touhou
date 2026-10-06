// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47a10a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adba0;

void sub_47a10a(unsigned int a0, unsigned int a1)
{
    g_4adba0 = ~(a1) & g_4adba0 | a0 & a1;
    return;
}
