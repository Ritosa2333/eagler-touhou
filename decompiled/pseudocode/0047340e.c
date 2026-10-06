// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47340e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adac0;
extern unsigned int g_4b40dc;

void sub_47340e(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]

    v4 = 1;
    v3 = a2;
    v2 = a1;
    v1 = a0;
    v0 = (!g_4b40dc ? &g_4adac0 : 0);
    sub_473197((!g_4b40dc ? &g_4adac0 : 0));
    return;
}
