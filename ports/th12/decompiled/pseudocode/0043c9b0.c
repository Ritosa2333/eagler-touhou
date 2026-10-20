// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43c9b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43c9b0_1 {
    char padding_0[5400];
    struct st_43c9b0_0 *field_1518;
} st_43c9b0_1;

typedef struct st_43c9b0_0 {
    unsigned short field_0;
    unsigned short field_2;
    unsigned short field_4;
    char field_6;
} st_43c9b0_0;

int sub_43c9b0(void)
{
    st_43c9b0_1 *idx;  // eax
    unsigned short v3;  // dx
    unsigned short v0;  // [bp+0x4]
    unsigned short v1;  // [bp+0x8]

    idx->field_1518->field_0 = v3;
    idx->field_1518->field_2 = v0;
    idx->field_1518->field_4 = v1;
    idx->field_1518 = &idx->field_1518->field_6;
    return;
}
