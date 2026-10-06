// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e1cf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48e1cf(unsigned int *a0, int a1, unsigned int a2)
{
    unsigned int v2;  // eax
    unsigned int v3;  // edi
    unsigned int *v4;  // edi
    unsigned int *v5;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    v2 = a2 & 0xfff7ffff;
    if (a1 & v2 & 4243651808)
    {
        v1 = v3;
        v4 = a0;
        if (v4)
            *(v4) = sub_4901bb(0, 0);
        v0 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 22;
    }
    else
    {
        v5 = a0;
        if (v5)
            *(v5) = sub_4901bb(a1, v2);
        else
            sub_4901bb(a1, v2);
        return 0;
    }
}
