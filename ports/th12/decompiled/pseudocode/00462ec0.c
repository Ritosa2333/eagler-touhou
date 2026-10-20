// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x462ec0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_462ec0_1 {
    unsigned int field_0;
} st_462ec0_1;

typedef struct st_462ec0_0 {
    char padding_0[28];
    struct st_462ec0_1 *field_1c;
    char padding_20[4];
    struct st_462ec0_1 *field_24;
} st_462ec0_0;

typedef struct st_462ec0_3 {
    struct st_462ec0_0 *field_0;
} st_462ec0_3;

extern st_462ec0_3 *g_4ce908;
extern unsigned int g_4cee78;
extern unsigned int g_4cf3fc;
extern unsigned int g_4d48b8;
extern unsigned int g_4d48bc;

void sub_462ec0(void)
{
    unsigned int v4;  // eax
    int v0;  // [bp-0x108]
    Byte v1[256];  // [bp-0x104]

    if (g_4cf3fc)
    {
        if (!(g_4cee78 & 0x400))
        {
            GetKeyboardState(v1);
        }
        else if (g_4ce908->field_0->field_24(g_4ce908, 0x100, v1) == 2147942430 || g_4ce908->field_0->field_24(g_4ce908, 0x100, v1))
        {
            g_4ce908->field_0->field_1c(g_4ce908);
        }
    }
    v4 = sub_462a80(v0);
    g_4d48bc = g_4d48b8;
    g_4d48b8 = v4;
    sub_465440();
    return;
}
