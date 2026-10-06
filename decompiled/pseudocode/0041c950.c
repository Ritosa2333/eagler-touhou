// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41c950; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41c950_0 {
    char padding_0[4];
    struct st_41c950_1 *field_4;
} st_41c950_0;

typedef struct st_41c950_1 {
    char padding_0[8];
    struct st_41c950_0 *field_8;
} st_41c950_1;

typedef struct st_41c950_2 {
    char padding_0[4];
    struct st_41c950_1 *field_4;
    struct st_41c950_0 *field_8;
} st_41c950_2;

int sub_41c950(void)
{
    st_41c950_2 *idx;  // eax

    if (idx->field_4)
        idx->field_4->field_8 = idx->field_8;
    if (idx->field_8)
        idx->field_8->field_4 = idx->field_4;
    idx->field_8 = NULL;
    idx->field_4 = NULL;
    return;
}
