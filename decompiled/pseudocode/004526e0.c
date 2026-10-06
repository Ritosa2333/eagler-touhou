// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4526e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4526e0_0 {
    char padding_0[28];
    int field_1c;
    unsigned int field_20;
    unsigned int field_24;
} st_4526e0_0;

extern unsigned int g_4ce55c;
extern unsigned int g_4cee04;
extern unsigned int g_4cee08;

unsigned int sub_4526e0(st_4526e0_0 *idx)
{
    int v4;  // esi
    unsigned int v5;  // eax
    unsigned int v6;  // edx
    unsigned int v8;  // edx
    unsigned int v0;  // [bp-0xc]
    int v1;  // [bp-0x8], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x4]

    if (g_4ce55c)
        return 7;
    sub_464a80(v4);
    v1 = idx->field_1c;
    if (*((int *)&idx[1].padding_0[12]) >= idx->field_1c)
        return 7;
    v0 = idx->field_24 - idx->field_20;
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
    v2 = idx->field_20;
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
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
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
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    v5 = sub_464440();
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
    v6 = v5 % 3;
    if (!v6)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        g_4cee04 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else if (v6 == 1)
    {
LABEL_452774:
        if (/* unsupported instruction */)
        {
            g_4cee04 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            g_4cee04 = nan;
            /* unsupported instruction */
        }
    }
    else if (v6 == 2)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        goto LABEL_452774;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v8 = sub_464440() % 3;
    if (!v8)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        g_4cee08 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return 1;
    }
    else if (v8 == 1)
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
        if (!/* unsupported instruction */)
        {
            g_4cee08 = nan;
            /* unsupported instruction */
            return 1;
        }
        g_4cee08 = /* unsupported instruction */;
        /* unsupported instruction */
        return 1;
    }
    else if (v8 == 2)
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
        /* unsupported instruction */
        /* unsupported instruction */
        g_4cee08 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return 1;
    }
    else
    {
        return 1;
    }
}
