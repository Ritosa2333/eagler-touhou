// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x402030; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_402030_1 {
    unsigned int field_0;
} st_402030_1;

typedef struct st_402030_0 {
    char padding_0[188];
    struct st_402030_1 *field_bc;
} st_402030_0;

typedef struct st_402030_2 {
    struct st_402030_0 *field_0;
} st_402030_2;

extern st_402030_2 *g_4ce8f0;
extern char g_4cec04;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;

unsigned int sub_402030(int a0)
{
    int v0;  // edi
    int v1;  // esi

    sub_45a3c0(v0, v1);
    sub_401790(a0, 1);
    sub_45a3c0(a0, 1);
    g_4cee34 = &g_4cec04;
    sub_430a70();
    g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
    g_4cee38 = 1;
    return 1;
}
