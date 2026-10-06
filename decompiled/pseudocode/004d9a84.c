// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9a84; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4d9a84(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4125
    unsigned int v6;  // edi
    unsigned int v7;  // edx

    v5 = _ccall(v1, v2, v3, v4);
    *((char *)(v6 - v7 - (v5 & 1) - 1637765806)) = *((char *)(v6 - v7 - (v5 & 1) - 1637765806)) - *((char *)((void*)&v7 + 1));
}
