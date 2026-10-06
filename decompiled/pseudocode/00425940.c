// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425940; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_425940_1 {
    char padding_0[8];
    struct st_425940_2 *field_8;
    struct st_425940_0 *field_c;
} st_425940_1;

typedef struct st_425940_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_425940_0 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_425940_1 *field_20;
} st_425940_0;

typedef struct st_425940_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_425940_2 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_425940_1 *field_20;
} st_425940_2;

unsigned int sub_425940(st_425940_1 *idx)
{
    st_425940_2 *idx1;  // eax
    unsigned int v5;  // eax
    st_425940_2 *index;  // eax
    st_425940_2 *idx2;  // esi
    unsigned int v8;  // ecx
    int v0;  // [bp-0x10]
    int v1;  // [bp-0xc]
    int v2;  // [bp-0x8]
    int v3;  // [bp-0x4]

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
    }
    else
    {
        idx1 = NULL;
    }
    v5 = idx1->field_4 & 0xfffffffd;
    idx1->field_8 = sub_427380;
    idx1->field_c = 0;
    idx1->field_10 = 0;
    idx1->field_20 = idx;
    idx1->field_4 = v5 | 1;
    sub_462380();
    idx->field_8 = idx1;
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
        idx2 = index;
    }
    else
    {
        idx2 = NULL;
    }
    v8 = idx2->field_4 & 0xfffffffd;
    idx2->field_8 = sub_4273c0;
    idx2->field_c = 0;
    idx2->field_10 = 0;
    idx2->field_20 = idx;
    idx2->field_4 = v8 | 1;
    sub_462420();
    idx->field_c = idx2;
    return 0;
}
