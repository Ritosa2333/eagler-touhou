// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412560_0 {
    char padding_0[120];
    unsigned int field_78;
} st_412560_0;

extern st_412560_0 *g_4b43dc;

int sub_412560(void)
{
    return g_4b43dc->field_78;
}
