// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4949ec; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4949ec(void)
{
    unsigned int v1;  // eax
    unsigned short v2;  // ftop
    unsigned int v4;  // fc3210
    unsigned int v5;  // eax
    unsigned int v0;  // [bp+0x4]

    v1 = v0 & 0x7f800000;
    if (v1 != 0x7f800000)
    {
        v5 = _INSERT(_INSERT(v1, 0, ((char)v2 & 7) * 0), 1, ((v2 & 7) * 0x800 | (unsigned short)v4 & 0x4700) >> 8);
        if (v5 & 0x3800)
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
            sub_4948d9();
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
            sub_4948d9();
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
