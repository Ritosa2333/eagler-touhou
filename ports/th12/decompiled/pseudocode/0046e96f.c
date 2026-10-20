// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e96f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad2a8;

unsigned int sub_46e96f(void)
{
    unsigned int v1;  // eax

    v1 = sub_4753ee();
    if (v1)
        return v1 + 8;
    return &g_4ad2a8;
}
