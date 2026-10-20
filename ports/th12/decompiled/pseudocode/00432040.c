// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x432040; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_432040_1 {
    char padding_0[4];
    unsigned int field_4;
} st_432040_1;

typedef struct st_432040_0 {
    char padding_0[20];
    int field_14;
} st_432040_0;

extern char g_4b0ce0;
extern st_432040_0 *g_4b44e8;
extern char g_4cee78;
extern unsigned int g_4d48c4;

unsigned int sub_432040(unsigned int a0)
{
    unsigned int v2;  // esi
    st_432040_1 *v3;  // edi
    st_432040_1 *v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v1 = v2;
    switch (v3->field_4)
    {
    case 0:
        if (!(g_4b0ce0 & 32) && (g_4d48c4 & 0x100 || g_4cee78 & 16) && sub_4358c0() && g_4b44e8->field_14 >= 30)
        {
            sub_432720();
            break;
        }
        break;
    case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11:
        sub_4329a0(v3);
        break;
    case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19:
        sub_433bb0(v3);
        break;
    case 20: case 21: case 22: case 23: case 24: case 25: case 26:
        sub_434900(v3);
    }
    sub_464a80(v0);
    sub_464a80();
    return 1;
}
