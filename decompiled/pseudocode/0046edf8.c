// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46edf8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46edf8_1 {
} st_46edf8_1;

typedef struct st_46edf8_9 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[4];
    unsigned int field_c;
    unsigned int field_10;
} st_46edf8_9;

typedef struct st_46edf8_11 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    void* field_c;
    struct st_46edf8_10 *field_10;
    char field_14;
} st_46edf8_11;

typedef struct st_46edf8_3 {
    char padding_0[324];
    unsigned int field_144;
} st_46edf8_3;

typedef struct st_46edf8_7 {
    char padding_0[4];
    char field_4;
} st_46edf8_7;

typedef struct st_46edf8_10 {
    char padding_0[67];
    char field_43;
} st_46edf8_10;

typedef struct st_46edf8_4 {
    char padding_0[4];
    struct st_46edf8_5 *field_4;
} st_46edf8_4;

typedef struct st_46edf8_5 {
    char padding_0[8];
    struct st_46edf8_4 *field_8;
} st_46edf8_5;

typedef struct st_46edf8_6 {
    unsigned int field_0;
    struct st_46edf8_5 *field_4;
    struct st_46edf8_4 *field_8;
} st_46edf8_6;

typedef struct st_46edf8_0 {
    char padding_0[4];
    struct st_46edf8_1 *field_4;
    struct st_46edf8_0 *field_8;
} st_46edf8_0;

extern HANDLE g_4b3c04;
extern st_46edf8_11 *g_4b3d58;
extern unsigned int g_4d6440;
extern unsigned int g_4d6444;
extern unsigned int g_4d644c;
extern unsigned int g_4d6454;

