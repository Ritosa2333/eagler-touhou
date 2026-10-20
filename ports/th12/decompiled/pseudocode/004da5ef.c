// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da5ef; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_4da5ef(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4104
    unsigned int v6;  // 4108
    unsigned int v7;  // 4101

    v5 = _ccall(2, v1, v2, v3, v4);
    if (v5 & 1)
        goto LABEL_0x4da597;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v6 = _ccall(8, v1, v2, v3, v4);
    if (v6 & 1)
        goto LABEL_0x4da63e;
    v7 = _ccall(8, v1, v2, v3, v4);
    if (!(v7 & 1))
        goto LABEL_0x4da590;
}
