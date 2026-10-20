// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42ef90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42ef90_0 {
    char padding_0[4];
    unsigned int field_4;
} st_42ef90_0;

typedef struct st_42ef90_4 {
    char padding_0[12];
    struct st_42ef90_0 *field_c;
    struct st_42ef90_0 *field_10;
    char padding_14[102316];
    struct st_42ef90_0 *field_18fc0;
} st_42ef90_4;

extern st_42ef90_4 *g_4b43b8;

st_42ef90_0 * sub_42ef90(void)
{
    st_42ef90_0 *idx;  // eax

    g_4b43b8->field_c->field_4 = g_4b43b8->field_c->field_4 | 2;
    g_4b43b8->field_10->field_4 = g_4b43b8->field_10->field_4 | 2;
    idx = g_4b43b8->field_18fc0;
    idx->field_4 = idx->field_4 | 2;
    return idx;
}
