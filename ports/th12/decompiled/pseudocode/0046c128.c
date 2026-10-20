// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46c128; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46c128_1 {
    struct st_46c128_1 *field_0;
} st_46c128_1;

typedef struct st_46c128_0 {
    char field_0;
} st_46c128_0;


void sub_46c128(void* a0, void* a1, char iter)
{
    void* v1;  // eax
    char v2;  // 4108
    void* v11;  // eax
    char v12;  // 4112
    void* v13;  // eax
    char v14;  // 4112
    void* v15;  // eax
    char v16;  // 4112
    void* v17;  // eax
    char v18;  // 4112
    unsigned int v19;  // 4133
    void* v20;  // eax
    void* v3;  // eax
    char v21;  // 4112
    unsigned int v22;  // 4133
    void* v23;  // eax
    char v24;  // 4112
    unsigned int v25;  // 4133
    void* v26;  // eax
    char v27;  // 4112
    unsigned int v28;  // 4133
    void* v29;  // eax
    char v30;  // 4112
    char v4;  // 4112
    unsigned int v31;  // 4133
    void* v32;  // eax
    char v33;  // 4112
    unsigned int v34;  // 4133
    void* v35;  // eax
    char v36;  // 4112
    unsigned int v37;  // 4133
    void* v38;  // eax
    char v39;  // 4112
    unsigned int v40;  // 4133
    void* v5;  // eax
    void* v41;  // eax
    void* v42;  // edi
    void* v43;  // eax
    void* v44;  // eax
    char *v45;  // eax
    st_46c128_0 **v46;  // eax
    char *v47;  // eax
    void* v48;  // eax
    st_46c128_1 **v49;  // eax
    void* v50;  // eax
    char v6;  // 4112
    char *v51;  // eax
    st_46c128_0 **v52;  // eax
    char *v53;  // eax
    void* v54;  // eax
    unsigned short v55;  // es
    void* v56;  // eax
    void* v57;  // eax
    void* v58;  // eax
    void* v59;  // eax
    void* v60;  // eax
    void* v7;  // eax
    void* v61;  // ebx
    void* v62;  // ebp
    void* v63;  // esi
    void* v64;  // eax
    char v8;  // 4112
    void* v9;  // eax
    char v10;  // 4112
    unsigned int v0;  // [bp+0x0]

    v1 = _INSERT(0, 0, 0xff);
    *((unsigned int *)v1) = *((int *)v1) + 1;
    *((char *)v1) = *((char *)v1) - 1;
    v2 = *((char *)v1);
    *((char *)v1) = *((char *)v1) - 1;
    if (!((v2 + 0xff >= 0x80000000 ? 1 : 0) & 1))
        goto LABEL_46c136;
LABEL_46c136:
    *((char *)v1) = *((char *)v1) + *((char *)&v1);
    v3 = _INSERT(v1, 0, 0xff);
    *((unsigned int *)v3) = *((int *)v3) + 1;
    *((char *)v3) = *((char *)v3) - 1;
    v4 = *((char *)v3);
    *((char *)v3) = *((char *)v3) - 1;
    if (!((v4 + 0xff >= 0x80000000 ? 1 : 0) & 1))
        goto LABEL_46c146;
LABEL_46c146:
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    v5 = _INSERT(v3, 0, 0xff);
    *((unsigned int *)v5) = *((int *)v5) + 1;
    *((char *)v5) = *((char *)v5) - 1;
    v6 = *((char *)v5);
    *((char *)v5) = *((char *)v5) - 1;
    if (v6 + 0xff >= 0)
        goto LABEL_46c156;
LABEL_46c156:
    *((char *)v5) = *((char *)v5) + *((char *)&v5);
    v7 = _INSERT(v5, 0, 0xff);
    *((unsigned int *)v7) = *((int *)v7) + 1;
    *((char *)v7) = *((char *)v7) - 1;
    v8 = *((char *)v7);
    *((char *)v7) = *((char *)v7) - 1;
    if (v8 + 0xff >= 0)
        goto LABEL_46c166;
LABEL_46c166:
    *((char *)v7) = *((char *)v7) + *((char *)&v7);
    v9 = _INSERT(v7, 0, 0xff);
    *((unsigned int *)v9) = *((int *)v9) + 1;
    *((char *)v9) = *((char *)v9) - 1;
    v10 = *((char *)v9);
    *((char *)v9) = *((char *)v9) - 1;
    if (v10 != 1)
        goto LABEL_46c176;
LABEL_46c176:
    *((char *)v9) = *((char *)v9) + *((char *)&v9);
    v11 = _INSERT(v9, 0, 0xff);
    *((unsigned int *)v11) = *((int *)v11) + 1;
    *((char *)v11) = *((char *)v11) - 1;
    v12 = *((char *)v11);
    *((char *)v11) = *((char *)v11) - 1;
    if (v12 != 1)
        goto LABEL_46c186;
LABEL_46c186:
    *((char *)v11) = *((char *)v11) + *((char *)&v11);
    v13 = _INSERT(v11, 0, 0xff);
    *((unsigned int *)v13) = *((int *)v13) + 1;
    *((char *)v13) = *((char *)v13) - 1;
    v14 = *((char *)v13);
    *((char *)v13) = *((char *)v13) - 1;
    if (v14 + 0xff > 0)
        goto LABEL_46c196;
LABEL_46c196:
    *((char *)v13) = *((char *)v13) + *((char *)&v13);
    v15 = _INSERT(v13, 0, 0xff);
    *((unsigned int *)v15) = *((int *)v15) + 1;
    *((char *)v15) = *((char *)v15) - 1;
    v16 = *((char *)v15);
    *((char *)v15) = *((char *)v15) - 1;
    if (v16 + 0xff > 0)
        goto LABEL_46c1a6;
LABEL_46c1a6:
    *((char *)v15) = *((char *)v15) + *((char *)&v15);
    v17 = _INSERT(v15, 0, 0xff);
    *((unsigned int *)v17) = *((int *)v17) + 1;
    *((char *)v17) = *((char *)v17) - 1;
    v18 = *((char *)v17);
    *((char *)v17) = *((char *)v17) - 1;
    v19 = _ccall(8, 1, v18, 0xff, 0);
    if (!(v19 & 1))
        goto LABEL_46c1b6;
LABEL_46c1b6:
    *((char *)v17) = *((char *)v17) + *((char *)&v17);
    v20 = _INSERT(v17, 0, 0xff);
    *((unsigned int *)v20) = *((int *)v20) + 1;
    *((char *)v20) = *((char *)v20) - 1;
    v21 = *((char *)v20);
    *((char *)v20) = *((char *)v20) - 1;
    v22 = _ccall(8, 1, v21, 0xff, 0);
    if (!(v22 & 1))
        goto LABEL_46c1c6;
LABEL_46c1c6:
    *((char *)v20) = *((char *)v20) + *((char *)&v20);
    v23 = _INSERT(v20, 0, 0xff);
    *((unsigned int *)v23) = *((int *)v23) + 1;
    *((char *)v23) = *((char *)v23) - 1;
    v24 = *((char *)v23);
    *((char *)v23) = *((char *)v23) - 1;
    v25 = _ccall(10, 1, v24, 0xff, 0);
    if (!(v25 & 1))
        goto LABEL_46c1d6;
LABEL_46c1d6:
    *((char *)v23) = *((char *)v23) + *((char *)&v23);
    v26 = _INSERT(v23, 0, 0xff);
    *((unsigned int *)v26) = *((int *)v26) + 1;
    *((char *)v26) = *((char *)v26) - 1;
    v27 = *((char *)v26);
    *((char *)v26) = *((char *)v26) - 1;
    v28 = _ccall(10, 1, v27, 0xff, 0);
    if (!(v28 & 1))
        goto LABEL_46c1e6;
LABEL_46c1e6:
    *((char *)v26) = *((char *)v26) + *((char *)&v26);
    v29 = _INSERT(v26, 0, 0xff);
    *((unsigned int *)v29) = *((int *)v29) + 1;
    *((char *)v29) = *((char *)v29) - 1;
    v30 = *((char *)v29);
    *((char *)v29) = *((char *)v29) - 1;
    v31 = _ccall(12, 1, v30, 0xff, 0);
    if (!(v31 & 1))
        goto LABEL_46c1f6;
LABEL_46c1f6:
    *((char *)v29) = *((char *)v29) + *((char *)&v29);
    v32 = _INSERT(v29, 0, 0xff);
    *((unsigned int *)v32) = *((int *)v32) + 1;
    *((char *)v32) = *((char *)v32) - 1;
    v33 = *((char *)v32);
    *((char *)v32) = *((char *)v32) - 1;
    v34 = _ccall(12, 1, v33, 0xff, 0);
    if (!(v34 & 1))
        goto LABEL_46c206;
LABEL_46c206:
    *((char *)v32) = *((char *)v32) + *((char *)&v32);
    v35 = _INSERT(v32, 0, 0xff);
    *((unsigned int *)v35) = *((int *)v35) + 1;
    *((char *)v35) = *((char *)v35) - 1;
    v36 = *((char *)v35);
    *((char *)v35) = *((char *)v35) - 1;
    v37 = _ccall(14, 1, v36, 0xff, 0);
    if (!(v37 & 1))
        goto LABEL_46c216;
LABEL_46c216:
    *((char *)v35) = *((char *)v35) + *((char *)&v35);
    v38 = _INSERT(v35, 0, 0xff);
    *((unsigned int *)v38) = *((int *)v38) + 1;
    *((char *)v38) = *((char *)v38) - 1;
    v39 = *((char *)v38);
    *((char *)v38) = *((char *)v38) - 1;
    v40 = _ccall(14, 1, v39, 0xff, 0);
    if (!(v40 & 1))
        goto LABEL_46c226;
LABEL_46c226:
    *((char *)v38) = *((char *)v38) + *((char *)&v38);
    v41 = _INSERT(v38, 0, 0xff);
    *((unsigned int *)v41) = *((int *)v41) + 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((char *)v41) = *((char *)v41);
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + *((char *)&a0);
    *((unsigned int *)v41) = *((int *)v41) + 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((unsigned int *)v41) = *((int *)v41) - 0xf40000;
    *((unsigned int *)v41) = *((int *)v41) + 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((char *)v41) = *((char *)v41);
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + *((char *)&a0);
    *((unsigned int *)v41) = *((int *)v41) + 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((int *)v41) = *((int *)v41);
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + *((char *)&a0);
    *((unsigned int *)v41) = *((int *)v41) + 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((char *)v41) = *((char *)v41) - 1;
    *((char *)v41) = *((char *)v41) - 1;
    v43 = _INSERT(v41, 0, 0xff);
    *((unsigned int *)v43) = *((int *)v43) + 1;
    *((char *)v43) = *((char *)v43) - 1;
    *((char *)v43) = *((char *)v43) - 1;
    *((char *)v43) = *((char *)v43) - 1;
    v44 = _INSERT(v43, 0, 0xff);
    *((unsigned int *)v44) = *((int *)v44) + 1;
    *((char *)v44) = *((char *)v44) - 1;
    *((char *)v44) = *((char *)v44) - 1;
    InterlockedExchange8(v44, 0xff);
    v45 = _INSERT(v44, 0, *((char *)v44));
    *(v45) = *(v45) + *((char *)v44);
    v46 = _INSERT(v45, 0, 0xff);
    *(v46) = *(v46) + 1;
    *((char *)v46) = *((char *)v46) - 1;
    *((char *)v46) = *((char *)v46) - 1;
    InterlockedExchange(v46, v46);
    v47 = &*(v46)->field_0;
    *(v47) = *(v47) + *((char *)&v47);
    v48 = _INSERT(v47, 0, 0xff);
    *((unsigned int *)v48) = *((int *)v48) + 1;
    *((char *)v48) = *((char *)v48) - 1;
    *((char *)v48) = *((char *)v48) - 1;
    *((char *)v48) = 0xff;
    *((char *)v48) = *((char *)v48) - 1;
    v49 = _INSERT(v48, 0, 0xff);
    *(v49) = (char *)&*(v49)->field_0 + 1;
    *((char *)v49) = *((char *)v49) - 1;
    *((char *)v49) = *((char *)v49) - 1;
    *(v49) = v49;
    *((char *)v49) = *((char *)v49) - 1;
    v50 = _INSERT(v49, 0, 0xff);
    *((unsigned int *)v50) = *((int *)v50) + 1;
    *((char *)v50) = *((char *)v50) - 1;
    *((char *)v50) = *((char *)v50) - 1;
    v51 = _INSERT(v50, 0, *((char *)v50));
    *(v51) = *(v51) + *((char *)v50);
    v52 = _INSERT(v51, 0, 0xff);
    *(v52) = *(v52) + 1;
    *((char *)v52) = *((char *)v52) - 1;
    *((char *)v52) = *((char *)v52) - 1;
    v53 = &*(v52)->field_0;
    *(v53) = *(v53) + *((char *)&v53);
    v54 = _INSERT(v53, 0, 0xff);
    *((unsigned int *)v54) = *((int *)v54) + 1;
    *((char *)v54) = *((char *)v54) - 1;
    *((char *)v54) = *((char *)v54) - 1;
    *((unsigned short *)v54) = v55;
    *((char *)v54) = *((char *)v54) - 1;
    v56 = _INSERT(v54, 0, 0xff);
    *((unsigned int *)v56) = *((int *)v56) + 1;
    *((char *)v56) = *((char *)v56) - 1;
    *((char *)v56) = *((char *)v56) - 1;
    v57 = v56;
    *((char *)v57) = *((char *)v57) + *((char *)&v57);
    v58 = _INSERT(v57, 0, 0xff);
    *((unsigned int *)v58) = *((int *)v58) + 1;
    *((char *)v58) = *((char *)v58) - 1;
    *((char *)v58) = *((char *)v58) - 1;
    *((char *)v58) = *((char *)v58) - 1;
    v59 = _INSERT(v58, 0, 0xff);
    *((unsigned int *)v59) = *((int *)v59) + 1;
    *((char *)v59) = *((char *)v59) - 1;
    *((char *)v59) = *((char *)v59) - 1;
    *((unsigned int *)v59) = v0;
    *((char *)v59) = *((char *)v59) - 1;
    v60 = _INSERT(v59, 0, 0xff);
    *((unsigned int *)v60) = *((int *)v60) + 1;
    *((char *)v60) = *((char *)v60) - 1;
    *((char *)v60) = *((char *)v60) - 1;
    *((char *)v60) = *((char *)v60) - 1;
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + *((char *)&a0);
    *(v46) = *(v46) + 1;
    *((char *)v46) = *((char *)v46) + *((char *)&v46);
    *((char *)v46) = *((char *)v46) + *((char *)&v46);
    *((char *)a0) = *((char *)a0) + *((char *)&a0);
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + 0xff;
    *((unsigned int *)a0) = *((int *)a0) + 1;
    *((char *)a0) = *((char *)a0) + *((char *)&a0);
    *((char *)a0) = *((char *)a0) + *((char *)&a0);
    *((char *)a1) = *((char *)a1) + *((char *)&a1);
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + 0xff;
    *((unsigned int *)a1) = *((int *)a1) + 1;
    *((char *)a1) = *((char *)a1) + *((char *)&a1);
    *((char *)a1) = *((char *)a1) + *((char *)&a1);
    *((char *)v61) = *((char *)v61) + *((char *)&v61);
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + 0xff;
    *((unsigned int *)v61) = *((int *)v61) + 1;
    *((char *)v61) = *((char *)v61) + *((char *)&v61);
    *((char *)v61) = *((char *)v61) + *((char *)&v61);
    iter = _INSERT(CONCAT(iter, 0), 0, iter + *((char *)&&iter));
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + 0xff;
    iter += 1;
    iter = (char)iter + *((char *)&&iter);
    iter += *((char *)&&iter);
    *((char *)v62) = *((char *)v62) + *((char *)&v62);
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + 0xff;
    *((unsigned int *)v62) = *((int *)v62) + 1;
    *((char *)v62) = *((char *)v62) + *((char *)&v62);
    *((char *)v62) = *((char *)v62) + *((char *)&v62);
    *((char *)v63) = *((char *)v63) + *((char *)&v63);
    *((char *)(v42 * 9)) = *((char *)(v42 * 9)) + 0xff;
    *((unsigned int *)v63) = *((int *)v63) + 1;
    *((char *)v63) = *((char *)v63) + *((char *)&v63);
    *((char *)v63) = *((char *)v63) + *((char *)&v63);
    *((char *)v42) = *((char *)v42) + *((char *)&v42);
    *((char *)(v63 * 9)) = *((char *)(v63 * 9)) + 0xff;
    *((unsigned int *)v42) = *((int *)v42) + 1;
    *((char *)v42) = *((char *)v42) + *((char *)&v42);
    *((char *)v42) = *((char *)v42) + *((char *)&v42);
    v64 = *((unsigned short *)&v42);
    *((char *)v64) = *((char *)v64) + *((char *)&v64);
    *((char *)(v63 * 9)) = *((char *)(v63 * 9)) + 0xff;
    *((unsigned int *)v64) = *((int *)v64) + 1;
    *((char *)v64) = *((char *)v64) + *((char *)&v64);
    *((char *)v64) = *((char *)v64) + *((char *)&v64);
    *((char *)v64) = *((char *)v64) + *((char *)&v64);
    *((char *)(v63 * 9)) = *((char *)(v63 * 9)) + 0xff;
    *((unsigned int *)v64) = *((int *)v64) + 1;
    *((char *)v64) = *((char *)v64) + *((char *)&v64);
    *((char *)v64) = *((char *)v64) + *((char *)&v64);
}
