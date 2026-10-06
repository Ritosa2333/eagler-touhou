// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41ca10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41ca10_0 {
    char padding_0[12];
    struct st_41ca10_1 *field_c;
} st_41ca10_0;

typedef struct st_41ca10_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    struct st_41ca10_1 *field_14;
    unsigned int field_18;
    unsigned int field_1c;
    struct st_41ca10_0 *field_20;
} st_41ca10_1;

unsigned int sub_41ca10(unsigned int a0)
{
    st_41ca10_1 *idx;  // eax
    st_41ca10_0 *v3;  // edi
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    idx = sub_46c9ea(36, v0, v1);
    if (idx)
    {
        idx->field_4 = idx->field_4 & 0xfffffffe;
        idx->field_8 = 0;
        idx->field_c = 0;
        idx->field_10 = 0;
        idx->field_0 = 0;
        idx->field_14 = idx;
        idx->field_18 = 0;
        idx->field_1c = 0;
    }
    else
    {
        idx = NULL;
    }
    idx->field_4 = idx->field_4 | 3;
    idx->field_8 = sub_41cd60;
    idx->field_c = 0;
    idx->field_10 = 0;
    idx->field_20 = v3;
    sub_462420();
    v3->field_c = idx;
    return 0;
}
