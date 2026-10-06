// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493ad9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52d0;
extern unsigned int g_4d52d8;

int sub_493ad9(unsigned int a0)
{
    unsigned int v0;  // eax

    if (a0)
    {
        g_4d52d8 = sub_475163(a0);
        g_4d52d0 = 1;
        return g_4d52d8;
    }
    g_4d52d0 = 0;
    return v0;
}
