// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4777b9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b41d0;

unsigned int sub_4777b9(unsigned int a0)
{
    unsigned int v0;  // eax

    v0 = g_4b41d0;
    g_4b41d0 = a0;
    return v0;
}
