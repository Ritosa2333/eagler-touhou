// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d7c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40d7c0_0 {
    char padding_0[28];
    unsigned int field_1c;
} st_40d7c0_0;

extern st_40d7c0_0 *g_4b43dc;

int sub_40d7c0(void)
{
    return g_4b43dc->field_1c;
}
