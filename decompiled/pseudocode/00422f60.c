// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422f60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41cf40_1 {
    unsigned int field_0;
} st_41cf40_1;

typedef struct st_422f60_0 {
    char padding_0[96];
    unsigned int field_60;
} st_422f60_0;

typedef struct st_41cf40_0 {
    char padding_0[10612];
    unsigned short field_2974;
    char padding_2976[206];
    struct st_41cf40_1 *field_2a44;
} st_41cf40_0;

extern int g_4b0ca0;
extern unsigned int g_4b0ca4;
extern st_41cf40_0 *g_4b43e4;

int sub_422f60(void)
{
    st_422f60_0 *v1;  // eax

    v1->field_60 = 2;
    if (g_4b43e4)
        sub_41cf40(g_4b43e4, g_4b0ca0, g_4b0ca4);
    return;
}
