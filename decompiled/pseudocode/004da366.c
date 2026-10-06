// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da366; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4da366(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4106
    unsigned int index;  // ecx
    unsigned int idx;  // edx
    unsigned int v8;  // 4104

    v5 = _ccall(4, v1, v2, v3, v4);
    if (!(index != 1 & v5 & 1))
    {
        v8 = _ccall(14, v1, v2, v3, v4);
        if (v8 & 1)
            goto LABEL_0x4da339;
        *((char *)(index + 161645387)) = *((char *)(index + 161645387)) + 1;
    }
    *((char *)(idx + 1465286668)) = *((char *)(idx + 1465286668)) | *((char *)((void*)&idx + 1));
}
