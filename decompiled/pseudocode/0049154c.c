// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49154c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_49154c_0 {
    char padding_0[16];
    struct st_49154c_0 *field_10;
    char padding_14[16];
    unsigned int field_24;
} st_49154c_0;


st_49154c_0 * sub_49154c(st_49154c_0 *a0)
{
    if (a0->field_24 < 16)
        return &a0->field_10;
    return a0->field_10;
}
