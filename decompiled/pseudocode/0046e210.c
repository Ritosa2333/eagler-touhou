// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e210; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

unsigned int sub_46e210(int a0, int a1, int a2, int a3)
{
    unsigned int v2;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    v1 = v2;
    if (!a3)
        return 0;
    if (!a0)
    {
        v0 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 22;
    }
    if (a2 && a1 >= a3)
    {
        sub_47a330(a0, a2, a3);
    }
    else
    {
        sub_477420(a0, 0, a1);
        if (!a2)
        {
            v0 = 22;
            *((unsigned int *)sub_46e96f()) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
            return 22;
        }
        else if (a1 < a3)
        {
            v0 = 0x22;
            *((unsigned int *)sub_46e96f()) = 0x22;
            sub_470f2d(0, 0, 0, 0, 0);
            return 0x22;
        }
        else
        {
            v0 = 22;
            return 22;
        }
    }
    return 0;
}
