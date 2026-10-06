// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4385b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4385b0_18 {
    char padding_0[33376];
    unsigned int field_8260;
    char padding_8264[172];
    int field_8310;
} st_4385b0_18;

typedef struct st_4385b0_3 {
    struct st_4385b0_1 *field_0;
} st_4385b0_3;

typedef struct st_4385b0_0 {
    char padding_0[2440];
    unsigned int field_988;
    unsigned int field_98c;
    char padding_990[30928];
    unsigned int field_8260;
    char padding_8264[88];
    unsigned int field_82bc;
    unsigned int field_82c0;
    char padding_82c4[76];
    unsigned int field_8310;
    char padding_8314[28];
    int field_8330;
    unsigned int field_8334;
    char padding_8338[224];
    unsigned int field_8418;
    char padding_841c[224];
    unsigned int field_84fc;
    char padding_8500[224];
    unsigned int field_85e0;
    char padding_85e4[224];
    unsigned int field_86c4;
    char padding_86c8[224];
    unsigned int field_87a8;
    char padding_87ac[224];
    unsigned int field_888c;
    char padding_8890[224];
    unsigned int field_8970;
    char padding_8974[15016];
    unsigned int field_c41c;
    char padding_c420[108];
    unsigned int field_c48c;
} st_4385b0_0;

typedef struct st_4385b0_4 {
    struct st_4385b0_2 *field_0;
    struct st_4385b0_4 *field_4;
} st_4385b0_4;

typedef struct st_4385b0_1 {
    char padding_0[1148];
    unsigned int field_47c;
} st_4385b0_1;

typedef struct st_4385b0_2 {
    char padding_0[20];
    struct st_4385b0_3 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_4385b0_2;

extern unsigned int g_438d30[4];
extern unsigned int g_438d48[4];
extern int g_4b0c48;
extern int g_4b0c90;
extern unsigned int g_4b0c94;
extern char g_4b0cd0;
extern int g_4b0cd4;
extern char g_4b31d8;
extern unsigned int g_4ce8cc;

int sub_4385b0(st_4385b0_0 *idx)
{
    int v6;  // ecx
    unsigned int v7;  // ebx
    st_4385b0_1 **v16;  // eax
    st_4385b0_0 *v18;  // edi
    unsigned int v19;  // esi
    unsigned int j;  // esi
    int v21;  // ebp
    int k;  // edi
    st_4385b0_0 *iter;  // esi
    unsigned int v24;  // ecx
    st_4385b0_4 *node;  // eax
    int v8;  // eax
    st_4385b0_2 *v26;  // edx
    st_4385b0_4 *iter1;  // eax
    st_4385b0_2 *index;  // eax
    st_4385b0_4 *v29;  // eax
    st_4385b0_4 *v30;  // eax
    int v32;  // esi
    unsigned int v33;  // eax
    st_4385b0_18 *iter2;  // esi
    unsigned int l;  // edi
    int v9;  // esi
    unsigned int v36;  // edi
    st_4385b0_0 *v10;  // ebx
    unsigned int m;  // ecx
    st_4385b0_4 *v12;  // eax
    st_4385b0_2 *idx1;  // eax
    st_4385b0_4 *v14;  // eax
    st_4385b0_1 **v15;  // eax
    int v0;  // [bp-0x54]
    unsigned int v1;  // [bp-0x3c]
    int v2;  // [bp-0x38]
    int v3;  // [bp-0x34]
    int i;  // [bp-0x34]
    unsigned int v5;  // [bp-0x8]

    v6 = g_4b0c48;
    v1 = v7;
    idx = &(&g_4b31d8)[128 * g_4b0c90 + 64 * g_4b0c94];
    v8 = g_4b0c48 / g_4b0cd4;
    v9 = v8;
    v2 = v9;
    if (g_4b0c48 < *((int *)&g_4b0cd0))
    {
        v18 = idx->padding_8314;
        v19 = 8;
        do
        {
            j = v19;
            v8 = sub_461970(*((int *)&v18->padding_0[0]));
            v18 = &v18->padding_0[228];
            v19 = j - 1;
        } while (j != 1);
        v9 = v21;
    }
    else if (v9 > 0)
    {
        v10 = idx->padding_8314;
        v3 = v9;
        do
        {
            i = v3;
            m = *((int *)&v10->padding_0[0]);
            if (m)
            {
                v12 = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)))
                {
                    do
                    {
                        if (*((int *)&v12->field_0->padding_0[0]) == m)
                        {
                            idx1 = v12->field_0;
                            goto LABEL_43864c;
                        }
                    } while ((v12 = (st_4385b0_4 *)v12->field_4, v12));
                }
                v14 = *((int *)(g_4ce8cc + 8935104));
                if (*((int *)(g_4ce8cc + 8935104)))
                {
                    while (*((int *)&v14->field_0->padding_0[0]) != m)
                    {
                        if (!v14->field_4)
                            goto LABEL_43867f;
                    }
                    idx1 = v14->field_0;
LABEL_43864c:
                    if (idx1)
                    {
                        idx1->field_47c = idx1->field_47c | 0x10000000;
                        if (!idx1->field_18)
                        {
                            v15 = idx1->field_14;
                            if (idx1->field_14)
                            {
                                do
                                {
                                    v16 = v15;
                                    *(v16)->field_47c = *(v16)->field_47c | 0x10000000;
                                    v15 = v16[1];
                                } while (v16[1]);
                            }
                        }
                    }
                }
            }
LABEL_43867f:
            *((unsigned int *)&v10->padding_0[0]) = 0;
            v6 = g_4b0c90;
            v8 = g_4b0c94 + g_4b0c90 * 2;
            if (v8 <= 5)
                goto *((void *)(g_438d30[v8]));
            v10 = &v10->padding_0[228];
            v3 = i - 1;
        } while (i != 1);
    }
    if (!idx->field_c41c)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = v6;
        /* unsupported instruction */
        v8 = sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        if (/* unsupported instruction */)
        {
            idx->field_c48c = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_c48c = nan;
            /* unsupported instruction */
        }
    }
    if (idx->field_c41c == v9)
        return v8;
    k = 0;
    if (v9 > 0)
    {
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        iter = &idx->padding_82c4[12];
        do
        {
            *((unsigned int *)(&iter->padding_0[0] - 20)) = idx->field_988;
            *((unsigned int *)(&iter->padding_0[0] - 16)) = idx->field_98c;
            v24 = *((int *)&iter->padding_0[64]);
            if (v24)
            {
                node = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)))
                {
                    do
                    {
                        v26 = node->field_0;
                        if (*((int *)&v26->padding_0[0]) == v24)
                            goto LABEL_4387d0;
                    } while ((node = (st_4385b0_4 *)node->field_4, node));
                }
                iter1 = *((int *)(g_4ce8cc + 8935104));
                if (*((int *)(g_4ce8cc + 8935104)))
                {
                    while (1)
                    {
                        v26 = iter1->field_0;
                        if (*((int *)&v26->padding_0[0]) == v24)
                            break;
                        iter1 = iter1->field_4;
                        if (!iter1)
                            goto LABEL_4387fd;
                    }
LABEL_4387d0:
                    index = v26;
                    if (index)
                    {
                        index->field_47c = index->field_47c | 0x10000000;
                        if (!index->field_18)
                        {
                            v29 = index->field_14;
                            if (index->field_14)
                            {
                                do
                                {
                                    v30 = v29;
                                    v30->field_0->field_47c = v30->field_0->field_47c | 0x10000000;
                                    v29 = v30->field_4;
                                } while (v30->field_4);
                            }
                        }
                    }
                }
            }
