// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472533; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_472533(char *a0, int a1, unsigned int a2, unsigned int a3, unsigned int a4)
{
    int v1;  // ebx
    int v2;  // eax
    int v3;  // esi
    char v0;  // [bp+0x0]

    if (!a2)
    {
        *((unsigned int *)sub_46e96f(v1, &v0)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return -0x1;
    }
    if (!a0 || a1 <= 0)
    {
        *((unsigned int *)sub_46e96f(v3)) = 22;
    }
    else
    {
        v2 = sub_472414(sub_47c9cb, a0, a1, a2, a3, a4);
        if (v2 < 0)
            *(a0) = 0;
        if (v2 != -0x2)
            return v2;
        *((unsigned int *)sub_46e96f()) = 0x22;
    }
    sub_470f2d(0, 0, 0, 0, 0);
    return -0x1;
}
