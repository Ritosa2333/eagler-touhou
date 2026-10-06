// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4942a7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4942a7(void)
{
    void* v1;  // ebp

    if (/* unsupported instruction */)
    {
        x86g_dirtyhelper_storeF80le((vvar_25{r28|4b} Sub 158<32>), Reinterpret(F64->I64, unsupported_<class 'pyvex.expr.GetI'>()))
        /* unsupported instruction */
    }
    else
    {
        x86g_dirtyhelper_storeF80le((vvar_25{r28|4b} Sub 158<32>), Reinterpret(F64->I64, nan<64>))
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
    if (*((char *)v1 - 151) & 64)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        x86g_dirtyhelper_storeF80le((vvar_25{r28|4b} Sub 158<32>), Reinterpret(F64->I64, (((unsupported_<class 'pyvex.expr.GetI'>() CmpNE 0<8>)) ? (unsupported_<class 'pyvex.expr.GetI'>()) : (nan<64>))))
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!(*((char *)v1 - 151) & 64))
            goto LABEL_4942dc;
        *((char *)v1 - 144) = 7;
    }
    else
    {
LABEL_4942dc:
        *((char *)v1 - 144) = 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return;
}
