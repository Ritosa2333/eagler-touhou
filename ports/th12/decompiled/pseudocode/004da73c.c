// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da73c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_1e0fcc76;

int sub_4da73c(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned short v12;  // ss
    unsigned int v13;  // eax
    unsigned int v14;  // 4115
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4106
    void* v7;  // ecx
    unsigned int v8;  // 4106
    void* v9;  // ebx
    unsigned int v10;  // esi
    unsigned int v11;  // cc_dep1
    unsigned int v0;  // [bp+0x0]

    v6 = _ccall(4, v2, v3, v4, v5);
    if (v7 != 0x1 & v6 & 1)
        goto LABEL_0x4da7a2;
    v8 = _ccall(8, v2, v3, v4, v5);
    if (!(v8 & 1))
        goto LABEL_0x4da745;
    *((unsigned int *)((char *)v9 - 41)) = *((int *)((char *)v9 - 41)) | v10;
    v11 = *((int *)((char *)v9 - 41)) | v10;
    *((unsigned short *)((char *)v7 - 95)) = v12;
    g_1e0fcc76 = v13;
    x86g_dirtyhelper_OUT(Conv(16->32, Extract((vvar_0{r8|4b} Sar 31<8>), 16bits@0<64>)), vvar_0{r8|4b}, 4<32>)
    v0 = 2024728288;
    v14 = _ccall(10, 15, v11, 0, 0);
    if (!(v14 & 1))
        goto LABEL_0x4da757;
    return;
}
