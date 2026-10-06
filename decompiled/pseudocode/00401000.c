// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x401000; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_401000_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[102264];
    unsigned int field_18f80;
    unsigned int field_18f84;
    unsigned int field_18f88;
    char padding_18f8c[4];
    unsigned int field_18f90;
    char padding_18f94[16];
    unsigned int field_18fa4;
    unsigned int field_18fa8;
    unsigned int field_18fac;
} st_401000_0;

extern char g_49f4b8;
extern unsigned int g_4b43b8;

st_401000_0 * sub_401000(void)
{
    st_401000_0 *idx;  // esi

    *((char **)&idx->padding_0[0]) = &g_49f4b8;
    sub_4027e0();
    sub_4027e0();
    sub_477420(idx, 0, 102340);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_4 = idx->field_4 | 2;
    idx->field_18f84 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_18f88 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx->field_18fa4 = 1;
    idx->field_18fa8 = 1;
    g_4b43b8 = idx;
    idx->field_18f80 = 0xffffffff;
    idx->field_18f90 = 0;
    idx->field_18fac = 9;
    return idx;
}
