// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43dec0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43dec0_3 {
    struct st_43dec0_0 *field_0;
    char padding_4[12];
    struct st_43dec0_2 *field_10;
    char padding_14[92];
    unsigned int field_70;
    char padding_74[32];
    unsigned int field_94;
    unsigned int field_98;
    unsigned int field_9c;
    unsigned int field_a0;
    unsigned int field_a4;
    unsigned int field_a8;
    unsigned int field_ac;
    unsigned int field_b0;
    char padding_b4[800];
    char field_3d4[4];
    char field_3d7;
    char padding_3d8[52];
    unsigned int field_40c;
    char padding_410[56];
    unsigned int field_448;
    unsigned int field_44c;
    char padding_450[68];
    unsigned int field_494;
    char padding_498[76];
    unsigned int field_4e4;
    char padding_4e8[4];
    int field_4ec;
    char padding_4f0[20];
    char field_504;
    char field_505;
} st_43dec0_3;

typedef struct st_43dec0_0 {
    char padding_0[228];
    struct st_43dec0_1 *field_e4;
} st_43dec0_0;

typedef struct st_43dec0_2 {
    char padding_0[280];
    unsigned int field_118;
} st_43dec0_2;

typedef struct st_43dec0_1 {
    unsigned int field_0;
} st_43dec0_1;

extern st_43dec0_3 *g_4ce8f0;
extern char g_4ceae8;
extern unsigned int g_4cf278;
extern unsigned int g_4d47e8;
extern unsigned int g_4d4804;

