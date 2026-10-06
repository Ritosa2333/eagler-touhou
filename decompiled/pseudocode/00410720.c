// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x410720; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_410720_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    char padding_18[4];
    unsigned int field_1c;
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
} st_410720_0;

typedef struct st_410720_1 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[60];
    struct st_410720_0 *field_478;
} st_410720_1;

extern char g_4b2ed0;

unsigned int sub_410720(st_410720_1 *index, unsigned int a1)
{
    int v1;  // edi
    int v2;  // esi
    int v3;  // ebx
    st_410720_0 *idx;  // esi
    unsigned int v5;  // eax
    unsigned int v6;  // eax

    idx = sub_46d04a(44, v1, v2, v3);
    index->field_478 = idx;
    sub_477420(idx, 0, 44);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v5 = idx->field_1c;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_1c & 1))
    {
        if (/* unsupported instruction */)
            idx->field_14 = /* unsupported instruction */;
        else
            idx->field_14 = nan;
        idx->field_10 = 0;
        idx->field_c = 0xfff0bdc1;
        *((char **)&idx->padding_18[0]) = &g_4b2ed0;
        idx->field_1c = v5 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_14 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_14 = nan;
        /* unsupported instruction */
    }
    idx->field_10 = 0;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    idx->field_c = 0xffffffff;
    v6 = idx->field_0;
    if (/* unsupported instruction */)
    {
        idx->field_20 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_20 = nan;
        /* unsupported instruction */
    }
    idx->field_24 = 0x40ff0000;
    idx->field_28 = 0xff00ff;
    index->field_430 = v6;
    index->field_434 = idx->field_4;
    index->field_438 = idx->field_8;
    index->field_20 = 13;
    return 0;
}
