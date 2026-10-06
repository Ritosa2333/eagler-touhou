// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42d6e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42d6e0_0 {
    char padding_0[104];
    unsigned int field_68;
    char padding_6c[8];
    unsigned int field_74;
    char padding_78[168];
    unsigned int field_120;
    int field_124;
    unsigned int field_128;
    char padding_12c[4];
    unsigned int field_130;
    char padding_134[24];
    int field_14c;
    unsigned int field_150;
    char padding_154[732];
    unsigned int field_430;
    char padding_434[508];
    int field_630;
} st_42d6e0_0;

extern char g_4b2ed0;

unsigned int sub_42d6e0(st_42d6e0_0 *idx)
{
    unsigned int v5;  // esi
    unsigned int v6;  // eax
    int v0;  // [bp-0x10], Other Possible Types: unsigned int
    int v1;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x8]
    st_42d6e0_0 *v3;  // [bp-0x4], Other Possible Types: unsigned int

    v3 = idx;
    v2 = v5;
    v3 = *((int *)&idx->padding_134[20]);
    if (idx->field_124 >= *((int *)&idx->padding_134[20]))
    {
        if (idx->field_630 >= 0)
            sub_453d90();
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
        idx->field_150 = idx->field_150 + 1;
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
            idx->field_68 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_68 = nan;
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
            idx->field_74 = /* unsupported instruction */;
        else
            idx->field_74 = nan;
        v6 = idx->field_130;
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
        if (!((char)v6 & 1))
        {
            if (/* unsupported instruction */)
                idx->field_128 = /* unsupported instruction */;
            else
                idx->field_128 = nan;
            idx->field_124 = 0;
            idx->field_120 = 0xfff0bdc1;
            *((char **)&idx->padding_12c[0]) = &g_4b2ed0;
            idx->field_130 = v6 | 1;
        }
        if (/* unsupported instruction */)
        {
            idx->field_128 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_128 = nan;
            /* unsupported instruction */
        }
        idx->field_124 = 0;
        idx->field_120 = 0xffffffff;
        if (idx->field_150 >= idx->field_14c)
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
            sub_42e880();
            idx->field_430 = idx->field_430 & 0xffffffef;
            return 1;
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
    sub_42e880();
    sub_464a80(v0, v1);
    return 0;
}
