// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485c9f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_485c9f(unsigned int a0)
{
    int v4;  // edi
    void* idx;  // esi
    unsigned int v6;  // edi
    int v7;  // eax
    unsigned int v8;  // eax
    unsigned int v9;  // edx
    unsigned int v10;  // eax
    int v11;  // eax
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0x90]
    int v1;  // [bp-0x88]
    int v2;  // [bp-0x84]
    Byte v3[120];  // [bp-0x80]

    idx = sub_475467(v4, v1, v2) + 156;
    v6 = sub_485b44(v4, v1, v2);
    if (GetLocaleInfoA(v6, (-(0 < (int)idx[20]) & 0xfffff005) + 4098, v3, 120))
    {
        if (sub_46e3f8((int)idx[4], v3))
        {
LABEL_485da5:
            if (((int)idx[8] & 0x300) == 0x300)
                return v12;
            if (GetLocaleInfoA(v6, (-(0 < (int)idx[16]) & 0xfffff002) + 4097, v3, 120))
            {
                if (!sub_46e3f8(*((int *)idx), v3))
                {
                    *((unsigned int *)&idx[8]) = (int)idx[8] | 0x200;
                    if ((int)idx[16])
                    {
                        *((unsigned int *)&idx[8]) = (int)idx[8] | 0x100;
                    }
                    else
                    {
                        if (!(int)idx[12] || (v11 = (int)sub_475860(*((int *)idx)), v11 != (int)idx[12]))
                            goto LABEL_485e46;
                        v0 = 1;
LABEL_485e38:
                        if (!sub_485c2b(v6))
                            return v12;
LABEL_485e46:
                        *((unsigned int *)&idx[8]) = (int)idx[8] | 0x100;
                    }
                    if ((int)idx[24])
                        return v12;
                    *((unsigned int *)&idx[24]) = v6;
                    return v12;
                }
                if ((int)idx[16])
                    return v12;
                if ((int)idx[12])
                {
                    if (!sub_46e3f8(*((int *)idx), v3))
                    {
                        v0 = 0;
                        goto LABEL_485e38;
                    }
                    else
                    {
                        return v12;
                    }
                }
                else
                {
                    return v12;
                }
            }
        }
        else if (GetLocaleInfoA(v6, (-(0 < (int)idx[16]) & 0xfffff002) + 4097, v3, 120))
        {
            if (!sub_46e3f8(*((int *)idx), v3))
            {
                *((unsigned int *)&idx[8]) = (int)idx[8] | 772;
                *((unsigned int *)&idx[24]) = v6;
                goto LABEL_485da2;
            }
            else
            {
                if (!((char)idx[8] & 2))
                {
                    if ((int)idx[12] && !sub_48d94e(*((int *)idx), v3, (int)idx[12]))
                    {
                        *((unsigned int *)&idx[8]) = (int)idx[8] | 2;
                        *((unsigned int *)&idx[28]) = v6;
                        v7 = sub_475860(*((int *)idx));
                        if (v7 == (int)idx[12])
                        {
                            *((unsigned int *)&idx[24]) = v6;
                            goto LABEL_485da5;
                        }
                    }
                    else if (!((char)(int)idx[8] & 1) && (v8 = sub_485b20(v6), sub_485b20(v6)))
                    {
                        *((unsigned int *)&idx[8]) = v9 | 1;
LABEL_485da2:
                        *((unsigned int *)&idx[28]) = v6;
                        goto LABEL_485da5;
                    }
                }
                goto LABEL_485da5;
            }
        }
    }
    *((unsigned int *)&idx[8]) = 0;
    return v10;
}
