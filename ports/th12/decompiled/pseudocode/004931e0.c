// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4931e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

int sub_4931e0(void)
{
    unsigned int v5;  // ebp
    unsigned int v0;  // [bp-0x24]
    unsigned long v1;  // [bp-0x14]
    double v2;  // [bp-0xc], Other Possible Types: long long, unsigned int
    unsigned int v3;  // [bp-0x4]

    if (!g_4d52dc)
    {
        v3 = v5;
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!v1)
        {
            v2 = *((unsigned int *)((void*)&v1 + 4));
            if (!(v2 & 0x7fffffff))
            {
                if (/* unsupported instruction */)
                {
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                }
                if (!/* unsupported instruction */)
                {
                    v2 = nan;
                    /* unsupported instruction */
                    return v1;
                }
                v2 = /* unsupported instruction */;
                /* unsupported instruction */
                return v1;
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (v2 < 0)
        {
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
            return v1 + ((v0 ^ 0x80000000) + 0x7fffffff < (v0 ^ 0x80000000));
        }
        else
        {
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
            return v1 - (v0 + 0x7fffffff < v0);
        }
    }
    else if (/* unsupported instruction */)
    {
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
        return v2;
    }
    else
    {
        v2 = (double)nan;
        /* unsupported instruction */
        return (int)v2;
    }
}
