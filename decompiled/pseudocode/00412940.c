// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x412940; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_412940_0 {
    char padding_0[8];
    unsigned int field_8;
} st_412940_0;

typedef struct st_412940_1 {
    char padding_0[1164];
    struct st_412940_0 *field_48c;
} st_412940_1;

extern st_412940_1 *g_4b44f4;

unsigned int sub_412940(void)
{
    unsigned int v1;  // eax

    if (!g_4b44f4->field_48c)
        return 0;
    v1 = g_4b44f4->field_48c->field_8;
    g_4b44f4->field_48c = v1;
    return v1;
}
