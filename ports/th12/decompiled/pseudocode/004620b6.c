// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4620b6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4620b6(void)
{
    unsigned int v1;  // ecx
    unsigned int v2;  // edx
    unsigned int *v3;  // eax
    unsigned int v0;  // [bp+0x4]

    if (!sub_461920(v1, v2, *(v3)))
        *(v3) = 0;
    sub_462020(v1, v0);
    return;
}
