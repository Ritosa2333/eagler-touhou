// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40fe30; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40fe30_0 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[60];
    struct st_40fe30_1 *field_478;
} st_40fe30_0;

typedef struct st_40fe30_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
} st_40fe30_1;

unsigned int sub_40fe30(void)
{
    st_40fe30_0 *idx;  // edi
    unsigned int *index;  // esi
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    index = &idx->field_478->field_0;
    sub_464d50(v0, v1);
    sub_464db0(v0, v1);
    idx->field_430 = *(index);
    idx->field_434 = index[1];
    idx->field_438 = index[2];
    return 0;
}
