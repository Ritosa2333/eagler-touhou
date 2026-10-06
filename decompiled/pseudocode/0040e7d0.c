// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e7d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e7d0_0 {
    char padding_0[102296];
    unsigned int field_18f98;
} st_40e7d0_0;

extern st_40e7d0_0 *g_4b43b8;

st_40e7d0_0 * sub_40e7d0(unsigned int a0)
{
    g_4b43b8->field_18f98 = a0;
    return g_4b43b8;
}
