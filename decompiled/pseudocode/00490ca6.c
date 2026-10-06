// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x490ca6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b40dc;

unsigned int sub_490ca6(int a0, int a1, int a2)
{
    int v0;  // [bp+0x0]

    if (g_4b40dc)
        return sub_490bac(a0, a1, a2, 0);
    return sub_48d94e(v0, a0, a1);
}
