// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491c40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_491c40_0 {
    char padding_0[8];
    unsigned int field_8;
    int field_c;
    int field_10;
    int field_14;
    int field_18;
    char padding_1c[8];
    unsigned int field_24;
} st_491c40_0;

typedef struct st_491c40_1 {
    char padding_0[4];
    unsigned int field_4;
} st_491c40_1;

unsigned int sub_491c40(st_491c40_1 *a0, st_491c40_0 *a1, int a2)
{
    unsigned int v2;  // ebx
    int v3;  // ecx
    unsigned int v0;  // [bp-0xc]
    char v1;  // [bp+0x0]

    v0 = v2;
    _security_check_cookie(a1->field_8 ^ a1, v3, &v1);
    if (a0->field_4 & 0x66)
    {
        a1->field_24 = 1;
        return 1;
    }
    sub_493064(a0, a1->field_10, a2, 0, a1->field_c, a1->field_14, a1->field_18, 1);
    if (!a1->field_24)
        sub_491a21(a1, a0);
    sub_491b69(291, &v3, 0, 0, 0, 0, 0);
    goto *((void *)(v3));
}
