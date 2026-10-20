// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e135; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46df5e_0 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
} st_46df5e_0;

extern char g_49cd60;

st_46df5e_0 * sub_46e135(st_46df5e_0 *a0, unsigned int a1, unsigned int a2)
{
    unsigned int v2;  // esi
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v1 = v2;
    v0 = a2;
    sub_46e0ef(a0, a1);
    *((char **)&a0->padding_0[0]) = &g_49cd60;
    return a0;
}
