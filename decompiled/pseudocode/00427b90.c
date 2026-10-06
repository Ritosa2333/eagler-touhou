// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427b90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427b90_0 {
    char padding_0[156];
    unsigned int field_9c;
    unsigned int field_a0;
    unsigned int field_a4;
    unsigned int field_a8;
    unsigned int field_ac;
    unsigned int field_b0;
    unsigned int field_b4;
    unsigned int field_b8;
    unsigned int field_bc;
    unsigned int field_c0;
    unsigned int field_c4;
    unsigned int field_c8;
    unsigned int field_cc;
    unsigned int field_d0;
    unsigned int field_d4;
    char padding_d8[4];
    unsigned int field_dc;
    unsigned int field_e0;
    unsigned int field_e4;
} st_427b90_0;

extern char g_4b2ed0;
extern unsigned int g_4ce8d0;
extern unsigned int g_4ce8d4;
extern unsigned int g_4ce8d8;

int sub_427b90(void)
{
    unsigned int v3;  // esi
    st_427b90_0 *idx;  // eax
    unsigned int *index;  // edx
    unsigned int *idx1;  // ecx
    unsigned int v7;  // ecx
    unsigned int v0;  // [bp-0x4]
    unsigned int v1;  // [bp+0x4]
    char v2;  // [bp+0x8]

    v0 = v3;
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_e0 = v1;
    idx->field_b4 = g_4ce8d0;
    idx->field_b8 = g_4ce8d4;
    idx->field_bc = g_4ce8d8;
    idx->field_c0 = g_4ce8d0;
    idx->field_c4 = g_4ce8d4;
    idx->field_c8 = g_4ce8d8;
    idx->field_e4 = v2;
    idx->field_9c = *(index);
    idx->field_a0 = index[1];
    idx->field_a4 = index[2];
    idx->field_a8 = *(idx1);
    idx->field_ac = idx1[1];
    idx->field_b0 = idx1[2];
    v7 = idx->field_dc;
    if (!((char)idx->field_dc & 1))
    {
        if (/* unsupported instruction */)
            idx->field_d4 = /* unsupported instruction */;
        else
            idx->field_d4 = nan;
        idx->field_d0 = 0;
        idx->field_cc = 0xfff0bdc1;
        *((char **)&idx->padding_d8[0]) = &g_4b2ed0;
        idx->field_dc = v7 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_d4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_d4 = nan;
        /* unsupported instruction */
    }
    idx->field_d0 = 0;
    idx->field_cc = 0xffffffff;
    return;
}
