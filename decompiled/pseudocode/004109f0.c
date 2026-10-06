// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4109f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4109f0_1 {
    char padding_0[4];
    struct st_4109f0_0 *field_4;
} st_4109f0_1;

typedef struct st_4109f0_0 {
    char padding_0[4];
    struct st_4109f0_0 *field_4;
    struct st_4109f0_0 *field_8;
} st_4109f0_0;

int sub_4109f0(void)
{
    st_4109f0_1 *v1;  // eax
    st_4109f0_1 *v2;  // ecx
    st_4109f0_0 *idx;  // eax
    st_4109f0_0 *index;  // edx

    v2 = &v1->field_4;
    if (v1->field_4)
    {
        do
        {
            idx = (st_4109f0_0 *)v2->padding_0;
            v2 = &idx->field_4;
        } while (idx->field_4);
    }
    if (idx->field_4)
    {
        index->field_4 = idx->field_4;
        idx->field_4->field_8 = index;
    }
    idx->field_4 = index;
    index->field_8 = idx;
    return;
}
