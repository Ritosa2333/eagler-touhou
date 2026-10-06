// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dab96; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dab96_0 {
    char padding_0[70];
    char field_46;
} st_4dab96_0;


int sub_4dab96(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    void* v11;  // edx
    st_4dab96_0 *v12;  // edi
    unsigned int v3;  // cc_dep2
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4101
    unsigned int v6;  // ecx
    unsigned int v7;  // esi
    unsigned int v8;  // cc_dep1
    unsigned int v9;  // ecx
    unsigned int v10;  // 4106

    v5 = _ccall(12, v1, v2, v3, v4);
    if (v5 & 1)
    {
        *((unsigned int *)(v6 - 1217292760)) = *((int *)(v6 - 1217292760)) | v7;
        v8 = *((int *)(v6 - 1217292760)) | v7;
        __unsupported_jumpkind_Ijk_NoDecode()
        v9 = v6 - 1;
        v10 = _ccall(5, 15, v8, 0, 0);
        if (v6 != 1 & v10 & 1)
            goto LABEL_0x4dac06;
        if (v9 != 1 & !v8)
            goto LABEL_0x4dac33;
        *((unsigned int *)((char *)v11 - 75)) = *((int *)((char *)v11 - 75)) - 1412028245 + (v12->field_46 < *((char *)((void*)&v9 - 1 + 1)));
    }
}
