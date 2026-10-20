// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db1b6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4db1b6(void)
{
    unsigned short v2;  // ss
    unsigned int v3;  // cc_op
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4135
    unsigned int v8;  // edx
    unsigned int v9;  // ecx
    unsigned int v10;  // esi
    unsigned short v0;  // [bp-0x4]

    v0 = v2;
    v7 = _ccall(v3, v4, v5, v6);
    *((unsigned int *)(v8 + v9)) = *((int *)(v8 + v9)) - v10 - (v7 & 1);
}
