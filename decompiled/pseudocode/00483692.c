// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x483692; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern Char g_49e1c8;
extern unsigned int g_4b4398;

int sub_483692(void)
{
    void* *v11;  // ecx
    unsigned int v12;  // eax
    unsigned int *v13;  // ebx
    unsigned int v14;  // edi
    int v15;  // edi
    unsigned int v16;  // eax
    unsigned int *iter;  // eax
    int v18;  // eax
    unsigned int v19;  // esi
    unsigned int v20;  // eax
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned short v2[2];  // [bp-0xc], Other Possible Types: void* *, int, unsigned int
    void* *v3;  // [bp-0x8]
    unsigned int v4;  // [bp+0x4]
    unsigned int v5;  // [bp+0x8]
    unsigned int v6;  // [bp+0xc]
    unsigned short *v7;  // [bp+0x10]
    unsigned int v8;  // [bp+0x14]
    unsigned int v9;  // [bp+0x18]
    unsigned int v10;  // [bp+0x1c]

    v3 = v11;
    v2 = v11;
    v12 = g_4b4398;
    v13 = NULL;
    v1 = v14;
    if (!v12)
    {
        if (GetStringTypeW(1, &g_49e1c8, 1, v2))
        {
            g_4b4398 = 1;
LABEL_483707:
            v2 = 0;
            if (!v8)
                v8 = (int)(*(v11))[4];
            v15 = MultiByteToWideChar(v8, (v10) * 8 + 1, v5, v6, NULL, 0);
            if (v15)
            {
                if (v15 > 0 && v15 <= 0x7ffffff0)
                {
                    v16 = v15 * 2 + 8;
                    if (v16 <= 0x400)
                    {
                        sub_48d830();
                        iter = &v1;
                        if (!&v1)
                            goto LABEL_483780;
                        v1 = 0xcccc;
                        goto LABEL_48377d;
                    }
                    else
                    {
                        iter = sub_46d04a(v16);
                        if (iter)
                        {
                            *(iter) = 0xdddd;
LABEL_48377d:
                            iter += 2;
                        }
                    }
LABEL_483780:
                    v13 = iter;
                }
                if (v13)
                {
                    sub_477420(v13, 0, v15 * 2);
                    v18 = MultiByteToWideChar(v8, MB_PRECOMPOSED, v5, v6, v13, v15);
                    if (v18)
                        v2 = GetStringTypeW(v4, v13, v18, v7);
                    sub_48326a(v13);
                    return;
                }
            }
            goto LABEL_4837ef;
        }
        else if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
        {
            v0 = 2;
            v12 = 2;
            g_4b4398 = 2;
        }
        else
        {
            v12 = g_4b4398;
        }
    }
    if (!(v12 != 2 && v12))
    {
        v19 = 0;
        if (!v9)
            v9 = (int)(*(v11))[20];
        if (!v8)
            v8 = (int)(*(v11))[4];
        v20 = sub_48d62f(v9);
        if (v20 != 0xffffffff)
        {
            if (v20 != v8)
            {
                v19 = sub_48d678(v8, v20, v5, &v6, 0, 0);
                if (v19)
                {
                    v5 = v19;
                    goto LABEL_483816;
                }
            }
            else
            {
LABEL_483816:
                GetStringTypeA(v9, v4, v5, v6, v7);
                if (!v19)
                    return;
                sub_46c941(v19);
                return;
            }
        }
    }
    else if (v12 == 1)
    {
        goto LABEL_483707;
    }
LABEL_4837ef:
    return;
}
