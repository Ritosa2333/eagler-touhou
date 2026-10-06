// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4102b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4102b0_1 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[120];
    char field_80[4];
    char field_82;
    char field_83;
    char padding_84[60];
    unsigned int field_c0;
    unsigned int field_c4;
    unsigned int field_c8;
    unsigned int field_cc;
    char padding_d0[4];
    unsigned int field_d4;
} st_4102b0_1;

typedef struct st_4102b0_0 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[120];
    unsigned int field_80;
    char field_82;
    char field_83;
    char padding_84[60];
    unsigned int field_c0;
    unsigned int field_c4;
    unsigned int field_c8;
    unsigned int field_cc;
    char padding_d0[4];
    unsigned int field_d4;
} st_4102b0_0;

typedef struct st_4102b0_2 {
    char padding_0[32];
    unsigned int field_20;
    char padding_24[1036];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[60];
    struct st_4102b0_1 *field_478;
} st_4102b0_2;

extern char g_4b2ed0;

unsigned int sub_4102b0(st_4102b0_2 *index, unsigned int *idx)
{
    st_4102b0_0 *idx1;  // edi
    unsigned int v7;  // eax
    int v8;  // ecx
    st_4102b0_0 *iter;  // eax
    int v0;  // [bp-0x18]
    int v1;  // [bp-0x14]
    int v2;  // [bp-0x10]
    int v3;  // [bp-0xc]
    int node;  // [bp-0x8], Other Possible Types: unsigned int

    idx1 = sub_46d04a(216, v0, v1, v2, v3);
    index->field_478 = idx1;
    sub_477420(idx1, 0, 216);
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
    idx1->field_0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx1->field_4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    node = sub_464440();
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
    if (node < 0)
    {
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
    }
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
    if (/* unsupported instruction */)
    {
        node = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        node = nan;
        /* unsupported instruction */
    }
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
    if (/* unsupported instruction */)
    {
        *((int *)&idx1->padding_84[58]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&idx1->padding_84[58]) = nan;
        /* unsupported instruction */
    }
    v7 = (unsigned int)idx1->padding_d0;
    if (!(*(&idx1->padding_d0[0]) & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        idx1->field_c8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx1->field_c4 = 0;
        idx1->field_c0 = 0xfff0bdc1;
        idx1->field_cc = &g_4b2ed0;
        *((unsigned int *)&idx1->padding_d0[0]) = v7 | 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    idx1->field_c4 = 1;
    idx1->field_c8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    idx1->field_c0 = 0;
    node = 0;
    v8 = 0;
    iter = (char *)&idx1->field_80 + 2;
    do
    {
        *((unsigned int *)((char *)iter - 2)) = 0xff0040ff;
        if (v8 < 8)
        {
            *((char *)&iter->field_0) = 0xff - (char)v8 * 32;
        }
        else
        {
            node += 1;
            *((char *)&iter->field_0 + 1) = 0xff - (char)node * 16;
        }
    } while ((v8 = (int)(v8 + 1), iter += 4, v8 < 16));
    index->field_430 = *(idx);
    index->field_434 = idx[1];
    index->field_438 = idx[2];
    index->field_20 = 13;
    return 0;
}
