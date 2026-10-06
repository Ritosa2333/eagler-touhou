// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41ce40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41ce40_0 {
    char padding_0[27928];
    unsigned int field_6d18;
} st_41ce40_0;

extern st_41ce40_0 *g_4b43e4;

st_41ce40_0 * sub_41ce40(void)
{
    g_4b43e4->field_6d18 = g_4b43e4->field_6d18 | 32;
    return g_4b43e4;
}
