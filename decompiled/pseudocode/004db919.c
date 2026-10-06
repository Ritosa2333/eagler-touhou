// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db919; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4db919(void)
{
    unsigned int v3;  // cc_op
    unsigned int v4;  // cc_dep1
    unsigned int v13;  // 4172
    unsigned int v14;  // 4174
    unsigned int v15;  // ecx
    unsigned int v16;  // 4101
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4101
    unsigned int v8;  // 4101
    unsigned short v9;  // ds
    int v10;  // eax
    unsigned int v11;  // edi
    unsigned int v12;  // eax
    unsigned short v0;  // [bp-0x8]
    unsigned short v1;  // [bp-0x4]

    v7 = _ccall(8, v3, v4, v5, v6);
    if (!(v7 & 1))
        goto LABEL_0x4db91b;
    v8 = _ccall(8, v3, v4, v5, v6);
    if (!(v8 & 1))
    {
        if (v10 > *((int *)v11))
            goto LABEL_0x4db8e1;
        v12 = v11 + 5;
        v13 = _ccall(6, v12, 1631282942, 0);
        v14 = _ccall(CONCAT((unsigned short)v13, (unsigned short)(v12 - 1631282942)), 55);
        if (v15 != 1 & ((v14 >> 16 & 2261) >> 6 & 1) == 1)
            goto LABEL_0x4db9f0;
        syscall();
        v16 = _ccall(6, v3, v4, v5, v6);
        if (v16 & 1)
            goto LABEL_0x4db9f5;
        return;
    }
    v1 = v9;
    v0 = v9;
}
