// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ec6e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int * sub_47ec6e(unsigned int *a0, unsigned int a1, unsigned int *a2, int a3)
{
    int v1;  // esi
    char v0;  // [bp+0x0]

    *(a2) = *(a0);
    a2[1] = a0[1];
    sub_47e9f6(a3, v1, &v0);
    return a2;
}
