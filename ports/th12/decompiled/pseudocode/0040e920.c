// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e920; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e920_0 {
    char padding_0[8];
    unsigned int field_8;
} st_40e920_0;

typedef struct st_40e920_1 {
    char padding_0[100];
    struct st_40e920_0 *field_64;
} st_40e920_1;

extern st_40e920_1 *g_4b43dc;

int sub_40e920(void)
{
    return g_4b43dc->field_64->field_8;
}
