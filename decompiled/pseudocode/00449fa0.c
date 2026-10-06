// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x449fa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_449fa0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_449fa0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_449fa0_1 *field_20;
} st_449fa0_0;

typedef struct st_449fa0_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_449fa0_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_449fa0_1 *field_20;
} st_449fa0_2;

typedef struct st_449fa0_1 {
    char padding_0[8];
    struct st_449fa0_2 *field_8;
    struct st_449fa0_0 *field_c;
    char padding_10[8];
    unsigned int field_18;
    char padding_1c[4];
    unsigned int field_20;
    struct st_449fa0_2 *field_24;
} st_449fa0_1;

extern char g_4b2ed0;

unsigned int sub_449fa0(unsigned int a0)
{
    int v1;  // ebp
    st_449fa0_2 *index;  // eax
    unsigned int v11;  // edx
    unsigned int v12;  // eax
    unsigned int v3;  // eax
    st_449fa0_1 *idx;  // edi
    int v5;  // ebx
    st_449fa0_2 *idx1;  // eax
    st_449fa0_2 *idx2;  // esi
    unsigned int v8;  // ecx
    st_449fa0_2 *v9;  // eax
    st_449fa0_2 *v10;  // esi
    int v0;  // [bp-0x8]

    index = sub_46c9ea(36, v0, v1);
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
    v3 = index->field_4 & 0xfffffffd;
    index->field_8 = sub_44a860;
    index->field_c = 0;
    index->field_10 = 0;
    index->field_20 = idx;
    index->field_4 = v3 | 1;
    sub_462380(v5);
    idx->field_8 = index;
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
        idx2 = idx1;
    }
    else
    {
        idx2 = NULL;
    }
    v8 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_44a870;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    idx2->field_4 = v8 | 1;
    sub_462420();
    idx->field_c = idx2;
    v9 = sub_46c9ea(36);
    if (v9)
    {
        v9->field_4 = v9->field_4 & 0xfffffffe;
        v9->field_8 = 0;
        v9->field_c = 0;
        v9->field_10 = 0;
        v9->field_0 = 0;
        v9->field_14 = v9;
        v9->field_18 = 0;
        v9->field_1c = 0;
        v10 = v9;
    }
    else
    {
        v10 = NULL;
    }
    v11 = v10->field_4 & 0xfffffffd;
    v10->field_8 = sub_44a880;
    v10->field_c = 0;
    v10->field_10 = 0;
    v10->field_20 = idx;
    v10->field_4 = v11 | 1;
    sub_462420();
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_24 = v10;
    v12 = idx->field_20;
    if (!((char)idx->field_20 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_18 = /* unsupported instruction */;
        else
            idx->field_18 = nan;
        *((unsigned int *)&idx->padding_10[4]) = 0;
        *((unsigned int *)&idx->padding_10[0]) = 0xfff0bdc1;
        *((char **)&idx->padding_1c[0]) = &g_4b2ed0;
        idx->field_20 = v12 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_18 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_18 = nan;
        /* unsupported instruction */
    }
    *((unsigned int *)&idx->padding_10[4]) = 0;
    *((unsigned int *)&idx->padding_10[0]) = 0xffffffff;
    return 0;
}
