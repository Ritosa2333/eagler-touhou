// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402870; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_402870_0 {
    unsigned int field_0;
    char padding_4[4];
    int field_8;
    int field_c;
    int field_10;
    int field_14;
} st_402870_0;

typedef struct st_402870_3 {
    struct st_402870_1 *field_0;
} st_402870_3;

typedef struct st_402870_4 {
    struct st_402870_2 *field_0;
    struct st_402870_4 *field_4;
} st_402870_4;

typedef struct st_402870_1 {
    char padding_0[1148];
    unsigned int field_47c;
} st_402870_1;

typedef struct st_402870_2 {
    char padding_0[20];
    struct st_402870_3 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_402870_2;

extern unsigned int g_4ce8cc;

void sub_402870(void)
{
    st_402870_0 *idx;  // esi
    unsigned int v3;  // edx
    unsigned int v12;  // eax
    unsigned int v4;  // ebx
    unsigned int v5;  // ebx
    unsigned int v6;  // ecx
    st_402870_4 *iter;  // eax
    st_402870_2 *v8;  // ebp
    st_402870_4 *node;  // eax
    st_402870_2 *index;  // eax
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0x8]

    if (idx->field_10)
    {
        sub_46c941(idx->field_10);
        idx->field_10 = 0;
    }
    if (idx->field_14)
    {
        sub_46c941(idx->field_14);
        idx->field_14 = 0;
    }
    v3 = 0;
    if (idx->field_0 - 1 > 0)
    {
        v0 = v4;
        do
        {
            v5 = idx->field_8 + v3 * 4;
            v6 = *((int *)v5);
            if (v6)
            {
                iter = *((int *)(g_4ce8cc + 8935096));
                if (*((int *)(g_4ce8cc + 8935096)))
                {
                    do
                    {
                        v8 = iter->field_0;
                        if (*((int *)&v8->padding_0[0]) == v6)
                            goto LABEL_4028f0;
                    } while ((iter = (st_402870_4 *)iter->field_4, iter));
                }
                node = *((int *)(g_4ce8cc + 8935104));
                if (*((int *)(g_4ce8cc + 8935104)))
                {
                    while (1)
                    {
                        v8 = node->field_0;
                        if (*((int *)&v8->padding_0[0]) == v6)
                            break;
                        node = node->field_4;
                    }
LABEL_4028f0:
                    index = v8;
                    if (index)
                    {
                        index->field_47c = index->field_47c | 0x10000000;
                        if (!index->field_18)
                        {
                            v11 = index->field_14;
                            if (index->field_14)
                            {
                                do
                                {
                                    v12 = v11;
                                    *((unsigned int *)(*((int *)v12) + 1148)) = *((int *)(*((int *)v12) + 1148)) | 0x10000000;
                                    v11 = *((int *)(v12 + 4));
                                } while (*((int *)(v12 + 4)));
                            }
                        }
                    }
                }
            }
        } while ((*((unsigned int *)v5) = 0, v3 += 1, v3 < idx->field_0 - 1));
    }
    if (idx->field_8)
    {
        sub_46c941(idx->field_8);
        idx->field_8 = 0;
    }
    if (idx->field_c)
    {
        sub_46c941(idx->field_c);
        idx->field_c = 0;
    }
    return;
}
