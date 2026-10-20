// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x453500; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_453500_2 {
    struct st_453500_0 *field_0;
} st_453500_2;

typedef struct st_453500_1 {
    unsigned int field_0;
} st_453500_1;

typedef struct st_453500_0 {
    char padding_0[8];
    struct st_453500_1 *field_8;
} st_453500_0;

extern unsigned int g_4cf4e8;
extern st_453500_2 *g_4cf4f0;
extern HWND g_4cf4f4;
extern st_453500_2 *g_4cf4f8;
extern int g_4d0da8;
extern int g_4d0de8;
extern int g_4d0e6c;
extern st_453500_2 *g_4d0e70;
extern st_453500_2 *g_4d1410;
extern st_453500_2 *g_4d14d4;
extern st_453500_2 *g_4d4754;

unsigned int sub_453500(void)
{
    st_453500_2 **iter;  // esi
    unsigned int v4;  // eax
    st_453500_2 **node;  // esi
    int v6;  // esi
    unsigned int v7;  // eax
    int *iter1;  // esi
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    if (g_4d0e6c)
    {
        sub_46c941(g_4d0e6c);
        g_4d0e6c = 0;
    }
    iter = &g_4d0e70;
    do
    {
        v4 = *(iter);
        if (v4)
        {
            (*((int *)(*((int *)v4) + 8)))(v4);
            *(iter) = 0;
        }
    } while ((iter += 24, iter < &g_4d1410));
    node = &g_4d1410;
    do
    {
        if (*(node))
        {
            sub_46c941(*(node));
            *(node) = 0;
        }
    } while ((node += 4, node < &g_4d14d4));
    if (!g_4cf4f8)
        return 0;
    KillTimer(g_4cf4f4, 0x1);
    sub_453c30(v0, v1);
    g_4cf4e8 = 0;
    (*((int *)&g_4cf4f0->field_0[6].padding_0[0]))(g_4cf4f0);
    if (g_4cf4f0)
    {
        g_4cf4f0->field_0->field_8(g_4cf4f0);
        g_4cf4f0 = 0;
    }
    if (g_4d4754)
    {
        (*((int *)&g_4d4754->field_0->padding_0[0]))(1);
        g_4d4754 = 0;
    }
    if (g_4cf4f8)
    {
        v6 = g_4cf4f8;
        v7 = g_4cf4f8->field_0;
        if (v7)
        {
            (*((int *)(*((int *)v7) + 8)))(v7);
            *((unsigned int *)v6) = 0;
        }
        sub_46ca4f(v6);
        g_4cf4f8 = 0;
    }
    iter1 = &g_4d0da8;
    do
    {
        if (*(iter1))
        {
            sub_46c941(*(iter1));
            *(iter1) = 0;
        }
    } while ((iter1 += 4, iter1 < &g_4d0de8));
    return 0;
}
