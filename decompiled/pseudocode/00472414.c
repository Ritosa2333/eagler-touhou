// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472414; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_472414(unsigned int *a0, char *a1, int a2, unsigned int a3, unsigned int a4, unsigned int a5)
{
    unsigned int v7;  // esi
    unsigned int v8;  // edi
    int v9;  // eax
    unsigned int v0;  // [bp-0x30]
    unsigned int v1;  // [bp-0x2c]
    int v2;  // [bp-0x28]
    char *v3;  // [bp-0x24]
    int v4;  // [bp-0x20]
    char *v5;  // [bp-0x1c]
    unsigned int v6;  // [bp-0x18]

    if (!a3)
    {
        *((unsigned int *)sub_46e96f(v2)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return -0x1;
    }
    v1 = v7;
    v0 = v8;
    if (a2 && !a1)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        v9 = -0x1;
    }
    else
    {
        v4 = 0x7fffffff;
        if (a2 <= 0x7fffffff)
            v4 = a2;
        v6 = 66;
        v5 = a1;
        v3 = a1;
        v9 = a0(&v3, a3, a4, a5);
        a3 = v9;
        if (a1)
        {
            if (v9 >= 0)
            {
                v4 -= 1;
                if (v4 - 1 >= 0)
                {
                    *(v3) = 0;
                }
                else
                {
                    v4 = v4;
                    if (sub_46ffdb(0, &v3) == 0xffffffff)
                        goto LABEL_4724cd;
                }
                v9 = a3;
            }
            else
            {
LABEL_4724cd:
                *(&a1[a2] - 1) = 0;
                v9 = (unsigned int)((v4 >= 0) - 2);
            }
        }
    }
    return v9;
}
