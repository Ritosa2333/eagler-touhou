// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428430; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428430_0 {
    char padding_0[96];
    char field_60;
} st_428430_0;

extern st_428430_0 *g_4b44e8;

int sub_428430(unsigned int a0)
{
    if (!(g_4b44e8->field_60 & 4))
        return sub_4283a0();
    return 1;
}
