// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4127d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4127d0_0 {
    char padding_0[124];
    unsigned int field_7c;
} st_4127d0_0;

extern st_4127d0_0 *g_4b43cc;

unsigned int sub_4127d0(void)
{
    return g_4b43cc->field_7c >> 3 & 1;
}
