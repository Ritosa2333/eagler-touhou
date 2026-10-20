// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473ac5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct CPINFO {
    unsigned int MaxCharSize;
    Byte DefaultChar[2];
    Byte LeadByte[12];
} CPINFO;

typedef struct st_473ac5_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[4];
    unsigned int field_c;
    unsigned short field_10;
    char padding_12[12];
    char field_1e;
} st_473ac5_0;

typedef struct st_473ac5_1 {
    char field_0;
    char padding_1[5];
    char field_6;
    char field_7;
} st_473ac5_1;

extern char g_4ad8dc;
extern char g_4ad8e0;
extern char g_4ad8e4;
extern char g_4ad8f0;
extern unsigned int g_4b40c0;

int sub_473ac5(unsigned int a0, st_473ac5_0 *idx)
{
    int v6;  // edi
    unsigned int v7;  // edi
    unsigned int v17;  // edx
    st_473ac5_0 *v18;  // edi
    st_473ac5_0 *v19;  // edi
    unsigned int v20;  // eax
    char *v21;  // esi
    unsigned int l;  // edi
    unsigned int v23;  // eax
    st_473ac5_0 *iter;  // eax
    unsigned int v8;  // eax
    unsigned int v26;  // ecx
    unsigned int v27;  // ecx
    unsigned int j;  // edx
    unsigned int v30;  // eax
    st_473ac5_1 *node;  // esi
    char v10;  // cl
    unsigned int k;  // eax
    st_473ac5_0 *iter1;  // eax
    unsigned int v13;  // ecx
    unsigned int i;  // ecx
    unsigned int v0;  // [bp-0x34]
    int v1;  // [bp-0x2c]
    int v2;  // [bp-0x28]
    unsigned int v3;  // [bp-0x24]
    char *iter2;  // [bp-0x20], Other Possible Types: unsigned int
    CPINFO v5;  // [bp-0x1c]

    v7 = sub_473a49(v6, v1, v2);
    a0 = v7;
    if (v7)
    {
        iter2 = 0;
        v8 = 0;
        do
        {
            if (*((int *)&(&g_4ad8e0)[v8]) == v7)
            {
                sub_477420(&idx->padding_12[10], 0, 0x101);
                v3 = 0;
                v21 = &(&g_4ad8f0)[48 * iter2];
                iter2 = v21;
                while (1)
                {
                    if (*(v21) && v21[1])
                    {
                        l = *(v21);
                        for (v23 = v21[1]; l <= v23; l += 1)
                        {
                            idx->padding_12[11 + l] = idx->padding_12[11 + l] | (&g_4ad8dc)[v3];
                            v23 = v21[1];
                        }
                        v21 += 2;
                        v7 = a0;
                    }
                    else
                    {
                        v3 += 1;
                        v21 = iter2 + 8;
                        iter2 = v21;
                        if (v3 >= 4)
                            break;
                    }
                }
                idx->field_4 = v7;
                *((unsigned int *)&idx->padding_8[0]) = 1;
                v0 = 6;
                idx->field_c = sub_47377f();
                iter = &idx->field_10;
                v27 = &(&g_4ad8e4)[v26];
                do
                {
                    j = v0;
                    *((short *)&iter->padding_0[0]) = *((short *)v27);
                    v27 += 2;
                    iter = &iter->padding_0[2];
                    v0 = j - 1;
                } while (j != 1);
            }
        } while ((iter2 += 1, v8 += 48, v8 < 240));
        if (v7 == 65000 || v7 == 65001 || !IsValidCodePage((unsigned short)v7))
            return v20;
        if (GetCPInfo(v7, &v5))
        {
            sub_477420(&idx->padding_12[10], 0, 0x101);
            idx->field_4 = v7;
            idx->field_c = 0;
            if (v5.MaxCharSize > 1)
            {
                if (*(&v5.LeadByte[0]))
                {
                    node = &v5.LeadByte[1];
                    do
                    {
                        v10 = node->field_0;
                        if (!node->field_0)
                            break;
                        for (k = *(&node->field_0 - 1); k <= v10; k += 1)
                        {
                            idx->padding_12[11 + k] = idx->padding_12[11 + k] | 4;
                        }
                        node = &node->padding_1[1];
                    } while (*(&node->field_0 - 1));
                }
                iter1 = &idx->field_1e;
                v13 = 254;
                do
                {
                    i = v13;
                    iter1->padding_0[0] = iter1->padding_0[0] | 8;
                    iter1 = &iter1->padding_0[1];
                    v13 = i - 1;
                } while (i != 1);
                idx->field_c = sub_47377f();
                *((unsigned int *)&idx->padding_8[0]) = v17;
            }
            else
            {
                *((unsigned int *)&idx->padding_8[0]) = 0;
            }
            v18 = &idx->field_10;
            *((unsigned int *)&v18->padding_0[0]) = 0;
            v19 = &v18->field_4;
            *((unsigned int *)&v19->padding_0[0]) = 0;
            v19->field_4 = 0;
            sub_473812();
            return v30;
        }
        else if (!g_4b40c0)
        {
            return v20;
        }
    }
    sub_4737ae();
    return v30;
}
