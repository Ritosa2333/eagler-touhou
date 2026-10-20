// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x483f5e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_483f5e_2 {
    char padding_0[24];
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[30];
    unsigned short field_3e;
    char padding_40[112];
    int field_b0;
    int field_b4;
    char padding_b8[4];
    struct st_483f5e_0 *field_bc;
} st_483f5e_2;

typedef struct st_483f5e_1 {
    char field_0;
    char field_1;
} st_483f5e_1;

typedef struct st_483f5e_0 {
    unsigned int field_0;
    unsigned int field_4;
    struct st_483f5e_1 *field_8;
} st_483f5e_0;

extern st_483f5e_0 g_4adf68;
extern unsigned int g_4adf6c;
extern st_483f5e_1 *g_4adf70;

unsigned int sub_483f5e(st_483f5e_2 *idx)
{
    unsigned int v9;  // edi
    st_483f5e_0 *node;  // edi
    st_483f5e_0 *i;  // esi
    unsigned int v12;  // esi
    st_483f5e_0 *v13;  // esi
    unsigned int v14;  // edi
    char *iter;  // eax
    char v16;  // cl
    char *v17;  // edi
    st_483f5e_0 *index;  // eax
    unsigned int v0;  // [bp-0x2c]
    unsigned int v1;  // [bp-0x2c]
    unsigned int v2;  // [bp-0x28]
    st_483f5e_2 *v3;  // [bp-0x1c]
    unsigned int v4;  // [bp-0x18]
    st_483f5e_0 *v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]
    int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]

    v2 = v9;
    v3 = idx;
    v4 = 0;
    if (!idx->field_1c && !idx->field_18)
    {
        v8 = 0;
        v7 = 0;
        idx = &g_4adf68.field_0;
        goto LABEL_4840ca;
    }
    node = sub_477813(1, 48);
    idx = node;
    if (!node)
        return 1;
    i = idx->field_bc;
    v0 = 12;
    for (v1 = 4; v0; i = &i->field_4)
    {
        v0 -= 1;
        node->field_0 = i->field_0;
        node = &node->field_4;
    }
    v7 = sub_4777ce();
    if (!v7)
    {
        sub_46c941(idx);
    }
    else
    {
        *((unsigned int *)v7) = 0;
        if (idx->field_1c)
        {
            v8 = sub_4777ce(4);
            if (!v8)
            {
                v12 = 1;
            }
            else
            {
                *((unsigned int *)v8) = 0;
                v13 = idx;
                v14 = idx->field_3e;
                v6 = sub_47a12b(&v3, 1, v14, 14, v13);
                v6 |= sub_47a12b(&v3, 1, v14, 15, &v13->field_4);
                v5 = &v13->field_8;
                if (sub_47a12b(&v3, 1, v14, 16, v5) || v6)
                {
                    sub_483f19(v13);
                    v12 = 0xffffffff;
                }
                else
                {
                    iter = v5->field_0;
                    while (*(iter))
                    {
                        v16 = *(iter);
                        if (*(iter) >= 48 && v16 <= 57)
                        {
                            *(iter) = v16 - 48;
                            iter += 1;
                        }
                        else if (v16 != 59)
                        {
                            iter += 1;
                        }
                        else
                        {
                            do
                            {
                                v17 = iter + 1;
                                *(iter) = *(v17);
                                iter = v17;
                            } while (*(iter));
                        }
                    }
                    goto LABEL_4840b9;
                }
            }
            sub_46c941(idx);
            sub_46c941(v7);
            return v12;
        }
        else
        {
            index = idx;
            index->field_0 = g_4adf68.field_0;
            index->field_4 = g_4adf6c;
            v8 = 0;
            index->field_8 = g_4adf70;
LABEL_4840b9:
            *((unsigned int *)v7) = 1;
            if (v8)
                *((unsigned int *)v8) = 1;
LABEL_4840ca:
            if (idx->field_b4)
                InterlockedDecrement(idx->field_b4);
            if (idx->field_b0 && !InterlockedDecrement(idx->field_b0))
            {
                sub_46c941(idx->field_b0);
                sub_46c941(idx->field_bc);
            }
            idx->field_b4 = v8;
            idx->field_b0 = v7;
            idx->field_bc = idx;
            return 0;
        }
    }
    return 1;
}
