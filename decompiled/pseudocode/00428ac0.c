// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428ac0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428ac0_1 {
    char padding_0[1104];
    short field_450;
    short field_452;
} st_428ac0_1;

typedef struct st_428ac0_0 {
    char padding_0[1200];
    struct st_428ac0_1 *field_4b0;
} st_428ac0_0;

extern unsigned int g_4af284[4];

unsigned int * sub_428ac0(st_428ac0_0 *a0, unsigned int *a1)
{
    unsigned int v1;  // ecx
    unsigned int *v2;  // eax
    unsigned int v3;  // esi

    v1 = a0->field_4b0->field_450;
    v2 = a1;
    if (g_4af284[52 * v1] >= NULL)
    {
        v3 = a0->field_4b0->field_452;
        v2 = *((int *)(208 * v1 + 0x4 * v2 + 4 * v3 + 8 * v3 + (char *)&g_4af284[0]));
    }
    return v2;
}
