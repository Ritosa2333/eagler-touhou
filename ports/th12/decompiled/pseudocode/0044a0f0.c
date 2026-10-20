// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44a0f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44a0f0_0 {
    char padding_0[4];
    unsigned int field_4;
} st_44a0f0_0;

typedef struct st_44a0f0_3 {
    char padding_0[8];
    struct st_44a0f0_0 *field_8;
    struct st_44a0f0_0 *field_c;
    char padding_10[20];
    struct st_44a0f0_0 *field_24;
} st_44a0f0_3;

extern st_44a0f0_3 *g_4b4534;

void sub_44a0f0(void)
{
    if (g_4b4534->field_8)
        g_4b4534->field_8->field_4 = g_4b4534->field_8->field_4 | 2;
    if (g_4b4534->field_c)
        g_4b4534->field_c->field_4 = g_4b4534->field_c->field_4 | 2;
    if (g_4b4534->field_24)
        g_4b4534->field_24->field_4 = g_4b4534->field_24->field_4 | 2;
    return;
}
