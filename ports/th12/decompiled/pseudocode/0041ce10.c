// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41ce10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41ce10_0 {
    char padding_0[27928];
    unsigned int field_6d18;
    char padding_6d1c[48];
    unsigned int field_6d4c;
} st_41ce10_0;

extern st_41ce10_0 *g_4b43e4;

unsigned int * sub_41ce10(void)
{
    g_4b43e4->field_6d18 = g_4b43e4->field_6d18 | 16;
    g_4b43e4->field_6d4c = 0;
    return g_4b43e4;
}
