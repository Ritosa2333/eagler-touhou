// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da270; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_4da270(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4101
    unsigned long v6;  // ldt
    unsigned long v7;  // gdt
    unsigned long long v8;  // 4110

    v5 = _ccall(8, v1, v2, v3, v4);
    if (!(v5 & 1))
        goto LABEL_0x4da22a;
    v8 = _ccall(v6, v7, 34371, 352531498);
    goto *((void *)((unsigned int)v8));
}
