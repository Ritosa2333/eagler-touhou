// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c2e5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48c2e5_0 {
    char padding_0[4];
    unsigned int field_4;
} st_48c2e5_0;

extern unsigned int g_4b43a0;

int sub_48c2e5(void)
{
    unsigned int v11;  // ecx
    unsigned int v12;  // eax
    unsigned int v13;  // edi
    unsigned int v14;  // eax
    unsigned int *v15;  // edi
    unsigned int *iter;  // eax
    PSTR v0;  // [bp-0x28]
    int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x18]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    st_48c2e5_0 **v7;  // [bp+0x4]
    unsigned int v8;  // [bp+0x8]
    unsigned int v9;  // [bp+0xc]
    unsigned int v10;  // [bp+0x18]

    v6 = v11;
    v5 = v11;
    v12 = g_4b43a0;
    v3 = v13;
    if (!v12)
    {
        if (!GetLocaleInfoW(0, 1, NULL, 0))
        {
            if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
            {
                v2 = 2;
                v12 = 2;
                g_4b43a0 = 2;
                goto LABEL_48c339;
            }
            else
            {
                v12 = g_4b43a0;
                goto LABEL_48c339;
            }
        }
        g_4b43a0 = 1;
LABEL_48c34e:
        if (!v10)
            v10 = *(v7)->field_4;
        v5 = GetLocaleInfoW(v8, v9, NULL, 0);
        if (v5)
        {
            if (v5 <= 0 || (v2 = 0xffffffe0, 0xffffffe0 / v5 < 2))
            {
                v15 = NULL;
                goto LABEL_48c3c1;
            }
            v14 = v5 * 2 + 8;
            if (v14 <= 0x400)
            {
                sub_48d830();
                if (&v3)
                {
                    v3 = 0xcccc;
                    v15 = &v4;
LABEL_48c3c1:
                    if (v15)
                    {
                        if (GetLocaleInfoW(v8, v9, v15, v5))
                        {
                            if (!v1)
                            {
                                v1 = 0;
                                v0 = NULL;
                            }
                            else
                            {
                                v1 = v1;
                                v0 = v0;
                            }
                            WideCharToMultiByte(v10, 0, v15, -0x1, v0, v1, NULL, NULL);
                        }
                        sub_48326a(v15);
                        return;
                    }
                }
            }
            else
            {
                iter = sub_46d04a(v14);
                if (iter)
                {
                    *(iter) = 0xdddd;
                    iter += 2;
                }
                v15 = iter;
                goto LABEL_48c3c1;
            }
        }
    }
    else
    {
LABEL_48c339:
        if (v12 == 2 || !v12)
        {
            GetLocaleInfoA(v8, v9, v0, v1);
            return;
        }
        if (v12 == 1)
            goto LABEL_48c34e;
    }
    return;
}
