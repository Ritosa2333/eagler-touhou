// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dbb7d; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4dbb7d(void)
{
    unsigned int v3;  // cc_op
    unsigned int v16;  // 4103
    unsigned int v17;  // 4114
    unsigned int idx;  // edx
    unsigned int v4;  // cc_dep1
    unsigned int v5;  // cc_dep2
    unsigned int v6;  // cc_ndep
    unsigned int v7;  // 4101

    while (1)
    {
        v7 = _ccall(4, v3, v4, v5, v6);
        if (v7 & 1)
            break;
    }
    v16 = _ccall(v3, v4, v5, v6);
    v17 = _ccall(5, 18, 1, 0, v16);
    *((char *)(idx * 5 + 0x66)) = 0;
    x86g_dirtyhelper_FSAVE(unsupported_<class 'pyvex.expr.GSPTR'>(), 2874791736<32>)
}
