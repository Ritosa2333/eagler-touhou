// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db5b2; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4db5b2(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4114
    unsigned int v6;  // edx
    unsigned long long v7;  // 4115
    char *v8;  // ebx
    void* v9;  // esi
    char v10;  // cl

    v5 = _ccall(v1, v2, v3, v4);
    v7 = _ccall(v6, 1, v5, 4);
    if (((unsigned int)(v7 >> 38) & 1) == 1)
        goto LABEL_0x4db56d;
    __unsupported_jumpkind_Ijk_Privileged()
    *(v8) = *(v8) + (char)v7;
    *((char *)v9 - 103) = *((char *)v9 - 103) & v10;
}
