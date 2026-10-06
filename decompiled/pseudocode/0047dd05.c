// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47dd05; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_47dd05(unsigned int *a0)
{
    unsigned int v1;  // eax

    v1 = 0;
    if (*(a0) && a0[1] & 0x200)
        v1 = 1;
    return v1;
}
