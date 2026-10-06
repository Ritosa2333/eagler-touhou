// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4283d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4283d0_0 {
    char padding_0[96];
    unsigned int field_60;
} st_4283d0_0;

extern unsigned int g_4b2ed0;
extern st_4283d0_0 *g_4b44e8;

unsigned int sub_4283d0(unsigned int a0)
{
    unsigned int v2;  // eax
    unsigned int v3;  // edx
    unsigned int v4;  // eax
    unsigned int v0;  // [bp-0x4]

    v0 = a0;
    v2 = g_4b44e8->field_60;
    if ((char)(v2 >> 2 | v2) & 1)
        return 1;
    v3 = v2;
    if (v3 & 0x400)
    {
        return 1;
    }
    else if ((char)v3 & 2)
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
            v0 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v0 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b2ed0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v4 = sub_428270();
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
            g_4b2ed0 = nan;
            /* unsupported instruction */
            return v4;
        }
        g_4b2ed0 = /* unsupported instruction */;
        /* unsupported instruction */
        return v4;
    }
    else
    {
        return sub_428270();
    }
}
