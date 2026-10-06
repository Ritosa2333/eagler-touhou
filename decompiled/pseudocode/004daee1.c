// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4daee1; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4daee1(void)
{
    unsigned int v2;  // esi
    unsigned long v3;  // ldt
    unsigned long v4;  // gdt
    unsigned long long v5;  // 4102
    unsigned int v0;  // [bp-0x4]

    v0 = v2;
    x86g_dirtyhelper_OUT(34<32>, Conv(8->32, vvar_8{r8|1b}), 1<32>)
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
    v5 = _ccall(v3, v4, 15991, 2078507689);
    goto *((void *)((unsigned int)v5));
}
