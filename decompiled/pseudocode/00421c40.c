// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421c40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421c40_0 {
    char padding_0[4];
    unsigned int field_4;
} st_421c40_0;

typedef struct st_421c40_3 {
    char padding_0[8];
    struct st_421c40_0 *field_8;
    struct st_421c40_0 *field_c;
} st_421c40_3;

extern st_421c40_3 *g_4b4524;

st_421c40_0 * sub_421c40(void)
{
    st_421c40_0 *idx;  // eax

    g_4b4524->field_8->field_4 = g_4b4524->field_8->field_4 | 2;
    idx = g_4b4524->field_c;
    idx->field_4 = idx->field_4 | 2;
    return idx;
}
