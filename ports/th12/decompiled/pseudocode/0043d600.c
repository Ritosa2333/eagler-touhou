// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d600; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43d600_1 {
    char padding_0[8];
    struct st_43d600_2 *field_8;
    struct st_43d600_0 *field_c;
} st_43d600_1;

typedef struct st_43d600_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_43d600_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_43d600_1 *field_20;
} st_43d600_0;

typedef struct st_43d600_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_43d600_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_43d600_1 *field_20;
} st_43d600_2;

extern char g_4aebf0;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern unsigned int g_4b452c;

unsigned int sub_43d600(st_43d600_1 *idx)
{
    st_43d600_2 *idx1;  // eax
    st_43d600_2 *idx2;  // esi
    st_43d600_2 *index;  // eax
    st_43d600_2 *v7;  // esi
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

    g_4b452c = &g_4aebf0;
    g_4b0cb0 = 0;
    g_4b0cb4 = 0;
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
    idx2->field_4 = idx2->field_4 | 3;
    idx2->field_8 = sub_43dac0;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
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
        v7 = index;
    }
    else
    {
        v7 = NULL;
    }
    v7->field_4 = v7->field_4 | 3;
    v7->field_8 = sub_43dad0;
    v7->field_c = 0;
    v7->field_10 = 0;
    v7->field_20 = idx;
    sub_462420();
    idx->field_c = v7;
    return 0;
}
