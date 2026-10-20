// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da3f9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4da3f9(void)
{
    char *v9;  // edi
    char v10;  // al
    char v19;  // 4102
    unsigned int v21;  // edx
    unsigned int v22;  // ebx
    char *v11;  // edi
    unsigned int v12;  // cc_op
    unsigned int v13;  // cc_dep1
    unsigned int v14;  // cc_dep2
    unsigned int v15;  // cc_ndep
    unsigned int v16;  // 4109
    unsigned int v17;  // ecx
    unsigned int v18;  // 4106
    char *v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    char *v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    unsigned int v7;  // [bp-0x4]

    v7 = 1612559211;
    *(v9) = v10;
    v11 = v9 + 0xffffffff;
    v16 = _ccall(6, v12, v13, v14, v15);
    if (v16 & 1)
        goto LABEL_0x4da423;
    v18 = _ccall(0, 4, *((char *)((void*)&v17 + 1)), (char)v17, 0);
    if (v18 & 1)
        goto LABEL_0x4da3bf;
    v19 = *(v11 - 310450291);
    *(v11 - 310450291) = -(*(v11 - 310450291));
    if (0 < v19)
    {
        v6 = _INSERT(CONCAT(v10, 0), 0, *(0x21120e8e));
        v5 = v17;
        v4 = v21;
        v3 = v22;
        v2 = &v7;
        v1 = 554831501;
        v0 = v11;
    }
    x86g_dirtyhelper_OUT(246<32>, Conv(8->32, vvar_0{r8|1b}), 1<32>)
}
