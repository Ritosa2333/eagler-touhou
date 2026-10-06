// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d7e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40d7e0_0 {
    char padding_0[13756];
    unsigned int field_35bc;
} st_40d7e0_0;

extern st_40d7e0_0 *g_4b43c0;

st_40d7e0_0 * sub_40d7e0(void)
{
    g_4b43c0->field_35bc = g_4b43c0->field_35bc | 1;
    return g_4b43c0;
}
