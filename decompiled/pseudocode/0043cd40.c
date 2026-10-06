// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43cd40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43cd40_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[8];
    struct st_43cd40_0 *field_18;
    unsigned int field_1c;
    unsigned int field_20;
} st_43cd40_0;

st_43cd40_0 * sub_43cd40(st_43cd40_0 *ptr)
{
    memset(ptr, 0, 36);
    ptr->field_4 = ptr->field_0;
    ptr->field_c = ptr->field_8;
    ptr->field_18 = ptr;
    ptr->field_1c = 0;
    ptr->field_20 = 0;
    return ptr;
}
