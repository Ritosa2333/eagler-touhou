// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431d70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431d70_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_431d70_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_431d70_1 *field_20;
} st_431d70_0;

typedef struct st_431d70_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_431d70_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_431d70_1 *field_20;
} st_431d70_2;

typedef struct st_431d70_1 {
    char padding_0[8];
    struct st_431d70_2 *field_8;
    struct st_431d70_0 *field_c;
    char padding_10[8];
    unsigned int field_18;
    char padding_1c[4];
    unsigned int field_20;
    char padding_24[8];
    unsigned int field_2c;
    char padding_30[4];
    unsigned int field_34;
} st_431d70_1;

extern char g_4b2ed0;

unsigned int sub_431d70(void)
{
    int v1;  // esi
    int v2;  // ebp
    unsigned int v11;  // eax
    st_431d70_2 *index;  // eax
    unsigned int v4;  // eax
    st_431d70_1 *idx;  // edi
    int v6;  // ebx
    st_431d70_2 *idx1;  // eax
    st_431d70_2 *idx2;  // esi
    unsigned int v9;  // ecx
    unsigned int v10;  // eax

    index = sub_46c9ea(36, v1, v2);
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
    v4 = index->field_4 & 0xfffffffd;
    index->field_8 = sub_432700;
    index->field_c = 0;
    index->field_10 = 0;
    index->field_20 = idx;
    index->field_4 = v4 | 1;
    sub_462380(v6);
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
    v9 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_432710;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    idx2->field_4 = v9 | 1;
    sub_462420();
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_c = idx2;
    v10 = idx->field_20;
    if (!((char)idx->field_20 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_18 = /* unsupported instruction */;
        else
            idx->field_18 = nan;
        *((unsigned int *)&idx->padding_10[4]) = 0;
        *((unsigned int *)&idx->padding_10[0]) = 0xfff0bdc1;
        *((char **)&idx->padding_1c[0]) = &g_4b2ed0;
        idx->field_20 = v10 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_18 = /* unsupported instruction */;
    else
        idx->field_18 = nan;
    *((unsigned int *)&idx->padding_10[4]) = 0;
    *((unsigned int *)&idx->padding_10[0]) = 0xffffffff;
    v11 = idx->field_34;
    if (!((char)idx->field_34 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_2c = /* unsupported instruction */;
        else
            idx->field_2c = nan;
        *((unsigned int *)&idx->padding_24[4]) = 0;
        *((unsigned int *)&idx->padding_24[0]) = 0xfff0bdc1;
        *((char **)&idx->padding_30[0]) = &g_4b2ed0;
        idx->field_34 = v11 | 1;
    }
    *((unsigned int *)&idx->padding_24[0]) = 0xffffffff;
    if (/* unsupported instruction */)
    {
        idx->field_2c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_2c = nan;
        /* unsupported instruction */
    }
    *((unsigned int *)&idx->padding_24[4]) = 0;
    return 0;
}
