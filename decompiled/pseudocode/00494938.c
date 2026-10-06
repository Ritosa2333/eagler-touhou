// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x494938; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_494938(void)
{
    unsigned int v2;  // eax
    unsigned short v3;  // ftop
    unsigned int v5;  // fc3210
    unsigned int v6;  // eax
    unsigned int v1;  // [bp+0x8]

    v2 = v1 & 0x7ff00000;
    if (v2 != 0x7ff00000)
    {
        v6 = _INSERT(_INSERT(v2, 0, ((char)v3 & 7) * 0), 1, ((v3 & 7) * 0x800 | (unsigned short)v5 & 0x4700) >> 8);
        if (v6 & 0x3800)
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
            sub_4948c6();
            return;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            x86g_dirtyhelper_storeF80le((Reference vvar_59{s-16|1b}), Reinterpret(F64->I64, (((unsupported_<class 'pyvex.expr.GetI'>() CmpNE 0<8>)) ? (unsupported_<class 'pyvex.expr.GetI'>()) : (nan<64>))))
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4948c6();
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
            return;
        }
    }
    else if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
}
