// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46f2c6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46f2c6_2 {
} st_46f2c6_2;

typedef struct st_46f2c6_0 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[4];
    unsigned int field_c;
    unsigned int field_10;
} st_46f2c6_0;

typedef struct st_46f2c6_6 {
    char padding_0[4];
    char field_4;
} st_46f2c6_6;

typedef struct st_46f2c6_12 {
    char padding_0[68];
    unsigned int field_44;
} st_46f2c6_12;

typedef struct st_46f2c6_1 {
    char padding_0[4];
    struct st_46f2c6_2 *field_4;
} st_46f2c6_1;

typedef struct st_46f2c6_8 {
    char padding_0[4];
    struct st_46f2c6_9 *field_4;
} st_46f2c6_8;

typedef struct st_46f2c6_9 {
    struct st_46f2c6_9 *field_0;
    struct st_46f2c6_8 *field_4;
    struct st_46f2c6_9 *field_8;
} st_46f2c6_9;


unsigned int sub_46f2c6(st_46f2c6_0 *idx, void* a1, unsigned int iter)
{
    unsigned int v7;  // eax
    unsigned int v8;  // edi
    char v17;  // 4110
    unsigned int v18;  // 4155
    st_46f2c6_6 *v19;  // ecx
    unsigned int v20;  // ebx
    char v21;  // 4113
    unsigned int v22;  // 4158
    unsigned int v23;  // edi
    st_46f2c6_9 *idx1;  // ecx
    st_46f2c6_12 *v25;  // eax
    char v26;  // cl
    void* v9;  // edi
    void* v27;  // edx
    void* v28;  // eax
    unsigned int v29;  // eax
    void* v30;  // ebx
    unsigned int v31;  // ecx
    void* idx2;  // ebx
    unsigned int v33;  // esi
    unsigned int v34;  // esi
    st_46f2c6_6 *v35;  // esi
    unsigned int v36;  // ebx
    unsigned int v10;  // edx
    char v37;  // 4109
    unsigned int v38;  // 4152
    st_46f2c6_6 *v39;  // ecx
    unsigned int v40;  // ebx
    char v41;  // 4110
    unsigned int v42;  // 4154
    st_46f2c6_1 *v43;  // ecx
    void* v44;  // edi
    st_46f2c6_12 *v45;  // eax
    char v46;  // cl
    unsigned int v11;  // esi
    unsigned int v12;  // ecx
    void* index;  // edi
    unsigned int v14;  // ecx
    st_46f2c6_6 *v15;  // ecx
    unsigned int v16;  // ebx
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    int v4;  // [bp-0x8], Other Possible Types: unsigned int
    char v5;  // [bp+0xb]
    char v6;  // [bp+0xf]

    v7 = idx->field_10;
    v1 = v8;
    v9 = a1;
    v10 = v9 - idx->field_c >> 15;
    v2 = v10 * 516 + v7 + 324;
    v11 = iter + 23 & 0xfffffff0;
    v12 = *((int *)((char *)v9 - 4)) - 1;
    index = v12 + v9 - 4;
    iter = v12;
    v4 = *((int *)index);
    if (v11 > iter)
    {
        if (!((char)v4 & 1) && v11 <= v4 + iter)
        {
            v14 = ((int)(v4) >> 4) - 1;
            v3 = v14;
            if (v14 > 63)
            {
                v0 = 63;
                v14 = v0;
                v3 = v14;
            }
            if ((int)index[4] == (int)index[8])
            {
                if (v14 < 32)
                {
                    v15 = v3 + v7 + 4;
                    v16 = ~(0x80000000 >> ((char)v14 & 31));
                    *((unsigned int *)(v7 + v10 * 4 + 0x44)) = *((int *)(v7 + v10 * 4 + 0x44)) & v16;
                    v17 = v15->padding_0[0];
                    v15->padding_0[0] = v15->padding_0[0] - 1;
                    v18 = _ccall(4, 19, v17 - 1, 0, 0);
                    if (v18 & 1)
                        idx->field_0 = idx->field_0 & v16;
                }
                else
                {
                    v19 = v3 + v7 + 4;
                    v20 = ~(0x80000000 >> ((char)(v14 - 32) & 31));
                    *((unsigned int *)(v7 + v10 * 4 + 196)) = *((int *)(v7 + v10 * 4 + 196)) & v20;
                    v21 = v19->padding_0[0];
                    v19->padding_0[0] = v19->padding_0[0] - 1;
                    v22 = _ccall(4, 19, v21 - 1, 0, 0);
                    if (v22 & 1)
                        idx->field_4 = idx->field_4 & v20;
                }
            }
            *((int *)((int)index[8] + 4)) = (int)index[4];
            *((int *)((int)index[4] + 8)) = (int)index[8];
            v4 += iter - v11;
            if (v4 > 0)
            {
                v23 = (v4 >> 4) - 1;
                idx1 = a1 + v11 - 4;
                if (v23 > 63)
                {
                    v0 = 63;
                    v23 = v0;
                }
                iter = v2 + v23 * 8;
                idx1->field_4 = iter->field_4;
                idx1->field_8 = iter;
                iter->field_4 = idx1;
                *((st_46f2c6_9 **)&idx1->field_4[1].padding_0[0]) = idx1;
                if (idx1->field_4 == idx1->field_8)
                {
                    v6 = *((char *)(v23 + v7 + 4));
                    *((char *)(v23 + v7 + 4)) = *((char *)(v23 + v7 + 4)) + 1;
                    if (v23 < 32)
                    {
                        if (!v6)
                            idx->field_0 = idx->field_0 | 0x80000000 >> ((char)v23 & 31);
                        v25 = v7 + v10 * 4 + 0x44;
                        v26 = v23;
                    }
                    else
                    {
                        if (!v6)
                            idx->field_4 = idx->field_4 | 0x80000000 >> ((char)(v23 - 32) & 31);
                        v25 = v7 + v10 * 4 + 196;
                        v26 = v23 - 32;
                    }
                    *((unsigned int *)&v25->padding_0[0]) = *((int *)&v25->padding_0[0]) | 0x80000000 >> (v26 & 31);
                }
                v27 = a1;
                v28 = v27 + v11 - 4;
                *((int *)v28) = v4;
                *((int *)(v4 + (char *)v28 - 4)) = v4;
            }
            else
            {
                v27 = a1;
            }
            v29 = v11 + 1;
            *((unsigned int *)((char *)v27 - 4)) = v29;
            *((unsigned int *)((char *)v27 + v11 - 8)) = v29;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (v11 < iter)
        {
            v30 = a1;
            iter -= v11;
            v31 = v11 + 1;
            *((unsigned int *)((char *)v30 - 4)) = v31;
            idx2 = v30 + v11 - 4;
            v33 = (iter >> 4) - 1;
            a1 = idx2;
            *((unsigned int *)((char *)idx2 - 4)) = v31;
            if (v33 > 63)
            {
                v0 = 63;
                v33 = v0;
            }
            if (!((char)v4 & 1))
            {
                v34 = ((int)(v4) >> 4) - 1;
                if (v34 > 63)
                {
                    v0 = 63;
                    v34 = v0;
                }
                if ((int)index[4] == (int)index[8])
                {
                    if (v34 < 32)
                    {
                        v35 = v34 + v7 + 4;
                        v36 = ~(0x80000000 >> ((char)v34 & 31));
                        *((unsigned int *)(v7 + v10 * 4 + 0x44)) = *((int *)(v7 + v10 * 4 + 0x44)) & v36;
                        v37 = v35->padding_0[0];
                        v35->padding_0[0] = v35->padding_0[0] - 1;
                        v38 = _ccall(4, 19, v37 - 1, 0, 0);
                        if (v38 & 1)
                            idx->field_0 = idx->field_0 & v36;
                    }
                    else
                    {
                        v39 = v34 + v7 + 4;
                        v40 = ~(0x80000000 >> ((char)(v34 - 32) & 31));
                        *((unsigned int *)(v7 + v10 * 4 + 196)) = *((int *)(v7 + v10 * 4 + 196)) & v40;
                        v41 = v39->padding_0[0];
                        v39->padding_0[0] = v39->padding_0[0] - 1;
                        v42 = _ccall(4, 19, v41 - 1, 0, 0);
                        if (v42 & 1)
                            idx->field_4 = idx->field_4 & v40;
                    }
                    idx2 = a1;
                }
                *((int *)((int)index[8] + 4)) = (int)index[4];
                *((int *)((int)index[4] + 8)) = (int)index[8];
                iter += v4;
                v33 = (iter >> 4) - 1;
                if (v33 > 63)
                {
                    v0 = 63;
                    v33 = v0;
                }
            }
            v43 = v2 + v33 * 8;
            v44 = v43->field_4;
            *((st_46f2c6_1 **)&idx2[8]) = v43;
            *((void* *)&idx2[4]) = v44;
            v43->field_4 = idx2;
            *((void* *)((int)idx2[4] + 8)) = idx2;
            if ((int)idx2[4] == (int)idx2[8])
            {
                v5 = *((char *)(v33 + v7 + 4));
                *((char *)(v33 + v7 + 4)) = *((char *)(v33 + v7 + 4)) + 1;
                if (v33 < 32)
                {
                    if (!v5)
                        idx->field_0 = idx->field_0 | 0x80000000 >> ((char)v33 & 31);
                    v45 = v7 + v10 * 4 + 0x44;
                    v46 = v33;
                }
                else
                {
                    if (!v5)
                        idx->field_4 = idx->field_4 | 0x80000000 >> ((char)(v33 - 32) & 31);
                    v45 = v7 + v10 * 4 + 196;
                    v46 = v33 - 32;
                }
                *((unsigned int *)&v45->padding_0[0]) = *((int *)&v45->padding_0[0]) | 0x80000000 >> (v46 & 31);
            }
            *((int *)idx2) = iter;
            *((int *)(iter + (char *)idx2 - 4)) = iter;
        }
    }
    return 1;
}
