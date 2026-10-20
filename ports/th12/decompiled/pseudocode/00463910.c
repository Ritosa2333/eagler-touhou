// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x463910; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_463910_1 {
    unsigned int field_0;
} st_463910_1;

typedef struct st_463910_0 {
    char padding_0[28];
    struct st_463910_1 *field_1c;
    char padding_20[4];
    struct st_463910_1 *field_24;
} st_463910_0;

typedef struct st_463910_3 {
    struct st_463910_0 *field_0;
} st_463910_3;

extern st_463910_3 *g_4ce908;
extern unsigned int g_4cee78;
extern unsigned int g_4cf3fc;

unsigned int sub_463910(void)
{
    unsigned int v1;  // esi
    unsigned int v2;  // eax

    sub_477420(v1, 0, 0x100);
    if (!g_4cf3fc)
    {
        return 0;
    }
    else if (!(g_4cee78 & 0x400))
    {
        GetKeyboardState(v1);
        return 2;
    }
    else
    {
        v2 = g_4ce908->field_0->field_24(g_4ce908, 0x100);
        if (v2 != 2147942430 && !v2)
            return 1;
        g_4ce908->field_0->field_1c(g_4ce908);
        return 1;
    }
}
