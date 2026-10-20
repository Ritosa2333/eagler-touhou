// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46f958; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b3c04;
extern unsigned int g_4d6448;
extern unsigned int g_4d6458;

unsigned int sub_46f958(unsigned int a0)
{
    unsigned int v1;  // edi
    unsigned int v0;  // [bp-0xc]

    if (!g_4b3c04)
        return 0;
    if (g_4d6458 != 3)
    {
        v0 = v1;
        if (!a0)
            return 1;
        if (g_4d6458 == 1)
        {
            if (a0 <= 1016 && sub_46ed7a(a0))
            {
                g_4d6448 = a0;
                g_4d6458 = 3;
                return 1;
            }
            *((unsigned int *)sub_46e96f()) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
        }
        else
        {
            *((unsigned int *)sub_46e96f()) = 22;
        }
        return 0;
    }
    else if (a0 > 1016)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        g_4d6448 = a0;
        return 1;
    }
    return 0;
}
