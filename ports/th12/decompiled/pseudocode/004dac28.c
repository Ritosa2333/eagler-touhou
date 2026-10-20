// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dac28; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4dac28(void)
{
    unsigned int *v2;  // edi
    unsigned int *v3;  // esi
    unsigned int v4;  // cc_op
    unsigned int v5;  // cc_dep1
    unsigned int v6;  // cc_dep2
    unsigned int v7;  // cc_ndep
    unsigned int v8;  // 4109
    unsigned int v9;  // 4101
    unsigned int v0;  // [bp+0x0]

    *(v2) = *(v3);
    v8 = _ccall(2, v4, v5, v6, v7);
    if (v8 & 1)
        goto LABEL_0x4dabe1;
    __unsupported_jumpkind_Ijk_NoDecode()
    v9 = _ccall(10, v4, v5, v6, v7);
    if (!(v9 & 1))
        goto LABEL_0x4dac2e;
    v0 = 0xffffffa4;
    x86g_dirtyhelper_OUT(Conv(16->32, vvar_1{r16|2b}), Conv(8->32, vvar_0{r8|1b}), 1<32>)
}
