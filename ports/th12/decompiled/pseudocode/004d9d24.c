// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9d24; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d9d24(void)
{
    unsigned int v3;  // cc_op
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4101
    unsigned int v8;  // 4101
    unsigned int v9;  // 4113
    unsigned int v10;  // id
    unsigned int v11;  // ac
    unsigned short v12;  // es
    unsigned short v0;  // [bp-0x8]
    unsigned int v1;  // [bp-0x4]

    v7 = _ccall(8, v3, v4, v5, v6);
    if (v7 & 1)
    {
        v8 = _ccall(6, v3, v4, v5, v6);
        if (v8 & 1)
            goto LABEL_0x4d9d5a;
        v9 = _ccall(v3, v4, v5, v6);
        v1 = v9 | 0x202 | v10 * 0x200000 & 0x200000 | v11 * 0x40000 & 0x40000;
        v0 = v12;
    }
}
