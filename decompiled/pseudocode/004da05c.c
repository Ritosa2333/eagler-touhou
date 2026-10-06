// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da05c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4da05c(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4101
    unsigned long v6;  // ldt
    unsigned long v7;  // gdt
    unsigned short v8;  // gs
    unsigned int v9;  // ebx
    unsigned long long v10;  // 4106

    v5 = _ccall(8, v1, v2, v3, v4);
    if (!(v5 & 1))
    {
        v10 = _ccall(v6, v7, v8, v9 * 4 + 1383262417);
        *((int *)(unsigned int)v10) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    }
    __unsupported_jumpkind_Ijk_NoDecode()
}
