// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e32e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int * sub_47e32e(unsigned int *a0, unsigned int a1, unsigned int *a2)
{
    unsigned int v2;  // esi
    unsigned int v3;  // edi
    unsigned int *v4;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    if (*(a0) == 9)
        return a0;
    v0 = v3;
    if (!*(a2))
        return a0;
    v4 = sub_47dc29(8, 0);
    if (v4)
    {
        *(v4) = *(a2);
        v4[1] = a2[1];
    }
    else
    {
        v4 = NULL;
    }
    if (v4)
    {
        *(a0) = *(a0) + 1;
        a0[1 + *(a0)] = v4;
    }
    return a0;
}
