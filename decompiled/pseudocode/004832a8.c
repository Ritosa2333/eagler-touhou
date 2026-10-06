// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4832a8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern Char g_49e1c8;
extern unsigned int g_4b4394;

int sub_4832a8(void)
{
    unsigned int v15;  // edi
    char *v16;  // eax
    unsigned int *iter;  // eax
    void* *v25;  // ecx
    int v26;  // eax
    unsigned int *v27;  // edi
    unsigned int *node;  // eax
    unsigned int v17;  // ecx
    unsigned int v18;  // eax
    unsigned int v19;  // edi
    int v20;  // eax
    unsigned int *iter1;  // eax
    int v22;  // eax
    unsigned int *v23;  // esi
    unsigned int v0;  // [bp-0x34]
    unsigned int v1;  // [bp-0x30]
    unsigned int v2;  // [bp-0x28]
    unsigned int v3;  // [bp-0x24]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]
    unsigned int v6;  // [bp-0x14]
    unsigned int *v7;  // [bp-0x10], Other Possible Types: unsigned int
    int v8;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v9;  // [bp+0x4]
    unsigned int v10;  // [bp+0x8]
    char *v11;  // [bp+0xc]
    unsigned int v12;  // [bp+0x10]
    unsigned int v13;  // [bp+0x1c]
    unsigned int v14;  // [bp+0x20]

    v3 = v15;
    if (!g_4b4394)
    {
        if (LCMapStringW(0, 0x100, &g_49e1c8, 1, NULL, 0))
        {
            g_4b4394 = 1;
        }
        else if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
        {
            g_4b4394 = 2;
        }
    }
    if (v12 > 0)
    {
        v16 = v11;
        v17 = v12;
        do
        {
            v17 -= 1;
            if (!*(v16))
                goto LABEL_483319;
        } while ((v16 += 1, v17));
        v17 = 0xffffffff;
LABEL_483319:
        v18 = v12 - v17 - 1;
        if (v18 < v12)
            v18 += 1;
        v12 = v18;
    }
    if (!(g_4b4394 != 2 && g_4b4394))
    {
        v7 = 0;
        v6 = 0;
        if (!v9)
            v9 = (int)(*(v25))[20];
        if (!v13)
            v13 = (int)(*(v25))[4];
        v5 = sub_48d62f(v9);
        if (v5 != 0xffffffff)
        {
            if (v5 != v13)
            {
                v7 = sub_48d678(v13, v5, v11, &v12, 0, 0);
                if (v7)
                {
                    v8 = LCMapStringA(v9, v10, v7, v12, NULL, 0);
                    if (!v8)
                    {
LABEL_48355a:
                        goto LABEL_483618;
                    }
                    else
                    {
                        if (v8 > 0 && v8 <= -0x20)
                        {
                            v26 = v8 + 8;
                            if (v26 <= 0x400)
                            {
                                sub_48d830();
                                if (!&v3)
                                    goto LABEL_48355a;
                                v3 = 0xcccc;
                                v27 = &v4;
                            }
                            else
                            {
                                node = sub_46d04a(v26);
                                if (node)
                                {
                                    *(node) = 0xdddd;
                                    node += 2;
                                }
                                v27 = node;
                            }
                        }
                        else
                        {
                            v27 = NULL;
                        }
                        if (!v27)
                            goto LABEL_48355a;
                        sub_477420(v27, 0, v8);
                        v8 = LCMapStringA(v9, v10, v7, v12, v27, v8);
                        if (v8)
                            v6 = sub_48d678(v5, v13, v27, &v8, v0, v1);
                        sub_48326a(v27);
                        goto LABEL_483618;
                    }
                }
            }
            else
            {
                LCMapStringA(v9, v10, v11, v12, v0, v1);
LABEL_483618:
                if (v7)
                    sub_46c941(v7, v3);
                if (!v6 || v0 == v6)
                    return;
                sub_46c941(v6, v3);
                return;
            }
        }
    }
    else if (g_4b4394 == 1)
    {
        v8 = 0;
        if (!v13)
            v13 = (int)(*(v25))[4];
        v19 = MultiByteToWideChar(v13, (v14) * 8 + 1, v11, v12, NULL, 0);
        if (v19)
        {
            if (v19 > 0 && !(v2 = 0xffffffe0, 0xffffffe0 / v19 < 2))
            {
                v20 = v19 * 2 + 8;
                if (v20 <= 0x400)
                {
                    sub_48d830();
                    iter1 = &v3;
                    if (!&v3)
                        goto LABEL_4833c4;
                    v3 = 0xcccc;
                    goto LABEL_4833c1;
                }
                else
                {
                    iter1 = sub_46d04a(v20);
                    if (iter1)
                    {
                        *(iter1) = 0xdddd;
LABEL_4833c1:
                        iter1 += 2;
                    }
                }
LABEL_4833c4:
                v7 = iter1;
            }
            else
            {
                v7 = NULL;
            }
            if (v7)
            {
                if (MultiByteToWideChar(v13, MB_PRECOMPOSED, v11, v12, v7, v19))
                {
                    v8 = LCMapStringW(v9, v10, v7, v19, NULL, 0);
                    if (v8)
                    {
                        if (!(v10 & 0x400))
                        {
                            if (v8 > 0 && !(v2 = 0xffffffe0, 0xffffffe0 / v8 < 2))
                            {
                                v22 = v8 * 2 + 8;
                                if (v22 <= 0x400)
                                {
                                    sub_48d830();
                                    if (!&v3)
                                    {
                                        sub_48326a(v7);
                                        return;
                                    }
                                    v3 = 0xcccc;
                                    v23 = &v4;
                                }
                                else
                                {
                                    iter = sub_46d04a(v22);
                                    if (iter)
                                    {
                                        *(iter) = 0xdddd;
                                        iter += 2;
                                    }
                                    v23 = iter;
                                }
                            }
                            else
                            {
                                v23 = NULL;
                            }
                            if (v23)
                            {
                                if (LCMapStringW(v9, v10, v7, v19, v23, v8))
                                {
                                    if (!v1)
                                    {
                                        v1 = 0;
                                        v0 = 0;
                                    }
                                    else
                                    {
                                        v1 = v1;
                                        v0 = v0;
                                    }
                                    v8 = WideCharToMultiByte(v13, 0, v23, v8, v0, v1, NULL, NULL);
                                }
                                sub_48326a(v23);
                            }
                        }
                        else if (v1 && v8 <= v1)
                        {
                            LCMapStringW(v9, v10, v7, v19, v0, v1);
                        }
                    }
                }
                sub_48326a(v7);
                return;
            }
        }
    }
    return;
}
