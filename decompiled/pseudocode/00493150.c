// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x493150; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_493150_0 {
    char padding_0[16];
    int field_10;
} st_493150_0;


int sub_493150(unsigned int a0, int a1, st_493150_0 *a2)
{
    int v2;  // ecx
    int v3;  // ebx
    unsigned int v5;  // edi
    unsigned int v6;  // esi
    int v7;  // ecx
    char v0;  // [bp-0x4]
    char v1;  // [bp+0x0]

    sub_48c994(a2, &v0, v2, v3, a1 + 12, &v1)(v5, v6);
    v7 = a2->field_10;
    if (a2->field_10 == 0x100)
        v7 = 2;
    return sub_48c994(v7);
}
