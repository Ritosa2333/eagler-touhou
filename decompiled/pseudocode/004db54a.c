// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db54a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db54a_0 {
    char padding_0[83];
    struct st_4db54a_1 *field_53;
} st_4db54a_0;

typedef struct st_4db54a_1 {
    char field_0;
} st_4db54a_1;


int sub_4db54a(void)
{
    unsigned int v1;  // cc_op
    unsigned int v2;  // cc_dep1
    unsigned int v11;  // eax
    unsigned int v12;  // ebx
    unsigned int *v13;  // eax
    unsigned int *v14;  // eax
    char *v15;  // edi
    short v16;  // ax
    unsigned short v17;  // ax
    unsigned short v18;  // ax
    unsigned int v19;  // 4200
    unsigned int v20;  // 4202
    unsigned int v3;  // cc_dep2
    unsigned int v22;  // eax
    void* v23;  // eax
    unsigned int v24;  // 4155
    unsigned int v25;  // id
    unsigned int v26;  // ac
    unsigned int v4;  // cc_ndep
    unsigned int v5;  // 4101
    st_4db54a_0 *v6;  // edi
    char *v7;  // edi
    unsigned int v8;  // eax
    unsigned int v9;  // edi
    unsigned int v10;  // 4138

    v5 = _ccall(8, v1, v2, v3, v4);
    if (v5 & 1)
        goto LABEL_0x4db4f9;
    InterlockedExchange((v6->padding_0)[1], v6);
    v7 = *((int *)&(v6->padding_0)[1]);
    __unsupported_jumpkind_Ijk_NoDecode()
    *(v7) = v8;
    v9 = v7 + 1;
    v10 = _ccall(21, 3728391231, 0, 0);
    v11 = v8 + 1;
    *((char *)(*((int *)0xde3abc3f) + 16)) = *((char *)(*((int *)0xde3abc3f) + 16)) - *((char *)((void*)&v12 + 1)) - ((char)v10 & 1);
    v13 = _INSERT(v11, 1, *((char *)((void*)&v11 + 1)) - *((char *)(*((int *)0xde3abc3f) + 103)));
    if (*((char *)((void*)&v11 + 1)) == *((char *)(*((int *)0xde3abc3f) + 103)))
        goto LABEL_0x4db508;
    v14 = v13 ^ *(v13) | 3147604688;
    v15 = v9 + *(v14);
    v16 = _INSERT(v14, 0, *(0xb5b479a3));
    v17 = _INSERT(v16, 0, v16 / *(v15));
    v18 = _INSERT(v17, 1, v16 % *(v15));
    v19 = _ccall(3, v9, *(v14), 0);
    v20 = _ccall(CONCAT((unsigned short)v19, v18), 55);
    x86g_dirtyhelper_OUT(Conv(16->32, vvar_2{r16|2b}), Insert((vvar_41{r8|2b} Concat 0<16>), 0<64>, Conv(32->16, vvar_71)), 4<32>)
    v22 = syscall();
    v23 = _INSERT(v22, 0, (char)v22 - 58);
    v24 = _ccall(12, v12, *((int *)0xdbce8193) ^ (char)v22 < 58, (char)v22 < 58);
    *((unsigned int *)((char *)v23 - 4)) = v24 | 0x202 | v25 * 0x200000 & 0x200000 | v26 * 0x40000 & 0x40000;
}
