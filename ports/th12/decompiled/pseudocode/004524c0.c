// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4524c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4524c0_0 {
    char padding_0[24];
    unsigned int field_18;
    int field_1c;
    char padding_20[12];
    unsigned int field_2c;
} st_4524c0_0;

unsigned int sub_4524c0(st_4524c0_0 *idx)
{
    st_4524c0_0 *v0;  // [bp-0x4], Other Possible Types: unsigned int

    v0 = idx;
    if (!idx->field_2c)
    {
        v0 = idx->field_1c;
        if (v0 && *((int *)&idx[1].padding_0[4]) <= v0)
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
            idx->field_18 = sub_4931e0();
            sub_464a80();
            return 1;
        }
    }
    else if (*((int *)&idx[1].padding_0[4]) <= 8)
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
        idx->field_18 = 128 - sub_4931e0();
    }
    else
    {
        return 7;
    }
    sub_464a80();
    return 1;
}
