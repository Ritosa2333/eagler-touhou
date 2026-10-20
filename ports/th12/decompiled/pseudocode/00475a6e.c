// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475a6e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_475a6e(unsigned int a0, unsigned int a1, unsigned int *a2)
{
    unsigned int v0;  // ecx
    unsigned int v1;  // eax

    v0 = a0 + a1;
    v1 = 0;
    if (v0 < a0 || v0 < a1)
        v1 = 1;
    *(a2) = v0;
    return v1;
}
