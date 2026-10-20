// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e378; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int * sub_47e378(unsigned int a0, unsigned int a1, unsigned int *a2, unsigned int a3)
{
    unsigned int *v1;  // ecx
    unsigned int v0;  // [bp-0x8]

    if (a3 <= 9)
    {
        a1 = *((int *)a0);
        if (a1 != 0xffffffff && a3 <= a1)
        {
            v1 = *((int *)(a0 + a3 * 4 + 4));
            *(a2) = *(v1);
            a2[1] = v1[1];
            return a2;
        }
        v0 = 2;
    }
    else
    {
        v0 = 3;
    }
    sub_47e1ce(a2, a1, v0);
    return a2;
}
