// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410c60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_410c60_2 {
    char padding_0[8];
    struct st_410c60_3 *field_8;
    struct st_410c60_1 *field_c;
    char padding_10[4];
    struct st_410c60_4 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
} st_410c60_2;

typedef struct st_410c60_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_410c60_1 *field_14;
    unsigned int field_18;
    char padding_1c[4];
    struct st_410c60_2 *field_20;
} st_410c60_1;

typedef struct st_410c60_3 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_410c60_3 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_410c60_2 *field_20;
} st_410c60_3;

typedef struct st_410c60_4 {
    char padding_0[4];
    unsigned int field_4;
} st_410c60_4;

typedef struct st_410c60_14 {
    char padding_0[125398];
    char field_1e9d6;
} st_410c60_14;

typedef struct st_410c60_0 {
    char padding_0[102324];
    int field_18fb4;
    char padding_18fb8[4];
    unsigned int field_18fbc;
} st_410c60_0;

extern int g_49f434;
extern unsigned int g_4ad138;
extern void g_4af128;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern int g_4b0ca8;
extern unsigned int g_4b0cc4;
extern st_410c60_0 *g_4b43b8;
extern st_410c60_14 *g_4b451c;
extern void g_4d4f37;
extern char g_4d4f38;

unsigned int sub_410c60(st_410c60_2 *idx)
{
    unsigned long v9;  // ldt
    unsigned long v10;  // gdt
    st_410c60_3 *idx1;  // esi
    st_410c60_1 *idx2;  // eax
    st_410c60_1 *v21;  // esi
    unsigned int v22;  // esi
    unsigned int v23;  // eax
    int v24;  // ecx
    void* v25;  // eax
    void* node;  // ecx
    void* v27;  // eax
    void* v28;  // eax
    unsigned short v11;  // fs
    unsigned int v29;  // eax
    void* v30;  // edi
    void* v31;  // edi
    void* iter;  // edi
    unsigned int v33;  // ecx
    unsigned int v34;  // ecx
    st_410c60_4 *v35;  // eax
    unsigned int v36;  // eax
    unsigned long long v38;  // 4122
    unsigned long long v12;  // 4140
    unsigned long long v13;  // 4162
    int v14;  // edi
    int v15;  // esi
    int v16;  // ebp
    int v17;  // ebx
    st_410c60_3 *index;  // eax
    unsigned int v0;  // [bp-0x44]
    unsigned int v1;  // [bp-0x40]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x18]
    int v4;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]
    unsigned int v8;  // [bp-0x4]

    v8 = 0xffffffff;
    v7 = sub_4975ab;
    v12 = _ccall(v9, v10, v11, 0);
    v6 = *((int *)(unsigned int)v12);
    v13 = _ccall(v9, v10, v11, 0);
    *((unsigned int **)(unsigned int)v13) = &v6;
    index = sub_46c9ea(36, g_4ad138 ^ &v14, v14, v15, v16, v17);
    if (index)
    {
        index->field_4 = index->field_4 & 0xfffffffe;
        index->field_8 = 0;
        index->field_c = 0;
        index->field_10 = 0;
        index->field_0 = 0;
        index->field_14 = index;
        index->field_18 = 0;
        index->field_1c = 0;
        idx1 = index;
    }
    else
    {
        idx1 = NULL;
    }
    idx1->field_4 = idx1->field_4 | 3;
    idx1->field_8 = sub_411190;
    idx1->field_c = 0;
    idx1->field_10 = 0;
    idx1->field_20 = idx;
    sub_462380();
    idx->field_8 = idx1;
    idx2 = sub_46c9ea(36);
    if (idx2)
    {
        idx2->field_4 = idx2->field_4 & 0xfffffffe;
        idx2->field_8 = 0;
        idx2->field_c = 0;
        idx2->field_10 = 0;
        idx2->field_0 = 0;
        idx2->field_14 = idx2;
        idx2->field_18 = 0;
        *((unsigned int *)&idx2->padding_1c[0]) = 0;
        v21 = idx2;
    }
    else
    {
        v21 = NULL;
    }
    v21->field_4 = v21->field_4 | 3;
    v21->field_8 = sub_4111a0;
    v21->field_c = 0;
    v21->field_10 = 0;
    v21->field_20 = idx;
    sub_462420();
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
    if (/* unsupported instruction */)
    {
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
        /* unsupported instruction */
    }
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
    idx->field_c = v21;
    if (/* unsupported instruction */)
    {
        v4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v4 = nan;
        /* unsupported instruction */
    }
    v22 = &g_4b43b8->field_18fbc;
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if (!*((int *)v22))
    {
        sub_4615a0(g_4b43b8->field_18fb4, &idx, 18, 0);
        *((unsigned int *)v22) = v6;
    }
    v23 = g_4b0c94 + g_4b0c90 * 2;
    idx->field_1c = v23;
    if (!g_4b0cc4)
        idx->field_1c = v23 + 6;
    v24 = idx->field_1c;
    if (!g_4b451c->padding_0[125386 + v24])
        idx->field_20 = idx->field_20 | 1;
    if (!g_4b451c->field_1e9d6)
        idx->field_20 = idx->field_20 | 2;
    g_4b451c->padding_0[125386 + v24] = g_4b451c->padding_0[125386 + v24] | 1;
    if (g_4b0ca8 >= 1)
        g_4b451c->padding_0[125386 + idx->field_1c] = g_4b451c->padding_0[125386 + idx->field_1c] | 16;
    if (idx->field_1c >= 6)
        g_4b451c->field_1e9d6 = 1;
    v25 = *((int *)&(&g_4af128)[4 * idx->field_1c]);
    g_4d4f38 = 0;
    node = v25;
    do
    {
        v27 = v25;
        v28 = v27 + 1;
        v25 = v28;
    } while (*((char *)v27));
    v29 = v28 - node;
    v30 = &g_4d4f37;
    do
    {
        v31 = v30;
        iter = v31 + 1;
        v30 = iter;
    } while ((char)v31[1]);
    for (v33 = v29 >> 2; v33; node += 4)
    {
        v33 -= 1;
        *((int *)iter) = *((int *)node);
        iter += 4;
    }
    v1 = 0;
    v34 = v29 & 3;
    for (v0 = 0; v34; node += 1)
    {
        v34 -= 1;
        *((char *)iter) = *((char *)node);
        iter += 1;
    }
    v35 = sub_463c10();
    idx->field_14 = v35;
    if (!v35)
    {
        sub_464220(&g_49f434);
        v36 = 0xffffffff;
    }
    else
    {
        v4 = sub_46c9ea(240);
        v2 = 0;
        idx->field_18 = (!v4 ? 0 : sub_4111b0(v4, &idx->field_14->padding_0[idx->field_14->field_4]));
        v36 = 0;
    }
    v38 = _ccall(v9, v10, v11, 0);
    *((unsigned long *)(unsigned int)v38) = g_4ad138 ^ &v14;
    return v36;
}
