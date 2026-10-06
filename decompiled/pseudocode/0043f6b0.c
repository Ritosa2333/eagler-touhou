// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f6b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f6b0_0 {
    char padding_0[23584];
    unsigned int field_5c20;
    char padding_5c24[4];
    unsigned int field_5c28;
    unsigned int field_5c2c;
    char padding_5c30[4];
    int field_5c34;
} st_43f6b0_0;

extern unsigned int g_4cf468;

st_43f6b0_0 * sub_43f6b0(void)
{
    st_43f6b0_0 *v4;  // edi
    st_43f6b0_0 *idx;  // esi
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    v4 = (!sub_46c9ea(23608, v0, v1) ? NULL : sub_43f250());
    idx = &v4->padding_0[23580];
    g_4cf468 = 0;
    sub_464c40();
    *((void* *)&idx->padding_0[24]) = sub_43f320;
    *((unsigned int *)&idx->padding_0[16]) = 1;
    *((unsigned int *)&idx->padding_0[12]) = 0;
    *((unsigned int *)&idx->padding_0[4]) = sub_46e54b(0, 0, sub_43f320, v4, 0, &idx->padding_0[8]);
    return v4;
}
