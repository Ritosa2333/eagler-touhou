// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9b56; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d9b56(void)
{
    unsigned int v2;  // eax
    unsigned int v3;  // cc_op
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4115
    unsigned int v8;  // 4106
    char v9;  // bh
    char v10;  // bl
    unsigned int v11;  // 4118
    char v0;  // [bp+0x0]

    v2 = _INSERT(&v0, 0, 38);
    v7 = _ccall(2, v3, v4, v5, v6);
    if (!(v7 & 1))
    {
        v8 = _ccall(v3, v4, v5, v6);
        v11 = _ccall(10, 10, v9, v10 ^ (char)v8 & 1, v8 & 1);
        if (!(v11 & 1))
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
        }
    }
    x86g_dirtyhelper_OUT(Conv(16->32, vvar_15{r16|2b}), vvar_28{r8|4b}, 4<32>)
}
