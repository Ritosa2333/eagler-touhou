// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x486001; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_49f018;
extern int g_49f220;

unsigned int sub_486001(char *a0, unsigned short *index, unsigned int a2)
{
    int v3;  // edi
    void* idx;  // esi
    char *v5;  // eax
    void* v6;  // edi
    char *v7;  // eax
    char *v8;  // edi
    unsigned int v9;  // eax
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    char v2;  // [bp+0x0]

    idx = sub_475467(v3, v0, v1, &v2) + 156;
    if (!a0)
    {
        *((unsigned int *)&idx[8]) = (int)idx[8] | 260;
        goto LABEL_4860e6;
    }
    else
    {
        v5 = a0 + 64;
        v6 = idx + 4;
        *((char **)idx) = a0;
        *((char **)v6) = v5;
        if (v5 && *(v5))
            sub_485a06(&g_49f220, 22, v6);
        v7 = *((int *)idx);
        *((unsigned int *)&idx[8]) = 0;
        if (v7 && *(v7))
        {
            if (*((int *)v6) && *((char *)*((int *)v6)))
                sub_485f5e();
            else
                sub_485fc5();
            if ((int)idx[8])
                goto LABEL_4860fc;
            if (sub_485a06(&g_49f018, 64, idx))
            {
                if (*((int *)v6) && *((char *)*((int *)v6)))
                    sub_485f5e();
                else
                    sub_485fc5();
            }
        }
        else
        {
            v8 = *((int *)v6);
            if (v8 && *(v8))
            {
                *((unsigned int *)&idx[20]) = -(0 < sub_475860(v8) - 3) + 1;
                EnumSystemLocalesA(sub_485b93, 1);
                if (!((char)idx[8] & 4))
                    *((unsigned int *)&idx[8]) = 0;
            }
            else
            {
                *((unsigned int *)&idx[8]) = 260;
LABEL_4860e6:
                v9 = GetUserDefaultLCID();
                *((unsigned int *)&idx[24]) = v9;
                *((unsigned int *)&idx[28]) = v9;
            }
        }
        if (!(int)idx[8])
            return 0;
LABEL_4860fc:
        a0 = sub_485a7c();
        if (!a0 || a0 == 65000 || a0 == 65001 || !IsValidCodePage((unsigned short)a0) || !IsValidLocale((int)idx[24], LCID_INSTALLED))
            return 0;
        if (index)
        {
            *(index) = (short)idx[24];
            index[1] = (short)idx[28];
            index[2] = a0;
        }
        if (!a2)
            return 1;
        if (*(index) == 2068)
        {
            if (sub_47a2b9(a2, 64, "Norwegian-Nynorsk"))
                sub_470dc6(0, 0, 0, 0, 0);
LABEL_4861bf:
            if (GetLocaleInfoA((int)idx[28], 4098, a2 + 64, 64))
            {
                sub_48da89(a0, a2 + 128, 16, 10);
                return 1;
            }
        }
        else if (!(!GetLocaleInfoA((int)idx[24], 4097, a2, 64)))
        {
            goto LABEL_4861bf;
        }
        return 0;
    }
}
