// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4124e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4124e0_0 {
    char padding_0[60];
    unsigned int field_3c;
} st_4124e0_0;

extern st_4124e0_0 *g_4b43dc;

void sub_4124e0(unsigned int a0)
{
    g_4b43dc->field_3c = g_4b43dc->field_3c ^ (g_4b43dc->field_3c ^ a0) & 1;
    return;
}
