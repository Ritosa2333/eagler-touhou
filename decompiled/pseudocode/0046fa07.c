// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46fa07; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46fa07_0 {
    unsigned int field_0;
    char padding_4[64];
    unsigned int field_44;
    char field_48;
    char padding_49[123];
    unsigned int field_c4;
    unsigned int field_c8;
} st_46fa07_0;

typedef struct st_46fa07_1 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[8];
    struct st_46fa07_0 *field_10;
    char field_14;
} st_46fa07_1;

typedef struct st_46fa07_2 {
    char padding_0[324];
    unsigned int field_144;
} st_46fa07_2;

typedef struct st_46fa07_4 {
    char padding_0[4];
    char field_4;
} st_46fa07_4;

typedef struct st_46fa07_3 {
    unsigned int field_0;
    struct st_46fa07_3 *field_4;
    struct st_46fa07_3 *field_8;
} st_46fa07_3;

extern unsigned int g_4b3d58;
extern unsigned int g_4d6440;
extern unsigned int g_4d6444;
extern st_46fa07_1 *g_4d644c;
extern char g_4d6454;

int * sub_46fa07(unsigned int i)
{
    unsigned int v7;  // eax
    int v8;  // ecx
    int k;  // edi
    unsigned int v18;  // ecx
    st_46fa07_3 *idx;  // edx
    int v20;  // ecx
    int v21;  // esi
    st_46fa07_4 *v22;  // edi
    char v23;  // 4112
    unsigned int v24;  // 4167
    st_46fa07_4 *v25;  // edi
    unsigned int v26;  // ebx
    unsigned int v9;  // edi
    char v27;  // 4112
    unsigned int v28;  // 4163
    st_46fa07_3 *idx1;  // ecx
    st_46fa07_3 *v30;  // edi
    int *v31;  // edx
    unsigned int v32;  // ecx
    unsigned int v33;  // ecx
    unsigned int v10;  // esi
    st_46fa07_1 *iter;  // ecx
    unsigned int v12;  // eax
    st_46fa07_0 *v13;  // eax
    unsigned int v14;  // edx
    unsigned int v15;  // edx
    unsigned int node;  // ecx
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    st_46fa07_2 *v4;  // [bp-0x10]
    int v5;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x8]

    v7 = g_4d6440 * 20 + g_4d6444;
    v3 = i + 23 & 0xfffffff0;
    v8 = ((int)(v3) >> 4) - 1;
    v1 = v9;
    if (v8 < 32)
    {
        v10 = 0xffffffff >> ((char)v8 & 31);
        v5 = 0xffffffff;
    }
    else
    {
        v10 = 0;
        v5 = 0xffffffff >> ((char)(v8 - 32) & 31);
    }
    iter = g_4d644c;
    while (1)
    {
        i = iter;
        if (iter >= v7 || iter->field_4 & v5 || iter->field_0 & v10)
            break;
        iter = &iter->field_14;
    }
    if (iter == v7)
    {
        iter = g_4d6444;
        while (1)
        {
            i = iter;
            if (iter >= g_4d644c || iter->field_4 & v5 || iter->field_0 & v10)
                break;
            iter = &iter->field_14;
        }
        if (iter == g_4d644c)
        {
            for (; iter < v7 && !*((int *)&iter->padding_8[0]); i = &iter->field_14);
            if (iter == v7)
            {
                iter = g_4d6444;
                while (1)
                {
                    i = iter;
                    if (iter >= g_4d644c || *((int *)&iter->padding_8[0]))
                        break;
                    iter = &iter->field_14;
                }
                if (iter == g_4d644c && !(iter = (st_46fa07_1 *)sub_46f10e(), i = iter, iter))
                    goto LABEL_46fad7;
            }
            v12 = sub_46f1be(iter);
            iter->field_10->field_0 = v12;
            if (iter->field_10->field_0 == 0xffffffff)
            {
LABEL_46fad7:
                return NULL;
            }
        }
    }
    g_4d644c = iter;
    v13 = iter->field_10;
    v14 = v13->field_0;
    v6 = v14;
    if (v14 == 0xffffffff || !((&v13->field_c4)[v14] & v5) && !((&v13->field_44)[v14] & v10))
    {
        v6 = 0;
        v15 = v13->field_c4;
        for (node = &v13->field_44; !(v15 & v5) && !(*((int *)node) & v10); node += 4)
        {
            v6 += 1;
            v15 = *((int *)(node + 132));
        }
        v14 = v6;
    }
    v4 = 516 * v14 + (char *)v13 + 324;
    k = 0;
    v18 = (&v13->field_44)[v14] & v10;
    if (!((&v13->field_44)[v14] & v10))
    {
        v18 = (&v13->field_c4)[v14] & v5;
        v0 = 32;
        k = 32;
    }
    for (; v18 >= 0; k += 1)
    {
        v18 *= 2;
    }
    idx = *((int *)&v4->padding_0[4 + 8 * k]);
    v20 = idx->field_0 - v3;
    v21 = (v20 >> 4) - 1;
    v5 = v20;
    if (v21 > 63)
    {
        v0 = 63;
        v21 = 63;
    }
    if (v21 != k)
    {
        if (idx->field_4 == idx->field_8)
        {
            if (k < 32)
            {
                v22 = &v13->padding_4[k];
                v2 = ~(0x80000000 >> ((char)k & 31));
                (&v13->field_44)[v6] = v2 & (&v13->field_44)[v6];
                v23 = v22->padding_0[0];
                v22->padding_0[0] = v22->padding_0[0] - 1;
                v24 = _ccall(4, 19, v23 - 1, 0, 0);
                if (!(v24 & 1))
                    goto LABEL_46fbfa;
                iter = i;
                iter->field_0 = iter->field_0 & v2;
            }
            else
            {
                v25 = &v13->padding_4[k];
                v26 = ~(0x80000000 >> ((char)(k - 32) & 31));
                (&v13->field_c4)[v6] = (&v13->field_c4)[v6] & v26;
                v27 = v25->padding_0[0];
                v25->padding_0[0] = v25->padding_0[0] - 1;
                v2 = v26;
                v28 = _ccall(4, 19, v27 - 1, 0, 0);
                if (v28 & 1)
                {
                    iter = i;
                    iter->field_4 = iter->field_4 & v2;
                }
                else
                {
LABEL_46fbfa:
                    iter = i;
                }
            }
        }
        idx->field_8->field_4 = idx->field_4;
        idx->field_4->field_8 = idx->field_8;
        if (v5)
        {
            idx1 = &v4->padding_0[8 * v21];
            v30 = idx1->field_4;
            idx->field_8 = idx1;
            idx->field_4 = v30;
            idx1->field_4 = idx;
            idx->field_4->field_8 = idx;
            if (idx->field_4 == idx->field_8)
            {
                *((char *)&i + 3) = v13->padding_4[v21];
                v13->padding_4[v21] = v13->padding_4[v21] + 1;
                if (v21 < 32)
                {
                    if (!*((char *)((void*)&i + 3)))
                        iter->field_0 = iter->field_0 | 0x80000000 >> ((char)v21 & 31);
                    (&v13->field_44)[v6] = (&v13->field_44)[v6] | 0x80000000 >> ((char)v21 & 31);
                }
                else
                {
                    if (!*((char *)((void*)&i + 3)))
                        iter->field_4 = iter->field_4 | 0x80000000 >> ((char)(v21 - 32) & 31);
                    (&v13->field_c4)[v6] = (&v13->field_c4)[v6] | 0x80000000 >> ((char)(v21 - 32) & 31);
                }
            }
            v20 = v5;
            goto LABEL_46fc9a;
        }
        else
        {
            v20 = v5;
        }
    }
    else
    {
LABEL_46fc9a:
        if (v20)
        {
            idx->field_0 = v20;
            *((int *)(v20 + (char *)idx - 4)) = v20;
        }
    }
    v31 = (char *)idx + v20;
    v32 = v3 + 1;
    *(v31) = v32;
    *((unsigned int *)((char *)v31 + v3 - 4)) = v32;
    v33 = *((int *)&v4->padding_0[0]);
    *((unsigned int *)&v4->padding_0[0]) = v33 + 1;
    if (!v33 && iter == g_4b3d58 && v6 == *((int *)&g_4d6454))
        g_4b3d58 = 0;
    v13->field_0 = v6;
    return v31 + 1;
}
