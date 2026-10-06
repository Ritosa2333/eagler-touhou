// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e8b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b44f0;

void sub_40e8b0(void)
{
    int v0;  // [bp-0x4]

    sub_477420(g_4b44f0 + 20, 0, 6713280, v0);
    *((unsigned int *)(g_4b44f0 + 6713304)) = 0;
    *((unsigned int *)(g_4b44f0 + 6713312)) = 0;
    return;
}
