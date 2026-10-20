// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x424210; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_424210(unsigned int a0, int a1)
{
    int v1;  // eax
    unsigned int *v2;  // edi
    unsigned int *v3;  // ecx
    unsigned int i;  // esi

    v1 = 0;
    if (a1 <= 0)
        return 0;
    v3 = v2 + 1;
    while (*(v3) != i)
    {
        v1 += 1;
        v3 += 2;
        if (v1 >= a1)
            return 0;
    }
    return v2[2 * v1];
}
