// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x409530; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_409530_1 {
    char padding_0[8];
    struct st_409530_0 *field_8;
    struct st_409530_2 *field_c;
    struct st_409530_1 *field_10;
} st_409530_1;

typedef struct st_45fe60_0 {
    char padding_0[292];
    unsigned int field_124;
} st_45fe60_0;

typedef struct st_409530_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_409530_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_409530_1 *field_20;
} st_409530_0;

typedef struct st_409530_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_409530_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_409530_1 *field_20;
} st_409530_2;

extern int g_49f6a8;

unsigned int sub_409530(void)
{
    st_45fe60_0 *v1;  // eax
    st_409530_1 *idx;  // edi
    int v3;  // esi
    st_409530_2 *idx1;  // eax
    st_409530_2 *idx2;  // esi
    unsigned int v6;  // edx
    st_409530_2 *index;  // eax
    st_409530_2 *v8;  // esi
    unsigned int v9;  // eax

    v1 = sub_45fe60();
    idx[255332].field_c = v1;
    if (!v1)
    {
        sub_464220(&g_49f6a8);
        return 0xffffffff;
    }
    idx->field_10 = &idx[5];
    *((unsigned short *)((char *)&idx[255271].field_8 + 2)) = 5;
    idx1 = sub_46c9ea(36, v3);
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
    v6 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_40a1f0;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    idx2->field_4 = v6 | 1;
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
        v8 = index;
    }
    else
    {
        v8 = NULL;
    }
    v9 = v8->field_4 & 0xfffffffd;
    v8->field_8 = sub_40a220;
    v8->field_c = 0;
    v8->field_10 = 0;
    v8->field_20 = idx;
    v8->field_4 = v9 | 1;
    sub_462420();
    idx->field_c = v8;
    return 0;
}
