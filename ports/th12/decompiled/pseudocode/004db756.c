// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db756; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_e6cdaad9;

int sub_4db756(void)
{
    unsigned int v9;  // cc_op
    unsigned int v10;  // cc_dep1
    char v19;  // 4105
    unsigned short v20;  // ebp
    unsigned int v21;  // edx
    unsigned int v22;  // ebx
    unsigned int v23;  // edi
    unsigned int v11;  // cc_dep2
    unsigned int v12;  // cc_ndep
    unsigned int v13;  // 4114
    unsigned short v14;  // ax
    unsigned int v15;  // 4115
    unsigned int v16;  // eax
    unsigned int v17;  // esi
    char *v18;  // ecx
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x1c]
    char *v3;  // [bp-0x18]
    unsigned int v4;  // [bp-0x14]
    unsigned int v5;  // [bp-0x10]
    char *v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x4]

    v13 = _ccall(v9, v10, v11, v12);
    v15 = _ccall(CONCAT((unsigned short)v13, v14), 63);
    v16 = _INSERT(CONCAT(v14, 0), 0, v15);
    v7 = v17 + 1;
    v19 = *(v18);
    *(v18) = *(v18) - *((char *)((void*)&v20 + 1));
    if (v19 >= *((char *)((void*)&v20 + 1)))
        goto LABEL_0x4db759;
    v6 = v18;
    v5 = v21;
    v4 = v22 & *((int *)(v21 - 416615770));
    v3 = &v7;
    v2 = v16;
    v1 = v7;
    v0 = v23;
    if (!(v22 & *((int *)(v21 - 416615770))))
        goto LABEL_0x4db74a;
    g_e6cdaad9 = g_e6cdaad9 ^ *((char *)((void*)&v4 + 1));
}
