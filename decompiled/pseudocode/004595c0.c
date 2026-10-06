// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4595c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4595c0_0 {
    char padding_0[624];
    unsigned int field_270;
    unsigned int field_274;
    char padding_278[8];
    unsigned int field_280;
    unsigned int field_284;
    unsigned int field_288;
    char padding_28c[4];
    unsigned int field_290;
    unsigned int field_294;
    unsigned int field_298;
} st_4595c0_0;

extern char g_4b2ed0;

int sub_4595c0(void)
{
    st_4595c0_0 *idx;  // eax
    char v3;  // dl
    unsigned int v4;  // ecx
    unsigned int v5;  // ecx
    char v0;  // [bp+0x4]
    char v1;  // [bp+0x8]

    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_298 = v3;
    idx->field_294 = v4;
    idx->field_274 = v1;
    idx->field_270 = v0;
    v5 = idx->field_290;
    if (!((char)idx->field_290 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_288 = /* unsupported instruction */;
        else
            idx->field_288 = nan;
        idx->field_284 = 0;
        idx->field_280 = 0xfff0bdc1;
        *((char **)&idx->padding_28c[0]) = &g_4b2ed0;
        idx->field_290 = v5 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_288 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_288 = nan;
        /* unsupported instruction */
    }
    idx->field_284 = 0;
    idx->field_280 = 0xffffffff;
    return;
}
