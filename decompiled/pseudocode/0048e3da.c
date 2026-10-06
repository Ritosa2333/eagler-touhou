// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e3da; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_48e3da(int a0, int a1)
{
    unsigned int v3;  // ecx
    unsigned int v4;  // eax
    int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]
    char v2;  // [bp+0x0]

    v1 = v3;
    v1 = 0;
    v4 = sub_48b4e8(a0, a1, &v1, v0, 0, &v2);
    if (v4)
    {
        return v4;
    }
    else if (v4)
    {
        if (!sub_46e96f())
            return v4;
        *((unsigned int *)sub_46e96f()) = 0;
        return v4;
    }
    else
    {
        return v4;
    }
}
