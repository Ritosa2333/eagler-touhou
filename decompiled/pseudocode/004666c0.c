// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4666c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4666c0_3 {
    char padding_0[4];
    struct st_4666c0_2 *field_4;
    char padding_8[12];
    int field_14;
    char padding_18[4];
    unsigned int field_1c;
} st_4666c0_3;

typedef struct st_4666c0_2 {
    struct st_4666c0_0 *field_0;
} st_4666c0_2;

typedef struct st_4666c0_1 {
    unsigned int field_0;
} st_4666c0_1;

typedef struct st_4666c0_0 {
    char padding_0[72];
    struct st_4666c0_1 *field_48;
} st_4666c0_0;

unsigned int sub_4666c0(void)
{
    st_4666c0_3 *idx;  // esi
    st_4666c0_2 **v2;  // eax

    if (idx->field_1c != 1)
        return 0;
    idx->field_14 = idx->field_14 - 1;
    if (idx->field_14 > 0)
    {
        sub_4663e0();
        return 0;
    }
    v2 = idx->field_4;
    idx->field_1c = 0;
    *(v2)->field_0->field_48(*(v2));
    return 1;
}
