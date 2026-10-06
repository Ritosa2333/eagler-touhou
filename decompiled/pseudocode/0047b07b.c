// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47b07b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47b07b_3 {
    char padding_0[4];
    char field_4;
} st_47b07b_3;

typedef struct st_47b07b_0 {
    unsigned int field_0;
    char field_4;
    char field_5;
    char padding_6[2];
    unsigned int field_8;
    char padding_c[24];
    char field_24;
    char field_25;
    char field_26;
    char padding_27[13];
    char field_34;
    char padding_35[3];
    unsigned int field_38;
} st_47b07b_0;

typedef struct STARTUPINFOA {
    unsigned int cb;
    PSTR lpReserved;
    PSTR lpDesktop;
    PSTR lpTitle;
    unsigned int dwX;
    unsigned int dwY;
    unsigned int dwXSize;
    unsigned int dwYSize;
    unsigned int dwXCountChars;
    unsigned int dwYCountChars;
    unsigned int dwFillAttribute;
    enum STARTUPINFOW_FLAGS dwFlags;
    unsigned short wShowWindow;
    unsigned short cbReserved2;
    Byte *lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA;

typedef struct st_47b07b_2 {
    HANDLE field_0;
    char field_4;
    char padding_5[3];
    unsigned int field_8;
} st_47b07b_2;

extern char g_4aae38;
extern unsigned int g_4d6308;
extern void g_4d6320;

int sub_47b07b(void)
{
    STARTUPINFOA *v4;  // ebp
    st_47b07b_0 *iter;  // eax
    void* idx;  // esi
    int v15;  // ebx
    st_47b07b_2 *idx1;  // esi
    enum STD_HANDLE v17;  // eax
    HANDLE v18;  // edi
    enum FILE_TYPE v19;  // eax
    unsigned int v20;  // eax
    st_47b07b_0 *i;  // ecx
    st_47b07b_3 *v7;  // eax
    unsigned int j;  // edi
    unsigned int v9;  // ebx
    st_47b07b_0 *node;  // eax
    unsigned int *v11;  // ecx
    st_47b07b_0 *k;  // edx
    unsigned int v13;  // ecx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp+0x0]
    unsigned int v3;  // [bp+0x4]

    sub_46fcec(&g_4aae38, 84);
    *((unsigned int *)((char *)v4 - 4)) = 0;
    GetStartupInfoA((char *)v4 - 100);
    *((unsigned int *)((char *)v4 - 4)) = 0xfffffffe;
    v0 = 32;
    iter = sub_477813(32, 64);
    if (iter)
    {
        *((st_47b07b_0 **)&g_4d6320) = iter;
        g_4d6308 = 32;
        for (i = &iter[0x22].field_8; iter < i; i = *((int *)&g_4d6320) + 0x800)
        {
            iter->field_4 = 0;
            iter->field_0 = 0xffffffff;
            iter->field_5 = 10;
            iter->field_8 = 0;
            iter->field_24 = 0;
            iter->field_25 = 10;
            iter->field_26 = 10;
            iter->field_38 = 0;
            iter->field_34 = 0;
            iter = &iter[1].field_4;
        }
        if (*((short *)((char *)v4 - 50)))
        {
            v7 = *((int *)((char *)v4 - 48));
            if (v7)
            {
                j = (unsigned int)v7->padding_0;
                v9 = &v7->field_4;
                *((unsigned int *)((char *)v4 - 28)) = v9 + j;
                if (j >= 0x800)
                    j = 0x800;
                for (*((unsigned int *)((char *)v4 - 32)) = 1; g_4d6308 < j; *((unsigned int *)((char *)v4 - 32)) = *((int *)((char *)v4 - 32)) + 1)
                {
                    node = sub_477813(32, 64);
                    if (node)
                    {
                        v11 = &(&g_4d6320)[4 * *((int *)((char *)v4 - 32))];
                        *(v11) = node;
                        g_4d6308 = g_4d6308 + 32;
                        for (k = &node[0x22].field_8; node < k; k = *(v11) + 0x800)
                        {
                            node->field_4 = 0;
                            node->field_0 = 0xffffffff;
                            node->field_5 = 10;
                            node->field_8 = 0;
                            node->field_24 = node->field_24 & 128;
                            node->field_25 = 10;
                            node->field_26 = 10;
                            node->field_38 = 0;
                            node->field_34 = 0;
                            node = &node[1].field_4;
                        }
                    }
                    else
                    {
                        j = g_4d6308;
                        break;
                    }
                }
                *((unsigned int *)((char *)v4 - 32)) = 0;
                if (j > NULL)
                {
                    do
                    {
                        v13 = *((int *)*((int *)((char *)v4 - 28)));
                        if (v13 == 0xffffffff || v13 == 0xfffffffe || !(*((char *)v9) & 1) || !(*((char *)v9) & 8) && !GetFileType(v13))
                            continue;
                        idx = (*((int *)((char *)v4 - 32)) & 31) * 64 + *((int *)&(&g_4d6320)[4 * (*((int *)((char *)v4 - 32)) >> 5)]);
                        *((int *)idx) = *((int *)*((int *)((char *)v4 - 28)));
                        *((char *)&idx[4]) = *((char *)v9);
                        if (!sub_47b416(idx + 12, 4000))
                            goto LABEL_47b2c6;
                        *((unsigned int *)&idx[8]) = (int)idx[8] + 1;
                    } while ((*((unsigned int *)((char *)v4 - 32)) = *((int *)((char *)v4 - 32)) + 1, v9 += 1, *((unsigned int *)((char *)v4 - 28)) = *((int *)((char *)v4 - 28)) + 4, *((int *)((char *)v4 - 32)) < j));
                }
            }
        }
        v15 = 0;
        do
        {
            idx1 = v15 * 64 + *((int *)&g_4d6320);
            if (idx1->field_0 != 0xffffffff && idx1->field_0 != 0xfffffffe)
            {
                idx1->field_4 = idx1->field_4 | 128;
            }
            else
            {
                idx1->field_4 = 129;
                if (!v15)
                {
                    v1 = 0xfffffff6;
                    v17 = STD_INPUT_HANDLE;
                }
                else
                {
                    v17 = (enum STD_HANDLE)(-(0 < v15 - 1) - 11);
                }
                v18 = GetStdHandle(v17);
                if (v18 != 0xffffffff && v18 && !(v19 = GetFileType(v18), !v19))
                {
                    idx1->field_0 = v18;
                    v20 = (unsigned int)(v19 & 0xff);
                    if (v20 == 2)
                    {
                        idx1->field_4 = idx1->field_4 | 64;
                    }
                    else if (v20 == 3)
                    {
                        idx1->field_4 = idx1->field_4 | 8;
                    }
                    if (!sub_47b416(idx1 + 1, 4000))
                        goto LABEL_47b2c6;
                    idx1->field_8 = idx1->field_8 + 1;
                }
                else
                {
                    idx1->field_4 = idx1->field_4 | 64;
                    idx1->field_0 = 0xfffffffe;
                }
            }
        } while ((v15 = (int)(v15 + 1), v15 < 3));
        SetHandleCount(g_4d6308);
    }
    else
    {
LABEL_47b2c6:
    }
    return (unsigned int)sub_46fd31(&g_4aae38, 84, v2, v3);
}
