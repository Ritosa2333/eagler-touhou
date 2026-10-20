// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41d250; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41d250_1 {
    char padding_0[8];
    struct st_41d250_0 *field_8;
    struct st_41d250_2 *field_c;
    char padding_10[27844];
    struct st_41d250_2 *field_6cd4;
    char padding_6cd8[108];
    struct st_45fe60_0 *field_6d44;
} st_41d250_1;

typedef struct st_45fe60_0 {
    char padding_0[292];
    unsigned int field_124;
} st_45fe60_0;

typedef struct st_41d250_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_41d250_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_41d250_1 *field_20;
} st_41d250_0;

typedef struct st_41d250_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_41d250_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_41d250_1 *field_20;
} st_41d250_2;

extern int g_49f434;

unsigned int sub_41d250(st_41d250_1 *idx)
{
    unsigned int v2;  // ebx
    unsigned int v3;  // edi
    st_41d250_2 *idx1;  // eax
    st_41d250_2 *idx2;  // esi
    unsigned int v14;  // edx
    st_45fe60_0 *v4;  // eax
    int v5;  // esi
    st_41d250_2 *v6;  // eax
    st_41d250_2 *v7;  // esi
    unsigned int v8;  // eax
    st_41d250_2 *index;  // eax
    st_41d250_2 *v10;  // esi
    unsigned int v11;  // ecx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x4]

    v1 = v2;
    v0 = v3;
    v4 = sub_45fe60();
    idx->field_6d44 = v4;
    if (!v4)
    {
        sub_464220(&g_49f434);
        return 0xffffffff;
    }
    else if (sub_41d3b0(idx))
    {
        return 0xffffffff;
    }
    else
    {
        v6 = sub_46c9ea(36, v5);
        if (v6)
        {
            v6->field_4 = v6->field_4 & 0xfffffffe;
            v6->field_8 = 0;
            v6->field_c = 0;
            v6->field_10 = 0;
            v6->field_0 = 0;
            v6->field_14 = v6;
            v6->field_18 = 0;
            v6->field_1c = 0;
            v7 = v6;
        }
        else
        {
            v7 = NULL;
        }
        v8 = v7->field_4 & 0xfffffffd;
        v7->field_8 = sub_41f8d0;
        v7->field_c = 0;
        v7->field_10 = 0;
        v7->field_20 = idx;
        v7->field_4 = v8 | 1;
        sub_462380();
        idx->field_8 = v7;
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
            v10 = index;
        }
        else
        {
            v10 = NULL;
        }
        v11 = v10->field_4 & 0xfffffffd;
        v10->field_8 = sub_41f8e0;
        v10->field_c = 0;
        v10->field_10 = 0;
        v10->field_20 = idx;
        v10->field_4 = v11 | 1;
        sub_462420();
        idx->field_c = v10;
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
        v14 = idx2->field_4 & 0xfffffffd;
        idx2->field_8 = sub_41f8f0;
        idx2->field_c = 0;
        idx2->field_10 = 0;
        idx2->field_20 = idx;
        idx2->field_4 = v14 | 1;
        sub_462420();
        idx->field_6cd4 = idx2;
        return 0;
    }
}
