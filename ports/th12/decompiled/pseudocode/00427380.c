// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427380; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427380_0 {
    char padding_0[96];
    unsigned int field_60;
} st_427380_0;

extern st_427380_0 *g_4b44e8;

unsigned int sub_427380(int a0)
{
    unsigned int v2;  // esi
    unsigned int v3;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    if (g_4b44e8)
    {
        v3 = g_4b44e8->field_60;
        if ((char)(v3 >> 2 | v3) & 1)
        {
            return 1;
        }
        else if (v3 & 0x400)
        {
            return 1;
        }
    }
    return sub_425c20(a0);
}
