// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423070; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423070_1 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[4];
    struct st_423070_1 *field_c;
    char padding_10[452];
    struct st_423070_0 *field_1d4;
} st_423070_1;

typedef struct st_423070_0 {
    char padding_0[4];
    unsigned int field_4;
} st_423070_0;

extern st_423070_1 *g_4b4518;

st_423070_1 * sub_423070(void)
{
    st_423070_1 *idx;  // eax

    idx = g_4b4518;
    if (g_4b4518->field_1d4)
        g_4b4518->field_1d4->field_4 = g_4b4518->field_1d4->field_4 & 0xfffffffd;
    if (g_4b4518->field_c)
    {
        idx = g_4b4518->field_c;
        idx->field_4 = idx->field_4 & 0xfffffffd;
    }
    return idx;
}
