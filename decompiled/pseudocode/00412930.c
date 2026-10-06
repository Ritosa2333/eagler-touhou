// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412930; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412930_0 {
    char padding_0[24];
    unsigned int field_18;
    char padding_1c[1136];
    unsigned int field_48c;
} st_412930_0;

extern st_412930_0 *g_4b44f4;

unsigned int sub_412930(void)
{
    unsigned int v1;  // eax

    v1 = g_4b44f4->field_18;
    g_4b44f4->field_48c = v1;
    return v1;
}
