// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db148; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db148_1 {
    char padding_0[28];
    unsigned int field_1c;
} st_4db148_1;

typedef struct st_4db148_0 {
    char padding_0[90];
    char field_5a;
} st_4db148_0;


int sub_4db148(void)
{
    unsigned int v11;  // cc_op
    unsigned int v12;  // cc_dep1
    st_4db148_1 *idx;  // ebx
    unsigned int v23;  // 4157
    unsigned short v24;  // ds
    unsigned int v25;  // 4108
    unsigned int v26;  // esi
    unsigned int v27;  // esi
    unsigned int v28;  // eax
    unsigned int v30;  // ebp
    unsigned int v13;  // cc_dep2
    unsigned int v31;  // edi
    unsigned int v14;  // cc_ndep
    unsigned int v15;  // 4122
    char v16;  // dh
    char v17;  // ch
    st_4db148_0 *v18;  // edx
    unsigned int v19;  // eax
    unsigned int v20;  // 4145
    st_4db148_0 *v0;  // [bp-0x31]
    unsigned int v1;  // [bp-0x2d]
    unsigned int v2;  // [bp-0x29]
    unsigned int v3;  // [bp-0x25]
    char *v4;  // [bp-0x21]
    st_4db148_1 *v5;  // [bp-0x1d]
    st_4db148_0 *v6;  // [bp-0x19]
    unsigned int v7;  // [bp-0x15]
    unsigned int v8;  // [bp-0x11]
    unsigned int v9;  // [bp-0xd]

    v15 = _ccall(v11, v12, v13, v14);
    v18 = _INSERT(0, 1, v16 + v17);
    v19 = _INSERT(0, 0, (char)((int)((v15 & 1) * 0x80000000) >> 31) | 56);
    v20 = _ccall(39, &v30, 3270328683, 0);
    v23 = _ccall(14, 10, *((char *)((void*)&v19 + 1)), *((char *)&idx) ^ (char)v20 & 1, v20 & 1);
    if (v23 & 1)
        goto LABEL_0x4db18f;
    x86g_dirtyhelper_FSAVE(unsupported_<class 'pyvex.expr.GSPTR'>(), (Insert(vvar_18{r8|4b}, 1<64>, ((Extract(vvar_18{r8|4b}, 8bits@1<64>) Sub Extract(vvar_3{r20|4b}, 8bits@0<64>)) Sub (Conv(32->8, vvar_52) And 1<8>))) Sub 60<32>))
    *((unsigned short *)&v9) = v24;
    v25 = _ccall(8, 10, *((char *)((void*)&v19 + 1)), *((char *)&idx) ^ (char)v20 & 1, v20 & 1);
    if (v25 & 1)
        goto LABEL_0x4db126;
    v27 = v26 + 4;
    v28 = v9 & v27;
    v18->field_5a = v28;
    idx->field_1c = idx->field_1c & 1429782094;
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
    v8 = _INSERT(v28, 0, (char)v28);
    v7 = &idx * 3270328683;
    v6 = v18;
    v5 = idx;
    v4 = &v9;
    v3 = v30;
    v2 = v27;
    v1 = v31;
    v0 = v18;
}
