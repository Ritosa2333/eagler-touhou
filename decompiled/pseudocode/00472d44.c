// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472d44; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_472d44_0 {
    unsigned int field_0;
} st_472d44_0;

extern int g_498334;
extern unsigned int *g_498338;
extern unsigned int *g_498354;
extern int g_49d574;
extern st_472d44_0 *g_4d6438;

unsigned int sub_472d44(int a0)
{
    unsigned int v1;  // eax
    char v0;  // [bp+0x0]

    if (sub_477a40(&g_49d574))
        sub_475a4b(a0);
    sub_47c20c(&v0);
    v1 = sub_472ca8(&g_498338, &g_498354);
    if (v1)
        return v1;
    sub_46da32(sub_47b343);
    sub_472c8b(&g_498334);
    if (!g_4d6438 || !sub_477a40(&g_4d6438))
        return 0;
    g_4d6438(0, 2, 0);
    return 0;
}
