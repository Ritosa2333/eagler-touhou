// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427f70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427f70_0 {
    char padding_0[4];
    struct st_427f70_1 *field_4;
} st_427f70_0;

typedef struct st_427f70_3 {
    char padding_0[1124];
    struct st_427f70_2 *field_464;
    unsigned int field_468;
} st_427f70_3;

typedef struct st_427f70_1 {
    char padding_0[8];
    struct st_427f70_0 *field_8;
} st_427f70_1;

typedef struct st_427f70_2 {
    char padding_0[4];
    struct st_427f70_1 *field_4;
    struct st_427f70_0 *field_8;
} st_427f70_2;

int sub_427f70(void)
{
    st_427f70_3 *index;  // ecx
    st_427f70_2 *idx;  // eax

    index->field_468 = index->field_468 - 1;
    idx->field_4->field_8 = idx->field_8;
    if (idx->field_8)
        idx->field_8->field_4 = idx->field_4;
    if (index->field_464 == idx)
        index->field_464 = idx->field_4;
    return;
}
