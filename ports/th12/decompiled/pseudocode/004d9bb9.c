// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9bb9; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4d9bb9(unsigned int a0)
{
    unsigned int v1;  // eax
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4107
    unsigned int v0;  // [bp-0x4]

    v1 = x86g_dirtyhelper_IN(69<32>, 4<32>);
    v0 = 0xffffffac;
    v6 = _ccall(14, v2, v3, v4, v5);
    if (!(v6 & 1))
        return v1;
}
