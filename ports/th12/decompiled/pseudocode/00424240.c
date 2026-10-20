// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x424240; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_424240_1 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[16];
    unsigned int field_18;
    char padding_1c[68];
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
    unsigned int field_6c;
    unsigned int field_70;
    char padding_74[8];
    unsigned int field_7c;
    unsigned int field_80;
    char field_84;
    char field_85;
    char field_86;
    char field_87;
} st_424240_1;

typedef struct st_424240_2 {
    unsigned int field_0;
    char padding_4[8];
    struct st_424240_2 *field_c;
    unsigned int field_10;
    unsigned int field_14;
} st_424240_2;

unsigned int sub_424240(unsigned int a0, unsigned int a1)
{
    int v17;  // esi
    int v18;  // esi
    char i;  // 4106
    char v117;  // 4101
    unsigned int v118;  // cc_dep1
    unsigned int v119;  // cc_dep2
    char v120;  // 4103
    unsigned int v121;  // eax
    unsigned int v122;  // 4111
    unsigned int v123;  // 4120
    char *v124;  // ecx
    char *node;  // eax
    char v126;  // 4101
    char *v28;  // eax
    unsigned int v127;  // cc_dep1
    unsigned int v128;  // cc_dep2
    char v129;  // 4103
    unsigned int v130;  // eax
    unsigned int v131;  // 4111
    unsigned int v132;  // 4120
    char *v133;  // ecx
    char *iter;  // eax
    char v135;  // 4101
    unsigned int v136;  // cc_dep1
    char *iter1;  // ebx
    unsigned int v137;  // cc_dep2
    char v138;  // 4103
    unsigned int v139;  // eax
    unsigned int v140;  // 4111
    unsigned int v141;  // 4120
    char *v142;  // ecx
    char *iter2;  // eax
    char v144;  // 4101
    unsigned int v145;  // cc_dep1
    unsigned int v146;  // cc_dep2
    char *v30;  // ecx
    char v147;  // 4103
    unsigned int v148;  // eax
    unsigned int v149;  // 4111
    unsigned int v150;  // 4120
    char *v151;  // ecx
    char *v152;  // eax
    char v153;  // 4101
    unsigned int v154;  // cc_dep1
    unsigned int v155;  // cc_dep2
    char v156;  // 4103
    char *v31;  // ecx
    unsigned int v157;  // eax
    unsigned int v158;  // 4111
    unsigned int v159;  // 4120
    char *v160;  // ecx
    char *v161;  // eax
    char v162;  // 4101
    unsigned int v163;  // cc_dep1
    unsigned int v164;  // cc_dep2
    char v165;  // 4103
    unsigned int v166;  // eax
    char j;  // 4102
    unsigned int v167;  // 4111
    unsigned int v168;  // 4120
    unsigned int v169;  // ftop
    unsigned int v170;  // ftop
    unsigned int v171;  // ftop
    unsigned int v172;  // ftop
    unsigned int v173;  // ftop
    unsigned int v175;  // 4133
    unsigned int v176;  // ftop
    char *v33;  // eax
    unsigned int v178;  // ftop
    char *v179;  // ecx
    char *v180;  // eax
    char v181;  // 4101
    unsigned int v182;  // cc_dep1
    unsigned int v183;  // cc_dep2
    char v184;  // 4103
    unsigned int v185;  // eax
    unsigned int v186;  // 4111
    char k;  // 4102
    unsigned int v187;  // 4120
    char *v188;  // esi
    char *v189;  // esi
    char *v190;  // esi
    char *v191;  // ecx
    char *v192;  // eax
    char v193;  // 4101
    unsigned int v194;  // cc_dep1
    unsigned int v195;  // cc_dep2
    char v196;  // 4103
    char *v35;  // eax
    unsigned int v197;  // eax
    unsigned int v198;  // 4111
    unsigned int v199;  // 4120
    char *v200;  // ecx
    char v201;  // 4101
    unsigned int v202;  // cc_dep1
    unsigned int v203;  // cc_dep2
    char v204;  // 4103
    unsigned int v205;  // eax
    unsigned int v206;  // 4111
    char *v36;  // ecx
    unsigned int v207;  // 4120
    st_424240_1 *v208;  // eax
    st_424240_1 *v209;  // eax
    st_424240_1 *v210;  // eax
    st_424240_1 *v211;  // eax
    st_424240_2 *v213;  // eax
    st_424240_2 *index;  // edx
    unsigned int v215;  // ecx
    st_424240_2 *v216;  // edi
    int v19;  // ebp
    char *v37;  // ecx
    char l;  // 4102
    char *v39;  // ecx
    char *v40;  // eax
    char v41;  // 4101
    unsigned int v42;  // cc_dep1
    unsigned int v43;  // cc_dep2
    char v44;  // 4103
    unsigned int v45;  // eax
    unsigned int v46;  // 4111
    int m;  // ebx
    unsigned int v47;  // 4120
    char *v48;  // ecx
    char v49;  // 4101
    unsigned int v50;  // cc_dep1
    unsigned int v51;  // cc_dep2
    char v52;  // 4103
    unsigned int v53;  // eax
    unsigned int v54;  // 4111
    unsigned int v55;  // 4120
    char *v56;  // ecx
    int v21;  // ebx
    char *v57;  // eax
    char v58;  // 4101
    unsigned int v59;  // cc_dep1
    unsigned int v60;  // cc_dep2
    char v61;  // 4103
    unsigned int v62;  // eax
    unsigned int v63;  // 4111
    unsigned int v64;  // 4120
    char *v65;  // ecx
    char *v66;  // eax
    char *v22;  // ebp
    char v67;  // 4101
    unsigned int v68;  // cc_dep1
    unsigned int v69;  // cc_dep2
    char v70;  // 4103
    unsigned int v71;  // eax
    unsigned int v72;  // 4111
    unsigned int v73;  // 4120
    char *v74;  // ecx
    char v75;  // 4101
    unsigned int v76;  // cc_dep1
    unsigned int v23;  // edi
    unsigned int v77;  // cc_dep2
    char v78;  // 4103
    unsigned int v79;  // eax
    unsigned int v80;  // 4111
    unsigned int v81;  // 4120
    st_424240_1 *n;  // ebp
    unsigned int v84;  // ftop
    unsigned int v85;  // ftop
    int v86;  // ebx
    int v24;  // esi
    char *v87;  // edi
    char *v88;  // ecx
    char *v89;  // eax
    char v90;  // 4101
    unsigned int v91;  // cc_dep1
    unsigned int v92;  // cc_dep2
    char v93;  // 4103
    unsigned int v94;  // eax
    unsigned int v95;  // 4111
    unsigned int v96;  // 4120
    char *v25;  // edi
    char *v97;  // esi
    unsigned int v98;  // ftop
    unsigned int v99;  // ftop
    unsigned int v100;  // ftop
    unsigned int v101;  // ftop
    unsigned int v102;  // ftop
    char *v103;  // ecx
    char *v104;  // eax
    char v105;  // 4101
    unsigned int v106;  // cc_dep1
    char *v26;  // eax
    unsigned int v107;  // cc_dep2
    char v108;  // 4103
    unsigned int v109;  // eax
    unsigned int v110;  // 4111
    unsigned int v111;  // 4120
    int v112;  // esi
    int v113;  // esi
    char *v114;  // eax
    char *v115;  // ecx
    char *v116;  // eax
    char *v0;  // [bp-0x58]
    unsigned int v1;  // [bp-0x40]
    int v2;  // [bp-0x3c]
    char *v3;  // [bp-0x34]
    int v4;  // [bp-0x30]
    unsigned int v5;  // [bp-0x2c]
    int v6;  // [bp-0x28]
    char v7;  // [bp-0x24]
    int v8;  // [bp-0x20]
    char *v9;  // [bp-0x1c], Other Possible Types: unsigned int
    int v10;  // [bp-0x18]
    char *v11;  // [bp-0x14], Other Possible Types: int
    int v12;  // [bp-0x10]
    char *v13;  // [bp-0xc]
    int v14;  // [bp-0x8]
    unsigned int v15;  // [bp-0x4]
    unsigned int v16;  // [bp+0x0]

    v4 = 1;
    v3 = &v7;
    v9 = 0;
    v18 = sub_463c10(&v7, 1, v17, -0x1);
    v10 = v18;
    if (!v18)
        return 0xffffffff;
    v21 = sub_46d04a(0x1000, v19, m);
    v6 = v21;
    v22 = sub_46d04a(0x1000);
    v16 = sub_46d04a(0x1000);
    if (v17 > 0)
    {
        v1 = v23;
        v24 = v21 - v22;
        v25 = &v22[-1 * v21];
        v12 = v24;
        v11 = v25;
        while (1)
        {
            v5 = sub_424a20(v4);
            v26 = v22;
            do
            {
                i = v26[v24];
                *(v26) = v26[v24];
                v26 += 1;
            } while (i);
            v28 = sub_46d1a0(v22, 58);
            if (!v28)
            {
                sub_424b50();
                iter1 = v13;
            }
            else
            {
                iter1 = v13;
                v30 = v28 + 1;
                v31 = v30;
                do
                {
                    j = *(v31);
                    *(iter1 - v30 + v31) = j;
                    v31 += 1;
                } while (j);
                *(v28) = j;
                sub_424b50();
                sub_424b50();
            }
            v33 = v3;
            do
            {
                k = *(v33);
                *((char *)(v25 + v33)) = *(v33);
                v33 += 1;
            } while (k);
            v35 = sub_46d1a0(v22, 58);
            if (v35)
            {
                v36 = v35 + 1;
                v37 = v36;
                do
                {
                    l = *(v37);
                    *(iter1 - v36 + v37) = l;
                    v37 += 1;
                } while (l);
                *(v35) = l;
                sub_424b50();
            }
            sub_424b50();
            v39 = "Version";
            v40 = v22;
            while (1)
            {
                v41 = *(v40);
                v42 = v41;
                v43 = *(v39);
                if (v41 != *(v39))
                    break;
                if (v41)
                {
                    v44 = v40[1];
                    v42 = v44;
                    v43 = v39[1];
                    if (v44 != v39[1])
                        break;
                    v40 += 2;
                    v39 += 2;
                    if (!v44)
                        goto LABEL_4243a1;
                }
                else
                {
LABEL_4243a1:
                    v45 = 0;
                    goto LABEL_4243b1;
                }
            }
            v46 = _ccall(4, v42, v43, 0);
            v47 = _ccall(12, 0, v46 & 1, v46 & 1);
            v45 = -(v46 & 1) + 1 - (v47 & 1);
LABEL_4243b1:
            if (!v45)
            {
                v48 = "0.0";
                while (1)
                {
                    v49 = *(iter1);
                    v50 = v49;
                    v51 = *(v48);
                    if (v49 != *(v48))
                        break;
                    if (v49)
                    {
                        v52 = iter1[1];
                        v50 = v52;
                        v51 = v48[1];
                        if (v52 != v48[1])
                            break;
                        iter1 += 2;
                        v48 += 2;
                        if (!v52)
                            goto LABEL_4243dc;
                    }
                    else
                    {
LABEL_4243dc:
                        v53 = 0;
                        goto LABEL_4243e4;
                    }
                }
                v54 = _ccall(4, v50, v51, 0);
                v55 = _ccall(12, 0, v54 & 1, v54 & 1);
                v53 = -(v54 & 1) + 1 - (v55 & 1);
LABEL_4243e4:
                if (v53)
                {
                    sub_4236f0();
                    break;
                }
            }
            else
            {
                v56 = "Stage";
                v57 = v22;
                while (1)
                {
                    v58 = *(v57);
                    v59 = v58;
                    v60 = *(v56);
                    if (v58 != *(v56))
                        break;
                    if (v58)
                    {
                        v61 = v57[1];
                        v59 = v61;
                        v60 = v56[1];
                        if (v61 != v56[1])
                            break;
                        v57 += 2;
                        v56 += 2;
                        if (!v61)
                            goto LABEL_42444c;
                    }
                    else
                    {
LABEL_42444c:
                        v62 = 0;
                        goto LABEL_424454;
                    }
                }
                v63 = _ccall(4, v59, v60, 0);
                v64 = _ccall(12, 0, v63 & 1, v63 & 1);
                v62 = -(v63 & 1) + 1 - (v64 & 1);
LABEL_424454:
                if (!v62)
                {
                    v2 = sub_46d143(iter1);
                    v4 = 0;
                    if (v2 <= 0 || v2 >= 8)
                        v2 = -0x1;
                }
                else
                {
                    v65 = "StageEnd";
                    v66 = v22;
                    while (1)
                    {
                        v67 = *(v66);
                        v68 = v67;
                        v69 = *(v65);
                        if (v67 != *(v65))
                            break;
                        if (v67)
                        {
                            v70 = v66[1];
                            v68 = v70;
                            v69 = v65[1];
                            if (v70 != v65[1])
                                break;
                            v66 += 2;
                            v65 += 2;
                            if (!v70)
                                goto LABEL_4244ac;
                        }
                        else
                        {
LABEL_4244ac:
                            v71 = 0;
                            goto LABEL_4244b4;
                        }
                    }
                    v72 = _ccall(4, v68, v69, 0);
                    v73 = _ccall(12, 0, v72 & 1, v72 & 1);
                    v71 = -(v72 & 1) + 1 - (v73 & 1);
LABEL_4244b4:
                    if (!v71)
                    {
                        v2 = -0x1;
                    }
                    else
                    {
                        v74 = "Tips";
                        while (1)
                        {
                            v75 = *(v22);
                            v76 = v75;
                            v77 = *(v74);
                            if (v75 != *(v74))
                                break;
                            if (v75)
                            {
                                v78 = v22[1];
                                v76 = v78;
                                v77 = v74[1];
                                if (v78 != v74[1])
                                    break;
                                v22 += 2;
                                v74 += 2;
                                if (!v78)
                                    goto LABEL_4244e4;
                            }
                            else
                            {
LABEL_4244e4:
                                v79 = 0;
                                goto LABEL_4244ec;
                            }
                        }
                        v80 = _ccall(4, v76, v77, 0);
                        v81 = _ccall(12, 0, v80 & 1, v80 & 1);
                        v79 = -(v80 & 1) + 1 - (v81 & 1);
LABEL_4244ec:
                        if (!v79 && v2 > 0)
                        {
                            if (v4 >= 0xff)
                            {
                                v2 = -0x1;
                            }
                            else
                            {
                                n = (!sub_46c9ea(0x88) ? NULL : sub_423340());
                                v4 += 1;
                                n->field_18 = 0;
                                v85 = v84;
                                if (m > 0)
                                {
                                    do
                                    {
                                        v4 = sub_424a20(&v7);
                                        v86 = v12;
                                        v87 = sub_424a20(v4);
                                        sub_424b00(&v7);
                                        v88 = "Pos";
                                        v89 = v87;
                                        while (1)
                                        {
                                            v90 = *(v89);
                                            v91 = v90;
                                            v92 = *(v88);
                                            if (v90 != *(v88))
                                                break;
                                            if (v90)
                                            {
                                                v93 = v89[1];
                                                v91 = v93;
                                                v92 = v88[1];
                                                if (v93 != v88[1])
                                                    break;
                                                v89 += 2;
                                                v88 += 2;
                                                if (!v93)
                                                    goto LABEL_42458d;
                                            }
                                            else
                                            {
LABEL_42458d:
                                                v94 = 0;
                                                goto LABEL_424596;
                                            }
                                        }
                                        v95 = _ccall(4, v91, v92, 0);
                                        v96 = _ccall(12, 0, v95 & 1, v95 & 1);
                                        v94 = -(v95 & 1) + 1 - (v96 & 1);
LABEL_424596:
                                        if (!v94)
                                        {
                                            v97 = sub_46d1a0(v86, 44);
                                            if (v97)
                                            {
                                                *(v97) = 0;
                                                sub_424b50();
                                                sub_424b50();
                                                v9 = sub_46d143(v86);
                                                v98 = v85 - 1;
                                                if (/* unsupported instruction */)
                                                {
                                                    v99 = v98 - 1;
                                                    /* unsupported instruction */
                                                    /* unsupported instruction */
                                                }
                                                else
                                                {
                                                    v99 = v98 - 1;
                                                    /* unsupported instruction */
                                                    /* unsupported instruction */
                                                }
                                                v0 = v97 + 1;
                                                if (/* unsupported instruction */)
                                                {
                                                    n->field_0 = /* unsupported instruction */;
                                                    /* unsupported instruction */
                                                    v100 = v99 + 1;
                                                }
                                                else
                                                {
                                                    n->field_0 = nan;
                                                    /* unsupported instruction */
                                                    v100 = v99 + 1;
                                                }
                                                v9 = sub_46d143();
                                                v101 = v100 - 1;
                                                if (/* unsupported instruction */)
                                                {
                                                    v102 = v101 - 1;
                                                    /* unsupported instruction */
                                                    /* unsupported instruction */
                                                }
                                                else
                                                {
                                                    v102 = v101 - 1;
                                                    /* unsupported instruction */
                                                    /* unsupported instruction */
                                                }
                                                if (/* unsupported instruction */)
                                                {
                                                    n->field_4 = /* unsupported instruction */;
                                                    /* unsupported instruction */
                                                    v85 = v102 + 1;
                                                }
                                                else
                                                {
                                                    n->field_4 = nan;
                                                    /* unsupported instruction */
                                                    v85 = v102 + 1;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            v103 = "Text";
                                            v104 = v87;
                                            while (1)
                                            {
                                                v105 = *(v104);
                                                v106 = v105;
                                                v107 = *(v103);
                                                if (v105 != *(v103))
                                                    break;
                                                if (v105)
                                                {
                                                    v108 = v104[1];
                                                    v106 = v108;
                                                    v107 = v103[1];
                                                    if (v108 != v103[1])
                                                        break;
                                                    v104 += 2;
                                                    v103 += 2;
                                                    if (!v108)
                                                        goto LABEL_424612;
                                                }
                                                else
                                                {
LABEL_424612:
                                                    v109 = 0;
                                                    goto LABEL_42461b;
                                                }
                                            }
                                            v110 = _ccall(4, v106, v107, 0);
                                            v111 = _ccall(12, 0, v110 & 1, v110 & 1);
                                            v109 = -(v110 & 1) + 1 - (v111 & 1);
LABEL_42461b:
                                            if (!v109)
                                            {
                                                v112 = sub_46d1a0(v86, 0x22);
                                                if (!v112)
                                                    continue;
                                                v113 = v112 + 1;
                                                v114 = sub_46d260(v113, 0x22);
                                                if (!v114)
                                                    break;
                                                *(v114) = 0;
                                                sub_46d290(n->padding_1c, v113, 65);
                                            }
                                            else
                                            {
                                                v115 = "Count";
                                                v116 = v87;
                                                while (1)
                                                {
                                                    v117 = *(v116);
                                                    v118 = v117;
                                                    v119 = *(v115);
                                                    if (v117 != *(v115))
                                                        break;
                                                    if (v117)
                                                    {
                                                        v120 = v116[1];
                                                        v118 = v120;
                                                        v119 = v115[1];
                                                        if (v120 != v115[1])
                                                            break;
                                                        v116 += 2;
                                                        v115 += 2;
                                                        if (!v120)
                                                            goto LABEL_424684;
                                                    }
                                                    else
                                                    {
LABEL_424684:
                                                        v121 = 0;
                                                        goto LABEL_42468d;
                                                    }
                                                }
                                                v122 = _ccall(4, v118, v119, 0);
                                                v123 = _ccall(12, 0, v122 & 1, v122 & 1);
                                                v121 = -(v122 & 1) + 1 - (v123 & 1);
LABEL_42468d:
                                                if (!v121)
                                                {
                                                    n->field_60 = sub_46d143(v86);
                                                }
                                                else
                                                {
                                                    v124 = "Time";
                                                    node = v87;
                                                    while (1)
                                                    {
                                                        v126 = *(node);
                                                        v127 = v126;
                                                        v128 = *(v124);
                                                        if (v126 != *(v124))
                                                            break;
                                                        if (v126)
                                                        {
                                                            v129 = node[1];
                                                            v127 = v129;
                                                            v128 = v124[1];
                                                            if (v129 != v124[1])
                                                                break;
                                                            node += 2;
                                                            v124 += 2;
                                                            if (!v129)
                                                                goto LABEL_4246cc;
                                                        }
                                                        else
                                                        {
LABEL_4246cc:
                                                            v130 = 0;
                                                            goto LABEL_4246d5;
                                                        }
                                                    }
                                                    v131 = _ccall(4, v127, v128, 0);
                                                    v132 = _ccall(12, 0, v131 & 1, v131 & 1);
                                                    v130 = -(v131 & 1) + 1 - (v132 & 1);
LABEL_4246d5:
                                                    if (!v130)
                                                    {
                                                        n->field_6c = sub_46d143(v86);
                                                    }
                                                    else
                                                    {
                                                        v133 = "Base";
                                                        iter = v87;
                                                        while (1)
                                                        {
                                                            v135 = *(iter);
                                                            v136 = v135;
                                                            v137 = *(v133);
                                                            if (v135 != *(v133))
                                                                break;
                                                            if (v135)
                                                            {
                                                                v138 = iter[1];
                                                                v136 = v138;
                                                                v137 = v133[1];
                                                                if (v138 != v133[1])
                                                                    break;
                                                                iter += 2;
                                                                v133 += 2;
                                                                if (!v138)
                                                                    goto LABEL_42470d;
                                                            }
                                                            else
                                                            {
LABEL_42470d:
                                                                v139 = 0;
                                                                goto LABEL_424716;
                                                            }
                                                        }
                                                        v140 = _ccall(4, v136, v137, 0);
                                                        v141 = _ccall(12, 0, v140 & 1, v140 & 1);
                                                        v139 = -(v140 & 1) + 1 - (v141 & 1);
LABEL_424716:
                                                        if (!v139)
                                                        {
                                                            n->field_68 = sub_4241b0(61);
                                                        }
                                                        else
                                                        {
                                                            v142 = "Align";
                                                            iter2 = v87;
                                                            while (1)
                                                            {
                                                                v144 = *(iter2);
                                                                v145 = v144;
                                                                v146 = *(v142);
                                                                if (v144 != *(v142))
                                                                    break;
                                                                if (v144)
                                                                {
                                                                    v147 = iter2[1];
                                                                    v145 = v147;
                                                                    v146 = v142[1];
                                                                    if (v147 != v142[1])
                                                                        break;
                                                                    iter2 += 2;
                                                                    v142 += 2;
                                                                    if (!v147)
                                                                        goto LABEL_424751;
                                                                }
                                                                else
                                                                {
LABEL_424751:
                                                                    v148 = 0;
                                                                    goto LABEL_42475a;
                                                                }
                                                            }
                                                            v149 = _ccall(4, v145, v146, 0);
                                                            v150 = _ccall(12, 0, v149 & 1, v149 & 1);
                                                            v148 = -(v149 & 1) + 1 - (v150 & 1);
LABEL_42475a:
                                                            if (!v148)
                                                            {
                                                                n->field_64 = sub_4241b0(3);
                                                            }
                                                            else
                                                            {
                                                                v151 = "Remain";
                                                                v152 = v87;
                                                                while (1)
                                                                {
                                                                    v153 = *(v152);
                                                                    v154 = v153;
                                                                    v155 = *(v151);
                                                                    if (v153 != *(v151))
                                                                        break;
                                                                    if (v153)
                                                                    {
                                                                        v156 = v152[1];
                                                                        v154 = v156;
                                                                        v155 = v151[1];
                                                                        if (v156 != v151[1])
                                                                            break;
                                                                        v152 += 2;
                                                                        v151 += 2;
                                                                        if (!v156)
                                                                            goto LABEL_42479c;
                                                                    }
                                                                    else
                                                                    {
LABEL_42479c:
                                                                        v157 = 0;
                                                                        goto LABEL_4247a5;
                                                                    }
                                                                }
                                                                v158 = _ccall(4, v154, v155, 0);
                                                                v159 = _ccall(12, 0, v158 & 1, v158 & 1);
                                                                v157 = -(v158 & 1) + 1 - (v159 & 1);
LABEL_4247a5:
                                                                if (!v157)
                                                                {
                                                                    n->field_70 = sub_46d143(v86);
                                                                }
                                                                else
                                                                {
                                                                    v160 = "Scale";
                                                                    v161 = v87;
                                                                    while (1)
                                                                    {
                                                                        v162 = *(v161);
                                                                        v163 = v162;
                                                                        v164 = *(v160);
                                                                        if (v162 != *(v160))
                                                                            break;
                                                                        if (v162)
                                                                        {
                                                                            v165 = v161[1];
                                                                            v163 = v165;
                                                                            v164 = v160[1];
                                                                            if (v165 != v160[1])
                                                                                break;
                                                                            v161 += 2;
                                                                            v160 += 2;
                                                                            if (!v165)
                                                                                goto LABEL_4247dd;
                                                                        }
                                                                        else
                                                                        {
LABEL_4247dd:
                                                                            v166 = 0;
                                                                            goto LABEL_4247e6;
                                                                        }
                                                                    }
                                                                    v167 = _ccall(4, v163, v164, 0);
                                                                    v168 = _ccall(12, 0, v167 & 1, v167 & 1);
                                                                    v166 = -(v167 & 1) + 1 - (v168 & 1);
LABEL_4247e6:
                                                                    if (!v166)
                                                                    {
                                                                        sub_46d4f7(v86);
                                                                        if (/* unsupported instruction */)
                                                                        {
                                                                            v9 = /* unsupported instruction */;
                                                                            /* unsupported instruction */
                                                                            v169 = v85 + 1;
                                                                        }
                                                                        else
                                                                        {
                                                                            v9 = (unsigned int)nan;
                                                                            /* unsupported instruction */
                                                                            v169 = v85 + 1;
                                                                        }
                                                                        v170 = v169 - 1;
                                                                        if (/* unsupported instruction */)
                                                                        {
                                                                            v171 = v170 - 1;
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                        }
                                                                        else
                                                                        {
                                                                            v171 = v170 - 1;
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                        }
                                                                        if (/* unsupported instruction */)
                                                                            n->field_7c = /* unsupported instruction */;
                                                                        else
                                                                            n->field_7c = nan;
                                                                        v172 = v171 - 1;
                                                                        if (/* unsupported instruction */)
                                                                        {
                                                                            v173 = v172 - 1;
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                        }
                                                                        else
                                                                        {
                                                                            v173 = v172 - 1;
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                        }
                                                                        v175 = _ccall(10, 13, (char)((((unsigned short)v173 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                                                        if (!(v175 & 1))
                                                                        {
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                            n->field_7c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                                                            /* unsupported instruction */
                                                                            v85 = v173 + 2;
                                                                        }
                                                                        else
                                                                        {
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                            v176 = v173 - 0;
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                            /* unsupported instruction */
                                                                            v178 = v176 + 1;
                                                                            if (!((char)((((unsigned short)v176 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                                                                            {
                                                                                if (/* unsupported instruction */)
                                                                                {
                                                                                    n->field_7c = /* unsupported instruction */;
                                                                                    /* unsupported instruction */
                                                                                    v85 = v178 + 1;
                                                                                }
                                                                                else
                                                                                {
                                                                                    n->field_7c = nan;
                                                                                    /* unsupported instruction */
                                                                                    v85 = v178 + 1;
                                                                                }
                                                                            }
                                                                            else
                                                                            {
                                                                                /* unsupported instruction */
                                                                                /* unsupported instruction */
                                                                                /* unsupported instruction */
                                                                                v85 = v178 + 1;
                                                                            }
                                                                        }
                                                                    }
                                                                    else
                                                                    {
                                                                        v179 = "Color";
                                                                        v180 = v87;
                                                                        while (1)
                                                                        {
                                                                            v181 = *(v180);
                                                                            v182 = v181;
                                                                            v183 = *(v179);
                                                                            if (v181 != *(v179))
                                                                                break;
                                                                            if (v181)
                                                                            {
                                                                                v184 = v180[1];
                                                                                v182 = v184;
                                                                                v183 = v179[1];
                                                                                if (v184 != v179[1])
                                                                                    break;
                                                                                v180 += 2;
                                                                                v179 += 2;
                                                                                if (!v184)
                                                                                    goto LABEL_42485c;
                                                                            }
                                                                            else
                                                                            {
LABEL_42485c:
                                                                                v185 = 0;
                                                                                goto LABEL_424865;
                                                                            }
                                                                        }
                                                                        v186 = _ccall(4, v182, v183, 0);
                                                                        v187 = _ccall(12, 0, v186 & 1, v186 & 1);
                                                                        v185 = -(v186 & 1) + 1 - (v187 & 1);
LABEL_424865:
                                                                        if (!v185)
                                                                        {
                                                                            v188 = sub_46d1a0(v86, 44);
                                                                            if (v188)
                                                                            {
                                                                                *(v188) = 0;
                                                                                v189 = v188 + 1;
                                                                                n->field_86 = sub_46d143(sub_424b50());
                                                                                v190 = sub_46d1a0(v189, 44);
                                                                                if (v190)
                                                                                {
                                                                                    *(v190) = 0;
                                                                                    sub_424b50();
                                                                                    n->field_85 = sub_46d143(v189);
                                                                                    n->field_84 = sub_46d143(sub_424b50());
                                                                                }
                                                                            }
                                                                        }
                                                                        else
                                                                        {
                                                                            v191 = "Alpha";
                                                                            v192 = v87;
                                                                            while (1)
                                                                            {
                                                                                v193 = *(v192);
                                                                                v194 = v193;
                                                                                v195 = *(v191);
                                                                                if (v193 != *(v191))
                                                                                    break;
                                                                                if (v193)
                                                                                {
                                                                                    v196 = v192[1];
                                                                                    v194 = v196;
                                                                                    v195 = v191[1];
                                                                                    if (v196 != v191[1])
                                                                                        break;
                                                                                    v192 += 2;
                                                                                    v191 += 2;
                                                                                    if (!v196)
                                                                                        goto LABEL_424903;
                                                                                }
                                                                                else
                                                                                {
LABEL_424903:
                                                                                    v197 = 0;
                                                                                    goto LABEL_42490c;
                                                                                }
                                                                            }
                                                                            v198 = _ccall(4, v194, v195, 0);
                                                                            v199 = _ccall(12, 0, v198 & 1, v198 & 1);
                                                                            v197 = -(v198 & 1) + 1 - (v199 & 1);
LABEL_42490c:
                                                                            if (!v197)
                                                                            {
                                                                                n->field_87 = sub_46d143(v86);
                                                                            }
                                                                            else
                                                                            {
                                                                                v200 = "End";
                                                                                while (1)
                                                                                {
                                                                                    v201 = *(v87);
                                                                                    v202 = v201;
                                                                                    v203 = *(v200);
                                                                                    if (v201 != *(v200))
                                                                                        break;
                                                                                    if (v201)
                                                                                    {
                                                                                        v204 = v87[1];
                                                                                        v202 = v204;
                                                                                        v203 = v200[1];
                                                                                        if (v204 != v200[1])
                                                                                            break;
                                                                                        v87 += 2;
                                                                                        v200 += 2;
                                                                                        if (!v204)
                                                                                            goto LABEL_424944;
                                                                                    }
                                                                                    else
                                                                                    {
LABEL_424944:
                                                                                        v205 = 0;
                                                                                        goto LABEL_42494d;
                                                                                    }
                                                                                }
                                                                                v206 = _ccall(4, v202, v203, 0);
                                                                                v207 = _ccall(12, 0, v206 & 1, v206 & 1);
                                                                                v205 = -(v206 & 1) + 1 - (v207 & 1);
LABEL_42494d:
                                                                                if (!v205)
                                                                                    break;
                                                                            }
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    } while (m > 0);
                                }
                                v208 = n->padding_1c;
                                v209 = v208;
                                do
                                {
                                    v210 = v209;
                                    v211 = (char *)&v210->field_0 + 1;
                                    v209 = v211;
                                } while ((char)v210->field_0);
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
                                v11 = v211 - ((char *)&v208->field_0 + 1);
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
                                if (v11 < 0)
                                {
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
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                /* unsupported instruction */
                                n->field_80 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                /* unsupported instruction */
                                sub_4109f0();
                                v4 = v4;
                                if (!v15)
                                {
                                    v213 = (!sub_46c9ea(0x88) ? NULL : sub_423340());
                                    index = &v213->field_c;
                                    v215 = 0x22;
                                    for (v216 = v213; v215; n = &n->field_4)
                                    {
                                        v215 -= 1;
                                        v216->field_0 = n->field_0;
                                        v216 = v216->padding_4;
                                    }
                                    index->field_0 = v213;
                                    *((unsigned int *)&index->padding_4[0]) = 0;
                                    *((unsigned int *)&index->padding_4[4]) = 0;
                                    sub_4109f0();
                                }
                            }
                        }
                    }
                }
            }
            v22 = (char *)v7;
            if (&v7 <= 0)
                break;
            v24 = v10;
            v25 = v9;
        }
        v18 = v8;
        v21 = v4;
    }
    sub_46c941(v21);
    sub_46c941(v22);
    sub_46c941(v14);
    sub_46c941(v18);
    return 0;
}
