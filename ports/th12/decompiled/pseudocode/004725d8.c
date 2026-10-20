// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4725d8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4725d8(char *a0, int a1, int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    int v8;  // ecx
    unsigned int v9;  // esi
    unsigned int v10;  // edi
    int v11;  // eax
    unsigned int *v12;  // eax
    unsigned int *v13;  // eax
    unsigned int v14;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    char v7;  // [bp+0x0]

    if (!a3)
    {
        *((unsigned int *)sub_46e96f(v5, v8, &v7)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return -0x1;
    }
    v4 = v9;
    v3 = v10;
    if (!a2)
    {
        if (!a1)
            return 0;
LABEL_47262b:
        *((unsigned int *)sub_46e96f()) = 22;
        goto LABEL_4726ca;
    }
    if (!a0 || a1 <= 0)
        goto LABEL_47262b;
    v12 = sub_46e96f();
    v2 = a5;
    v1 = a4;
    v0 = a3;
    if (a1 > a2)
    {
        v11 = sub_472414(sub_47c9cb, a0, a2 + 1);
        if (v11 != -0x2)
            goto LABEL_4726b4;
        if (*((int *)sub_46e96f()) == 0x22)
        {
            v13 = sub_46e96f();
            *(v13) = *(v12);
        }
    }
    else
    {
        v14 = *(v12);
        v6 = *(v12);
        v11 = sub_472414(sub_47c9cb, a0, a1);
        *(&a0[a1] - 1) = 0;
        if (v11 == -0x2)
        {
            if (a2 != -0x1)
                goto LABEL_4726b8;
            if (*((int *)sub_46e96f()) == 0x22)
                *((unsigned int *)sub_46e96f()) = v14;
        }
        else
        {
LABEL_4726b4:
            if (v11 >= 0)
                return v11;
LABEL_4726b8:
            *(a0) = 0;
            if (v11 == -0x2)
            {
                *((unsigned int *)sub_46e96f()) = 0x22;
LABEL_4726ca:
                sub_470f2d(0, 0, 0, 0, 0);
            }
        }
    }
    return -0x1;
}
