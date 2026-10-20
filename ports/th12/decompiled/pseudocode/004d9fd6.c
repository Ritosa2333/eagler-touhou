// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9fd6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4d9fd6(unsigned int a0)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4108
    unsigned int v6;  // ebx
    unsigned int v7;  // edi
    unsigned int v8;  // 4122
    unsigned int v9;  // 4101

    v5 = _ccall(v1, v2, v3, v4);
    v8 = _ccall(5, 12, v6, v7 ^ v5 & 1, v5 & 1);
    if (a0 != 1 & v8 & 1)
        goto LABEL_0x4da026;
    v9 = _ccall(10, 12, v6, v7 ^ v5 & 1, v5 & 1);
    if (v9 & 1)
        goto LABEL_0x4d9fe0;
}
