// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4841e9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4841e9_2 {
    char padding_0[24];
    unsigned int field_18;
    unsigned int field_1c;
    char padding_20[24];
    unsigned short field_38;
    char padding_3a[118];
    int field_b0;
    char padding_b4[4];
    int field_b8;
    struct st_4841e9_0 *field_bc;
} st_4841e9_2;

typedef struct st_4841e9_1 {
    char field_0;
    char field_1;
} st_4841e9_1;

typedef struct st_4841e9_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[16];
    struct st_4841e9_1 *field_1c;
} st_4841e9_0;

extern st_4841e9_0 g_4adf68;

unsigned int sub_4841e9(st_4841e9_2 *index)
{
    st_4841e9_2 *v6;  // esi
    unsigned int v7;  // edi
    unsigned int v16;  // edi
    unsigned int v17;  // eax
    unsigned int v18;  // eax
    unsigned int v19;  // eax
    unsigned int v20;  // eax
    unsigned int v21;  // eax
    unsigned int v22;  // eax
    unsigned int v23;  // eax
    unsigned int iter;  // eax
    char v25;  // cl
    st_4841e9_0 *idx;  // ebx
    unsigned int v26;  // edi
    st_4841e9_0 *i;  // esi
    st_4841e9_0 *node;  // edi
    st_4841e9_2 *v29;  // eax
    unsigned int v9;  // esi
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned int v12;  // eax
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    st_4841e9_2 *v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    int v4;  // [bp-0xc]
    int v5;  // [bp-0x8]

    v6 = index;
    v1 = v7;
    v5 = 0;
    v2 = v6;
    v3 = 0;
    if (!v6->field_18 && !v6->field_1c)
    {
        v5 = 0;
        v4 = 0;
        idx = &g_4adf68.field_0;
        index = v6;
        goto LABEL_484452;
    }
    idx = sub_477813(1, 48);
    if (!idx)
        return 1;
    v4 = sub_4777ce(4);
    if (!v4)
    {
        sub_46c941(idx);
        goto LABEL_48424a;
    }
    else
    {
        *((unsigned int *)v4) = 0;
        if (!v6->field_18)
        {
            v0 = 12;
            i = &g_4adf68.field_0;
            for (node = idx; v0; i = &i->field_4)
            {
                v0 -= 1;
                node->field_0 = i->field_0;
                node = &node->field_4;
            }
LABEL_48441d:
            v29 = &index->field_bc;
            idx->field_0 = *((int *)*((int *)&v29->padding_0[0]));
            idx->field_4 = *((int *)(*((int *)&v29->padding_0[0]) + 4));
            idx->field_8 = *((int *)(*((int *)&v29->padding_0[0]) + 8));
            *((unsigned int *)v4) = 1;
            if (v5)
                *((unsigned int *)v5) = 1;
LABEL_484452:
            if (index->field_b8)
                InterlockedDecrement(index->field_b8);
            if (index->field_b0 && !InterlockedDecrement(index->field_b0))
            {
                sub_46c941(index->field_bc);
                sub_46c941(index->field_b0);
            }
            index->field_b8 = v5;
            index->field_b0 = v4;
            index->field_bc = idx;
            return 0;
        }
        v5 = sub_4777ce(4);
        if (!v5)
        {
            sub_46c941(idx);
            sub_46c941(v4);
LABEL_48424a:
        }
        else
        {
            *((unsigned int *)v5) = 0;
            v9 = v6->field_38;
            v10 = sub_47a12b(&v2, 1, v9, 21, idx->padding_c);
            v11 = sub_47a12b(&v2, 1, v9, 20, &idx->padding_c[4]);
            v12 = sub_47a12b(&v2, 1, v9, 22, &idx->padding_c[8]);
            v13 = sub_47a12b(&v2, 1, v9, 23, &idx->padding_c[12]);
            v14 = sub_47a12b(&v2, 1, v9, 24, &idx->field_1c);
            v15 = sub_47a12b(&v2, 1, v9, 80, idx + 1);
            v16 = v10 | v11 | v12 | v13 | v14 | v15 | sub_47a12b(&v2, 1, v9, 81, &idx[1].field_4);
            v17 = sub_47a12b(&v2, 0, v9, 26, &idx[1].field_8);
            v18 = sub_47a12b(&v2, 0, v9, 25, (char *)&idx[1].field_8 + 1);
            v19 = sub_47a12b(&v2, 0, v9, 84, (char *)&idx[1].field_8 + 2);
            v20 = sub_47a12b(&v2, 0, v9, 0x55, (char *)&idx[1].field_8 + 3);
            v21 = sub_47a12b(&v2, 0, v9, 86, idx[1].padding_c);
            v22 = sub_47a12b(&v2, 0, v9, 87, &idx[1].padding_c[1]);
            v23 = sub_47a12b(&v2, 0, v9, 82, &idx[1].padding_c[2]);
            if (sub_47a12b(&v2, 0, v9, 83, &idx[1].padding_c[3]) || v16 || v17 || v18 || v19 || v20 || v21 || v22 || v23)
            {
                sub_48415b(idx);
                sub_46c941(idx);
                sub_46c941(v4);
                sub_46c941(v5);
            }
            else
            {
                iter = idx->field_1c;
                while (*((char *)iter))
                {
                    v25 = *((char *)iter);
                    if (*((char *)iter) >= 48 && v25 <= 57)
                    {
                        *((char *)iter) = v25 - 48;
                        iter += 1;
                    }
                    else if (v25 != 59)
                    {
                        iter += 1;
                    }
                    else
                    {
                        do
                        {
                            v26 = iter + 1;
                            *((char *)iter) = *((char *)v26);
                            iter = v26;
                        } while (*((char *)iter));
                    }
                }
                goto LABEL_48441d;
            }
        }
    }
    return 1;
}
