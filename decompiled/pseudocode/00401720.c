// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x401720; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_401720_0 {
    char padding_0[102268];
    unsigned int field_18f7c;
} st_401720_0;

void sub_401720(int a0)
{
    st_401720_0 *v2;  // esi
    char v0;  // [bp-0x104]
    char v1;  // [bp+0x8]

    sub_46cad8(&v0, a0, &v1);
    sub_4014f0();
    *((unsigned int *)&v2->padding_0[2404 + 312 * v2->field_18f7c]) = 1;
    return;
}
