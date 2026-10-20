// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406de0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406de0_0 {
    char padding_0[60];
    unsigned int field_3c;
} st_406de0_0;

extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern st_406de0_0 *g_4b43c4;

unsigned int sub_406de0(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v2;  // esi
    int v3;  // edi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = a0;
    v0 = v2;
    if (!g_4b43c4->field_3c)
        return 0;
    switch (g_4b0c94 + g_4b0c90 * 2)
    {
    case 0: case 1: case 5:
        return 0;
    case 2:
        return sub_4073b0();
    case 3:
        return sub_4079d0(v3);
    case 4:
        return sub_408c10();
    default:
        return 0;
    }
}
