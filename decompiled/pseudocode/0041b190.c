// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41b190; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41b190_1 {
    unsigned int field_0;
} st_41b190_1;

typedef struct st_41b190_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_41b190_1 *field_494;
} st_41b190_0;

extern st_41b190_0 *g_4ceaa4;

unsigned int sub_41b190(void)
{
    unsigned int v0;  // [bp-0x4]

    if (g_4ceaa4->field_494)
        g_4ceaa4->field_494(v0);
    g_4ceaa4->field_3c4 = 3;
    return 0;
}
