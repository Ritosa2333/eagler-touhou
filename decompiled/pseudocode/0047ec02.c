// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47ec02; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;

int sub_47ec02(void)
{
    unsigned int v4;  // ecx
    unsigned int v5;  // edx
    unsigned int v0;  // [bp-0x8]
    st_47e7bf_0 *v1;  // [bp+0x4]
    unsigned int v2;  // [bp+0x8]
    st_47e7bf_0 *v3;  // [bp+0xc]

    v0 = v4;
    sub_47e9ae(sub_47e56d(&v4, v5, v2), v5, v1, v3);
    return;
}
