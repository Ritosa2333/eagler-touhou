// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dac54; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4dac54(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4101
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    unsigned int v8;  // 4117
    unsigned int *v9;  // edx

    v5 = _ccall(6, v1, v2, v3, v4);
    if (!(v5 & 1))
        goto LABEL_0x4dac91;
    x86g_dirtyhelper_OUT(Conv(16->32, Extract(vvar_1{r16|4b}, 16bits@0<64>)), Conv(8->32, vvar_0{r8|1b}), 1<32>)
    v8 = _ccall(14, 1, *((char *)((void*)&v6 + 1)), *((char *)(v7 + v6 * 8 - 26)), 0);
    if (!(v8 & 1))
        *(v9) = *(v9) | v7;
}
