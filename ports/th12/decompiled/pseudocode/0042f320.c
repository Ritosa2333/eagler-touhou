// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f320; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42f320_0 {
    char padding_0[148];
    struct st_42f320_1 *field_94;
    char padding_98[20];
    struct st_42f320_1 *field_ac;
} st_42f320_0;

typedef struct st_42f320_1 {
    unsigned int field_0;
} st_42f320_1;

typedef struct st_42f320_3 {
    struct st_42f320_0 *field_0;
} st_42f320_3;

extern st_42f320_3 *g_4ce8f0;
extern unsigned int g_4cea94;
extern unsigned int g_4cea9c;
extern unsigned int g_4cf2a8;

unsigned int sub_42f320(void)
{
    int v2;  // esi
    st_42f320_0 *v0;  // [bp-0x18]

    if (g_4cea94)
    {
        sub_45a3c0(v2);
        g_4ce8f0->field_0->field_94(g_4ce8f0, 0, g_4cea9c);
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = g_4ce8f0->field_0;
        /* unsupported instruction */
        g_4ce8f0->field_0->field_ac(g_4ce8f0, 0, 0, 3, g_4cf2a8, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0);
    }
    return 1;
}
