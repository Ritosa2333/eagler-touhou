// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48afc9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48afc9(void)
{
    unsigned int *v6;  // eax
    int v7;  // esi
    int v8;  // ebx
    int v9;  // edi
    unsigned int v10;  // edi
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    char v2;  // [bp+0x0]
    char *v3;  // [bp+0x4]
    unsigned int v4;  // [bp+0x8]
    int v5;  // [bp+0xc]

    if (v6)
    {
        *(v6) = 0;
        if (v3)
        {
            if (v4 > 0)
                goto LABEL_48b008;
        }
        else
        {
            if (!v4)
            {
LABEL_48b008:
                if (v3)
                    *(v3) = 0;
                v10 = sub_48af42(v5, v9);
                if (!v10 || (v11 = sub_475860(v10) + 1, *(v6) = v11, !v4))
                    return;
                if (v11 > v4)
                {
                    v0 = 0x22;
                }
                else if (sub_47a2b9(v3, v4, v10))
                {
                    sub_470dc6(0, 0, 0, 0, 0);
                }
                return;
            }
        }
    }
    v1 = 22;
    *((unsigned int *)sub_46e96f(v7, v8, &v2)) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    return;
}
