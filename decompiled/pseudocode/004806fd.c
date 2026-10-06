// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4806fd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47ea48_0 {
    unsigned int field_0;
    char field_4;
} st_47ea48_0;

typedef struct st_47e7bf_0 {
    unsigned int field_0;
    char field_4;
} st_47e7bf_0;

typedef struct st_4806fd_0 {
    st_47ea48_0 field_0;
    unsigned int field_4;
} st_4806fd_0;

extern unsigned int g_4b4328;

int sub_4806fd(void)
{
    int v31;  // edi
    int v32;  // esi
    unsigned int v41;  // eax
    st_4806fd_0 *v42;  // eax
    unsigned int v44;  // edx
    unsigned int v45;  // eax
    st_47e7bf_0 *v46;  // eax
    unsigned int v47;  // edx
    st_47e7bf_0 *v49;  // eax
    int v33;  // ebx
    unsigned int v51;  // edx
    unsigned int *v53;  // eax
    st_47e7bf_0 *v54;  // eax
    unsigned int v55;  // eax
    unsigned int *v56;  // eax
    unsigned int *v57;  // eax
    unsigned int *v58;  // eax
    unsigned int *v59;  // eax
    st_4806fd_0 *v60;  // eax
    st_47e7bf_0 *v63;  // eax
    st_4806fd_0 *v65;  // eax
    void* v67;  // edi
    void* idx;  // eax
    st_4806fd_0 *v35;  // ecx
    st_4806fd_0 *v70;  // eax
    unsigned int v71;  // edx
    unsigned int v72;  // esi
    unsigned int v73;  // eax
    unsigned int *v75;  // eax
    void* v76;  // eax
    unsigned int *v77;  // eax
    void* v78;  // eax
    unsigned int v79;  // edx
    unsigned int v36;  // ebx
    void* v83;  // eax
    st_47e7bf_0 *v86;  // eax
    st_4806fd_0 *v37;  // eax
    unsigned int v88;  // eax
    unsigned int v89;  // eax
    unsigned int v90;  // edx
    unsigned int v91;  // eax
    unsigned int v92;  // eax
    unsigned int v93;  // eax
    unsigned int v94;  // eax
    unsigned int v95;  // eax
    unsigned int v38;  // ecx
    unsigned int v96;  // eax
    unsigned int v97;  // eax
    unsigned int v98;  // eax
    unsigned int v99;  // eax
    st_4806fd_0 *v100;  // eax
    unsigned int v101;  // eax
    unsigned int v102;  // eax
    unsigned int v103;  // eax
    unsigned int v39;  // eax
    unsigned int v104;  // eax
    unsigned int v105;  // eax
    st_4806fd_0 *v106;  // eax
    unsigned int v107;  // eax
    unsigned int v108;  // eax
    unsigned int v109;  // eax
    unsigned int v110;  // eax
    unsigned int v111;  // eax
    unsigned int v112;  // eax
    st_4806fd_0 *v113;  // eax
    unsigned int v40;  // eax
    unsigned int v114;  // eax
    unsigned int v115;  // eax
    st_4806fd_0 *v116;  // eax
    st_4806fd_0 *v117;  // eax
    unsigned int v0;  // [bp-0xa4]
    char *v1;  // [bp-0xa0], Other Possible Types: unsigned int
    char *v2;  // [bp-0x98]
    int v3;  // [bp-0x94]
    char *v4;  // [bp-0x80]
    unsigned int v5[2];  // [bp-0x70]
    st_47e7bf_0 v6;  // [bp-0x68]
    st_47e7bf_0 v7;  // [bp-0x60]
    unsigned int v8[2];  // [bp-0x58]
    char v9;  // [bp-0x50], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x4c]
    st_47e7bf_0 v11;  // [bp-0x48], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x44]
    char v13;  // [bp-0x40], Other Possible Types: unsigned int
    unsigned int v14;  // [bp-0x3c]
    st_47e7bf_0 v15;  // [bp-0x38], Other Possible Types: st_47ea48_0
    unsigned int v16;  // [bp-0x34]
    st_47e7bf_0 v17;  // [bp-0x30]
    unsigned int v18;  // [bp-0x2c]
    st_47ea48_0 v19;  // [bp-0x28]
    unsigned int v20;  // [bp-0x24]
    st_47e7bf_0 v21;  // [bp-0x20]
    unsigned int v22;  // [bp-0x1c]
    unsigned int v23;  // [bp-0x18]
    unsigned int v24;  // [bp-0x14]
    unsigned int v25;  // [bp-0x10]
    unsigned int v26;  // [bp-0xc]
    unsigned int v27;  // [bp-0x8]
    unsigned int v28;  // [bp-0x8]
    st_4806fd_0 *v29;  // [bp+0x4]
    st_4806fd_0 *v30;  // [bp+0x8]

    v20 &= 0xffff0000;
    v19 = (st_47ea48_0)0;
    v35 = v30;
    v36 = sub_47d64b(v31, v32, v33);
    if (!v35->field_0.field_0 || (v22 = 1, !(*((int *)&v35->field_0.field_4) & 0x200)))
        v22 = 0;
    if (v36 == 0xffff)
    {
        sub_47e1ce(2);
        goto LABEL_480d66;
    }
    if (v36 == 0xfffe)
    {
        sub_47ec26(v29, 1, v35);
        goto LABEL_480d66;
    }
    if (v36 == 0xfffd)
    {
        v37 = v29;
        v37->field_0.field_0 = v35->field_0.field_0;
        *((int *)&v37->field_0.field_4) = *((int *)&v35->field_0.field_4);
        return;
    }
    v27 = v36;
    v28 = v27 & 0x8000;
    if (v27 & 0x8000 && (v24 = v36, v24 &= 0x1800, v26 = (unsigned int)(char)(v24 == 0x800), v39 = v36, (!(!v26 ? v39 & 0x1000 : v39 & 0x400) || (v36 & 0x1b00) != 0x1000) && (v40 = v36, !(!v26 ? v40 & 0x1000 : v40 & 0x400) || !(v41 = v36 & 0x1b00, (v36 & 0x1b00) == 0x1100 || (v36 & 0x1b00) == 0x1200))))
    {
        if (v36 & 0x4000)
        {
            if ((char)~(g_4b4328 >> 1) & 1 && (char)~(g_4b4328 >> 3) & 1)
            {
                v42 = sub_47ec02(&v11, 32, sub_480665(&v9));
                v19 = (st_47ea48_0)v42->field_0.field_0;
                v20 = *((int *)&v42->field_0.field_4);
            }
            else
            {
                sub_47ddc1(sub_480665(&v9));
            }
        }
        v44 = v26;
        v45 = v36;
        if ((!v44 ? v45 & 0x1000 : v45 & 0x400) && v24 == 0x1800)
        {
            v46 = sub_47f377(&v9);
            v49 = sub_47e9ae(sub_47ec6e(v30, v47, &v13, 123), v47, &v11, v46);
            sub_47e7bf(&v19, v47, v49);
            sub_47e975(&v9);
            if (!(g_4b4328 & 0x1000))
            {
                sub_47ec02(&v13, 44, &v9, &v11, "}' ");
                sub_47e7bf(&v19, v47, sub_47ec92(&v11, "}' "));
            }
            sub_47ea48(&v19, v51, "}'");
            sub_47e8cb(&v9);
            if ((char)~(g_4b4328 >> 1) & 1 && (char)~(g_4b4328 >> 4) & 1 && !(g_4b4328 & 0x1000))
            {
                v53 = sub_47ec6e(sub_47ec02(&v15, 32, &v9, &v13, 32, &v11, &v19), v47, &v13, 32);
                v54 = sub_47e9ae(v53, v47, &v11, &v19);
                goto LABEL_480961;
            }
        }
        else
        {
            v12 &= 0xffff0000;
            v14 &= 0xffff0000;
            v24 &= 0xffff0000;
            v10 &= 0xffff0000;
            v18 &= 0xffff0000;
            v11 = 0;
            v13 = 0;
            v23 = 0;
            v9 = 0;
            v17 = (st_47e7bf_0)0;
            v55 = v36;
            if ((!v44 ? v55 & 0x1000 : v55 & 0x400))
            {
                if (v44)
                {
                    if ((v36 & 0x700) == 0x600)
                    {
                        v56 = sub_47f361(&v15);
                        v12 = v56[1];
                        v11 = *(v56);
                        v57 = sub_47f361(&v15);
                        v14 = v57[1];
                        v13 = *(v57);
                        v58 = sub_47f361(&v15);
                    }
                    else
                    {
                        if (!v44 || (v36 & 0x700) != 0x500)
                            goto LABEL_480a17;
                        v58 = sub_47f361(&v15);
                    }
                    v24 = v58[1];
                    v23 = *(v58);
                }
LABEL_480a17:
                v59 = sub_47f361(&v15);
                v9 = *(v59);
                v10 = v59[1];
            }
            if (v26 && (!v26 || (v36 & 0x700) != 0x200))
            {
                if (((char)g_4b4328 & 96) != 96)
                {
                    v60 = sub_47e0f7(&v15);
                    v17 = (st_47e7bf_0)v60->field_0.field_0;
                    v18 = *((int *)&v60->field_0.field_4);
                }
                else
                {
                    sub_47ddc1(sub_47e0f7(&v15));
                }
            }
            if ((char)~(g_4b4328 >> 1) & 1 && (char)~(g_4b4328 >> 4) & 1)
            {
                v63 = sub_47e9ae(sub_47e8cb(v8, &v15, &v19), v47, &v15, &v19);
                v19 = (st_47ea48_0)v63->field_0;
                v20 = *((int *)&v63->field_4);
            }
            else
            {
                sub_47ddc1(sub_47e8cb(v8));
            }
            v65 = v30;
            if (v65->field_0.field_0)
            {
                if (v19 && !(g_4b4328 & 0x1000))
                {
                    sub_47e7bf(&v19, v47, sub_47ec02(v8, 32, v65));
                }
                else
                {
                    v19 = (st_47ea48_0)v65->field_0.field_0;
                    v20 = *((int *)&v65->field_0.field_4);
                }
            }
            v16 &= 0xffff0000;
            v67 = NULL;
            v15 = (st_47ea48_0)0;
            if (v22)
            {
                sub_47e7bf(&v19, v47, sub_47ec4a(&v21, " ", sub_47e46b(v8, 0)));
                if (g_4b4328 & 0x1000)
                {
                    v37 = v29;
                    v37->field_0 = v19;
                    *((unsigned int *)&v37->field_0.field_4) = v20;
                    return;
                }
            }
            else
            {
                v67 = NULL;
                idx = sub_47dc29(8, 0);
                if (idx)
                {
                    *((struct st_47ea48_0 *)idx) = 0;
                    *((char *)&idx[4]) = 0;
                    *((unsigned int *)&idx[4]) = (int)idx[4] & 0xffff00ff;
                    v67 = idx;
                }
                v70 = sub_47e46b(v8, v67);
                v15 = (st_47ea48_0)v70->field_0.field_0;
                v16 = *((int *)&v70->field_0.field_4);
            }
            v72 = v26;
            v73 = v36;
            if ((!v72 ? v73 & 0x1000 : v73 & 0x400))
            {
                if (v72)
                {
                    if ((v36 & 0x700) == 0x600)
                    {
                        v3 = 44;
                        v2 = v8;
                        v75 = sub_47ec6e(sub_47ec4a(v5, "`vtordispex{", &v11, &v6, 44, &v7, &v13, &v25, 44, &v21, &v23, v8, 44), v47, &v6, 44);
                        v76 = sub_47e9ae(v75, v47, &v7, &v13);
                        v77 = sub_47ec6e(v76, v47, &v25, 44);
                        v78 = sub_47e9ae(v77, v47, &v21, &v23);
                    }
                    else
                    {
                        if (!v72 || (v36 & 0x700) != 0x500)
                            goto LABEL_480c58;
                        v3 = 44;
                        v2 = v5;
                        v78 = sub_47ec4a(&v6, "`vtordisp{", &v23, v5, 44);
                    }
                    sub_47e7bf(&v19, v47, sub_47ec6e(v78, v79, v2, v3));
                }
                else
                {
LABEL_480c58:
                    sub_47ea48(&v19, v71, "`adjustor{");
                }
                sub_47e7bf(&v19, v47, sub_47ec92(v5, "}' "));
            }
            v83 = sub_47ec6e(sub_47ec02(&v7, 40, sub_47eedc(&v6, v5, 41)), v47, v5, 41);
            sub_47e7bf(&v19, v47, v83);
            if (v72 && (v36 & 0x700) != 0x200)
                sub_47e7bf(&v19, v47, &v17);
            if ((char)~(g_4b4328 >> 8) & 1)
                sub_47e7bf(&v19, v47, sub_47efb8(v5));
            else
                sub_47ddc1(sub_47efb8(v5));
            if ((char)~(g_4b4328 >> 2) & 1 && v67)
            {
                v38 = v16;
                *((struct st_47ea48_0 *)v67) = v19;
                *((unsigned int *)&v67[4]) = v20;
                v19 = v15;
                v20 = v38;
                goto LABEL_480f8b;
            }
        }
        v38 = v20;
LABEL_480f8b:
LABEL_480f90:
        v98 = v36;
        if (!(0 < (!v28 ? v98 & 0x6000 : (v98 & 0x1800) - 0x800)) != 0xffffffff)
        {
            if ((char)~(g_4b4328 >> 9) & 1)
            {
                v99 = v36;
                if (!(0 < (!v28 ? v99 & 0x6000 : (v99 & 0x1800) - 0x800)) != 0xffffffff && (!v28 ? 1 : -(0 < (v36 & 0x700) - 0x200) + 1))
                {
                    v100 = sub_47ec4a(v5, "static ", &v19);
                    v19 = (st_47ea48_0)v100->field_0.field_0;
                    v38 = *((int *)&v100->field_0.field_4);
                    v20 = v38;
                }
                if (v28)
                {
                    if ((v36 & 0x700) == 0x100)
                        goto LABEL_4810e5;
                    if (!v28)
                        goto LABEL_481047;
                    v101 = (v36 & 0x1800) - 0x800;
                }
                else
                {
LABEL_481047:
                    v101 = v36 & 0x6000;
                }
                v102 = v36;
                if (!(-(0 < v101) == 0xffffffff ? v102 & 0x1000 : v102 & 0x400) || (v103 = v36, (~((unsigned int)(char)(0 < (!v28 ? v103 & 0x6000 : (v103 & 0x1800) - 0x800))) == 0xffffffff || (v36 & 0x700) != 0x500) && !(v104 = v36, ~((unsigned int)(char)(0 < (!v28 ? v104 & 0x6000 : (v104 & 0x1800) - 0x800))) != 0xffffffff && (v36 & 0x700) == 0x600 || (v105 = v36, ~((unsigned int)(char)(0 < (!v28 ? v105 & 0x6000 : (v105 & 0x1800) - 0x800))) != 0xffffffff && (v36 & 0x700) == 0x400))))
                    goto LABEL_481105;
LABEL_4810e5:
                v106 = sub_47ec4a(v5, "virtual ", &v19);
                v19 = (st_47ea48_0)v106->field_0.field_0;
                v38 = *((int *)&v106->field_0.field_4);
                v20 = v38;
            }
LABEL_481105:
            if ((char)~(g_4b4328 >> 7) & 1)
            {
                v107 = v36;
                if (~(0 < (!v28 ? v107 & 0x6000 : (v107 & 0x1800) - 0x800)) != 0xffffffff && (v108 = v36, (!v28 ? -((unsigned int)(char)(0 < (v108 & 0x1800) - 0x800)) + 1 : (unsigned int)(char)(((char)v108 & 192) == 64))))
                {
                    v1 = &v19;
                    v0 = "private: ";
                }
                else
                {
                    v109 = v36;
                    if (~(0 < (!v28 ? v109 & 0x6000 : (v109 & 0x1800) - 0x800)) != 0xffffffff && (v110 = v36, (!v28 ? -((unsigned int)(char)(0 < (v110 & 0x1800) - 0x1000)) + 1 : (unsigned int)(char)(((char)v110 & 192) == 128))))
                    {
                        v1 = &v19;
                        v0 = "protected: ";
                    }
                    else
                    {
                        v111 = v36;
                        if (-(0 < (!v28 ? v111 & 0x6000 : (v111 & 0x1800) - 0x800)) == 0xffffffff)
                            goto LABEL_481209;
                        if (v28)
                        {
                            v1 = 0;
                            v112 = !((char)v36 & 192);
                        }
                        else
                        {
                            v112 = -(0 < (v36 & 0x1800)) + 1;
                        }
                        if (!v112)
                            goto LABEL_481209;
                        v1 = &v19;
                        v0 = "public: ";
                    }
                }
                v113 = sub_47ec4a(v5);
                v19 = (st_47ea48_0)v113->field_0.field_0;
                v38 = *((int *)&v113->field_0.field_4);
                v20 = v38;
            }
        }
LABEL_481209:
        v114 = v36;
        v115 = v36;
        if ((-(0 < (!v28 ? v114 & 0x6000 : (v114 & 0x1800) - 0x800)) == 0xffffffff ? v115 & 0x1000 : v115 & 0x400) && !(g_4b4328 & 0x1000))
        {
            v116 = sub_47ec4a(v5, "[thunk]:", &v19);
            v19 = (st_47ea48_0)v116->field_0.field_0;
            v38 = *((int *)&v116->field_0.field_4);
            v20 = v38;
        }
        if (v36 & 0x10000)
        {
            v117 = sub_47ec4a(v5, "extern \"C\" ", &v19);
            v19 = (st_47ea48_0)v117->field_0.field_0;
            v38 = *((int *)&v117->field_0.field_4);
        }
        v37 = v29;
        v37->field_0 = v19;
        *((unsigned int *)&v37->field_0.field_4) = v38;
        return;
    }
    sub_47e7bf(&v19, v47, v30);
    if (!v28)
    {
        if ((v36 & 0x7c00) == 0x6800)
        {
LABEL_480d5a:
            sub_47f3a3(v29, &v19);
            goto LABEL_480d66;
        }
        if ((v36 & 0x7c00) == 0x7000)
            goto LABEL_480d5a;
        if (1)
        {
            if ((v36 >> 10 & 31) == 24)
            {
                v86 = sub_47f38d(v5, v29, "}'");
                sub_47e9ae(sub_47ec6e(&v19, v47, &v7, 123), v47, &v6, v86);
                sub_47ec92();
                goto LABEL_480d66;
            }
            else if (1)
            {
                if ((v36 >> 10 & 31) == 31)
                {
                    sub_47ebae(v29, &v19);
LABEL_480d66:
                    return;
                }
                else if (1)
                {
                    v88 = v36 & 0x6000;
LABEL_480df5:
                    v89 = v36;
                    if ((-(0 < v88) == 0xffffffff ? v89 & 0x1000 : v89 & 0x400) && (v90 = -((unsigned int)(char)(0 < (v36 & 0x1b00) - 0x1000)) + 1, !(unsigned int)(char)(0 < v28) & v90))
                    {
                        v4 = "`local static destructor helper'";
                    }
                    else
                    {
                        v91 = v36;
                        v92 = v36;
                        if ((-(0 < (!v28 ? v91 & 0x6000 : (v91 & 0x1800) - 0x800)) == 0xffffffff ? v92 & 0x1000 : v92 & 0x400) && (v90 = -((unsigned int)(char)(0 < (v36 & 0x1b00) - 0x1100)) + 1, !(unsigned int)(char)(0 < v28) & v90))
                        {
                            v4 = "`template static data member constructor helper'";
                        }
                        else
                        {
                            v93 = v36;
                            v94 = v36;
                            if ((-(0 < (!v28 ? v93 & 0x6000 : (v93 & 0x1800) - 0x800)) == 0xffffffff ? v94 & 0x1000 : v94 & 0x400) && (v90 = -((unsigned int)(char)(0 < (v36 & 0x1b00) - 0x1200)) + 1, !(unsigned int)(char)(0 < v28) & v90))
                            {
                                v4 = "`template static data member destructor helper'";
                            }
                            else
                            {
                                if (v28)
                                    goto LABEL_480eff;
                                if ((v36 & 0x7c00) == 0x7800)
                                {
                                    v37 = v29;
                                    v37->field_0 = v19;
                                    *((unsigned int *)&v37->field_0.field_4) = v20;
                                    return;
                                }
LABEL_480ef9:
                                if (!v28)
                                {
                                    v95 = v36 & 0x6000;
LABEL_480f11:
                                    v96 = v36;
                                    if ((-(0 < v95) == 0xffffffff ? v96 & 0x1000 : v96 & 0x400) && !(v97 = v36 & 0x1b00, !(~((unsigned int)(char)(0 < v28)) & (unsigned int)(char)(v97 == 0x1100)) && !(~((unsigned int)(char)(0 < v28)) & (unsigned int)(char)(v97 == 0x1200))))
                                        v54 = sub_47ec4a(v5, " ", &v19);
                                    else
                                        v54 = sub_48289f(v5, &v19);
LABEL_480961:
                                    v19 = (st_47ea48_0)v54->field_0;
                                    v38 = *((int *)&v54->field_4);
                                    v20 = v38;
                                    goto LABEL_480f90;
                                }
LABEL_480eff:
                                v95 = (v36 & 0x1800) - 0x800;
                                goto LABEL_480f11;
                            }
                        }
                    }
                    sub_47ea48(&v19, v90, v4);
                    goto LABEL_480ef9;
                }
            }
        }
    }
    v88 = (v36 & 0x1800) - 0x800;
    goto LABEL_480df5;
}
