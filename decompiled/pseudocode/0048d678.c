// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d678; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CPINFO {
    unsigned int MaxCharSize;
    Byte DefaultChar[2];
    Byte LeadByte[12];
} CPINFO;


int sub_48d678(unsigned int a0, unsigned int a1, unsigned int a2, int *a3, PSTR a4, int a5)
{
    unsigned int v6;  // edi
    int v7;  // esi
    int v9;  // eax
    unsigned int *iter;  // eax
    int v11;  // eax
    unsigned int v12;  // eax
    unsigned int v13;  // eax
    unsigned int v0;  // [bp-0x44]
    unsigned int v1;  // [bp-0x30]
    int v2;  // [bp-0x28]
    PSTR v3;  // [bp-0x24], Other Possible Types: unsigned int
    unsigned int *v4;  // [bp-0x20]
    CPINFO v5;  // [bp-0x1c]

    v2 = *(a3);
    v0 = v6;
    v3 = 0;
    v1 = 0;
    if (a0 == a1)
        return v13;
    if (GetCPInfo(a0, &v5) && v5.MaxCharSize == 1 && GetCPInfo(a1, &v5) && v5.MaxCharSize == 1)
    {
        v7 = v2;
        v1 = 1;
        if (v7 == -0x1)
            v7 = sub_475860(a2) + 1;
LABEL_48d762:
        v4 = NULL;
    }
    else
    {
        v7 = MultiByteToWideChar(a0, MB_PRECOMPOSED, a2, v2, NULL, 0);
        if (!v7)
            return v12;
        if (v7 <= 0 || v7 > 0x7ffffff0)
            goto LABEL_48d762;
        v9 = v7 * 2 + 8;
        if (v9 <= 0x400)
        {
            sub_48d830();
            iter = &v0;
            if (!&v0)
                goto LABEL_48d75d;
            v0 = 0xcccc;
            goto LABEL_48d75a;
        }
        else
        {
            iter = sub_46d04a(v9);
            if (iter)
            {
                *(iter) = 0xdddd;
LABEL_48d75a:
                iter += 2;
            }
        }
LABEL_48d75d:
        v4 = iter;
    }
    if (!v4)
        return v12;
    sub_477420(v4, 0, v7 * 2);
    if (MultiByteToWideChar(a0, MB_PRECOMPOSED, a2, v2, v4, v7))
    {
        if (a4)
        {
            if (WideCharToMultiByte(a1, 0, v4, v7, a4, a5, NULL, NULL))
                v3 = a4;
        }
        else
        {
            if (v1 || (v7 = WideCharToMultiByte(a1, 0, v4, v7, NULL, 0, NULL, NULL), v7))
            {
                v3 = sub_477813(1, v7);
                if (v3)
                {
                    v11 = WideCharToMultiByte(a1, 0, v4, v7, v3, v7, NULL, NULL);
                    if (!v11)
                    {
                        sub_46c941(v3);
                        v3 = 0;
                    }
                    else if (v2 != -0x1)
                    {
                        *(a3) = v11;
                    }
                }
            }
        }
    }
    sub_48326a(v4);
    return v13;
}
