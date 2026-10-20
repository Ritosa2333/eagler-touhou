// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dbacf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dbacf_0 {
    char padding_0[5];
    unsigned int field_5;
} st_4dbacf_0;


int sub_4dbacf(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    char v12;  // dl
    unsigned short v13;  // ss
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4101
    st_4dbacf_0 *index;  // ecx
    char *v8;  // esi
    char v9;  // dh
    unsigned int v10;  // 4147
    unsigned int idx;  // eax
    unsigned short v0;  // [bp-0x4]

    v6 = _ccall(14, v2, v3, v4, v5);
    if (v6 & 1)
        goto LABEL_0x4dba94;
    __unsupported_jumpkind_Ijk_Privileged()
    if (!index)
        goto LABEL_0x4dbab9;
    if (!index)
        goto LABEL_0x4dbab9;
    *(v8) = v9;
    v10 = _ccall(v2, v3, v4, v5);
    *((char *)(idx + 0xa1dfe7e)) = *((char *)(idx + 0xa1dfe7e)) - v12 - ((char)v10 & 1);
    v0 = v13;
    *((char **)&(index->padding_0)[1]) = &v8[*((int *)&(index->padding_0)[1])];
    __unsupported_jumpkind_Ijk_NoDecode()
}
