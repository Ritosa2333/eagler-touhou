// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4daea2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4daea2(unsigned int a0, unsigned int a1)
{
    void* v2;  // eax
    char *v3;  // edi
    unsigned int v12;  // cc_dep2
    unsigned int v13;  // cc_ndep
    unsigned int v14;  // 4140
    unsigned int v15;  // 4109
    unsigned int v16;  // 4110
    unsigned int v17;  // eax
    unsigned int v18;  // ecx
    unsigned int v19;  // ebp
    unsigned int v4;  // 4103
    unsigned int v5;  // cc_op
    unsigned int v6;  // cc_dep1
    unsigned int v7;  // cc_dep2
    unsigned int v8;  // cc_ndep
    unsigned int v9;  // 4131
    unsigned int v10;  // cc_op
    char *v11;  // cc_dep1
    char v0;  // [bp+0x0]

    v2 = x86g_dirtyhelper_IN(Conv(16->32, Extract(vvar_1, 16bits@0<64>)), 4<32>);
    *(v3) = *((char *)&v2);
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = *((int *)((char *)v2 - 19));
    v9 = _ccall(v5, v6, v7, v8);
    v10 = 12;
    v11 = &v0;
    v12 = v4 ^ v9 & 1;
    v13 = v9 & 1;
    esp = &(&v0)[-1 * v4 + -1 * (v9 & 1)];
    v14 = _ccall(14, 12, esp, v4 ^ v9 & 1, v9 & 1);
    if (!(v14 & 1))
        goto LABEL_0x4daee3;
    do
    {
        esp = esp - 4;
        *((unsigned int *)(esp - 4)) = a1;
        v15 = _ccall(v10, v11, v12, v13);
        v16 = _ccall(CONCAT((unsigned short)v15, *((unsigned short *)&v2)), 47);
        v17 = _INSERT(v2, 0, v16);
        v18 = _INSERT(a0, 1, 56);
        esp = esp - 4;
        *((int *)(esp - 4)) = esp;
        esp = esp - 4;
        *((unsigned int *)(esp - 4)) = v19;
        v10 = 15;
        v11 = v17 | 670509600;
        v12 = 0;
        v13 = 0;
        v2 = v17 | 670509600;
        a0 = v18 - 1;
    } while (v18 != 1 & (v17 || 1));
    if (a0 != 2)
        goto LABEL_0x4daee7;
    __debugbreak();
}
