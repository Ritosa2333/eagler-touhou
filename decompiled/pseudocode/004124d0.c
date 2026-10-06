// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4124d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b43dc;

int sub_4124d0(unsigned int a0, unsigned int a1)
{
    *((unsigned int *)(g_4b43dc + a1 * 4 + 28)) = a0;
    return g_4b43dc;
}
