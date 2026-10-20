// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e810; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e810_2 {
    char padding_0[8];
    struct st_40e810_0 *field_8;
    struct st_40e810_0 *field_c;
} st_40e810_2;

typedef struct st_40e810_0 {
    char padding_0[4];
    unsigned int field_4;
} st_40e810_0;

int sub_40e810(void)
{
    st_40e810_2 *idx;  // eax

    if (idx->field_8)
        idx->field_8->field_4 = idx->field_8->field_4 | 2;
    if (idx->field_c)
        idx->field_c->field_4 = idx->field_c->field_4 | 2;
    return;
}