unsigned int sub_43dec0(st_43dec0_3 *index)
{
    unsigned int v14;  // ebx
    unsigned int v15;  // esi
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    int v27;  // eax
    char v28;  // cl
    unsigned int v29;  // edx
    unsigned int v30;  // eax
    void* v31;  // edx
    st_43dec0_3 *idx;  // esi
    unsigned int v16;  // edi
    unsigned int v33;  // eax
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    st_43dec0_3 *v17;  // edi
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // ftop
    unsigned int v52;  // ftop
    unsigned int v53;  // ftop
    unsigned int v54;  // ftop
    unsigned int v55;  // ftop
    unsigned int v56;  // ftop
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v61;  // ftop
    unsigned int v62;  // ftop
    unsigned int v18;  // ftop
    unsigned int v63;  // ftop
    unsigned int v64;  // ftop
    unsigned int v65;  // ftop
    unsigned int v66;  // ftop
    unsigned int v67;  // ftop
    unsigned int v68;  // ftop
    unsigned int v69;  // ftop
    unsigned int v70;  // ftop
    unsigned int v71;  // ftop
    unsigned int v72;  // ftop
    unsigned int v19;  // ftop
    unsigned int v73;  // ftop
    unsigned int v74;  // ftop
    unsigned int v75;  // ftop
    unsigned int v76;  // ftop
    unsigned int v77;  // ftop
    unsigned int v78;  // ftop
    unsigned int v79;  // ftop
    unsigned int v80;  // ftop
    unsigned int v81;  // ftop
    unsigned int v82;  // ftop
    unsigned int v20;  // ftop
    unsigned int v83;  // ftop
    unsigned int v84;  // ftop
    unsigned int v85;  // ftop
    unsigned int v86;  // ftop
    unsigned int v87;  // ftop
    unsigned int v88;  // ftop
    unsigned int v89;  // ftop
    unsigned int v90;  // ftop
    unsigned int v91;  // ftop
    unsigned int v92;  // ftop
    unsigned int v21;  // ftop
    unsigned int v93;  // ftop
    unsigned int v94;  // ftop
    unsigned int v95;  // ftop
    unsigned int v96;  // ftop
    unsigned int v97;  // ftop
    unsigned int v98;  // ftop
    unsigned int v99;  // ftop
    unsigned int v100;  // ftop
    unsigned int v101;  // ftop
    unsigned int v102;  // ftop
    unsigned int v22;  // ftop
    unsigned int v103;  // ftop
    unsigned int v104;  // ftop
    unsigned int v105;  // ftop
    unsigned int v106;  // ftop
    unsigned int v0;  // [bp-0x48]
    unsigned int v1;  // [bp-0x44]
    unsigned int v2;  // [bp-0x3c]
    void* v3;  // [bp-0x38]
    st_43dec0_3 *v4;  // [bp-0x34]
    unsigned int v5;  // [bp-0x30]
    unsigned int v6;  // [bp-0x2c]
    unsigned int i;  // [bp-0x2c]
    unsigned int v8;  // [bp-0x28]
    unsigned int v9;  // [bp-0x24]
    unsigned int v10;  // [bp-0x20]
    unsigned int v11;  // [bp-0x1c]
    unsigned int v12;  // [bp-0x14]
    st_43dec0_3 *v13;  // [bp-0x10]

    v11 = v14;
    v9 = v15;
    v8 = v16;
    v17 = &index->padding_498[48];
    v13 = v17;
    if (!(g_4ceae8 & 4) && g_4cf278)
    {
        sub_45a3c0();
        v6 = 0;
        g_4cf278 = 0;
        v4 = g_4ce8f0;
        g_4ce8f0->field_0->field_e4(g_4ce8f0, 28, 0);
    }
    v12 = 13;
    do
    {
        i = v6;
        if (v17->padding_14[36])
        {
            if (*((int *)&v17->padding_14[12]) < 8)
            {
                v19 = v18 - 1;
                if (/* unsupported instruction */)
                {
                    v20 = v19 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v20 = v19 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
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
            }
            else
            {
                v21 = v18 - 1;
                if (/* unsupported instruction */)
                {
                    v20 = v21 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v20 = v21 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
            }
            if (/* unsupported instruction */)
            {
                v2 = /* unsupported instruction */;
                /* unsupported instruction */
                v22 = v20 + 1;
            }
            else
            {
                v2 = nan;
                /* unsupported instruction */
                v22 = v20 + 1;
            }
            v23 = v22 - 1;
            if (/* unsupported instruction */)
            {
                v24 = v23 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v24 = v23 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v10 = v17->padding_14[37];
            v25 = v24 - 1;
            if (/* unsupported instruction */)
            {
                v26 = v25 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v26 = v25 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&index->padding_410[52]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            index->field_448 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            *((int *)&index->field_3d4[0]) = *((int *)&v17->padding_14[4]);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = v26 + 1;
            v27 = sub_4931e0();
            if (v27 > 0x4000)
            {
                v28 = 208;
                v10 = 208;
            }
            else
            {
                if (v27 > 0x1000)
                {
                    v29 = 715827883 * ((v27 * 5 - 0x5000) * 32) >> 43;
                    v10 = v29 + (v29 >> 31) + 48;
                }
                else
                {
                    v10 = 48;
                }
                v28 = v10;
            }
            v30 = v17->padding_14[37];
            v31 = v30 + (char *)v17 - 1;
            v3 = v31;
            v5 = v30;
            if (v30 > 0)
            {
                idx = &index->padding_14[4];
                while (1)
                {
                    if (*((int *)&v17->padding_14[12]) >= 52 && *((char *)v31) != 10)
                    {
                        v33 = *((char *)v31);
                        if (*((int *)&v17->padding_14[12]) < 56)
                        {
                            *((unsigned int *)&idx->padding_3d8[27]) = index->field_10->field_118 + (v33 + 207) * 72;
                            v34 = v18 - 1;
                            if (/* unsupported instruction */)
                            {
                                v35 = v34 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v35 = v34 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v11 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v36 = v35 + 1;
                            }
                            else
                            {
                                v11 = nan;
                                /* unsupported instruction */
                                v36 = v35 + 1;
                            }
                            v37 = v36 - 1;
                            if (/* unsupported instruction */)
                            {
                                v38 = v37 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v38 = v37 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                                *((int *)&idx->padding_74[24]) = /* unsupported instruction */;
                            else
                                *((unsigned int *)&idx->padding_74[24]) = nan;
                            if (/* unsupported instruction */)
                            {
                                *((int *)&idx->padding_74[8]) = /* unsupported instruction */;
                                /* unsupported instruction */
                                v39 = v38 + 1;
                            }
                            else
                            {
                                *((unsigned int *)&idx->padding_74[8]) = nan;
                                /* unsupported instruction */
                                v39 = v38 + 1;
                            }
                            v40 = v39 - 1;
                            if (/* unsupported instruction */)
                            {
                                v41 = v40 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v41 = v40 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v11 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v42 = v41 + 1;
                            }
                            else
                            {
                                v11 = nan;
                                /* unsupported instruction */
                                v42 = v41 + 1;
                            }
                            v43 = v42 - 1;
                            if (/* unsupported instruction */)
                            {
                                v44 = v43 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v44 = v43 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                                idx->field_94 = /* unsupported instruction */;
                            else
                                idx->field_94 = nan;
                            if (/* unsupported instruction */)
                            {
                                *((int *)&idx->padding_74[16]) = /* unsupported instruction */;
                                /* unsupported instruction */
                                v45 = v44 + 1;
                            }
                            else
                            {
                                *((unsigned int *)&idx->padding_74[16]) = nan;
                                /* unsupported instruction */
                                v45 = v44 + 1;
                            }
                            v46 = v45 - 1;
                            if (/* unsupported instruction */)
                            {
                                v47 = v46 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v47 = v46 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v11 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v48 = v47 + 1;
                            }
                            else
                            {
                                v11 = nan;
                                /* unsupported instruction */
                                v48 = v47 + 1;
                            }
                            v49 = v48 - 1;
                            if (/* unsupported instruction */)
                            {
                                v50 = v49 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v50 = v49 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                                *((int *)&idx->padding_74[20]) = /* unsupported instruction */;
                            else
                                *((unsigned int *)&idx->padding_74[20]) = nan;
                            if (/* unsupported instruction */)
                            {
                                *((int *)&idx->padding_74[12]) = /* unsupported instruction */;
                                /* unsupported instruction */
                                v51 = v50 + 1;
                            }
                            else
                            {
                                *((unsigned int *)&idx->padding_74[12]) = nan;
                                /* unsupported instruction */
                                v51 = v50 + 1;
                            }
                            v52 = v51 - 1;
                            if (/* unsupported instruction */)
                            {
                                v53 = v52 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v53 = v52 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v11 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v54 = v53 + 1;
                            }
                            else
                            {
                                v11 = nan;
                                /* unsupported instruction */
                                v54 = v53 + 1;
                            }
                            v55 = v54 - 1;
                            if (/* unsupported instruction */)
                            {
                                v56 = v55 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v56 = v55 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                        }
                        else
                        {
                            *((unsigned int *)&idx->padding_3d8[27]) = index->field_10->field_118 + (v33 + 217) * 72;
                            v57 = v18 - 1;
                            if (/* unsupported instruction */)
                            {
                                v58 = v57 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v58 = v57 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v11 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v59 = v58 + 1;
                            }
                            else
                            {
                                v11 = nan;
                                /* unsupported instruction */
                                v59 = v58 + 1;
                            }
                            v60 = v59 - 1;
                            if (/* unsupported instruction */)
                            {
                                v61 = v60 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v61 = v60 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                                *((int *)&idx->padding_74[24]) = /* unsupported instruction */;
                            else
                                *((unsigned int *)&idx->padding_74[24]) = nan;
                            if (/* unsupported instruction */)
                            {
                                *((int *)&idx->padding_74[8]) = /* unsupported instruction */;
                                /* unsupported instruction */
                                v62 = v61 + 1;
                            }
                            else
                            {
                                *((unsigned int *)&idx->padding_74[8]) = nan;
                                /* unsupported instruction */
                                v62 = v61 + 1;
                            }
                            v63 = v62 - 1;
                            if (/* unsupported instruction */)
                            {
                                v64 = v63 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v64 = v63 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v11 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v65 = v64 + 1;
                            }
                            else
                            {
                                v11 = nan;
                                /* unsupported instruction */
                                v65 = v64 + 1;
                            }
                            v66 = v65 - 1;
                            if (/* unsupported instruction */)
                            {
                                v67 = v66 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v67 = v66 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                                idx->field_94 = /* unsupported instruction */;
                            else
                                idx->field_94 = nan;
                            if (/* unsupported instruction */)
                            {
                                *((int *)&idx->padding_74[16]) = /* unsupported instruction */;
                                /* unsupported instruction */
                                v68 = v67 + 1;
                            }
                            else
                            {
                                *((unsigned int *)&idx->padding_74[16]) = nan;
                                /* unsupported instruction */
                                v68 = v67 + 1;
                            }
                            v69 = v68 - 1;
                            if (/* unsupported instruction */)
                            {
                                v70 = v69 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v70 = v69 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v11 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v71 = v70 + 1;
                            }
                            else
                            {
                                v11 = nan;
                                /* unsupported instruction */
                                v71 = v70 + 1;
                            }
                            v72 = v71 - 1;
                            if (/* unsupported instruction */)
                            {
                                v73 = v72 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v73 = v72 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                                *((int *)&idx->padding_74[20]) = /* unsupported instruction */;
                            else
                                *((unsigned int *)&idx->padding_74[20]) = nan;
                            if (/* unsupported instruction */)
                            {
                                *((int *)&idx->padding_74[12]) = /* unsupported instruction */;
                                /* unsupported instruction */
                                v74 = v73 + 1;
                            }
                            else
                            {
                                *((unsigned int *)&idx->padding_74[12]) = nan;
                                /* unsupported instruction */
                                v74 = v73 + 1;
                            }
                            v75 = v74 - 1;
                            if (/* unsupported instruction */)
                            {
                                v76 = v75 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v76 = v75 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v11 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v77 = v76 + 1;
                            }
                            else
                            {
                                v11 = nan;
                                /* unsupported instruction */
                                v77 = v76 + 1;
                            }
                            v78 = v77 - 1;
                            if (/* unsupported instruction */)
                            {
                                v56 = v78 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v56 = v78 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                        }
                    }
                    else
                    {
                        *((unsigned int *)&idx->padding_3d8[27]) = index->field_10->field_118 + (*((char *)v31) + 196) * 72;
                        v79 = v18 - 1;
                        if (/* unsupported instruction */)
                        {
                            v80 = v79 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v80 = v79 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                        {
                            v11 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v81 = v80 + 1;
                        }
                        else
                        {
                            v11 = nan;
                            /* unsupported instruction */
                            v81 = v80 + 1;
                        }
                        v82 = v81 - 1;
                        if (/* unsupported instruction */)
                        {
                            v83 = v82 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v83 = v82 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                            *((int *)&idx->padding_74[24]) = /* unsupported instruction */;
                        else
                            *((unsigned int *)&idx->padding_74[24]) = nan;
                        if (/* unsupported instruction */)
                        {
                            *((int *)&idx->padding_74[8]) = /* unsupported instruction */;
                            /* unsupported instruction */
                            v84 = v83 + 1;
                        }
                        else
                        {
                            *((unsigned int *)&idx->padding_74[8]) = nan;
                            /* unsupported instruction */
                            v84 = v83 + 1;
                        }
                        v85 = v84 - 1;
                        if (/* unsupported instruction */)
                        {
                            v86 = v85 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v86 = v85 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                        {
                            v11 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v87 = v86 + 1;
                        }
                        else
                        {
                            v11 = nan;
                            /* unsupported instruction */
                            v87 = v86 + 1;
                        }
                        v88 = v87 - 1;
                        if (/* unsupported instruction */)
                        {
                            v89 = v88 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v89 = v88 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                            idx->field_94 = /* unsupported instruction */;
                        else
                            idx->field_94 = nan;
                        if (/* unsupported instruction */)
                        {
                            *((int *)&idx->padding_74[16]) = /* unsupported instruction */;
                            /* unsupported instruction */
                            v90 = v89 + 1;
                        }
                        else
                        {
                            *((unsigned int *)&idx->padding_74[16]) = nan;
                            /* unsupported instruction */
                            v90 = v89 + 1;
                        }
                        v91 = v90 - 1;
                        if (/* unsupported instruction */)
                        {
                            v92 = v91 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v92 = v91 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                        {
                            v11 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v93 = v92 + 1;
                        }
                        else
                        {
                            v11 = nan;
                            /* unsupported instruction */
                            v93 = v92 + 1;
                        }
                        v94 = v93 - 1;
                        if (/* unsupported instruction */)
                        {
                            v95 = v94 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v95 = v94 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                            *((int *)&idx->padding_74[20]) = /* unsupported instruction */;
                        else
                            *((unsigned int *)&idx->padding_74[20]) = nan;
                        if (/* unsupported instruction */)
                        {
                            *((int *)&idx->padding_74[12]) = /* unsupported instruction */;
                            /* unsupported instruction */
                            v96 = v95 + 1;
                        }
                        else
                        {
                            *((unsigned int *)&idx->padding_74[12]) = nan;
                            /* unsupported instruction */
                            v96 = v95 + 1;
                        }
                        v97 = v96 - 1;
                        if (/* unsupported instruction */)
                        {
                            v98 = v97 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v98 = v97 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        if (/* unsupported instruction */)
                        {
                            v11 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v99 = v98 + 1;
                        }
                        else
                        {
                            v11 = nan;
                            /* unsupported instruction */
                            v99 = v98 + 1;
                        }
                        v100 = v99 - 1;
                        if (/* unsupported instruction */)
                        {
                            v56 = v100 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v56 = v100 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                    }
                    if (/* unsupported instruction */)
                        idx->field_98 = /* unsupported instruction */;
                    else
                        idx->field_98 = nan;
                    v1 = &g_4d4804;
                    if (/* unsupported instruction */)
                    {
                        *((int *)&idx->padding_74[28]) = /* unsupported instruction */;
                        /* unsupported instruction */
                        v101 = v56 + 1;
                    }
                    else
                    {
                        *((unsigned int *)&idx->padding_74[28]) = nan;
                        /* unsupported instruction */
                        v101 = v56 + 1;
                    }
                    index->field_3d4[3] = v28;
                    v102 = v101 - 1;
                    if (/* unsupported instruction */)
                    {
                        v103 = v102 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v103 = v102 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    *((unsigned int *)&idx->padding_450[40]) = *((int *)&idx->padding_450[40]) | 8;
                    if (/* unsupported instruction */)
                    {
                        *((int *)&idx->padding_14[0x44]) = /* unsupported instruction */;
                        /* unsupported instruction */
                        v104 = v103 + 1;
                    }
                    else
                    {
                        *((unsigned int *)&idx->padding_14[0x44]) = nan;
                        /* unsupported instruction */
                        v104 = v103 + 1;
                    }
                    v0 = &g_4d47e8;
                    sub_45a570();
                    sub_459e50(1);
                    v105 = v104 - 1;
                    if (/* unsupported instruction */)
                    {
                        v106 = v105 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v106 = v105 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
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
                    v3 -= 1;
                    v17 = v4;
                    v30 -= 1;
                    if (/* unsupported instruction */)
                    {
                        *((int *)&index->padding_410[52]) = /* unsupported instruction */;
                        /* unsupported instruction */
                        v18 = v106 + 1;
                    }
                    else
                    {
                        *((unsigned int *)&index->padding_410[52]) = nan;
                        /* unsupported instruction */
                        v18 = v106 + 1;
                    }
                    v5 = v30;
                    if (v30 <= 0)
                        break;
                    v28 = v10;
                    v31 = v3;
                }
            }
        }
        v4 = &v17->padding_14[44];
        v6 = i - 1;
        v17 = v4;
    } while (i != 1);
    return 1;
}
