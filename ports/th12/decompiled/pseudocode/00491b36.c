// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x491b36; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_491b36_0 {
    char padding_0[8];
    unsigned int field_8;
    int field_c;
    int field_10;
    int field_14;
} st_491b36_0;


void sub_491b36(int a0, st_491b36_0 *a1, int a2)
{
    char v0;  // [bp+0x0]

    _security_check_cookie(a1->field_8 ^ a1, &v0);
    sub_493064(a0, a1->field_10, a2, 0, a1->field_c, a1->field_14, a1, 0);
    return;
}
