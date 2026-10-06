// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412ee0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412ee0_0 {
    char padding_0[4];
    unsigned int field_4;
} st_412ee0_0;

typedef struct st_412ee0_2 {
    char padding_0[8];
    struct st_412ee0_0 *field_8;
    struct st_412ee0_0 *field_c;
} st_412ee0_2;

extern st_412ee0_2 *g_4b43dc;

void sub_412ee0(void)
{
    if (g_4b43dc->field_8)
        g_4b43dc->field_8->field_4 = g_4b43dc->field_8->field_4 | 2;
    if (g_4b43dc->field_c)
        g_4b43dc->field_c->field_4 = g_4b43dc->field_c->field_4 | 2;
    return;
}
