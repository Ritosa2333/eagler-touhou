// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ec50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44ec50_0 {
    char padding_0[4];
    struct st_44ec50_0 *field_4;
    char padding_8[16];
    unsigned int field_18;
} st_44ec50_0;

st_44ec50_0 * sub_44ec50(st_44ec50_0 *a0)
{
    if (a0->field_18 < 16)
        return &a0->field_4;
    return a0->field_4;
}
