// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b39a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


char * sub_48b39a(char *a0, unsigned int a1, unsigned int a2)
{
    char *v3;  // eax
    unsigned int v4;  // edi
    unsigned int v5;  // edx
    char *iter;  // edi
    unsigned int v7;  // ecx
    unsigned int v0;  // [bp-0x14]
    unsigned int iter1;  // [bp-0x10]
    char *node;  // [bp-0xc]

    v3 = a0;
    v4 = ((v3 ^ v3 >> 31) - (v3 >> 31) & 15 ^ v3 >> 31) - (v3 >> 31);
    if (!v4)
    {
        v5 = a2 & 127;
        iter1 = v5;
        if (a2 != v5)
        {
            sub_48b343(v3, a2 - v5);
            v3 = a0;
            v5 = iter1;
        }
        if (!v5)
            return v3;
        for (node = &v3[a2 + -1 * v5]; iter1; node += 1)
        {
            iter1 -= 1;
            *(node) = 0;
        }
        return a0;
    }
    else
    {
        v0 = -(v4) + 16;
        iter = a0;
        for (v7 = v0; v7; iter += 1)
        {
            v7 -= 1;
            *(iter) = 0;
        }
        sub_48b39a(&a0[v0], 0, a2 - v0);
        return a0;
    }
}
