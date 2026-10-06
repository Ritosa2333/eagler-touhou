// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da297; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4da297(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4101
    unsigned short v7;  // ds
    unsigned short v0;  // [bp-0x4]

    v6 = _ccall(8, v2, v3, v4, v5);
    if (v6 & 1)
        goto LABEL_0x4da233;
    v0 = v7;
    __unsupported_jumpkind_Ijk_NoDecode()
}
