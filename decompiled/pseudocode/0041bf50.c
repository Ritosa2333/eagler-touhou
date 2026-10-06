// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41bf50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41bf50_1 {
    char padding_0[224];
    unsigned int field_e0;
    char padding_e4[396];
    int field_270;
} st_41bf50_1;

typedef struct st_41bf50_2 {
    char padding_0[88];
    unsigned int field_58;
    char padding_5c[1056];
    unsigned int field_47c;
} st_41bf50_2;

typedef struct st_41bf50_0 {
    char padding_0[100];
    char field_64;
    char padding_65[1291];
    unsigned int field_570;
    char padding_574[34];
    unsigned short field_596;
    char padding_598[1220];
    char field_a5c;
} st_41bf50_0;

extern st_41bf50_0 *g_4b43c8;

unsigned int sub_41bf50(st_41bf50_1 *index)
{
    unsigned int v3;  // eax
    int v4;  // ecx
    st_41bf50_2 *idx;  // eax
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x8]

    v3 = &g_4b43c8->field_64;
    v4 = 0;
    while (!(*((char *)v3) & 1) || *((short *)(v3 + 1330)) != 1 || *((int *)&index->padding_e4[352]) != *((int *)(v3 + 1292)))
    {
        v3 += 2552;
        if (v4 + 1 >= 2000)
            return index->field_270 >= 20;
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
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_4937c0();
    if (/* unsupported instruction */)
    {
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
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
        v1 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v1 = nan;
        /* unsupported instruction */
    }
    v0 = index->field_e0;
    idx = sub_461920();
    if (!idx)
        index->field_e0 = 0;
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
    idx->field_47c = idx->field_47c | 8;
    if (!/* unsupported instruction */)
    {
        idx->field_58 = nan;
        /* unsupported instruction */
        return 0;
    }
    idx->field_58 = /* unsupported instruction */;
    /* unsupported instruction */
    return 0;
}
