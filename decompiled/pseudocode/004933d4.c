// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4933d4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4b4114;

char sub_4933d4(void)
{
    unsigned int v1;  // eax
    unsigned int v9;  // 4126
    unsigned int v2;  // fc3210
    unsigned int v3;  // cc_op
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned short v7;  // ftop

    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    do
    {
        v3 = 6;
        v4 = g_4b4114;
        v5 = 1;
        v6 = 0;
        if (g_4b4114 != 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v2 = unsupported_Iop_PRemC3210F64();
        }
        else
        {
            v1 = sub_494d1c();
        }
        v1 = _INSERT(_INSERT(v1, 0, ((char)v7 & 7) * 0), 1, ((v7 & 7) * 0x800 | (unsigned short)v2 & 0x4700) >> 8);
        v9 = _ccall(v3, v4, v5, v6);
    } while ((v9 & 0x800 | v1 >> 8 & 213) >> 2 & 1);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return v1;
}
