// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dab85; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4dab85(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4106
    unsigned int v7;  // ecx
    unsigned int v8;  // esi
    unsigned int v9;  // 4110
    unsigned int v0;  // [bp-0x4]

    v6 = _ccall(4, v2, v3, v4, v5);
    if (v7 != 1 & v6 & 1)
        goto LABEL_0x4dab6a;
    v0 = v8;
    v9 = _ccall(5, v2, v3, v4, v5);
    if (v7 != 2 & v9 & 1)
    {
        x86g_dirtyhelper_OUT(Conv(16->32, vvar_2{r16|2b}), vvar_0{r8|4b}, 4<32>)
        __debugbreak();
    }
}
