// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421690; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421690_0 {
    char padding_0[26504];
    unsigned int field_6788;
} st_421690_0;

extern st_421690_0 *g_4b43e4;

unsigned int sub_421690(unsigned int a0)
{
    unsigned int v3;  // esi
    unsigned int v4;  // esi
    unsigned int v5;  // esi
    unsigned int v6;  // esi
    unsigned int v7;  // eax
    int v0;  // [bp-0xc]
    int v1;  // [bp-0x8]
    int v2;  // [bp-0x4]

    v3 = g_4b43e4->field_6788 + 1;
    if (*((int *)&g_4b43e4->padding_0[22704 + 1204 * v3]))
        sub_454b80(v0, v1, v2);
    v4 = v3 + 1;
    if (v4 >= 4)
        v4 = 1;
    if (*((int *)&g_4b43e4->padding_0[22704 + 1204 * v4]))
        sub_454b80();
    v5 = v4 + 1;
    if (v5 >= 4)
        v5 = 1;
    v6 = v5 * 1204;
    v7 = &g_4b43e4->padding_0[v6 + 21688];
    if (*((int *)&g_4b43e4->padding_0[22704 + v6]))
        v7 = sub_454b80();
    return v7;
}
