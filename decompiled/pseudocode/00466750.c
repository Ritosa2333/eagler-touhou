// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466750; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466750_0 {
    char padding_0[20];
    int field_14;
    char padding_18[4];
    unsigned int field_1c;
} st_466750_0;

unsigned int sub_466750(void)
{
    st_466750_0 *idx;  // esi

    if (idx->field_1c != 3)
        return 0;
    idx->field_14 = idx->field_14 - 1;
    if (idx->field_14 > 0)
    {
        sub_4663e0();
        return 0;
    }
    idx->field_1c = 0;
    return 1;
}
