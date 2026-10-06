// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4daeff; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_48aa8e35;
extern char g_a01b6825;

int sub_4daeff(void)
{
    unsigned short v2;  // ax
    unsigned int v3;  // cc_op
    unsigned int v12;  // esi
    unsigned int v13;  // ebp
    unsigned int v15;  // 4111
    unsigned int v16;  // 4112
    unsigned int v17;  // edi
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4103
    unsigned int v8;  // 4181
    unsigned int v9;  // 4183
    unsigned int v10;  // id
    unsigned int v11;  // ac
    unsigned int v0;  // [bp-0x8]

    g_48aa8e35 = v2;
    v7 = _ccall(14, v3, v4, v5, v6);
    if (!(v7 & 1))
        goto LABEL_0x4daeb4;
    v8 = _ccall(v3, v4, v5, v6);
    v9 = _ccall(CONCAT((unsigned short)v8, v2), 39);
    v15 = _ccall(v3, v4, v5, v6);
    v16 = _ccall(CONCAT((unsigned short)v15, (unsigned short)(*((int *)(v12 + v13 * 8)))(v9 >> 16 & 2261 | 0x202 | v10 * 0x200000 & 0x200000 | v11 * 0x40000 & 0x40000)), 47);
    v0 = v17;
    g_a01b6825 = g_a01b6825 ^ (char)v16;
}
