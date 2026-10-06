// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x422fe0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_422fe0_3 {
    struct st_422fe0_4 *field_0;
    char padding_4[16];
    unsigned int field_14;
} st_422fe0_3;

typedef struct st_422fe0_4 {
    struct st_422fe0_0 *field_0;
} st_422fe0_4;

typedef struct st_422fe0_1 {
    unsigned int field_0;
} st_422fe0_1;

typedef struct st_422fe0_0 {
    char padding_0[36];
    struct st_422fe0_1 *field_24;
    char padding_28[32];
    struct st_422fe0_1 *field_48;
} st_422fe0_0;

int sub_422fe0(void)
{
    st_422fe0_3 *idx;  // eax
    st_422fe0_0 **v3;  // eax
    char v0;  // [bp-0x4], Other Possible Types: unsigned int

    v3 = idx->field_0;
    idx->field_14 = 0;
    if (v3)
    {
        *(v3)->field_24(v3, &v0);
        idx->field_14 = v0 & 1;
        idx->field_0->field_0->field_48(idx->field_0);
    }
    return;
}
