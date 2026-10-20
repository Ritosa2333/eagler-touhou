// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462a50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_462a50(short a0, unsigned int *a1, unsigned int a2, unsigned int a3)
{
    if (a0 >= 0)
    {
        *(a1) = *(a1) | -(0 < (1 << ((char)a0 & 31) & a3)) & a2;
        return -(0 < (1 << ((char)a0 & 31) & a3)) & a2;
    }
    return 0;
}
