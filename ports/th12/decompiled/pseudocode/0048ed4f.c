// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ed4f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48ed4f(unsigned int a0, LPArray a1, int a2, unsigned short *a3)
{
    if (a2 >= -0x1)
        return GetStringTypeW(a0, a1, a2, a3);
    return 0;
}
