// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47735d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adb88[4];

unsigned int sub_47735d(int *a0, char *a1, int a2, unsigned int a3)
{
    int *v2;  // esi
    int v3;  // eax
    unsigned int v4;  // eax
    int v5;  // edi
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]

    if (a1)
    {
        if (a2 <= NULL)
        {
            v1 = 22;
            *((unsigned int *)sub_46e96f()) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
            return 22;
        }
    }
    else
    {
        if (a2)
        {
            v1 = 22;
            *((unsigned int *)sub_46e96f()) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
            return 22;
        }
    }
    if (a1)
        *(a1) = 0;
    if (!a0 || a3 && a3 != 1)
    {
        v0 = 22;
        *((unsigned int *)sub_46e96f(v5)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        v4 = 22;
    }
    else
    {
        v2 = &g_4adb88[a3];
        v3 = sub_475860(*(v2)) + 1;
        *(a0) = v3;
        if (!a1)
        {
            v4 = 0;
        }
        else if (v3 > a2)
        {
            v0 = 0x22;
            v4 = 0x22;
        }
        else
        {
            v4 = sub_47a2b9(a1, a2, *(v2));
        }
    }
    return v4;
}
