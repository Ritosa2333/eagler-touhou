// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4772eb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adb00;

unsigned int sub_4772eb(unsigned int *a0)
{
    int v2;  // esi
    char v0;  // [bp+0x0]
    unsigned int v1;  // [bp+0x8]

    if (a0)
    {
        *(a0) = g_4adb00;
        return 0;
    }
    *((unsigned int *)sub_46e96f(v2, &v0)) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    v1 = 22;
    return v1;
}
