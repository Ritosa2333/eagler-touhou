// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dbbd6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4dbbd6(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4101
    unsigned int v6;  // edi
    unsigned int v7;  // ebx
    char v8;  // ah

    v5 = _ccall(2, v1, v2, v3, v4);
    if (!(v5 & 1))
        goto LABEL_0x4dbc0c;
    x86g_dirtyhelper_OUT(Conv(16->32, vvar_2{r16|2b}), Conv(8->32, vvar_0{r8|1b}), 1<32>)
    *((char *)((v6 & *((int *)(v7 - 1266783129))) + 89105644)) = v8;
    __debugbreak();
}
