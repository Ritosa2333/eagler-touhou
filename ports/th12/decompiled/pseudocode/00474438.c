// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474438; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_474438(int a0, int a1, int a2, unsigned int a3)
{
    unsigned int v1;  // edi
    unsigned int v2;  // esi
    int *v3;  // esi
    unsigned int v4;  // edi
    unsigned int v0;  // [bp-0x10]

    v1 = *((unsigned int *)&a2);
    if (v1 <= 0)
        return;
    v0 = v2;
    v3 = &a2;
    do
    {
        v4 = v1;
        v3 += 1;
        if (sub_483089(a0, a1, *(v3)))
            sub_470dc6(0, 0, 0, 0, 0);
    } while ((v1 = v4 - 1, v4 != 1));
    return;
}
