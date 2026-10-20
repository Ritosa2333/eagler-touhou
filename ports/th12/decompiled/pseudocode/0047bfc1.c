// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47bfc1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4d6308;
extern unsigned int g_4d6320[4];

unsigned int sub_47bfc1(int a0)
{
    int v1;  // esi
    char v0;  // [bp+0x0]

    if (a0 == -0x2)
    {
        *((unsigned int *)sub_46e96f(&v0)) = 9;
        return 0;
    }
    if (a0 >= 0 && a0 < *((int *)&g_4d6308))
        return *((char *)(g_4d6320[a0 >> 5] + (a0 & 31) * 64 + 4)) & 64;
    *((unsigned int *)sub_46e96f(v1)) = 9;
    sub_470f2d(0, 0, 0, 0, 0);
    return 0;
}
