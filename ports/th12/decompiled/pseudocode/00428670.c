// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x428670; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_428670_0 {
    char padding_0[20];
    struct st_428670_1 *field_14;
} st_428670_0;

typedef struct st_428670_1 {
    unsigned int field_0;
} st_428670_1;

typedef struct st_428670_2 {
    struct st_428670_0 *field_0;
    char padding_4[4];
    struct st_428670_2 *field_8;
    char padding_c[1068];
    unsigned int field_438;
    unsigned int field_43c;
    unsigned int field_440;
    char padding_444[4];
    unsigned int field_448;
    unsigned int field_44c;
} st_428670_2;

typedef struct st_428670_3 {
    char padding_0[24];
    struct st_428670_2 *field_18;
} st_428670_3;

extern char g_4b2ed0;
extern st_428670_3 *g_4b44f4;

unsigned int sub_428670(void)
{
    st_428670_2 *idx;  // ecx
    unsigned int v5;  // esi
    unsigned int v6;  // ftop
    unsigned int v7;  // eax
    unsigned int v8;  // ftop
    st_428670_2 *v9;  // esi
    unsigned int *v10;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    idx = g_4b44f4->field_18;
    if (!idx)
        return 0;
    v2 = v5;
    do
    {
        v7 = idx->field_448;
        v8 = v6 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = idx->field_8;
        if (!((char)idx->field_448 & 1))
        {
            if (/* unsupported instruction */)
                idx->field_440 = /* unsupported instruction */;
            else
                idx->field_440 = nan;
            idx->field_43c = 0;
            idx->field_438 = 0xfff0bdc1;
            *((char **)&idx->padding_444[0]) = &g_4b2ed0;
            idx->field_448 = v7 | 1;
        }
        if (/* unsupported instruction */)
        {
            idx->field_440 = /* unsupported instruction */;
            /* unsupported instruction */
            v6 = v8 + 1;
        }
        else
        {
            idx->field_440 = nan;
            /* unsupported instruction */
            v6 = v8 + 1;
        }
        idx->field_43c = 0;
        idx->field_438 = 0xffffffff;
        v10 = &idx->field_0->field_14->field_0;
        v1 = 0;
        v0 = 1;
        idx->field_44c = 0;
        v10();
        idx = v9;
    } while (idx);
    return 0;
}
