// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f3b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42f3b0_0 {
    char padding_0[228];
    struct st_42f3b0_1 *field_e4;
} st_42f3b0_0;

typedef struct st_42f3b0_2 {
    struct st_42f3b0_0 *field_0;
} st_42f3b0_2;

typedef struct st_42f3b0_1 {
    unsigned int field_0;
} st_42f3b0_1;

extern st_42f3b0_1 *g_4b450c;
extern int g_4ce8cc;
extern st_42f3b0_2 *g_4ce8f0;
extern unsigned int g_4cea94;

unsigned int sub_42f3b0(void)
{
    unsigned int v1;  // edi
    unsigned int v2;  // esi

    if (!g_4cea94)
        return 1;
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 171, 1, v1, v2);
    sub_45c900(g_4ce8cc);
    sub_4611d0(g_4ce8cc);
    sub_45a3c0();
    if (!g_4b450c)
        return 1;
    g_4b450c();
    return 1;
}
