// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40d6e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40d6e0_1 {
    unsigned int field_0;
} st_40d6e0_1;

typedef struct st_40d6e0_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_40d6e0_1 *field_494;
} st_40d6e0_0;

void sub_40d6e0(void)
{
    st_40d6e0_0 *v1;  // esi
    unsigned short v2;  // di

    if (v1->field_494)
        v1->field_494();
    v1->field_3c4 = v2;
    return;
}
