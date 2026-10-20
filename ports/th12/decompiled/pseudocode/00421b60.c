// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421b60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421b60_0 {
    char padding_0[27928];
    unsigned int field_6d18;
    char padding_6d1c[4];
    int field_6d20;
} st_421b60_0;

extern st_421b60_0 *g_4b43e4;

unsigned int sub_421b60(void)
{
    if (!(g_4b43e4->field_6d18 & 0x200))
    {
        return 0;
    }
    else if (g_4b43e4->field_6d20 < 120)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
