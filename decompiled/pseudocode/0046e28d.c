// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e28d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_46e28d(int a0, int a1, int a2, int a3)
{
    unsigned int v2;  // edi
    unsigned int v3;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    v1 = v2;
    if (!a3)
        return 0;
    if (!a0 || !a2)
    {
        v0 = 22;
        v3 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
    }
    else if (a1 < a3)
    {
        v0 = 0x22;
        *((unsigned int *)sub_46e96f()) = 0x22;
        v3 = 0x22;
    }
    else
    {
        sub_47a6a0(a0, a2, a3);
        return 0;
    }
    sub_470f2d(0, 0, 0, 0, 0);
    return v3;
}
