// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40def0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40def0_0 {
    char padding_0[28];
    unsigned int field_1c;
    char padding_20[88];
    unsigned int field_78;
    char field_7c;
    char padding_7d[3];
    unsigned int field_80;
} st_40def0_0;

typedef struct st_40def0_1 {
    char padding_0[959];
    char field_3bf;
} st_40def0_1;

extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b43b8;
extern unsigned int g_4b451c;
extern unsigned int g_4ce8cc;

unsigned int sub_40def0(unsigned int a0)
{
    st_40def0_0 *idx;  // edi
    st_40def0_1 *v8;  // eax
    unsigned int v9;  // esi
    unsigned int *index;  // eax
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0xc]
    unsigned int v4;  // [bp-0x8]
    unsigned int v5;  // [bp-0x4]

    if (!(idx->field_7c & 1))
        return 1;
    v8 = sub_461920(a0, g_4ce8cc, idx->field_1c);
    if (!v8)
    {
        idx->field_1c = 0;
        return 1;
    }
    v2 = v9;
    *((unsigned int *)(g_4b43b8 + 102300)) = 1;
    *((unsigned int *)(g_4b43b8 + 102296)) = 2;
    *((char *)(g_4b43b8 + 102275)) = v8->field_3bf;
    if (idx->field_7c & 2)
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
        if (/* unsupported instruction */)
        {
            v3 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v3 = nan;
            /* unsupported instruction */
        }
        v1 = idx->field_80;
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
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_4015c0("%8d");
    }
    else
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
        if (/* unsupported instruction */)
        {
            v3 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v3 = nan;
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
            v4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v4 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_4015c0("$");
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
        v3 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v3 = nan;
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
        v4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v4 = nan;
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    index = idx->field_78 * 144 + (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c;
    v1 = index[444];
    v0 = index[443];
    sub_4015c0("%.2d/%.2d");
    *((unsigned int *)(g_4b43b8 + 102296)) = 0;
    *((unsigned int *)(g_4b43b8 + 102300)) = 0;
    *((char *)(g_4b43b8 + 102275)) = 0xff;
    return 1;
}
