// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da391; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4da391(void)
{
    unsigned int v1;  // ebp
    unsigned int v2;  // esi
    char v11;  // cl
    unsigned int v3;  // 4096
    unsigned int v4;  // cc_op
    unsigned int v5;  // cc_dep1
    unsigned int v6;  // cc_dep2
    unsigned int v7;  // cc_ndep
    unsigned int v8;  // 4108
    unsigned int v9;  // 4114
    char *v10;  // edi

    v3 = *((int *)(v1 + v2 - 1435107936));
    *((unsigned int *)(v1 + v2 - 1435107936)) = *((int *)(v1 + v2 - 1435107936)) + 1;
    v8 = _ccall(v4, v5, v6, v7);
    v9 = _ccall(8, 18, v3 + 1, 0, v8);
    if (!(v9 & 1))
        goto LABEL_0x4da3d5;
    *(v10) = *(v10) | v11;
    __unsupported_jumpkind_Ijk_Privileged()
}
