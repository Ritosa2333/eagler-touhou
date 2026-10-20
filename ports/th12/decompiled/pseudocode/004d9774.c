// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9774; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d9774(void)
{
    unsigned int v1;  // ecx
    unsigned int v2;  // cc_op
    unsigned short v11;  // ds
    unsigned int v12;  // esi
    unsigned long long v13;  // 4108
    unsigned int v14;  // 4098
    unsigned int v15;  // 4117
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4101
    unsigned int *v7;  // ebx
    unsigned int v8;  // edi
    unsigned long v9;  // ldt
    unsigned long v10;  // gdt

    if (v1)
    {
        v6 = _ccall(0, v2, v3, v4, v5);
        if (v6 & 1)
            *(v7) = *(v7) | v8;
    }
    else
    {
        v13 = _ccall(v9, v10, v11, v12 + 123);
        v14 = *((int *)(unsigned int)v13);
        *((unsigned int *)(unsigned int)v13) = v14 + 2038079041;
        v15 = _ccall(8, 3, v14, 2038079041, 0);
        if (v15 & 1)
            goto LABEL_0x4d97dd;
    }
}
