// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43cb70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43cb70_0 {
    char padding_0[5400];
    struct st_43cb70_0 *field_1518;
    char padding_151c[900];
    struct st_43cb70_0 *field_18a0;
    struct st_43cb70_0 *field_18a4;
    unsigned int field_18a8;
    unsigned int field_18ac;
} st_43cb70_0;

st_43cb70_0 * sub_43cb70(void)
{
    st_43cb70_0 *idx;  // esi

    sub_477420(idx, 0, 6320);
    idx->field_18a0 = idx->padding_151c;
    idx->field_1518 = idx;
    idx->field_18a4 = idx;
    idx->field_18a8 = 0;
    idx->field_18ac = 0;
    return idx;
}
