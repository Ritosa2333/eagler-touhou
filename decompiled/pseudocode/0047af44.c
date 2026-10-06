// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47af44; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b42e8;

int sub_47af44(void)
{
    unsigned int v6;  // eax
    unsigned int v7;  // edi
    unsigned int v8;  // ebx
    unsigned short *v9;  // edi
    unsigned short *iter;  // eax
    char *node;  // eax
    char *v12;  // esi
    unsigned int v13;  // edi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    int v2;  // [bp-0x10]
    int v3;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x8]

    v6 = g_4b42e8;
    v1 = v7;
    v8 = 0;
    v9 = NULL;
    if (!v6)
    {
        v9 = GetEnvironmentStringsW();
        if (v9)
        {
            g_4b42e8 = 1;
LABEL_47af99:
            if (v9 || !(v9 = (unsigned short *)GetEnvironmentStringsW(), !v9))
            {
                iter = v9;
                if (*(v9))
                {
                    while (1)
                    {
                        iter += 1;
                        if (*(iter))
                            continue;
                        iter += 1;
                        if (!*(iter))
                            break;
                    }
                }
                v2 = ((int)(iter - v9) >> 1) + 1;
                v3 = WideCharToMultiByte(0, 0, v9, v2, NULL, 0, NULL, NULL);
                if (v3)
                {
                    v4 = sub_4777ce(v3);
                    if (v4)
                    {
                        if (!WideCharToMultiByte(0, 0, v9, v2, v4, v3, NULL, NULL))
                        {
                            sub_46c941(v4);
                            v4 = 0;
                        }
                        v8 = v4;
                    }
                }
                FreeEnvironmentStringsW(v9);
                return v8;
            }
            goto LABEL_47afa5;
        }
        else if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
        {
            v0 = 2;
            v6 = 2;
            g_4b42e8 = 2;
        }
        else
        {
            v6 = g_4b42e8;
        }
    }
    if (v6 == 1)
        goto LABEL_47af99;
    if (v6 == 2 || !v6)
    {
        node = GetEnvironmentStrings();
        v12 = node;
        if (v12)
        {
            if (*(v12))
            {
                while (1)
                {
                    node += 1;
                    if (*(node))
                        continue;
                    node += 1;
                    if (!*(node))
                        break;
                }
            }
            v3 = node - v12 + 1;
            v13 = sub_4777ce(v3);
            if (v13)
            {
                sub_47a330(v13, v12, v3);
                FreeEnvironmentStringsA(v12);
                return v13;
            }
            FreeEnvironmentStringsA(v12);
        }
    }
LABEL_47afa5:
    return 0;
}
