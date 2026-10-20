// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45fe60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45fe60_0 {
    char padding_0[292];
    unsigned int field_124;
} st_45fe60_0;

extern char g_4b50c0;
extern int g_4ce8cc;
extern unsigned int g_4cee78;

st_45fe60_0 * sub_45fe60(void)
{
    int v2;  // ecx
    int v3;  // ebx
    unsigned int v4;  // eax
    int v5;  // esi
    st_45fe60_0 *v6;  // esi
    unsigned int v7;  // edi
    int v8;  // ebx
    unsigned int v0;  // [bp-0x10]

    if (*((int *)&(&g_4b50c0)[4 * v2 + g_4ce8cc]))
    {
        v4 = sub_4622e0("::preloadAnim already : %s\n", v3);
        return *((int *)&(&g_4b50c0)[4 * v2 + v4]);
    }
    v6 = sub_45fc90(g_4ce8cc, v2, v5);
    if (!v6)
        return v6;
    v0 = v7;
    v6->field_124 = 1;
    do
    { } while (!(g_4cee78 & 384) && (Sleep(1), v6->field_124));
    sub_4622e0("::preloadAnimEnd : %s\n", v8);
    return v6;
}
