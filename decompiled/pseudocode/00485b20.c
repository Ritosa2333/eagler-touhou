// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485b20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_49f2d8;

unsigned int sub_485b20(unsigned short i)
{
    unsigned int v0;  // eax

    v0 = 0;
    while (i != *((short *)&(&g_49f2d8)[v0]))
    {
        v0 += 2;
        if (v0 >= 20)
            return 1;
    }
    return 0;
}
