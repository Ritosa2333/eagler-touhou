// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41a6a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41a6a0_0 {
    char padding_0[9996];
    int field_270c;
    char padding_2710[4];
    unsigned int field_2714;
    unsigned int field_2718;
} st_41a6a0_0;

st_41a6a0_0 * sub_41a6a0(int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4)
{
    unsigned int v0;  // esi
    st_41a6a0_0 *idx;  // eax

    idx = a1 * 16 + v0;
    idx->field_270c = a0;
    if (a0 >= 0)
    {
        *((unsigned int *)((a1 + 625) * 16 + v0)) = a2;
        idx->field_2714 = a3;
        idx->field_2718 = a4;
    }
    return idx;
}