LABEL_4387fd:
            *((unsigned int *)&iter->padding_0[64]) = 0;
            *((int *)&iter->padding_0[96]) = k;
            if (g_4b0c94 + g_4b0c90 * 2 <= 5)
                goto *((void *)(g_438d48[2 * g_4b0c90 + g_4b0c94]));
            *((unsigned int *)(&iter->padding_0[0] - 112)) = 2;
            k += 1;
            iter = &iter->padding_0[228];
        } while (k < v32);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (k >= 8)
        {
            idx->field_c41c = v36;
            idx->field_8334 = 1;
            idx->field_8418 = 1;
            idx->field_84fc = 1;
            idx->field_85e0 = 1;
            idx->field_86c4 = 1;
            idx->field_87a8 = 1;
            idx->field_888c = 1;
            idx->field_8970 = 1;
            return 1;
        }
    }
    v33 = 8 - k;
    iter2 = &idx->padding_0[228 * k + 33552];
    v5 = v33;
    do
    {
        l = v33;
        *((unsigned int *)(&iter2->padding_0[0] - 176)) = 0;
        sub_461970(*((int *)&iter2->padding_0[0]));
        iter2 = &iter2->padding_0[228];
        v33 = l - 1;
    } while (l != 1);
    idx->field_c41c = v36;
    idx->field_8334 = 1;
    idx->field_8418 = 1;
    idx->field_84fc = 1;
    idx->field_85e0 = 1;
    idx->field_86c4 = 1;
    idx->field_87a8 = 1;
    idx->field_888c = 1;
    idx->field_8970 = 1;
    return 1;
}
