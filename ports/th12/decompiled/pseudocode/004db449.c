// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db449; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned short g_d43f2f16;

int sub_4db449(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4116
    unsigned short v7;  // ax
    unsigned int v8;  // 4118
    unsigned short v9;  // ss
    unsigned int v0;  // [bp+0x0]

    v6 = _ccall(v2, v3, v4, v5);
    v8 = _ccall(CONCAT((unsigned short)v6, v7), 47);
    if (((v8 >> 16 & 2261) >> 6 & 1) != 1)
        goto LABEL_0x4db439;
    g_d43f2f16 = v9;
    syscall(v0);
    __debugbreak();
}
