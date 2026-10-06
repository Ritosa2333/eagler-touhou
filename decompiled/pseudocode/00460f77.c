// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460f77; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460f77_0 {
    char padding_0[188];
    struct st_460f77_1 *field_bc;
} st_460f77_0;

typedef struct st_460f77_1 {
    unsigned int field_0;
} st_460f77_1;

typedef struct st_460f77_4 {
    struct st_460f77_0 *field_0;
} st_460f77_4;

extern st_460f77_4 *g_4ce8f0;
extern char g_4cec04;
extern unsigned int g_4cee34;
extern unsigned int g_4cee38;

unsigned int sub_460f77(st_460f77_0 *a0, int a1)
{
    int v1;  // eax
    unsigned int v2;  // eax

    a0->field_bc(v1, a1);
    g_4cee38 = 0;
    v2 = sub_4611d0(v1, a1);
    g_4cee34 = &g_4cec04;
    sub_430910();
    g_4ce8f0->field_0->field_bc(g_4ce8f0, g_4cee34 + 0xcc);
    g_4cee38 = 1;
    return v2;
}
