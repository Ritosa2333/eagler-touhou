// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43c970; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_43c970(unsigned int *a0)
{
    unsigned int v1;  // edx

    v1 = 715827883 * (a0[1350] - (char *)a0) >> 32;
    return a0[1576] + 6 * (v1 >> 31) + 6 * v1 - (char *)a0 - 5404;
}
