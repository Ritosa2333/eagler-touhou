// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d880; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40d880_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_40d880_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_40d880_1 *field_20;
} st_40d880_0;

typedef struct st_40d880_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_40d880_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_40d880_1 *field_20;
} st_40d880_2;

typedef struct st_40d880_1 {
    char padding_0[8];
    struct st_40d880_2 *field_8;
    struct st_40d880_0 *field_c;
    char padding_10[28];
    unsigned int field_2c;
    char padding_30[4];
    unsigned int field_34;
} st_40d880_1;

extern char g_4b2ed0;

unsigned int sub_40d880(unsigned int a0)
{
    int v1;  // ebp
    st_40d880_2 *idx;  // eax
    unsigned int v3;  // eax
    st_40d880_1 *index;  // edi
    int v5;  // ebx
    st_40d880_2 *idx1;  // eax
    st_40d880_2 *idx2;  // esi
    unsigned int v8;  // ecx
    unsigned int v9;  // eax
    int v0;  // [bp-0x8]

    idx = sub_46c9ea(36, v0, v1);
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
    v3 = idx->field_4 & 0xfffffffd;
    idx->field_8 = sub_40e040;
    idx->field_c = 0;
    idx->field_10 = 0;
    idx->field_20 = index;
    idx->field_4 = v3 | 1;
    sub_462380(v5);
    index->field_8 = idx;
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
    idx2->field_8 = sub_40e050;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = index;
    idx2->field_4 = v8 | 1;
    sub_462420();
    /* unsupported instruction */
    /* unsupported instruction */
    index->field_c = idx2;
    v9 = index->field_34;
    if (!((char)index->field_34 & 1))
    {
        if (/* unsupported instruction */)
            index->field_2c = /* unsupported instruction */;
        else
            index->field_2c = nan;
        *((unsigned int *)&index->padding_10[24]) = 0;
        *((unsigned int *)&index->padding_10[20]) = 0xfff0bdc1;
        *((char **)&index->padding_30[0]) = &g_4b2ed0;
        index->field_34 = v9 | 1;
    }
    if (/* unsupported instruction */)
    {
        index->field_2c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        index->field_2c = nan;
        /* unsupported instruction */
    }
    *((unsigned int *)&index->padding_10[24]) = 0;
    *((unsigned int *)&index->padding_10[20]) = 0xffffffff;
    return 0;
}
