// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406ed0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406ed0_0 {
    char padding_0[60];
    unsigned int field_3c;
} st_406ed0_0;

extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern st_406ed0_0 *g_4b43c4;

unsigned int sub_406ed0(unsigned int a0)
{
    unsigned int v2;  // eax
    unsigned int v3;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    if (g_4b43c4->field_3c)
    {
        v2 = g_4b0c94 + g_4b0c90 * 2;
        v3 = v2;
        if (v3)
        {
            v3 = v2 - 1;
            if (v2 == 1)
                v3 = sub_408580(g_4b43c4);
        }
    }
    return v3;
}
