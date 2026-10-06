// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408940; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408940_0 {
    char padding_0[4];
    unsigned int field_4;
} st_408940_0;

int sub_408940(st_408940_0 *a0, unsigned int a1, unsigned int a2)
{
    return a0->field_4 == a2;
}
