// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48b05e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_48b05e(int *a0, int a1)
{
    int *v3;  // ebx
    int v4;  // edi
    int v5;  // esi
    int v6;  // ecx
    int v8;  // esi
    int v9;  // eax
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x8]
    char v2;  // [bp+0x0]

    if (v3)
    {
        *(v3) = 0;
        if (a0)
            *(a0) = 0;
        if (a1)
        {
            v1 = sub_48af42(a1);
            if (!sub_48af42(a1))
                return 0;
            v8 = sub_475860(sub_48af42(a1)) + 1;
            v9 = sub_48e3da(v8, 1);
            *(v3) = v9;
            if (!v9)
            {
                *((unsigned int *)sub_46e96f()) = 12;
                v10 = *((int *)sub_46e96f());
                v11 = *((int *)sub_46e96f());
                return *((int *)sub_46e96f());
            }
            if (sub_47a2b9(v9, v8, sub_48af42(a1)))
                sub_470dc6(0, 0, 0, 0, 0);
            if (a0)
                *(a0) = v8;
            return 0;
        }
    }
    v0 = 22;
    *((unsigned int *)sub_46e96f(v4, v5, v6, &v2)) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return 22;
}
