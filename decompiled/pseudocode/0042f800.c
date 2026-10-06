// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42f800; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42f800_0 {
    char padding_0[1144];
    int field_478;
} st_42f800_0;

st_42f800_0 * sub_42f800(void)
{
    st_42f800_0 *v1;  // esi

    if (v1->field_478)
        sub_46c941(v1->field_478);
    v1->field_478 = 0;
    sub_46ca4f(v1);
    return v1;
}
