// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40baf0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40baf0_0 {
    char padding_0[1236];
    unsigned int field_4d4;
    unsigned int field_4d8;
    char padding_4dc[76];
    unsigned int field_528;
    char padding_52c[24];
    int field_544;
    char padding_548[596];
    unsigned int field_79c;
    int field_7a0;
    unsigned int field_7a4;
    char padding_7a8[4];
    unsigned int field_7ac;
    char padding_7b0[24];
    int field_7c8;
    unsigned int field_7cc;
} st_40baf0_0;

extern char g_4b2ed0;

int sub_40baf0(void)
{
    unsigned int v5;  // ecx
    unsigned int v6;  // esi
    st_40baf0_0 *idx;  // eax
    unsigned int v8;  // eax
    int v0;  // [bp-0x10], Other Possible Types: unsigned int
    int v1;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x8]
    int v3;  // [bp-0x4], Other Possible Types: unsigned int

    v3 = v5;
    v2 = v6;
    v3 = *((int *)&idx->padding_7b0[20]);
    if (idx->field_7a0 >= *((int *)&idx->padding_7b0[20]))
    {
        if (idx->field_544 >= 0)
            sub_453d90(v5, idx->field_544);
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
        idx->field_7cc = idx->field_7cc + 1;
        if (/* unsupported instruction */)
        {
            idx->field_4d8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_4d8 = nan;
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
            idx->field_4d4 = /* unsupported instruction */;
        else
            idx->field_4d4 = nan;
        v8 = idx->field_7ac;
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
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)v8 & 1))
        {
            if (/* unsupported instruction */)
                idx->field_7a4 = /* unsupported instruction */;
            else
                idx->field_7a4 = nan;
            idx->field_7a0 = 0;
            idx->field_79c = 0xfff0bdc1;
            *((char **)&idx->padding_7a8[0]) = &g_4b2ed0;
            idx->field_7ac = v8 | 1;
        }
        if (/* unsupported instruction */)
        {
            idx->field_7a4 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_7a4 = nan;
            /* unsupported instruction */
        }
        idx->field_7a0 = 0;
        idx->field_79c = 0xffffffff;
        if (idx->field_7cc >= idx->field_7c8)
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
                v0 = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                v0 = nan;
                /* unsupported instruction */
            }
            sub_40d640();
            idx->field_528 = idx->field_528 & 0xffffffbf;
            return;
        }
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
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        v0 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v0 = nan;
        /* unsupported instruction */
    }
    sub_40d640();
    sub_464a80(v0, v1);
    return;
}
