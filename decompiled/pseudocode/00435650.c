// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x435650; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_435650_2 {
    char padding_0[12];
    unsigned int field_c;
} st_435650_2;

typedef struct st_435650_3 {
    struct st_435650_4 *field_0;
    char padding_4[4];
    struct st_435650_2 *field_8;
    char padding_c[8];
    unsigned int field_14;
} st_435650_3;

typedef struct st_435650_4 {
    struct st_435650_0 *field_0;
} st_435650_4;

typedef struct st_435650_1 {
    unsigned int field_0;
} st_435650_1;

typedef struct st_435650_0 {
    char padding_0[48];
    struct st_435650_1 *field_30;
} st_435650_0;

st_435650_0 ** sub_435650(st_435650_3 *a0)
{
    st_435650_0 **v1;  // eax

    v1 = a0->field_0;
    if (v1 && a0->field_14)
        v1 = *(v1)->field_30(v1, 0, 0, a0->field_8->field_c);
    return v1;
}
