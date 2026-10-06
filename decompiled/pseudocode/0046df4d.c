// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46df4d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49cd28;

void sub_46df4d(unsigned int *idx)
{
    idx[1] = 0;
    idx[2] = 0;
    *(idx) = &g_49cd28;
    return;
}
