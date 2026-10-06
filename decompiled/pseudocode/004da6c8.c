// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4da6c8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4da6c8_0 {
    char padding_0[59];
    unsigned int field_3b;
} st_4da6c8_0;


int sub_4da6c8(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v12;  // edi
    unsigned int v13;  // cc_dep2
    unsigned int v14;  // cc_ndep
    unsigned short v15;  // es
    unsigned int v16;  // 4107
    unsigned int v17;  // 4110
    unsigned int v18;  // ebx
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4108
    unsigned int v7;  // edx
    unsigned int v8;  // eax
    unsigned int v9;  // 4146
    unsigned int v10;  // eax
    st_4da6c8_0 *idx;  // esi
    unsigned short v0;  // [bp-0x4]

    x86g_dirtyhelper_OUT(Conv(16->32, Extract(vvar_2{r16|4b}, 16bits@0<64>)), Conv(8->32, vvar_1{r12|1b}), 1<32>)
    v6 = _ccall(14, v2, v3, v4, v5);
    if (v6 & 1)
        goto LABEL_0x4da66a;
    v8 = x86g_dirtyhelper_IN(Conv(16->32, Extract(vvar_2{r16|4b}, 16bits@0<64>)), 4<32>);
    v9 = _ccall(v2, v3, v4, v5);
    x86g_dirtyhelper_OUT(Conv(16->32, Extract(vvar_2{r16|4b}, 16bits@0<64>)), Conv(8->32, Extract(vvar_16{r8|4b}, 8bits@0<64>)), 1<32>)
    v10 = _INSERT(v8, 0, 0xff);
    *((unsigned int *)&(idx->padding_0)[1]) = *((int *)&(idx->padding_0)[1]) + v12 + ((v9 ^ 1) & 1);
    v13 = v12 ^ (v9 ^ 1) & 1;
    v14 = (v9 ^ 1) & 1;
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    x86g_dirtyhelper_OUT(102<32>, vvar_24{r8|4b}, 4<32>)
    v0 = v15;
    v16 = _ccall(10, 9, *((int *)&(idx->padding_0)[1]), v13, v14);
    if (v16 & 1)
        goto LABEL_0x4da745;
    v17 = _ccall(9, *((int *)&(idx->padding_0)[1]), v13, v14);
    *((char *)(v7 + v18 * 2 + 100)) = *((char *)(v7 + v18 * 2 + 100)) + (char)(v10 + 1892913163 - (v17 & 1));
}
