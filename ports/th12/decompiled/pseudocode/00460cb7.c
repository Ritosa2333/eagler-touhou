// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460cb7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460cb7_0 {
    char padding_0[188];
    struct st_460cb7_1 *field_bc;
} st_460cb7_0;

typedef struct st_460cb7_1 {
    unsigned int field_0;
} st_460cb7_1;

typedef struct st_460cb7_2 {
    char padding_0[228];
    struct st_460cb7_1 *field_e4;
} st_460cb7_2;

typedef struct st_460cb7_4 {
    struct st_460cb7_2 *field_0;
} st_460cb7_4;

extern st_460cb7_4 *g_4ce8f0;
extern unsigned int g_4cee38;
extern unsigned int g_4cf278;

int sub_460cb7(void)
{
    unsigned int v2;  // eax
    unsigned int v3;  // edx
    st_460cb7_0 *v4;  // ecx
    unsigned int v5;  // 4098
    unsigned int v6;  // esi
    unsigned int v0;  // [bp-0xc]

    v4->field_bc(v2, v3);
    v5 = g_4cf278;
    g_4cee38 = 2;
    if (v5)
    {
        v0 = v6;
        sub_45a3c0();
        g_4cf278 = 0;
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 28, 0);
    }
    sub_4611d0();
    return;
}
