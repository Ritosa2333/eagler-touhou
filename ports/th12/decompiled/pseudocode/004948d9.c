// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4948d9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4948d9(void)
{
    unsigned int v0;  // [bp-0x2c]
    unsigned int v1;  // [bp-0x28]
    unsigned int v2;  // [bp-0x24]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x1c]
    unsigned int v5;  // [bp-0x18]

    if (/* unsupported instruction */)
    {
        x86g_dirtyhelper_storeF80le((Reference vvar_28{s-32|4b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
        /* unsupported instruction */
    }
    else
    {
        x86g_dirtyhelper_storeF80le((Reference vvar_28{s-32|4b}), Reinterpret(F64->I64, nan<64>))
        /* unsupported instruction */
    }
    if (/* unsupported instruction */)
    {
        x86g_dirtyhelper_storeF80le((Reference vvar_25{s-44|4b}), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
        /* unsupported instruction */
    }
    else
    {
        x86g_dirtyhelper_storeF80le((Reference vvar_25{s-44|4b}), Reinterpret(F64->I64, nan<64>))
        /* unsupported instruction */
    }
    sub_494310(v0, v1, v2, v3, v4, v5);
    return;
}
