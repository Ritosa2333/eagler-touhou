// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421bd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421bd0_0 {
    char padding_0[4];
    struct st_421bd0_1 *field_4;
} st_421bd0_0;

typedef struct st_421bd0_1 {
    char padding_0[8];
    struct st_421bd0_0 *field_8;
} st_421bd0_1;

typedef struct st_421bd0_2 {
    char padding_0[4];
    struct st_421bd0_1 *field_4;
    struct st_421bd0_0 *field_8;
} st_421bd0_2;

int sub_421bd0(void)
{
    st_421bd0_2 *idx;  // eax

    idx->field_4->field_8 = idx->field_8;
    if (idx->field_8)
        idx->field_8->field_4 = idx->field_4;
    return;
}
