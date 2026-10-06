// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43c530; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43c530_1 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[444];
    int field_1d0;
} st_43c530_1;

typedef struct st_43c530_0 {
    char padding_0[96];
    char field_60;
} st_43c530_0;

extern st_43c530_0 *g_4b44e8;
extern unsigned int g_4d48b8;

unsigned int sub_43c530(st_43c530_1 *a0)
{
    if (g_4b44e8 && !(g_4b44e8->field_60 & 4) && a0->field_10 == 1 && g_4d48b8 & 0x200 && a0->field_1d0 % 6)
        return 6;
    return 1;
}
