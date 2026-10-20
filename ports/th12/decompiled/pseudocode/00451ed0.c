// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451ed0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ce55c;

unsigned int sub_451ed0(int *idx)
{
    unsigned int v3;  // edi
    int v4;  // eax
    unsigned int v0;  // [bp-0xc]
    int *v1;  // [bp-0x4], Other Possible Types: unsigned int

    v1 = idx;
    if (g_4ce55c)
        return 7;
    v0 = v3;
    v1 = idx[7];
    if (v1)
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = sub_4931e0();
        idx[6] = v4;
        if (v4 < 0)
            idx[6] = 0;
    }
    if (idx[13] < v1)
    {
        sub_464a80();
        return 1;
    }
    return 7;
}
