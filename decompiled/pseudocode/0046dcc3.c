// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46dcc3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_46dcc3(unsigned int *a0, unsigned int a1)
{
    int v5;  // esi
    int v6;  // edi
    unsigned int v7;  // eax
    unsigned int v0;  // [bp-0x20]
    int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp+0xc]
    unsigned int v4;  // [bp+0x10]

    v7 = sub_475860(v5, v6);
    if (v5 && a1)
    {
        v2 = 73;
        v1 = v5;
        v0 = 0x7fffffff;
        if (v7 > 0x7fffffff)
            return a0(&v5, a1, v3, v4);
        v0 = v7;
        return a0(&v5, a1, v3, v4);
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 0xffffffff;
}
