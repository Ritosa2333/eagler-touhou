// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x421a10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_421a10_1 {
    unsigned int field_0;
} st_421a10_1;

typedef struct st_421a10_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_421a10_1 *field_494;
} st_421a10_0;

int sub_421a10(void)
{
    st_421a10_0 *v2;  // esi
    unsigned short v3;  // ax
    unsigned int v0;  // [bp-0x4]

    if (v2->field_494)
        v2->field_494(v0);
    v2->field_3c4 = v3 + 7;
    return;
}
