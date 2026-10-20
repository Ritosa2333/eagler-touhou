// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4916bf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4916bf_0 {
    char padding_0[24];
    unsigned int field_18;
} st_4916bf_0;

st_4916bf_0 * sub_4916bf(st_4916bf_0 *a0, unsigned int a1, int a2)
{
    int v1;  // esi
    char v0;  // [bp+0x0]

    a0->field_18 = 15;
    sub_44edf0(0, v1, &v0);
    sub_44ebd0(a2);
    return a0;
}
