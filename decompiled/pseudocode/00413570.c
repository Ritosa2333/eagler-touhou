// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x413570; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_413570_4 {
    struct st_413570_1 *field_0;
    char padding_4[116];
    char field_78;
    char padding_79[2611];
    char field_aac;
    char padding_aad[32467];
    unsigned int field_8980;
    char field_8984;
} st_413570_4;

typedef struct st_413570_11 {
    struct st_413570_8 *field_0;
} st_413570_11;

typedef struct st_413570_9 {
    struct st_413570_10 *field_0;
    struct st_413570_9 *field_4;
} st_413570_9;

typedef struct st_413570_13 {
    char padding_0[4];
    unsigned int field_4;
} st_413570_13;

typedef struct st_413570_0 {
    unsigned int field_0;
    struct st_413570_0 *field_4;
} st_413570_0;

typedef struct st_413570_1 {
    char padding_0[4148];
    struct st_413570_0 *field_1034;
    char padding_1038[5824];
    unsigned int field_26f8;
    char padding_26fc[8];
    unsigned int field_2704;
    char padding_2708[136];
    unsigned int field_2790;
} st_413570_1;

typedef struct st_413570_8 {
    char padding_0[1148];
    unsigned int field_47c;
} st_413570_8;

typedef struct st_413570_10 {
    char padding_0[20];
    struct st_413570_11 *field_14;
    char padding_18[1124];
    unsigned int field_47c;
} st_413570_10;

extern char g_49fc34;
extern char g_49fc50;
extern unsigned int g_4ad138;
extern unsigned int g_4b43dc;
extern st_413570_4 *g_4b4514;
extern unsigned int g_4ce8cc;

st_413570_4 * sub_413570(st_413570_1 *a0)
{
    unsigned long v8;  // ldt
    unsigned long v9;  // gdt
    st_413570_9 *iter;  // eax
    unsigned int v19;  // ecx
    st_413570_0 *node;  // eax
    unsigned int idx;  // eax
    unsigned int v22;  // eax
    unsigned int v23;  // eax
    st_413570_4 *iter1;  // eax
    st_413570_1 *index;  // edi
    unsigned int v27;  // ecx
    unsigned short v10;  // fs
    unsigned int v28;  // ecx
    st_413570_13 *v29;  // esi
    st_413570_13 *v30;  // esi
    unsigned long long v31;  // 4122
    unsigned long long v11;  // 4142
    unsigned int v12;  // eax
    unsigned long long v13;  // 4166
    st_413570_1 *v14;  // esi
    int v15;  // ecx
    st_413570_1 *iter2;  // esi
    unsigned int v17;  // edx
    char v0;  // [bp-0x20]
    int v1;  // [bp-0x1c]
    int v2;  // [bp-0x18]
    int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x4]

    v7 = 0xffffffff;
    v11 = _ccall(v8, v9, v10, 0);
    v12 = *((int *)(unsigned int)v11);
    v6 = *((int *)(unsigned int)v11);
    v13 = _ccall(v8, v9, v10, 0);
    *((unsigned int **)(unsigned int)v13) = &v6;
    v14 = a0;
    *((char **)&v14->padding_0[0]) = &g_49fc50;
    sub_412be0(g_4ad138 ^ &v0, v0, v1, v2, v3, v15, v12, sub_497408, 0);
    if (v14->field_26f8 & 0x400000)
        *((unsigned int *)(g_4b43dc + v14->field_2704 * 4 + 28)) = 0;
    iter2 = &v14->padding_1038[232];
    v4 = 16;
    do
    {
        v5 = v4;
        v17 = *((int *)&iter2->padding_0[0]);
        if (v17)
        {
            iter = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    v19 = iter->field_0;
                    if (*((int *)v19) == v17)
                        goto LABEL_413617;
                } while ((iter = (st_413570_9 *)iter->field_4, iter));
            }
            node = *((int *)(g_4ce8cc + 8935104));
            if (*((int *)(g_4ce8cc + 8935104)))
            {
                while (1)
                {
                    v19 = node->field_0;
                    if (*((int *)v19) == v17)
                        break;
                    node = node->field_4;
                }
LABEL_413617:
                idx = v19;
                if (idx)
                {
                    *((unsigned int *)(idx + 1148)) = *((int *)(idx + 1148)) | 0x10000000;
                    if (!*((int *)(idx + 24)))
                    {
                        v22 = *((int *)(idx + 20));
                        if (*((int *)(idx + 20)))
                        {
                            do
                            {
                                v23 = v22;
                                *((unsigned int *)(*((int *)v23) + 1148)) = *((int *)(*((int *)v23) + 1148)) | 0x10000000;
                                v22 = *((int *)(v23 + 4));
                            } while (*((int *)(v23 + 4)));
                        }
                    }
                }
            }
        }
    } while ((*((unsigned int *)&iter2->padding_0[0]) = 0, iter2 += 4, v4 = v5 - 1, v5 != 1));
    iter1 = g_4b4514;
    index = a0;
    if (g_4b4514)
    {
        if (g_4b4514->field_8980 == index)
        {
            g_4b4514->field_8980 = 0;
            g_4b4514->field_8984 = 0;
        }
        iter1 = &g_4b4514->field_aac;
        v27 = 0x100;
        do
        {
            v28 = v27;
            if (iter1->field_0 == index)
                iter1->field_0 = NULL;
        } while ((iter1 += 120, v27 = v28 - 1, v28 != 1));
    }
    if (index->field_2790)
    {
        sub_402870();
        iter1 = sub_46ca4f(index->field_2790);
    }
    v7 = 0xffffffff;
    v29 = index->field_1034;
    index->field_2790 = 0;
    *((char **)&index->padding_0[0]) = &g_49fc34;
    if (v29)
    {
        do
        {
            v30 = v29;
            sub_46ca4f(v30->padding_0);
            iter1 = sub_46ca4f(v30);
            v29 = v30->field_4;
        } while (v30->field_4);
    }
    v31 = _ccall(v8, v9, v10, 0);
    *((unsigned int *)(unsigned int)v31) = v12;
    return iter1;
}
