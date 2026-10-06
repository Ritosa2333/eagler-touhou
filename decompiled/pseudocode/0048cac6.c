// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48cac6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4d6308;
extern void g_4d6320;

unsigned int sub_48cac6(int a0)
{
    void* v1;  // eax
    int v2;  // esi
    char v0;  // [bp+0x0]

    if (a0 == -0x2)
    {
        *((unsigned int *)sub_46e982(&v0)) = 0;
        *((unsigned int *)sub_46e96f()) = 9;
        return 0xffffffff;
    }
    if (a0 >= 0 && a0 < *((int *)&g_4d6308))
    {
        v1 = (a0 & 31) * 64 + *((int *)&(&g_4d6320)[4 * (a0 >> 5)]);
        if ((char)v1[4] & 1)
            return *((int *)v1);
    }
    *((unsigned int *)sub_46e982(v2)) = 0;
    *((unsigned int *)sub_46e96f()) = 9;
    sub_470f2d(0, 0, 0, 0, 0);
    return 0xffffffff;
}
