// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x490b6a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_490b6a_0 {
    char padding_0[112];
    unsigned int field_70;
} st_490b6a_0;

unsigned int sub_490b6a(int a0, int a1, int a2, unsigned int a3, int a4, int a5, int a6, int a7)
{
    unsigned int v2;  // eax
    st_490b6a_0 *idx;  // [bp-0xc]
    char v1;  // [bp-0x8]

    sub_46d3b4(a0);
    v2 = sub_4907fa(a1, a2, a4, a5, a6, a7);
    if (v1)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v2;
}
