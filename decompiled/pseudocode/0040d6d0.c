// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d6d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40d6d0_0 {
    char padding_0[96];
    unsigned int field_60;
} st_40d6d0_0;

extern st_40d6d0_0 *g_4b44e8;

unsigned int sub_40d6d0(void)
{
    return g_4b44e8->field_60 >> 1 & 1;
}
