// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43c520; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43b950_2 {
    struct st_43b950_3 *field_0;
    char padding_4[168];
    struct st_43b950_0 *field_ac;
    char padding_b0[8];
    struct st_43b950_1 *field_b8;
    char padding_bc[272];
    char field_1cc;
    char padding_1cd[3];
    int field_1d0;
    char padding_1d4[4];
    int field_1d8;
} st_43b950_2;

typedef struct st_43b950_0 {
    unsigned short field_0;
    unsigned short field_2;
    unsigned short field_4;
} st_43b950_0;

typedef struct st_43b950_1 {
    char padding_0[4];
    int field_4;
} st_43b950_1;

typedef struct st_43b950_3 {
    char field_0;
} st_43b950_3;

unsigned int sub_43c520(st_43b950_2 *a0)
{
    st_43b950_2 *v0;  // [bp-0x4]

    v0 = a0;
    return sub_43b950(a0);
}
