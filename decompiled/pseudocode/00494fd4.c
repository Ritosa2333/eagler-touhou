// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x494fd4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_494fd4(unsigned int a0, unsigned int a1)
{
    unsigned int v7;  // fpround
    unsigned int v8;  // 4234
    unsigned long long v9;  // 4237
    unsigned int v10;  // 4111
    unsigned int v0;  // [bp-0x34]
    int v1;  // [bp-0x30]
    unsigned int v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x4]

    v5 = a1;
    if (/* unsupported instruction */)
    {
        x86g_dirtyhelper_storeF80le((Reference vvar_148{s-28|1b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
        /* unsupported instruction */
    }
    else
    {
        x86g_dirtyhelper_storeF80le((Reference vvar_148{s-28|1b}), Reinterpret(F64->I64, nan<64>))
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        x86g_dirtyhelper_storeF80le((Reference vvar_145{s-52|4b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
        /* unsupported instruction */
    }
    else
    {
        x86g_dirtyhelper_storeF80le((Reference vvar_145{s-52|4b}), Reinterpret(F64->I64, nan<64>))
        /* unsupported instruction */
    }
    if (*((unsigned int *)(&v1 + 2)) & 0x7fff0000)
    {
        sub_494dce();
        return;
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
    if (!v0 && !*((unsigned int *)&v1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        return;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    x86g_dirtyhelper_storeF80le((Reference vvar_147{s-40|1b}), Reinterpret(F64->I64, (((unsupported_<class 'pyvex.expr.GetI'>() CmpNE 0<8>)) ? (unsupported_<class 'pyvex.expr.GetI'>()) : (nan<64>))))
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = _ccall(v7);
    *((unsigned short *)&v3) = v8;
    v4 = v3 | 831;
    v9 = _ccall((unsigned short)v4);
    if ((v2 & 0x7fff) <= 0x7fbe)
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
            x86g_dirtyhelper_storeF80le((Reference vvar_148{s-28|1b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
            /* unsupported instruction */
        }
        else
        {
            x86g_dirtyhelper_storeF80le((Reference vvar_148{s-28|1b}), Reinterpret(F64->I64, nan<64>))
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
            x86g_dirtyhelper_storeF80le((Reference vvar_145{s-52|4b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
            /* unsupported instruction */
        }
        else
        {
            x86g_dirtyhelper_storeF80le((Reference vvar_145{s-52|4b}), Reinterpret(F64->I64, nan<64>))
            /* unsupported instruction */
        }
    }
    else
    {
        v10 = _ccall((unsigned int)v9);
        *((unsigned short *)&v3) = v10;
        v4 = v3 | 0x300;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        x86g_dirtyhelper_storeF80le((Reference vvar_145{s-52|4b}), Reinterpret(F64->I64, (((unsupported_<class 'pyvex.expr.GetI'>() CmpNE 0<8>)) ? (unsupported_<class 'pyvex.expr.GetI'>()) : (nan<64>))))
        /* unsupported instruction */
    }
    sub_494dce();
    return;
}
