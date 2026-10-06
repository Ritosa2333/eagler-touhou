// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427fd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427fd0_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_427fd0_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_427fd0_1 *field_20;
} st_427fd0_0;

typedef struct st_427fd0_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_427fd0_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_427fd0_1 *field_20;
} st_427fd0_2;

typedef struct st_427fd0_1 {
    char padding_0[8];
    struct st_427fd0_0 *field_8;
    struct st_427fd0_2 *field_c;
    char padding_10[1108];
    struct st_427fd0_1 *field_464;
    char padding_468[32];
    unsigned int field_488;
} st_427fd0_1;

extern int g_49f6a8;

unsigned int sub_427fd0(st_427fd0_1 *idx)
{
    unsigned int v3;  // eax
    int v4;  // esi
    st_427fd0_2 *idx1;  // eax
    st_427fd0_2 *idx2;  // esi
    unsigned int v7;  // eax
    st_427fd0_2 *index;  // eax
    st_427fd0_2 *v9;  // esi
    unsigned int v10;  // ecx
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    int v2;  // [bp-0x4]

    v3 = sub_45fe60(v0, v1, v2);
    idx->field_488 = v3;
    if (!v3)
    {
        sub_464220(&g_49f6a8);
        return 0xffffffff;
    }
    idx1 = sub_46c9ea(36, v4);
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
    v7 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_4283d0;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    idx2->field_4 = v7 | 1;
    sub_462380();
    idx->field_8 = idx2;
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
        v9 = index;
    }
    else
    {
        v9 = NULL;
    }
    v10 = v9->field_4 & 0xfffffffd;
    v9->field_8 = sub_428430;
    v9->field_c = 0;
    v9->field_10 = 0;
    v9->field_20 = idx;
    v9->field_4 = v10 | 1;
    sub_462420();
    idx->field_c = v9;
    idx->field_464 = idx->padding_10;
    return 0;
}
