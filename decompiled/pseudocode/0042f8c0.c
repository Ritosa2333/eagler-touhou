// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f8c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42f8c0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_42f8c0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
} st_42f8c0_0;

extern char g_4ce8e8;
extern unsigned int g_4cee3c;
extern unsigned int g_4cee40;
extern unsigned int g_4cee48;

unsigned int sub_42f8c0(void)
{
    st_42f8c0_0 *idx;  // eax
    unsigned int v6;  // eax
    st_42f8c0_0 *idx1;  // eax
    st_42f8c0_0 *idx2;  // eax
    st_42f8c0_0 *v8;  // esi
    st_42f8c0_0 *index;  // eax
    st_42f8c0_0 *v10;  // eax
    st_42f8c0_0 *v11;  // eax
    st_42f8c0_0 *v12;  // eax
    st_42f8c0_0 *v13;  // eax
    st_42f8c0_0 *v14;  // eax
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

    g_4cee3c = 0xfffffffe;
    g_4cee40 = 0;
    g_4cee48 = 0;
    idx = sub_46c9ea(36, v0, v1, v2, v3);
    if (idx)
    {
        idx->field_4 = idx->field_4 & 0xfffffffe;
        idx->field_8 = 0;
        idx->field_c = 0;
        idx->field_10 = 0;
        idx->field_0 = 0;
        idx->field_14 = idx;
        idx->field_18 = 0;
        idx->field_1c = 0;
    }
    else
    {
        idx = NULL;
    }
    idx->field_4 = idx->field_4 | 3;
    idx->field_8 = sub_42f000;
    idx->field_10 = 0;
    idx[1].field_0 = &g_4ce8e8;
    idx->field_c = sub_42f560;
    v6 = sub_462380();
    if (v6)
        return v6;
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
        idx2->field_1c = 0;
        v8 = idx2;
    }
    else
    {
        v8 = NULL;
    }
    v8->field_4 = v8->field_4 | 3;
    v8->field_8 = sub_42f0b0;
    v8->field_c = 0;
    v8->field_10 = 0;
    v8[1].field_0 = &g_4ce8e8;
    sub_462420();
    index = sub_46c9ea(36);
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
    }
    else
    {
        index = NULL;
    }
    index->field_4 = index->field_4 | 3;
    index->field_8 = sub_42f200;
    index->field_c = 0;
    index->field_10 = 0;
    index[1].field_0 = &g_4ce8e8;
    sub_462420();
    v10 = sub_46c9ea(36);
    if (v10)
    {
        v10->field_4 = v10->field_4 & 0xfffffffe;
        v10->field_8 = 0;
        v10->field_c = 0;
        v10->field_10 = 0;
        v10->field_0 = 0;
        v10->field_14 = v10;
        v10->field_18 = 0;
        v10->field_1c = 0;
    }
    else
    {
        v10 = NULL;
    }
    v10->field_4 = v10->field_4 | 3;
    v10->field_8 = sub_42f3b0;
    v10->field_c = 0;
    v10->field_10 = 0;
    v10[1].field_0 = &g_4ce8e8;
    sub_462420();
    v11 = sub_46c9ea(36);
    if (v11)
    {
        v11->field_4 = v11->field_4 & 0xfffffffe;
        v11->field_8 = 0;
        v11->field_c = 0;
        v11->field_10 = 0;
        v11->field_0 = 0;
        v11->field_14 = v11;
        v11->field_18 = 0;
        v11->field_1c = 0;
    }
    else
    {
        v11 = NULL;
    }
    v11->field_4 = v11->field_4 | 3;
    v11->field_8 = sub_42f290;
    v11->field_c = 0;
    v11->field_10 = 0;
    v11[1].field_0 = &g_4ce8e8;
    sub_462420();
    v12 = sub_46c9ea(36);
    if (v12)
    {
        v12->field_4 = v12->field_4 & 0xfffffffe;
        v12->field_8 = 0;
        v12->field_c = 0;
        v12->field_10 = 0;
        v12->field_0 = 0;
        v12->field_14 = v12;
        v12->field_18 = 0;
        v12->field_1c = 0;
    }
    else
    {
        v12 = NULL;
    }
    v12->field_4 = v12->field_4 | 3;
    v12->field_8 = sub_42f410;
    v12->field_c = 0;
    v12->field_10 = 0;
    v12[1].field_0 = &g_4ce8e8;
    sub_462420();
    v13 = sub_46c9ea(36);
    if (v13)
    {
        v13->field_4 = v13->field_4 & 0xfffffffe;
        v13->field_8 = 0;
        v13->field_c = 0;
        v13->field_10 = 0;
        v13->field_0 = 0;
        v13->field_14 = v13;
        v13->field_18 = 0;
        v13->field_1c = 0;
    }
    else
    {
        v13 = NULL;
    }
    v13->field_4 = v13->field_4 | 3;
    v13->field_8 = sub_42f320;
    v13->field_c = 0;
    v13->field_10 = 0;
    v13[1].field_0 = &g_4ce8e8;
    sub_462420();
    v14 = sub_46c9ea(36);
    if (v14)
    {
        v14->field_4 = v14->field_4 & 0xfffffffe;
        v14->field_8 = 0;
        v14->field_c = 0;
        v14->field_10 = 0;
        v14->field_0 = 0;
        v14->field_14 = v14;
        v14->field_18 = 0;
        v14->field_1c = 0;
    }
    else
    {
        v14 = NULL;
    }
    v14->field_4 = v14->field_4 | 3;
    v14->field_8 = sub_42f470;
    v14->field_c = 0;
    v14->field_10 = 0;
    v14[1].field_0 = &g_4ce8e8;
    sub_462420();
    idx1 = sub_46c9ea(36);
    if (idx1)
    {
        idx1->field_4 = idx1->field_4 & 0xfffffffe;
        idx1->field_8 = 0;
        idx1->field_c = 0;
        idx1->field_10 = 0;
        idx1->field_0 = 0;
        idx1->field_14 = idx1;
        idx1->field_18 = 0;
        idx1->field_1c = 0;
    }
    else
    {
        idx1 = NULL;
    }
    idx1->field_4 = idx1->field_4 | 3;
    idx1->field_8 = sub_42f380;
    idx1->field_c = 0;
    idx1->field_10 = 0;
    idx1[1].field_0 = &g_4ce8e8;
    sub_462420();
    return 0;
}
