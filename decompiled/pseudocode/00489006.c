// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x489006; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_489006(unsigned int a0)
{
    int v0;  // eax

    v0 = 0;
    while (!*((int *)(a0 + v0 * 4)))
    {
        v0 += 1;
        if (v0 >= 3)
            return 1;
    }
    return 0;
}