void sub_46edf8(st_46edf8_9 *iter, void* index)
{
    st_46edf8_9 *idx;  // ecx
    unsigned int v7;  // eax
    char v15;  // 4109
    unsigned int v16;  // 4152
    st_46edf8_7 *v17;  // ecx
    unsigned int v18;  // ebx
    char v19;  // 4110
    unsigned int v20;  // 4154
    st_46edf8_9 *v21;  // edx
    unsigned int v22;  // edx
    st_46edf8_9 *v23;  // ebx
    unsigned int v24;  // ecx
    void* idx1;  // esi
    unsigned int v25;  // esi
    char v26;  // 4108
    unsigned int v27;  // 4151
    unsigned int v28;  // esi
    char v29;  // 4109
    unsigned int v30;  // 4153
    st_46edf8_0 *idx2;  // ecx
    void* v32;  // ebx
    unsigned int v33;  // 4100
    unsigned int v9;  // edi
    st_46edf8_11 *v34;  // eax
    unsigned int v10;  // ecx
    unsigned int v11;  // ebx
    st_46edf8_6 *v12;  // ebx
    st_46edf8_7 *v13;  // ecx
    unsigned int v14;  // ebx
    unsigned int v0;  // [bp-0x20]
    st_46edf8_3 *v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    char v5;  // [bp+0xb]

    idx = iter;
    v7 = idx->field_10;
    idx1 = index - 4;
    v9 = index - idx->field_c >> 15;
    v1 = v9 * 516 + v7 + 324;
    v10 = *((int *)idx1) - 1;
    v4 = v10;
    if ((char)v10 & 1)
        return;
    v0 = v11;
    v12 = v10 + idx1;
    v2 = v12->field_0;
    v3 = *((int *)((char *)idx1 - 4));
    index = v12;
    if (!((char)v2 & 1))
    {
        v22 = ((int)(v2) >> 4) - 1;
        if (v22 > 0x3f)
            v22 = 63;
        if (v12->field_4 == v12->field_8)
        {
            if (v22 < 0x20)
            {
                v13 = v22 + v7 + 4;
                v14 = ~(0x80000000 >> ((char)v22 & 31));
                *((unsigned int *)(v7 + v9 * 4 + 0x44)) = *((int *)(v7 + v9 * 4 + 0x44)) & v14;
                v15 = v13->padding_0[0];
                v13->padding_0[0] = v13->padding_0[0] - 1;
                v16 = _ccall(4, 19, v15 - 1, 0, 0);
                if (v16 & 1)
                    iter->field_0 = iter->field_0 & v14;
            }
            else
            {
                v17 = v22 + v7 + 4;
                v18 = ~(0x80000000 >> ((char)(v22 - 32) & 31));
                *((unsigned int *)(v7 + v9 * 4 + 196)) = *((int *)(v7 + v9 * 4 + 196)) & v18;
                v19 = v17->padding_0[0];
                v17->padding_0[0] = v17->padding_0[0] - 1;
                v20 = _ccall(4, 19, v19 - 1, 0, 0);
                if (v20 & 1)
                    iter->field_4 = iter->field_4 & v18;
            }
            v12 = index;
        }
        v10 = v4 + v2;
        v12->field_8->field_4 = v12->field_4;
        index->field_4->field_8 = index->field_8;
        v4 = v10;
    }
    v21 = ((int)(v10) >> 4) - 1;
    if (v21 > 0x3f)
    {
        v22 = 63;
        v21 = 0x3f;
    }
    v2 = v3 & 1;
    if (!(v3 & 1))
    {
        v22 = 63;
        index = idx1 - v3;
        v23 = ((int)(v3) >> 4) - 1;
        if (v23 > 0x3f)
            v23 = 0x3f;
        v24 = v10 + v3;
        v21 = ((int)(v24) >> 4) - 1;
        v4 = v24;
        if (v21 > 0x3f)
            v21 = 0x3f;
        if (v23 != v21)
        {
            if ((int)index[4] == (int)index[8])
            {
                if (v23 < 0x20)
                {
                    v25 = ~(0x80000000 >> (*((char *)&v23) & 31));
                    *((unsigned int *)(v7 + v9 * 4 + 0x44)) = *((int *)(v7 + v9 * 4 + 0x44)) & v25;
                    v26 = *((char *)v23 + v7 + 4);
                    *((char *)v23 + v7 + 4) = *((char *)v23 + v7 + 4) - 1;
                    v27 = _ccall(4, 19, v26 - 1, 0, 0);
                    if (v27 & 1)
                        iter->field_0 = iter->field_0 & v25;
                }
                else
                {
                    v28 = ~(0x80000000 >> (*((char *)&(struct st_46edf8_9 *)((char *)v23 - 32)) & 31));
                    *((unsigned int *)(v7 + v9 * 4 + 196)) = *((int *)(v7 + v9 * 4 + 196)) & v28;
                    v29 = *((char *)v23 + v7 + 4);
                    *((char *)v23 + v7 + 4) = *((char *)v23 + v7 + 4) - 1;
                    v30 = _ccall(4, 19, v29 - 1, 0, 0);
                    if (v30 & 1)
                        iter->field_4 = iter->field_4 & v28;
                }
            }
            *((int *)((int)index[8] + 4)) = (int)index[4];
            *((int *)((int)index[4] + 8)) = (int)index[8];
        }
        idx1 = index;
    }
    else
    {
        v23 = iter;
    }
    if (v2 || v23 != v21)
    {
        idx2 = (char *)v1 + 0x8 * v21;
        v32 = idx2->field_4;
        *((st_46edf8_0 **)&idx1[8]) = idx2;
        *((void* *)&idx1[4]) = v32;
        idx2->field_4 = idx1;
        *((void* *)((int)idx1[4] + 8)) = idx1;
        if ((int)idx1[4] == (int)idx1[8])
        {
            v5 = *((char *)v21 + v7 + 4);
            *((char *)v21 + v7 + 4) = *((char *)v21 + v7 + 4) + 1;
            if (v21 < 0x20)
            {
                if (!v5)
                    iter->field_0 = iter->field_0 | 0x80000000 >> (*((char *)&v21) & 31);
                *((unsigned int *)(v7 + v9 * 4 + 0x44)) = *((int *)(v7 + v9 * 4 + 0x44)) | 0x80000000 >> (*((char *)&v21) & 31);
            }
            else
            {
                if (!v5)
                    iter->field_4 = iter->field_4 | 0x80000000 >> (*((char *)&(struct st_46edf8_9 *)((char *)v21 - 32)) & 31);
                *((unsigned int *)(v7 + v9 * 4 + 196)) = *((int *)(v7 + v9 * 4 + 196)) | 0x80000000 >> (*((char *)&(struct st_46edf8_9 *)((char *)v21 - 32)) & 31);
            }
        }
    }
    *((unsigned int *)idx1) = v4;
    *((unsigned int *)(v4 + (char *)idx1 - 4)) = v4;
    v33 = *((int *)&v1->padding_0[0]);
    *((unsigned int *)&v1->padding_0[0]) = *((int *)&v1->padding_0[0]) - 1;
    if (v33 != 1)
        return;
    if (g_4b3d58)
    {
        VirtualFree(g_4d6454 * 0x8000 + g_4b3d58->field_c, 0x8000, MEM_DECOMMIT);
        g_4b3d58->field_8 = g_4b3d58->field_8 | 0x80000000 >> ((char)g_4d6454 & 31);
        *((unsigned int *)&g_4b3d58->field_10[2].padding_0[60 + 4 * g_4d6454]) = 0;
        g_4b3d58->field_10->field_43 = g_4b3d58->field_10->field_43 - 1;
        v34 = g_4b3d58;
        if (!g_4b3d58->field_10->field_43)
        {
            g_4b3d58->field_4 = g_4b3d58->field_4 & 0xfffffffe;
            v34 = g_4b3d58;
        }
        if (v34->field_8 == 0xffffffff)
        {
            VirtualFree(v34->field_c, NULL, MEM_RELEASE);
            HeapFree(g_4b3c04, HEAP_NONE, g_4b3d58->field_10);
            sub_47a6a0(g_4b3d58, &g_4b3d58->field_14, 20 * g_4d6440 - (char *)g_4b3d58 + g_4d6444 - 20);
            g_4d6440 = g_4d6440 - 1;
            if (iter > g_4b3d58)
                iter = (char *)iter - 20;
            g_4d644c = g_4d6444;
        }
    }
    g_4b3d58 = iter;
    g_4d6454 = v9;
    return;
}
