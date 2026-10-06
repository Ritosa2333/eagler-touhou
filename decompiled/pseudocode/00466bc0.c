// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466bc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466bc0_0 {
    char padding_0[8];
    unsigned int field_8;
    char padding_c[32];
    unsigned int field_2c;
    char padding_30[72];
    unsigned int field_78;
    unsigned int field_7c;
    char padding_80[12];
    unsigned int field_8c;
    struct st_466bc0_1 *field_90;
} st_466bc0_0;

typedef struct st_466bc0_1 {
    char padding_0[24];
    unsigned int field_18;
    unsigned int field_1c;
} st_466bc0_1;

int sub_466bc0(void)
{
    st_466bc0_0 *idx;  // ecx
    int v2;  // edi
    int *v3;  // eax

    if (idx->field_7c)
        return;
    if (idx->field_8c == 0xffffffff)
    {
        idx->field_78 = 1;
        idx->field_7c = 0;
        sub_466b10(v2);
        if (idx->field_8c == 0xffffffff)
            return;
    }
    idx->field_90 = v3;
    sub_466e90("%s %d %d\n", v3, v3[6], v3[7]);
    sub_466c90();
    idx->field_2c = idx->field_8;
    return;
}
