// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464a00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_464a00_2 {
    char padding_0[12];
    struct st_464a00_0 *field_c;
} st_464a00_2;

typedef struct st_464a00_0 {
    char padding_0[4];
    unsigned int field_4;
} st_464a00_0;

extern char g_4b2ed0;

st_464a00_0 * sub_464a00(st_464a00_2 *idx)
{
    st_464a00_0 *index;  // eax

    index = idx->field_c;
    if (index)
        index->field_4 = index->field_4 - 1;
    idx->field_c = &g_4b2ed0;
    return index;
}
