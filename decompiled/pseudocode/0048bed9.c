// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48bed9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern void g_4adbb0;
extern void g_4d6320;

unsigned int sub_48bed9(void* idx)
{
    int v2;  // edi
    unsigned int v3;  // eax
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    int v6;  // eax
    unsigned int v7;  // eax
    void* v8;  // eax
    char *v9;  // ecx
    unsigned int v10;  // eax
    int v0;  // [bp-0x8]
    char v1;  // [bp+0x0]

    if (!idx)
    {
        *((unsigned int *)sub_46e96f(v2, v0, &v1)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        v3 = (int)idx[12];
        if ((char)v3 & 131 && !((char)v3 & 64))
        {
            if ((char)v3 & 2)
            {
                *((unsigned int *)&idx[12]) = v3 | 32;
            }
            else
            {
                v4 = v3 | 1;
                *((unsigned int *)&idx[12]) = v4;
                if (!(v4 & 268))
                    sub_47bf78(idx);
                else
                    *((int *)idx) = (int)idx[8];
                v5 = sub_48e9f9(sub_47c1da(idx, (int)idx[8], (int)idx[24]));
                *((unsigned int *)&idx[4]) = v5;
                if (v5 && v5 != 0xffffffff)
                {
                    if (!((char)idx[12] & 130))
                    {
                        if (sub_47c1da(idx) != 0xffffffff && sub_47c1da(idx) != 0xfffffffe)
                        {
                            v6 = sub_47c1da(idx);
                            v7 = sub_47c1da(idx);
                            v8 = (v7 & 31) * 64 + *((int *)&(&g_4d6320)[4 * (v6 >> 5)]);
                        }
                        else
                        {
                            v8 = &g_4adbb0;
                        }
                        if (((char)v8[4] & 130) == 130)
                            *((unsigned int *)&idx[12]) = (int)idx[12] | 0x2000;
                    }
                    if ((int)idx[24] == 0x200 && (char)(int)idx[12] & 8 && !((int)idx[12] & 0x400))
                        *((int *)&idx[24]) = 0x1000;
                    v9 = *((int *)idx);
                    *((unsigned int *)&idx[4]) = (int)idx[4] - 1;
                    v10 = *(v9);
                    *((char **)idx) = v9 + 1;
                    return v10;
                }
                *((unsigned int *)&idx[12]) = (int)idx[12] | (-(0 < v5) & 16) + 16;
                *((unsigned int *)&idx[4]) = 0;
            }
        }
    }
    return 0xffffffff;
}
