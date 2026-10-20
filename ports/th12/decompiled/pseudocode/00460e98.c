// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460e98; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460e98_0 {
    char padding_0[188];
    struct st_460e98_1 *field_bc;
} st_460e98_0;

typedef struct st_460e98_1 {
    unsigned int field_0;
} st_460e98_1;

typedef struct st_460e98_2 {
    char padding_0[228];
    struct st_460e98_1 *field_e4;
} st_460e98_2;

typedef struct st_460e98_4 {
    struct st_460e98_2 *field_0;
} st_460e98_4;

extern st_460e98_4 *g_4ce8f0;
extern unsigned int g_4cee38;

int sub_460e98(st_460e98_0 *a0, int a1)
{
    int v1;  // eax

    a0->field_bc(v1, a1);
    g_4cee38 = 1;
    sub_45a3c0(v1, a1);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 14, 0);
    sub_45a3c0(g_4ce8f0, 14, 0);
    g_4ce8f0->field_0->field_e4(g_4ce8f0, 23, 8);
    return sub_4611d0(g_4ce8f0, 23, 8);
}
