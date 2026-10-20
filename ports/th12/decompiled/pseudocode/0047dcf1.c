// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47dcf1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47dcf1_0 {
    char padding_0[4];
    unsigned int field_4;
} st_47dcf1_0;

st_47dcf1_0 * sub_47dcf1(st_47dcf1_0 *idx)
{
    idx->field_4 = idx->field_4 | 0x100;
    return idx;
}
