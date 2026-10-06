// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425860; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_425860_0 {
    char padding_0[44];
    unsigned int field_2c;
} st_425860_0;

extern st_425860_0 *g_4b4534;

int sub_425860(void)
{
    return g_4b4534->field_2c;
}
