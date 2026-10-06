// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421740; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421740_0 {
    char padding_0[27876];
    int field_6ce4;
} st_421740_0;

extern char g_4b0ce0;
extern st_421740_0 *g_4b43e4;
extern unsigned int g_4cee40;

void sub_421740(unsigned int a0)
{
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    if (g_4cee40 != 8 && !(g_4b0ce0 & 32))
        sub_4615a0(g_4b43e4->field_6ce4, &v0, 0, 0);
    return;
}
