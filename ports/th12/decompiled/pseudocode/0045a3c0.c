// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45a3c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45a3c0_0 {
    char padding_0[172];
    unsigned int field_ac;
} st_45a3c0_0;

typedef struct st_45a3c0_2 {
    unsigned int field_0;
} st_45a3c0_2;

typedef struct st_45a3c0_1 {
    char padding_0[268];
    struct st_45a3c0_2 *field_10c;
    char padding_110[60];
    struct st_45a3c0_2 *field_14c;
    char padding_150[20];
    struct st_45a3c0_2 *field_164;
} st_45a3c0_1;

typedef struct st_45a3c0_5 {
    struct st_45a3c0_1 *field_0;
} st_45a3c0_5;

extern char g_4b5647;
extern char g_4b56a0;
extern unsigned int g_4ce8cc;
extern st_45a3c0_5 *g_4ce8f0;

void sub_45a3c0(void)
{
    st_45a3c0_0 *idx;  // esi
    unsigned int v2;  // ecx

    if (!*((int *)(idx + &g_4b56a0)))
        return;
    if ((&g_4b5647)[g_4ce8cc] != 1)
    {
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 4);
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 4);
        (&g_4b5647)[g_4ce8cc] = 1;
    }
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 6, 0);
    g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 3, 0);
    g_4ce8f0->field_0->field_164(g_4ce8f0, 324);
    g_4ce8f0->field_0->field_14c(g_4ce8f0, 4, *((int *)(idx + &g_4b56a0)) * 2, *((int *)&idx[48905].padding_0[120]), 28);
    v2 = *((int *)&idx[48905].padding_0[116]);
    idx->field_ac = idx->field_ac + 1;
    *((unsigned int *)&idx[48905].padding_0[120]) = v2;
    *((unsigned int *)(idx + &g_4b56a0)) = 0;
    return;
}
