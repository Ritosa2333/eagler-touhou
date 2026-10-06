// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477793; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4b41d0;

unsigned int sub_477793(unsigned int a0)
{
    Sleep(a0);
    if (a0 + 1000 <= *((int *)&g_4b41d0))
        return a0 + 1000;
    return 0xffffffff;
}
