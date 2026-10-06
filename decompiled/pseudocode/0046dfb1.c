// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46dfb1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49cd28;

void sub_46dfb1(unsigned int *idx, unsigned int a1, unsigned int *a2)
{
    unsigned int v0;  // ecx

    *(idx) = &g_49cd28;
    v0 = *(a2);
    idx[2] = 0;
    idx[1] = v0;
    return;
}
