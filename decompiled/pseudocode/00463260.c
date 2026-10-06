// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x463260; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_463260_1 {
    unsigned int field_0;
} st_463260_1;

typedef struct st_463260_0 {
    char padding_0[28];
    struct st_463260_1 *field_1c;
    char padding_20[4];
    struct st_463260_1 *field_24;
} st_463260_0;

typedef struct st_463260_3 {
    struct st_463260_0 *field_0;
} st_463260_3;

extern st_463260_3 *g_4ce908;
extern unsigned int g_4cee78;
extern unsigned int g_4cf3fc;
extern unsigned int g_4d48b8[4];

int sub_463260(unsigned int a0)
{
    unsigned int *v3;  // edi
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    int v0;  // [bp-0x10c]
    int v1;  // [bp-0x108]
    Byte v2[256];  // [bp-0x104]

    v3 = &g_4d48b8[82 * a0];
    if (g_4cf3fc)
    {
        if (!(g_4cee78 & 0x400))
        {
            GetKeyboardState(v2);
        }
        else if (g_4ce908->field_0->field_24(g_4ce908, 0x100, v2) == 2147942430 || g_4ce908->field_0->field_24(g_4ce908, 0x100, v2))
        {
            g_4ce908->field_0->field_1c(g_4ce908);
        }
    }
    v5 = sub_462a80(v0, v1);
    v3[1] = *(v3);
    *(v3) = v5;
    sub_465440();
    return v6;
}
