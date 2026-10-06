// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x459640; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_459640_0 {
    char padding_0[548];
    unsigned int field_224;
    unsigned int field_228;
    unsigned int field_22c;
    unsigned int field_230;
    unsigned int field_234;
    unsigned int field_238;
    unsigned int field_23c;
    unsigned int field_240;
    unsigned int field_244;
    unsigned int field_248;
    unsigned int field_24c;
    unsigned int field_250;
    unsigned int field_254;
    unsigned int field_258;
    unsigned int field_25c;
    char padding_260[4];
    unsigned int field_264;
    unsigned int field_268;
    unsigned int field_26c;
} st_459640_0;

extern char g_4b2ed0;

int sub_459640(void)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v15;  // edx
    unsigned int v16;  // ecx
    st_459640_0 *idx;  // eax
    unsigned int v8;  // edi
    char *v9;  // ecx
    unsigned int v10;  // edi
    unsigned int v11;  // ebx
    char *v12;  // edx
    unsigned int v13;  // ebp
    unsigned int v14;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]
    unsigned int v3;  // [bp+0x4]
    char v4;  // [bp+0x8]

    v2 = v5;
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = v6;
    idx->field_268 = v3;
    idx->field_23c = 0;
    idx->field_248 = 0;
    v0 = v8;
    idx->field_240 = 0;
    idx->field_24c = 0;
    idx->field_26c = v4;
    idx->field_244 = 0;
    idx->field_250 = 0;
    v10 = v9[2];
    v11 = v9[1];
    v13 = v12[2];
    v14 = v12[1];
    v15 = *(v12);
    idx->field_230 = *(v9);
    idx->field_234 = v11;
    idx->field_224 = v15;
    idx->field_238 = v10;
    idx->field_228 = v14;
    idx->field_22c = v13;
    v16 = idx->field_264;
    if (!((char)idx->field_264 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_25c = /* unsupported instruction */;
        else
            idx->field_25c = nan;
        idx->field_258 = 0;
        idx->field_254 = 0xfff0bdc1;
        *((char **)&idx->padding_260[0]) = &g_4b2ed0;
        idx->field_264 = v16 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_25c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_25c = nan;
        /* unsupported instruction */
    }
    idx->field_258 = 0;
    idx->field_254 = 0xffffffff;
    return;
}
