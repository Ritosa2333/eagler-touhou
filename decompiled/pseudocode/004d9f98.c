// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9f98; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d9f98(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v12;  // 4108
    unsigned int v13;  // 4248
    unsigned int v14;  // edi
    unsigned int v15;  // 4249
    unsigned int v16;  // eax
    unsigned long v17;  // ldt
    unsigned long v18;  // gdt
    unsigned short v19;  // ds
    unsigned long long v20;  // 4252
    char *v21;  // 4116
    unsigned int v4;  // cc_dep2
    unsigned int v22;  // 4257
    unsigned int v23;  // 4259
    unsigned int v24;  // id
    unsigned int v25;  // ac
    unsigned int v26;  // 4261
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4239
    unsigned short v7;  // ax
    unsigned int v8;  // 4241
    unsigned int idx;  // eax
    char *v10;  // ecx
    char v11;  // 4098
    unsigned int v0;  // [bp+0x0]

    v6 = _ccall(v2, v3, v4, v5);
    v8 = _ccall(CONCAT((unsigned short)v6, v7), 55);
    idx = _INSERT(CONCAT(v7, 0), 0, v8);
    v11 = *(v10);
    *(v10) = v11 >> (*((char *)&v10) & 31);
    v12 = *((int *)(idx + 0x3fc06a3c));
    v13 = _ccall((*((char *)&v10) & 31 ? 25 : 0), (*((char *)&v10) & 31 ? v11 >> (*((char *)&v10) & 31) : v8 >> 16 & 2261), (*((char *)&v10) & 31 ? v11 >> ((*((char *)&v10) & 31) - 1 & 31) : 0), 0);
    *((unsigned int *)(idx + 0x3fc06a3c)) = *((int *)(idx + 0x3fc06a3c)) - v14 - (v13 & 1);
    v15 = _ccall(12, v12, v14 ^ v13 & 1, v13 & 1);
    v16 = _INSERT(idx, 0, (int)((v15 & 1) * 0x80000000) >> 31);
    v20 = _ccall(v17, v18, v19, v14);
    v21 = *((int *)(unsigned int)v20);
    v22 = _ccall(9, v16, 3918328816 ^ v21 < v10, v21 < v10);
    v23 = _ccall(CONCAT((unsigned short)v22, (unsigned short)_INSERT(v16 - 376638480 + (v21 < v10), 0, *(0xb058c70a))), 63);
    v0 = v23 >> 16 & 2261 | 0x202 | v24 * 0x200000 & 0x200000 | v25 * 0x40000 & 0x40000;
    v26 = _ccall(0, 0, v23 >> 16 & 2261, 0, 0);
    if (v26 & 1)
        goto LABEL_0x4d9f71;
}
