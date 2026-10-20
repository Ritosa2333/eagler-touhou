// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43cc50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43cc50_2 {
    char padding_0[6312];
    struct st_43cc50_1 *field_18a8;
    struct st_43cc50_0 *field_18ac;
} st_43cc50_2;

typedef struct st_43cc50_0 {
    char padding_0[4];
    struct st_43cc50_1 *field_4;
} st_43cc50_0;

typedef struct st_43cc50_1 {
    char padding_0[8];
    struct st_43cc50_0 *field_8;
} st_43cc50_1;

int sub_43cc50(void)
{
    st_43cc50_2 *idx;  // eax

    if (idx->field_18a8)
        idx->field_18a8->field_8 = idx->field_18ac;
    if (idx->field_18ac)
        idx->field_18ac->field_4 = idx->field_18a8;
    idx->field_18ac = NULL;
    idx->field_18a8 = NULL;
    return;
}
