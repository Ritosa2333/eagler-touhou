// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4907fa; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CPINFO {
    unsigned int MaxCharSize;
    Byte DefaultChar[2];
    Byte LeadByte[12];
} CPINFO;

extern Char g_49e1c8;
extern unsigned int g_4b43b0;

int sub_4907fa(void)
{
    unsigned int v15;  // edi
    char *v16;  // edx
    unsigned int v24;  // eax
    unsigned int *v25;  // edi
    unsigned int *iter;  // eax
    char *v27;  // edi
    char *v28;  // ebx
    void* *v29;  // ecx
    unsigned int v30;  // esi
    unsigned int v17;  // edx
    char *i;  // eax
    char *v19;  // eax
    unsigned int v20;  // eax
    unsigned int *node;  // eax
    unsigned int v22;  // ebx
    unsigned int v23;  // 4101
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x34]
    unsigned int v3;  // [bp-0x30]
    enum COMPARESTRING_RESULT v4;  // [bp-0x2c], Other Possible Types: unsigned int
    char *v5;  // [bp-0x28]
    unsigned int *v6;  // [bp-0x24]
    char *v7;  // [bp-0x20]
    CPINFO v8;  // [bp-0x1c]
    unsigned int v9;  // [bp+0x4]
    unsigned int v10;  // [bp+0x8]
    unsigned int v11;  // [bp+0xc]
    char *v12;  // [bp+0x10]
    unsigned int v13;  // [bp+0x14]
    unsigned int v14;  // [bp+0x18]

    v1 = v15;
    v5 = v16;
    v7 = v12;
    if (!g_4b43b0)
    {
        if (CompareStringW(0, 0, &g_49e1c8, 1, &g_49e1c8, 1))
        {
            g_4b43b0 = 1;
        }
        else if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
        {
            g_4b43b0 = 2;
        }
    }
    if (v11 > 0)
    {
        v11 = sub_4907dc(v11);
        goto LABEL_490871;
    }
    if (v11 >= 0xffffffff)
    {
LABEL_490871:
        v17 = v13;
        if (v17 > 0)
        {
            v17 = sub_4907dc(v17);
            v13 = v17;
            goto LABEL_49089b;
        }
        if (v17 >= 0xffffffff)
        {
LABEL_49089b:
            if (!(g_4b43b0 != 2 && g_4b43b0))
            {
                v27 = NULL;
                v28 = NULL;
                if (!v9)
                    v9 = (int)(*(v29))[20];
                if (!v14)
                    v14 = (int)(*(v29))[4];
                v30 = sub_48d62f(v9);
                if (v30 != 0xffffffff)
                {
                    if (v30 != v14)
                    {
                        v28 = sub_48d678(v14, v30, v5, &v11, 0, 0);
                        if (v28)
                        {
                            v27 = sub_48d678(v14, v30, v7, &v13, 0, 0);
                            if (!v27)
                            {
                                sub_46c941(v28);
                            }
                            else
                            {
                                v5 = v28;
                                v7 = v27;
                                goto LABEL_490b2a;
                            }
                        }
                    }
                    else
                    {
LABEL_490b2a:
                        CompareStringA(v9, v10, v5, v11, v7, v13);
                        if (!v28)
                            return;
                        sub_46c941(v28);
                        sub_46c941(v27);
                        return;
                    }
                }
            }
            else if (g_4b43b0 == 1)
            {
                v4 = 0;
                if (!v14)
                    v14 = (int)(*(v29))[4];
                if (!v11 || !v17)
                {
                    if (v11 == v17)
                    {
LABEL_4908db:
                        v0 = 2;
                        return;
                    }
                    if (v17 > 1)
                        return;
                    if (v11 > 1)
                    {
LABEL_4908f0:
                        v0 = 3;
                        return;
                    }
                    else if (GetCPInfo(v14, &v8))
                    {
                        if (v11 <= 0)
                        {
                            if (v13 <= 0)
                                goto LABEL_490970;
                            if (v8.MaxCharSize < 2)
                                return;
                            v19 = &v8.LeadByte[0];
                            if (!*(&v8.LeadByte[0]))
                                return;
                            while (1)
                            {
                                if (!v19[1])
                                    return;
                                if (*(v7) >= *(v19) && *(v7) <= v19[1])
                                    break;
                                v19 += 2;
                                if (!*(v19))
                                    return;
                            }
                            goto LABEL_4908db;
                        }
                        else if (v8.MaxCharSize >= 2 && (i = (char *)(&v8 + 6), *((char *)((void*)&v8 + 6))))
                        {
                            for (; i[1]; i += 2)
                            {
                                if (*(v16) >= *(i) && *(v16) <= i[1])
                                    goto LABEL_4908db;
                            }
                        }
                        goto LABEL_4908f0;
                    }
                }
                else
                {
LABEL_490970:
                    v3 = MultiByteToWideChar(v14, 9, v16, v11, NULL, 0);
                    if (v3)
                    {
                        if (v3 > 0 && !(v0 = 0xffffffe0, 0xffffffe0 / v3 < 2))
                        {
                            v20 = v3 * 2 + 8;
                            if (v20 <= 0x400)
                            {
                                sub_48d830();
                                node = &v1;
                                if (!&v1)
                                    goto LABEL_4909d4;
                                v1 = 0xcccc;
                                goto LABEL_4909d1;
                            }
                            else
                            {
                                node = sub_46d04a(v20);
                                if (node)
                                {
                                    *(node) = 0xdddd;
LABEL_4909d1:
                                    node += 2;
                                }
                            }
LABEL_4909d4:
                            v6 = node;
                        }
                        else
                        {
                            v6 = NULL;
                        }
                        if (v6)
                        {
                            if (MultiByteToWideChar(v14, MB_PRECOMPOSED, v5, v11, v6, v3))
                            {
                                v22 = MultiByteToWideChar(v14, 9, v7, v13, NULL, 0);
                                if (v22)
                                {
                                    v23 = _ccall(14, 15, v22, 0, 0);
                                    if (!(v23 & 1) && !(v0 = 0xffffffe0, 0xffffffe0 / v22 < 2))
                                    {
                                        v24 = v22 * 2 + 8;
                                        if (v24 <= 0x400)
                                        {
                                            sub_48d830();
                                            if (!&v1)
                                            {
                                                sub_48326a(v6);
                                                return;
                                            }
                                            v1 = 0xcccc;
                                            v25 = &v2;
                                        }
                                        else
                                        {
                                            iter = sub_46d04a(v24);
                                            if (iter)
                                            {
                                                *(iter) = 0xdddd;
                                                iter += 2;
                                            }
                                            v25 = iter;
                                        }
                                    }
                                    else
                                    {
                                        v25 = NULL;
                                    }
                                    if (v25)
                                    {
                                        if (MultiByteToWideChar(v14, MB_PRECOMPOSED, v7, v13, v25, v22))
                                            v4 = CompareStringW(v9, v10, v6, v3, v25, v22);
                                        sub_48326a(v25);
                                    }
                                }
                            }
                            sub_48326a(v6);
                            return;
                        }
                    }
                }
            }
        }
    }
    return;
}
