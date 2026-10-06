// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x401490; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_401490_0 {
    char padding_0[102320];
    unsigned int field_18fb0;
} st_401490_0;

unsigned int sub_401490(void)
{
    st_401490_0 *idx;  // esi

    sub_4022a0(idx);
    idx->field_18fb0 = idx->field_18fb0 + 1;
    return 1;
}
