// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e0a9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46df5e_0 {
    char padding_0[4];
    int field_4;
    unsigned int field_8;
} st_46df5e_0;

extern char g_49cd48;

st_46df5e_0 * sub_46e0a9(st_46df5e_0 *a0, unsigned int a1)
{
    int v0;  // [bp+0x4]

    sub_46df5e(a0, a1, &v0);
    *((char **)&a0->padding_0[0]) = &g_49cd48;
    return a0;
}
