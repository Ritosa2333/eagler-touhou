// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474261; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_474261_0 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    char padding_c[156];
    unsigned int field_a8;
    unsigned int field_ac;
    char padding_b0[12];
    unsigned int field_bc;
    char padding_c0[8];
    unsigned int field_c8;
    char padding_cc[8];
    unsigned int field_d4;
} st_474261_0;

extern st_474261_0 *g_4adab8;
extern unsigned int g_4adabc;
extern unsigned int g_4adea0;
extern unsigned int g_4adf60;
extern unsigned int g_4adf98;
extern unsigned int g_4adf9c;
extern unsigned int g_4b40f8;
extern unsigned int g_4b40fc;

void sub_474261(void)
{
    g_4b40f8 = g_4adab8->field_4;
    g_4b40fc = g_4adab8->field_8;
    g_4adabc = g_4adab8->field_a8;
    g_4adf60 = g_4adab8->field_d4;
    g_4adf98 = g_4adab8->field_bc;
    g_4adea0 = g_4adab8->field_c8;
    g_4adf9c = g_4adab8->field_ac;
    return;
}
