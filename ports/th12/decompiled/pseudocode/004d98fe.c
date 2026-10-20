// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d98fe; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d98fe(void)
{
    unsigned int v3;  // cc_op
    unsigned int v4;  // cc_dep1
    char v13;  // 4106
    unsigned int v14;  // eax
    unsigned int v15;  // 4108
    unsigned int v16;  // 4156
    unsigned int v17;  // 4170
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4121
    unsigned int v8;  // ecx
    unsigned int v9;  // 4129
    unsigned int v10;  // id
    unsigned int v11;  // ac
    void* v12;  // esi
    unsigned int v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v7 = _ccall(v3, v4, v5, v6);
    v9 = _ccall(18, v8 + 1, 0, v7);
    v1 = v9 | 0x202 | v10 * 0x200000 & 0x200000 | v11 * 0x40000 & 0x40000;
    v0 = v8 + 1;
    v13 = *((char *)v12 - 66);
    *((char *)v12 - 66) = *((char *)v12 - 66) & 64;
    v15 = *((int *)(v14 - 1739662896));
    v16 = _ccall(13, v13 & 64, 0, 0);
    *((unsigned int *)(v14 - 1739662896)) = v15 >> 0x11 | v15 * 0x8000;
    v17 = _ccall(0, 33, v15 >> 0x11 | v15 * 0x8000, 0, v16);
    if (v17 & 1)
        goto LABEL_0x4d98d2;
    else
        goto LABEL_0x4d990e;
}
