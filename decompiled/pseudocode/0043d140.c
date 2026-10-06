// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d140; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43d140_1 {
    struct st_43d140_0 *field_0;
    void* field_4;
    char padding_8[125356];
    unsigned int field_1e9b4;
} st_43d140_1;

typedef struct st_43d140_0 {
    unsigned int field_0;
    char padding_4[4];
    unsigned short field_8;
    char padding_a[2];
    unsigned int field_c;
    int field_10;
    int field_14;
    char field_18;
} st_43d140_0;

unsigned int sub_43d140(st_43d140_1 *idx)
{
    st_43d140_0 *v2;  // eax
    unsigned int v3;  // edi
    unsigned int v12;  // eax
    unsigned int v13;  // eax
    st_43d140_1 *iter;  // edi
    unsigned int v15;  // ecx
    void* iter1;  // esi
    unsigned int v17;  // eax
    st_43d140_0 *v18;  // eax
    int v4;  // esi
    int v5;  // ebx
    st_43d140_0 *v6;  // eax
    void* v7;  // eax
    st_43d140_0 *v8;  // ecx
    int v9;  // esi
    void* node;  // ebx
    int v11;  // esi
    int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x8]

    v2 = idx->field_0;
    v1 = v3;
    if (v2)
    {
        if (v2->field_0 == 825378900 && v2->field_8 == 2)
        {
            sub_4639d0(&v2->field_18, v2->field_10, 53, 16, v2->field_10, v4, v5);
            v6 = idx->field_0;
            v7 = sub_46d04a(v6->field_14 * 4);
            v8 = idx->field_0;
            idx->field_4 = v7;
            sub_44c710(&v6->field_18, v8->field_10, v7);
            v9 = idx->field_0->field_14;
            node = idx->field_4;
            v0 = v9;
            if (v9 <= NULL)
                return 0;
            while (1)
            {
                v0 = v9;
                if (*((short *)node) == 21059)
                {
                    v11 = v9;
                    if ((short)node[2] == 2)
                    {
                        v12 = sub_43cdb0(node, 21059, 17908);
                        v11 = v9;
                        if (v12 == (int)node[4])
                        {
                            v11 = v9;
                            if ((int)node[8] == 17908)
                            {
                                sub_47a330(&(&idx->field_0)[4477 * node[12] + 2], node, 17908);
                                v11 = v9;
                            }
                        }
                    }
                }
                else if (*((short *)node) != 21587)
                {
                    return 0;
                }
                else
                {
                    v11 = v9;
                    if ((short)node[2] == 2)
                    {
                        v13 = sub_43cdb0(node, 21587, 1096);
                        v11 = v9;
                        if (v13 == (int)node[4])
                        {
                            v11 = v9;
                            if ((int)node[8] == 1096)
                            {
                                iter = &idx->field_1e9b4;
                                v15 = 274;
                                for (iter1 = node; v15; iter1 += 4)
                                {
                                    v15 -= 1;
                                    iter->field_0 = *((int *)iter1);
                                    iter = &iter->field_4;
                                }
                                v11 = v0;
                            }
                        }
                    }
                }
                v17 = (int)node[8];
                v9 = v11 - v17;
                v0 = v9;
                if (v11 - v17 < NULL)
                    return 0;
                node += v17;
                if (v9 <= NULL)
                    return 0;
            }
        }
        sub_46c941(v2);
        idx->field_0 = NULL;
    }
    v18 = sub_46d04a(24);
    idx->field_0 = v18;
    memset(v18, 0, 24);
    idx->field_0->field_0 = 825378900;
    idx->field_0->field_8 = 2;
    idx->field_0->field_c = 0x100;
    return 0;
}
