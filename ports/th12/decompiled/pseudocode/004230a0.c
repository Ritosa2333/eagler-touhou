// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4230a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4230a0_0 {
    char padding_0[132];
    int field_84;
} st_4230a0_0;

int sub_4230a0(void)
{
    st_4230a0_0 *idx;  // eax

    idx->field_84 = idx->field_84 + 1;
    if (idx->field_84 >= 10)
        idx->field_84 = 9;
    return;
}
