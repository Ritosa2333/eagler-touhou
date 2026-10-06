// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48fb3b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48fb3b_1 {
    unsigned int field_0;
    char padding_4[28];
    unsigned int field_20;
    unsigned int field_24;
} st_48fb3b_1;

typedef struct st_48fb3b_0 {
    char padding_0[4];
    struct st_48fb3b_1 *field_4;
} st_48fb3b_0;


unsigned int * sub_48fb3b(void)
{
    st_48fb3b_0 **v2;  // eax
    unsigned int *v3;  // eax
    unsigned int *idx;  // eax
    int v0;  // [bp-0x4]

    v2 = sub_482a13(v0);
    x86g_dirtyhelper_FINIT(unsupported_<class 'pyvex.expr.GSPTR'>())
    sub_48ac71();
    v3 = sub_491220(8064);
    if (!*(v2))
        return v3;
    idx = &*(v2)->field_4->field_0;
    if (!(*(idx) & 65544))
        return idx;
    idx[8] = 0;
    idx[9] = 0xffff;
    return idx;
}
