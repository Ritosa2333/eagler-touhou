// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41d9c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41d9c0_1 {
    char padding_0[8];
    struct st_41d9c0_0 *field_8;
    char padding_c[27740];
    int field_6c68;
    int field_6c6c;
    char padding_6c70[8];
    int field_6c78;
    char padding_6c7c[104];
    unsigned int field_6ce4;
    char padding_6ce8[48];
    unsigned int field_6d18;
    char padding_6d1c[20];
    int field_6d30;
    unsigned int field_6d34;
} st_41d9c0_1;

typedef struct st_41d9c0_0 {
    char padding_0[4];
    unsigned int field_4;
} st_41d9c0_0;

typedef struct st_41d9c0_3 {
    struct st_41d9c0_2 *field_0;
    struct st_41d9c0_3 *field_4;
} st_41d9c0_3;

typedef struct st_41d9c0_2 {
    struct st_41d9c0_2 *field_0;
    struct st_41d9c0_2 *field_4;
    char padding_8[12];
    struct st_41d9c0_2 *field_14;
    char padding_18[1124];
    unsigned int field_47c;
} st_41d9c0_2;

extern char g_4b0ce0;
extern char g_4b512c;
extern unsigned int g_4ce8a0;
extern unsigned int g_4ce8cc;

st_41d9c0_2 * sub_41d9c0(st_41d9c0_1 *idx)
{
    unsigned int v2;  // esi
    int v3;  // edi
    st_41d9c0_3 *node;  // eax
    int v4;  // esi
    int v5;  // ebp
    int v6;  // ebx
    int v7;  // esi
    st_41d9c0_0 *idx1;  // eax
    st_41d9c0_2 *iter;  // eax
    st_41d9c0_1 *iter1;  // esi
    st_41d9c0_2 *v11;  // ecx
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x8]

    if (!(g_4b0ce0 & 9))
    {
        v2 = &(&g_4b512c)[g_4ce8cc];
        if (*((int *)&(&g_4b512c)[g_4ce8cc]))
        {
            sub_4604e0();
            sub_46ca4f(*((int *)v2));
            *((unsigned int *)v2) = 0;
        }
    }
    else
    {
        sub_461be0(v3, v4, v5, v6);
    }
    v7 = idx->field_6d30;
    idx->field_6ce4 = 0;
    if (v7)
    {
        sub_41cd90();
        sub_46ca4f(v7);
        idx->field_6d30 = 0;
    }
    if (!(g_4b0ce0 & 9))
    {
        if (idx->field_6d34)
        {
            sub_46c941(idx->field_6d34);
            idx->field_6d34 = 0;
        }
        idx->field_6d34 = 0;
        g_4ce8a0 = 0;
    }
    else
    {
        g_4ce8a0 = idx->field_6d34;
    }
    idx1 = idx->field_8;
    if (idx1)
        idx1->field_4 = idx1->field_4 & 0xfffffffd;
    sub_461a70(idx->field_6c68);
    idx->field_6c68 = 0;
    sub_461a70(idx->field_6c6c);
    idx->field_6c6c = 0;
    sub_461a70(idx->field_6c78);
    iter = NULL;
    memset(&idx->field_6c78, 0, 44);
    iter1 = &idx->padding_c[27700];
    v0 = 10;
    do
    {
        v1 = v0;
        v11 = *((int *)&iter1->padding_0[0]);
        if (v11)
        {
            node = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (node->field_0->field_0 == v11)
                    {
                        iter = node->field_0;
                        goto LABEL_41db4c;
                    }
                } while ((node = (st_41d9c0_3 *)node->field_4, node));
            }
            if (*((int *)(g_4ce8cc + 8935104)))
            {
                for (; iter->field_0->field_0 != v11; iter = iter->field_4);
                iter = iter->field_0;
LABEL_41db4c:
                if (iter)
                {
                    iter->field_47c = iter->field_47c | 0x10000000;
                    if (!*((int *)&iter->padding_18[0]))
                    {
                        iter = iter->field_14;
                        if (iter)
                        {
                            do
                            {
                                iter->field_0->field_47c = iter->field_0->field_47c | 0x10000000;
                                iter = iter->field_4;
                            } while (iter);
                        }
                    }
                }
            }
        }
    } while ((*((unsigned int *)&iter1->padding_0[0]) = 0, iter1 += 4, v0 = v1 - 1, v1 != 1));
    idx->field_6d18 = idx->field_6d18 | 448;
    *((unsigned int *)&idx->padding_6ce8[12]) = 0;
    return iter;
}
