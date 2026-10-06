// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43db40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43db40_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_43db40_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_43db40_1 *field_20;
} st_43db40_0;

typedef struct st_43db40_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_43db40_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_43db40_1 *field_20;
} st_43db40_2;

typedef struct st_43db40_1 {
    char padding_0[8];
    struct st_43db40_0 *field_8;
    struct st_43db40_2 *field_c;
    unsigned int field_10;
    char padding_14[1020];
    unsigned int field_410;
} st_43db40_1;

typedef struct st_43db40_3 {
    char padding_0[102324];
    unsigned int field_18fb4;
} st_43db40_3;

extern st_43db40_3 *g_4b43b8;

int sub_43db40(void)
{
    st_43db40_1 *idx;  // eax
    st_43db40_2 *idx1;  // eax
    st_43db40_2 *idx2;  // esi
    unsigned int v8;  // edx
    st_43db40_2 *index;  // eax
    st_43db40_2 *v10;  // esi
    unsigned int v11;  // eax
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

    idx->field_10 = g_4b43b8->field_18fb4;
    idx1 = sub_46c9ea(36, v0, v1, v2, v3);
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
    idx2->field_8 = sub_43e230;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    idx2->field_4 = v8 | 1;
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
        v10 = index;
    }
    else
    {
        v10 = NULL;
    }
    v11 = v10->field_4 & 0xfffffffd;
    v10->field_8 = sub_43e240;
    v10->field_c = 0;
    v10->field_10 = 0;
    v10->field_20 = idx;
    v10->field_4 = v11 | 1;
    sub_462420();
    idx->field_c = v10;
    sub_402520();
    idx->field_410 = idx->field_10;
    sub_454b80();
    return;
}
