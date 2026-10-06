// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473736; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adac0;
extern unsigned int g_4b40dc;

int sub_473736(char *a0, char *a1[3], unsigned int a2)
{
    unsigned int v0;  // [bp-0x18]

    v0 = (!g_4b40dc ? &g_4adac0 : 0);
    return sub_473457((!g_4b40dc ? &g_4adac0 : 0), a0, a1, a2, 1);
}
