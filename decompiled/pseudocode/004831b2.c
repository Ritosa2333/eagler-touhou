// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4831b2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b38d8;

unsigned int sub_4831b2(int a0)
{
    unsigned int v0;  // eax

    if (a0 >= 0)
    {
        if (a0 <= 2)
        {
            v0 = g_4b38d8;
            g_4b38d8 = a0;
            return v0;
        }
        else if (a0 == 3)
        {
            return g_4b38d8;
        }
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 0xffffffff;
}
