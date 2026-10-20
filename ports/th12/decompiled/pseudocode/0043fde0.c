// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43fde0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43fde0_66 {
    char padding_0[16];
    struct st_43fde0_0 *field_10;
    struct st_43fde0_66 *field_14;
} st_43fde0_66;

typedef struct st_43fde0_83 {
    char padding_0[16];
    struct st_43fde0_0 *field_10;
    unsigned int field_14;
} st_43fde0_83;

typedef struct st_43fde0_8 {
    char padding_0[4];
    struct st_43fde0_2 *field_4;
    char padding_8[8];
    struct st_43fde0_0 *field_10;
    unsigned int field_14;
} st_43fde0_8;

typedef struct st_43fde0_2 {
    char padding_0[4];
    struct st_43fde0_2 *field_4;
    char padding_8[8];
    struct st_43fde0_0 *field_10;
    struct st_43fde0_2 *field_14;
} st_43fde0_2;

typedef struct st_43fde0_1 {
    unsigned int field_0;
} st_43fde0_1;

typedef struct st_43fde0_9 {
    struct st_43fde0_8 *field_0;
    struct st_43fde0_9 *field_4;
} st_43fde0_9;

typedef struct st_43fde0_59 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    struct st_43fde0_1 *field_494;
} st_43fde0_59;

typedef struct st_43fde0_118 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[206];
    unsigned int field_494;
} st_43fde0_118;

typedef struct st_43fde0_112 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[36];
    unsigned short field_3ea;
    char padding_3ec[168];
    unsigned int field_494;
} st_43fde0_112;

typedef struct st_43fde0_0 {
    char padding_0[964];
    unsigned short field_3c4;
    char padding_3c6[36];
    unsigned short field_3ea;
    char padding_3ec[168];
    struct st_43fde0_1 *field_494;
} st_43fde0_0;

typedef struct st_43fde0_63 {
    char padding_0[125392];
    char field_1e9d0;
    char field_1e9d1;
    char field_1e9d2;
    char field_1e9d3;
    char field_1e9d4;
    char field_1e9d5;
} st_43fde0_63;

extern int g_4aebd0;
extern unsigned int g_4b0ca8;
extern void g_4b0ce0;
extern char g_4b2ed0;
extern st_43fde0_63 *g_4b451c;
extern unsigned int g_4ce8cc;
extern char g_4d48c0;
extern void g_4d48c4;

unsigned int sub_43fde0(void* idx)
{
    int v2;  // edi
    st_43fde0_2 *v3;  // esi
    unsigned int v11;  // ecx
    st_43fde0_118 *v99;  // esi
    int v100;  // esi
    st_43fde0_2 *v101;  // eax
    st_43fde0_2 *iter;  // ecx
    st_43fde0_118 *v103;  // esi
    st_43fde0_118 *v104;  // esi
    int v105;  // edi
    int v106;  // edi
    st_43fde0_2 *v107;  // ecx
    int v108;  // esi
    st_43fde0_59 *v12;  // esi
    st_43fde0_9 *node;  // eax
    st_43fde0_8 *iter1;  // eax
    unsigned int iter2;  // ecx
    st_43fde0_0 *v112;  // esi
    st_43fde0_0 *v113;  // esi
    st_43fde0_2 *v114;  // ecx
    st_43fde0_9 *v115;  // eax
    st_43fde0_8 *v116;  // eax
    unsigned int v117;  // ecx
    st_43fde0_0 *v118;  // esi
    st_43fde0_59 *v13;  // esi
    st_43fde0_0 *v119;  // esi
    st_43fde0_2 *v120;  // ecx
    unsigned int idx1;  // edx
    st_43fde0_9 *v122;  // eax
    st_43fde0_66 *v123;  // esi
    st_43fde0_9 *v124;  // eax
    st_43fde0_66 *v125;  // eax
    st_43fde0_66 *v126;  // eax
    st_43fde0_112 *v127;  // esi
    st_43fde0_2 *v128;  // ecx
    int v14;  // edi
    st_43fde0_9 *v129;  // eax
    st_43fde0_83 *v130;  // eax
    st_43fde0_9 *v131;  // eax
    unsigned int v132;  // eax
    st_43fde0_0 *v133;  // esi
    void* v134;  // esi
    int v135;  // eax
    unsigned int v136;  // 4101
    int v137;  // edi
    unsigned int v138;  // ecx
    int v15;  // edi
    st_43fde0_2 *v16;  // ecx
    int v17;  // esi
    st_43fde0_9 *v18;  // eax
    st_43fde0_8 *v19;  // eax
    unsigned int v20;  // ecx
    int v4;  // ebx
    st_43fde0_0 *v21;  // esi
    st_43fde0_0 *v22;  // esi
    st_43fde0_2 *v23;  // ecx
    st_43fde0_9 *v24;  // eax
    st_43fde0_8 *v25;  // eax
    unsigned int v26;  // ecx
    st_43fde0_0 *v27;  // esi
    st_43fde0_0 *v28;  // esi
    st_43fde0_66 *v29;  // eax
    st_43fde0_66 *v30;  // eax
    int v5;  // eax
    st_43fde0_0 *v31;  // esi
    st_43fde0_66 *v32;  // eax
    st_43fde0_66 *v33;  // eax
    st_43fde0_0 *v34;  // esi
    unsigned int v35;  // eax
    int v36;  // edi
    st_43fde0_2 *v37;  // ecx
    int v38;  // esi
    st_43fde0_9 *v39;  // eax
    st_43fde0_8 *v40;  // eax
    unsigned int v41;  // ecx
    st_43fde0_0 *v42;  // esi
    st_43fde0_0 *v43;  // esi
    st_43fde0_2 *v44;  // ecx
    int v45;  // esi
    st_43fde0_9 *v46;  // eax
    st_43fde0_8 *v47;  // eax
    unsigned int v48;  // ecx
    int v49;  // esi
    int v50;  // esi
    int v51;  // edi
    int v52;  // edi
    st_43fde0_2 *v53;  // ecx
    int v54;  // esi
    st_43fde0_9 *v55;  // eax
    st_43fde0_2 *v56;  // eax
    st_43fde0_2 *v57;  // ecx
    st_43fde0_0 *v58;  // esi
    st_43fde0_0 *v59;  // esi
    st_43fde0_2 *v60;  // ecx
    unsigned int v7;  // ecx
    st_43fde0_9 *v61;  // eax
    st_43fde0_8 *v62;  // eax
    unsigned int v63;  // ecx
    st_43fde0_0 *v64;  // esi
    st_43fde0_0 *v65;  // esi
    st_43fde0_2 *v66;  // ecx
    unsigned int index;  // edx
    st_43fde0_9 *v68;  // eax
    st_43fde0_66 *v69;  // esi
    st_43fde0_9 *v70;  // eax
    st_43fde0_66 *v71;  // eax
    st_43fde0_66 *v72;  // eax
    st_43fde0_112 *v73;  // esi
    st_43fde0_2 *v74;  // ecx
    st_43fde0_9 *v75;  // eax
    st_43fde0_83 *v76;  // eax
    st_43fde0_9 *v77;  // eax
    unsigned int v78;  // eax
    st_43fde0_0 *v79;  // esi
    void* v80;  // esi
    int v9;  // edi
    int v81;  // esi
    int v82;  // esi
    int v83;  // edi
    int v84;  // esi
    st_43fde0_2 *v85;  // eax
    st_43fde0_2 *v86;  // ecx
    st_43fde0_0 *v87;  // esi
    st_43fde0_0 *v88;  // esi
    st_43fde0_2 *v89;  // eax
    st_43fde0_2 *v90;  // ecx
    st_43fde0_118 *v91;  // esi
    st_43fde0_118 *v92;  // esi
    int v93;  // eax
    int v94;  // edi
    int v95;  // esi
    st_43fde0_2 *v96;  // eax
    st_43fde0_2 *v97;  // ecx
    st_43fde0_118 *v98;  // esi
    st_43fde0_2 *v0;  // [bp-0x50]
    st_43fde0_2 *v1;  // [bp-0x18]

    switch ((int)idx[36])
    {
    case 2:
        v80 = idx + 40;
        *((int *)&v80[4]) = (int)idx[40];
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if ((int)v80[4] != *((int *)v80))
        {
            sub_453d90();
            sub_4619e0((int)idx[712]);
            sub_461970((int)idx[712]);
            v81 = 0;
            if ((int)idx[40] > 0)
            {
                do
                {
                    sub_43f0a0(v81 + 3);
                } while ((sub_43f0a0(v81 + 11), v81 = (int)(v81 + 1), v81 < (int)idx[40]));
            }
            v82 = v81 + 1;
            if (v82 < 8)
            {
                v83 = v82 + 11;
                do
                {
                    v84 = v83 - 8;
                    v85 = sub_461920((int)idx[712], g_4ce8cc, (int)idx[712]);
                    if (!v85)
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                    v86 = &v85->field_10;
                    if (v86)
                    {
                        do
                        {
                            if (*((short *)&v86->padding_0[1002]) == v84 || v84 == -0x1 && v87 != v85)
                            {
                                v87 = (st_43fde0_0 *)v86->padding_0;
                                v88 = v87;
                                goto LABEL_440917;
                            }
                        } while ((v86 = (st_43fde0_2 *)v86->field_4, v86));
                    }
                    v88 = NULL;
LABEL_440917:
                    if (v88->field_494)
                        v88->field_494();
                    v88->field_3c4 = 31;
                    sub_455630(v88);
                    v89 = sub_461920(v138, g_4ce8cc, (int)idx[712]);
                    if (!v89)
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                    v90 = &v89->field_10;
                    if (v90)
                    {
                        do
                        {
                            if (*((short *)&v90->padding_0[1002]) == v83 || v83 == -0x1 && v91 != v89)
                            {
                                v91 = (st_43fde0_118 *)v90->padding_0;
                                v92 = v91;
                                goto LABEL_440987;
                            }
                        } while ((v90 = (st_43fde0_2 *)v90->field_4, v90));
                    }
                    v92 = NULL;
LABEL_440987:
                    if (v92->field_494)
                        v92->field_494();
                    v92->field_3c4 = 31;
                    sub_455630(v92);
                    v83 += 1;
                } while (v83 < 19);
            }
            if (!(g_4b451c->field_1e9d0 & 16) && !(g_4b451c->field_1e9d1 & 16) && !(g_4b451c->field_1e9d2 & 16) && !(g_4b451c->field_1e9d3 & 16) && !(g_4b451c->field_1e9d4 & 16) && !(g_4b451c->field_1e9d5 & 16))
            {
                sub_43f040(4);
                sub_43f040(12);
            }
        }
        if (*((int *)&g_4d48c4) & 258)
        {
            if ((int)idx[40] == 7)
                goto LABEL_440f7d;
            sub_453d90();
            v93 = (int)idx[48];
            if (!v93)
            {
                *((int *)&idx[40]) = 7;
            }
            else if (v93 <= 7)
            {
                *((unsigned int *)&idx[40]) = v93 - 1;
            }
            else
            {
                *((int *)&idx[40]) = 7;
            }
            sub_4619e0((int)idx[712]);
            sub_461970((int)idx[712]);
            v94 = 0;
            if ((int)idx[40] > 0)
            {
                do
                {
                    v95 = v94 + 3;
                    v96 = sub_461920((int)idx[712], g_4ce8cc, (int)idx[712]);
                    if (!v96)
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                    v97 = &v96->field_10;
                    if (v97)
                    {
                        do
                        {
                            if (*((short *)&v97->padding_0[1002]) == v95 || v95 == -0x1 && v98 != v96)
                            {
                                v98 = (st_43fde0_118 *)v97->padding_0;
                                v99 = v98;
                                goto LABEL_440ae7;
                            }
                        } while ((v97 = (st_43fde0_2 *)v97->field_4, v97));
                    }
                    v99 = NULL;
LABEL_440ae7:
                    if (v99->field_494)
                        v99->field_494();
                    v99->field_3c4 = 30;
                    sub_455630(v99);
                    v100 = v94 + 11;
                    v101 = sub_461920(v138, g_4ce8cc, (int)idx[712]);
                    if (!v101)
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                    iter = &v101->field_10;
                    if (iter)
                    {
                        do
                        {
                            if (*((short *)&iter->padding_0[1002]) == v100 || v100 == -0x1 && v103 != v101)
                            {
                                v103 = (st_43fde0_118 *)iter->padding_0;
                                v104 = v103;
                                goto LABEL_440b51;
                            }
                        } while ((iter = (st_43fde0_2 *)iter->field_4, iter));
                    }
                    v104 = NULL;
LABEL_440b51:
                    if (v104->field_494)
                        v104->field_494();
                    v104->field_3c4 = 30;
                    sub_455630(v104);
                    v94 += 1;
                } while (v94 < (int)idx[40]);
            }
            v105 = v94 + 1;
            if (v105 < 8)
            {
                v106 = v105 + 11;
                do
                {
                    v107 = (int)idx[712];
                    v108 = v106 - 8;
                    if (!v107)
                    {
LABEL_440b9d:
                        iter1 = NULL;
                        goto LABEL_440bf0;
                    }
                    else
                    {
                        node = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)))
                        {
                            do
                            {
                                if (node->field_0->padding_0 == v107)
                                {
                                    iter1 = node->field_0;
                                    goto LABEL_440bec;
                                }
                            } while ((node = (st_43fde0_9 *)node->field_4, node));
                        }
                        iter1 = *((int *)(g_4ce8cc + 8935104));
                        if (!*((int *)(g_4ce8cc + 8935104)))
                            goto LABEL_440b9d;
                        while (*((int *)iter1->padding_0) != v107)
                        {
                            iter1 = iter1->field_4;
                            if (!iter1)
                                goto LABEL_440bf0;
                        }
                        iter1 = (st_43fde0_8 *)iter1->padding_0;
LABEL_440bec:
                        if (iter1)
                            goto LABEL_440bfa;
LABEL_440bf0:
                        *((st_43fde0_2 **)&idx[712]) = NULL;
LABEL_440bfa:
                        iter2 = &iter1->field_10;
                        if (iter2)
                        {
                            do
                            {
                                if (*((short *)(*((int *)iter2) + 1002)) == v108 || v108 == -0x1 && v112 != iter1)
                                {
                                    v112 = *((int *)iter2);
                                    v113 = v112;
                                    goto LABEL_440c20;
                                }
                            } while ((iter2 = (unsigned int)*((int *)(iter2 + 4)), iter2));
                        }
                        v113 = NULL;
LABEL_440c20:
                        if (v113->field_494)
                            v113->field_494();
                        v113->field_3c4 = 31;
                        sub_455630(v113);
                        v114 = (int)idx[712];
                        if (!v114)
                        {
LABEL_440c4f:
                            v116 = NULL;
                            goto LABEL_440c99;
                        }
                        else
                        {
                            v115 = *((int *)(g_4ce8cc + 8935096));
                            if (*((int *)(g_4ce8cc + 8935096)))
                            {
                                do
                                {
                                    if (v115->field_0->padding_0 == v114)
                                    {
                                        v116 = v115->field_0;
                                        goto LABEL_440c95;
                                    }
                                } while ((v115 = (st_43fde0_9 *)v115->field_4, v115));
                            }
                            v116 = *((int *)(g_4ce8cc + 8935104));
                            if (!*((int *)(g_4ce8cc + 8935104)))
                                goto LABEL_440c4f;
                            v106 = v106;
                            while (*((int *)v116->padding_0) != v114)
                            {
                                v116 = v116->field_4;
                                if (!v116)
                                    goto LABEL_440c99;
                            }
                            v116 = (st_43fde0_8 *)v116->padding_0;
LABEL_440c95:
                            if (v116)
                                continue;
LABEL_440c99:
                            *((st_43fde0_2 **)&idx[712]) = NULL;
                        }
                    }
                    v117 = &v116->field_10;
                    if (v117)
                    {
                        do
                        {
                            if (*((short *)(*((int *)v117) + 1002)) == v106 || v106 == -0x1 && v118 != v116)
                            {
                                v118 = *((int *)v117);
                                v119 = v118;
                                goto LABEL_440cd7;
                            }
                        } while ((v117 = (unsigned int)*((int *)(v117 + 4)), v117));
                    }
                    v119 = NULL;
LABEL_440cd7:
                    if (v119->field_494)
                        v119->field_494();
                    v119->field_3c4 = 31;
                    sub_455630(v119);
                    v106 += 1;
                } while (v106 < 19);
            }
            if (!(g_4b451c->field_1e9d0 & 16) && !(g_4b451c->field_1e9d1 & 16) && !(g_4b451c->field_1e9d2 & 16) && !(g_4b451c->field_1e9d3 & 16) && !(g_4b451c->field_1e9d4 & 16) && !(g_4b451c->field_1e9d5 & 16))
            {
                v120 = (int)idx[712];
                idx1 = g_4ce8cc;
                if (!v120)
                {
LABEL_440d67:
                    v125 = NULL;
                    goto LABEL_440dae;
                }
                else
                {
                    v122 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v123 = v122->field_0;
                            if (*((int *)&v123->padding_0[0]) == v120)
                                goto LABEL_440da8;
                        } while ((v122 = (st_43fde0_9 *)v122->field_4, v122));
                    }
                    v124 = *((int *)(g_4ce8cc + 8935104));
                    if (!*((int *)(g_4ce8cc + 8935104)))
                        goto LABEL_440d67;
                }
                while (1)
                {
                    v123 = v124->field_0;
                    if (*((int *)&v123->padding_0[0]) != v120)
                    {
                        v124 = v124->field_4;
                        if (!v124)
                        {
                            v125 = NULL;
                            goto LABEL_440dae;
                        }
                    }
                    else
                    {
LABEL_440da8:
                        v125 = v123;
                        if (v125)
                            break;
LABEL_440dae:
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                        break;
                    }
                }
                v126 = &v125->field_10;
                if (v126)
                {
                    do
                    {
                        v127 = *((int *)&v126->padding_0[0]);
                        if (v127->field_3ea == 4)
                            goto LABEL_440dd4;
                    } while ((v126 = (st_43fde0_66 *)*((int *)&v126->padding_0[4]), v126));
                }
                v127 = NULL;
LABEL_440dd4:
                if (v127->field_494)
                {
                    v127->field_494();
                    idx1 = g_4ce8cc;
                }
                v127->field_3c4 = 29;
                v128 = (int)idx[712];
                if (!v128)
                {
LABEL_440e03:
                    v130 = NULL;
                    goto LABEL_440e43;
                }
                else
                {
                    v129 = *((int *)(idx1 + 8935096));
                    if (*((int *)(idx1 + 8935096)))
                    {
                        do
                        {
                            if (v129->field_0->padding_0 == v128)
                            {
                                v130 = v129->field_0;
                                goto LABEL_440e3f;
                            }
                        } while ((v129 = (st_43fde0_9 *)v129->field_4, v129));
                    }
                    v131 = *((int *)(idx1 + 8935104));
                    if (!*((int *)(idx1 + 8935104)))
                        goto LABEL_440e03;
                }
                while (1)
                {
                    if (v131->field_0->padding_0 != v128)
                    {
                        v131 = v131->field_4;
                        if (!v131)
                        {
                            v130 = NULL;
                            goto LABEL_440e43;
                        }
                    }
                    else
                    {
                        v130 = v131->field_0;
LABEL_440e3f:
                        if (v130)
                            break;
LABEL_440e43:
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                        break;
                    }
                }
                v132 = &v130->field_10;
                if (v132)
                {
                    do
                    {
                        if (*((short *)(*((int *)v132) + 1002)) == 12)
                        {
                            v133 = *((int *)v132);
                            goto LABEL_440e69;
                        }
                    } while ((v132 = (unsigned int)*((int *)(v132 + 4)), v132));
                }
                v133 = NULL;
LABEL_440e69:
                if (v133->field_494)
                    v133->field_494();
                v133->field_3c4 = 29;
            }
        }
        if (*((int *)&g_4d48c4) & 524289)
        {
            sub_461970((int)idx[712]);
            switch ((int)idx[40])
            {
            case 0: case 1: case 2: case 3: case 4:
                sub_453d90();
                sub_461970((int)idx[1136]);
                *((unsigned int *)&idx[1136]) = 0;
                sub_461970((int)idx[1140]);
                *((unsigned int *)&idx[1140]) = 0;
                sub_461970((int)idx[1824]);
                sub_43ef40((int)idx[1824]);
                return 1;
            case 5:
                sub_453d90();
                sub_461970((int)idx[1136]);
                *((unsigned int *)&idx[1136]) = 0;
                sub_461970((int)idx[1140]);
                *((unsigned int *)&idx[1140]) = 0;
                sub_461970((int)idx[1824]);
                sub_43ef40((int)idx[1824]);
                sub_453d90();
                return 1;
            case 6:
                break;
            case 7:
LABEL_440f7d:
                sub_453d90();
                sub_43ef40();
                return 1;
            default:
                break;
            }
        }
        break;
    case 4:
        if ((int)idx[696] >= 20)
        {
            v134 = idx + 40;
            switch ((int)idx[40])
            {
            case 0:
                *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) & 0xffffffef;
                break;
            case 1:
                *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) & 0xffffffef;
                sub_4619e0((int)idx[1108]);
                sub_43eee0((int)idx[1108]);
                sub_464900();
                *((unsigned int *)&idx[22924]) = g_4b0ca8;
                g_4aebd0 = 4;
                g_4b0ca8 = 4;
                v135 = (int)v134[8];
                if (!v135)
                {
                    *((int *)v134) = 0;
                    return 1;
                }
                v136 = _ccall(14, 15, v135, 0, 0);
                if (v136 & 1)
                    *((unsigned int *)v134) = v135 - 1;
                else
                    *((int *)v134) = 0;
                return 1;
            case 2:
                *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) | 16;
                sub_4619e0((int)idx[1108]);
                sub_43eee0((int)idx[1108]);
                sub_464900();
                v137 = g_4aebd0;
                if (v137 >= 4)
                {
                    v137 = 1;
                    g_4aebd0 = 1;
                    g_4b0ca8 = 1;
                }
                sub_40f790(v137, v134);
                g_4b0ca8 = v137;
                return 1;
            case 3:
                sub_4619e0((int)idx[1108]);
                sub_43eee0((int)idx[1108]);
                sub_464900();
                return 1;
            case 4:
                sub_4619e0((int)idx[1108]);
                sub_43eee0((int)idx[1108]);
                sub_464900();
                return 1;
            case 5:
                sub_4619e0((int)idx[1108]);
                sub_43eee0((int)idx[1108]);
                sub_464900();
                return 1;
            case 6:
                sub_43eee0();
                sub_464900();
                return 1;
            case 7:
                sub_43eee0();
            default:
                break;
            }
        }
        break;
    case 0:
        *((int *)&idx[48]) = 8;
        if (!sub_43f180(v2, v3, v4))
        {
            *((unsigned int *)((char *)idx + 4 * idx[252] + 184)) = 1;
            *((unsigned int *)&idx[252]) = (int)idx[252] + 1;
        }
        if (g_4b0ce0 & 16)
        {
            v5 = (int)idx[48];
            if (!v5)
            {
                *((int *)&idx[40]) = 2;
            }
            else if (v5 <= 2)
            {
                *((unsigned int *)&idx[40]) = v5 - 1;
            }
            else
            {
                *((int *)&idx[40]) = 2;
            }
            *((unsigned int *)&g_4b0ce0) = *((int *)&g_4b0ce0) & 0xffffffef;
        }
        sub_43ef40();
        if ((char)idx[23576] & 2)
        {
            sub_43efa0();
            sub_43efa0();
            *((unsigned int *)&idx[23576]) = (int)idx[23576] & 0xfffffffd;
            goto LABEL_43ff17;
        }
        else
        {
            if (!sub_461920(v138, g_4ce8cc, (int)idx[1108]))
            {
                sub_43efa0();
                sub_4619e0((int)idx[1108]);
            }
            if ((int)idx[32] != 3)
            {
                v1 = (int)idx[1108];
                sub_4619e0((int)idx[1108]);
            }
            if (!sub_461920(v7, g_4ce8cc, (int)idx[1136]) && !sub_461920(v138, g_4ce8cc, (int)idx[1140]))
                sub_43efa0();
            sub_4067e0(180);
        }
    case 1:
LABEL_43ff17:
        if ((int)idx[696] == 180)
        {
            v9 = 0;
            sub_4615a0((int)idx[20], &v4, 0, 0);
            *((st_43fde0_2 **)&idx[712]) = v1;
            if (!sub_461920((int)idx[1824], g_4ce8cc, (int)idx[1824]))
            {
                *((unsigned int *)&idx[1824]) = 0;
                sub_4615a0((int)idx[24], &v1, 0, 0);
                v11 = &v4;
                *((unsigned int *)&idx[1824]) = v11;
            }
            if ((int)idx[40] > 0)
            {
                do
                {
                    if (!sub_461920(v11, g_4ce8cc, (int)idx[712]))
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                    v12 = sub_462020();
                    if (v12->field_494)
                        v12->field_494();
                    v12->field_3c4 = 30;
                    sub_455630(v12);
                    if (!sub_461920((int)idx[712], g_4ce8cc, (int)idx[712]))
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                    v13 = sub_462020();
                    if (v13->field_494)
                        v13->field_494();
                    v13->field_3c4 = 30;
                    sub_455630(v13);
                    v9 += 1;
                } while (v9 < (int)idx[40]);
            }
            v14 = v9 + 1;
            if (v14 < 8)
            {
                v15 = v14 + 11;
                do
                {
                    v16 = (int)idx[712];
                    v17 = v15 - 8;
                    if (!v16)
                    {
LABEL_440054:
                        v19 = NULL;
                        goto LABEL_440099;
                    }
                    else
                    {
                        v18 = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)))
                        {
                            do
                            {
                                if (v18->field_0->padding_0 == v16)
                                {
                                    v19 = v18->field_0;
                                    goto LABEL_440095;
                                }
                            } while ((v18 = (st_43fde0_9 *)v18->field_4, v18));
                        }
                        v19 = *((int *)(g_4ce8cc + 8935104));
                        if (!*((int *)(g_4ce8cc + 8935104)))
                            goto LABEL_440054;
                        while (*((int *)v19->padding_0) != v16)
                        {
                            v19 = v19->field_4;
                            if (!v19)
                                goto LABEL_440099;
                        }
                        v19 = (st_43fde0_8 *)v19->padding_0;
LABEL_440095:
                        if (v19)
                            goto LABEL_4400a3;
LABEL_440099:
                        *((st_43fde0_2 **)&idx[712]) = NULL;
LABEL_4400a3:
                        v20 = &v19->field_10;
                        if (v20)
                        {
                            do
                            {
                                if (*((short *)(*((int *)v20) + 1002)) == v17 || v17 == -0x1 && v21 != v19)
                                {
                                    v21 = *((int *)v20);
                                    v22 = v21;
                                    goto LABEL_4400cf;
                                }
                            } while ((v20 = (unsigned int)*((int *)(v20 + 4)), v20));
                        }
                        v22 = NULL;
LABEL_4400cf:
                        if (v22->field_494)
                            v22->field_494();
                        v22->field_3c4 = 31;
                        sub_455630(v22);
                        v23 = (int)idx[712];
                        if (!v23)
                        {
LABEL_4400fe:
                            v25 = NULL;
                            goto LABEL_440149;
                        }
                        else
                        {
                            v24 = *((int *)(g_4ce8cc + 8935096));
                            if (*((int *)(g_4ce8cc + 8935096)))
                            {
                                do
                                {
                                    if (v24->field_0->padding_0 == v23)
                                    {
                                        v25 = v24->field_0;
                                        goto LABEL_440145;
                                    }
                                } while ((v24 = (st_43fde0_9 *)v24->field_4, v24));
                            }
                            v25 = *((int *)(g_4ce8cc + 8935104));
                            if (!*((int *)(g_4ce8cc + 8935104)))
                                goto LABEL_4400fe;
                            while (*((int *)v25->padding_0) != v23)
                            {
                                v25 = v25->field_4;
                                if (!v25)
                                    goto LABEL_440149;
                            }
                            v25 = (st_43fde0_8 *)v25->padding_0;
LABEL_440145:
                            if (v25)
                                continue;
LABEL_440149:
                            *((st_43fde0_2 **)&idx[712]) = NULL;
                        }
                    }
                    v26 = &v25->field_10;
                    if (v26)
                    {
                        do
                        {
                            if (*((short *)(*((int *)v26) + 1002)) == v15 || v15 == -0x1 && v27 != v25)
                            {
                                v27 = *((int *)v26);
                                v28 = v27;
                                goto LABEL_440187;
                            }
                        } while ((v26 = (unsigned int)*((int *)(v26 + 4)), v26));
                    }
                    v28 = NULL;
LABEL_440187:
                    if (v28->field_494)
                        v28->field_494();
                    v28->field_3c4 = 31;
                    sub_455630(v28);
                    v15 += 1;
                } while (v15 < 19);
            }
            if (!(g_4b451c->field_1e9d0 & 16) && !(g_4b451c->field_1e9d1 & 16) && !(g_4b451c->field_1e9d2 & 16) && !(g_4b451c->field_1e9d3 & 16) && !(g_4b451c->field_1e9d4 & 16) && !(g_4b451c->field_1e9d5 & 16))
            {
                v29 = sub_461920((int)idx[712], g_4ce8cc, (int)idx[712]);
                if (!v29)
                    *((st_43fde0_2 **)&idx[712]) = NULL;
                v30 = &v29->field_10;
                if (v30)
                {
                    do
                    {
                        if (*((short *)(*((int *)&v30->padding_0[0]) + 1002)) == 4)
                        {
                            v31 = *((int *)&v30->padding_0[0]);
                            goto LABEL_440258;
                        }
                    } while ((v30 = (st_43fde0_66 *)*((int *)&v30->padding_0[4]), v30));
                }
                v31 = NULL;
LABEL_440258:
                if (v31->field_494)
                    v31->field_494();
                v31->field_3c4 = 29;
                v32 = sub_461920((int)idx[712], g_4ce8cc, (int)idx[712]);
                if (!v32)
                    *((st_43fde0_2 **)&idx[712]) = NULL;
                v33 = &v32->field_10;
                if (v33)
                {
                    do
                    {
                        if (*((short *)(*((int *)&v33->padding_0[0]) + 1002)) == 12)
                        {
                            v34 = *((int *)&v33->padding_0[0]);
                            goto LABEL_4402b8;
                        }
                    } while ((v33 = (st_43fde0_66 *)*((int *)&v33->padding_0[4]), v33));
                }
                v34 = NULL;
LABEL_4402b8:
                if (v34->field_494)
                    v34->field_494();
                v34->field_3c4 = 29;
            }
        }
        if ((int)idx[696] > 190)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&idx[36]) = 2;
            v35 = (int)idx[708];
            if (!((char)(int)idx[708] & 1))
            {
                if (/* unsupported instruction */)
                    *((int *)&idx[700]) = /* unsupported instruction */;
                else
                    *((unsigned int *)&idx[700]) = nan;
                *((int *)&idx[696]) = 0;
                *((unsigned int *)&idx[692]) = 0xfff0bdc1;
                *((char **)&idx[704]) = &g_4b2ed0;
                *((unsigned int *)&idx[708]) = v35 | 1;
            }
            v36 = 0;
            if (/* unsupported instruction */)
            {
                *((int *)&idx[700]) = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                *((unsigned int *)&idx[700]) = nan;
                /* unsupported instruction */
            }
            *((int *)&idx[696]) = 0;
            *((unsigned int *)&idx[692]) = 0xffffffff;
            v0 = (int)idx[712];
            sub_4619e0();
            sub_461970((int)idx[712]);
            if ((int)idx[40] > 0)
            {
                do
                {
                    v37 = (int)idx[712];
                    v38 = v36 + 3;
                    if (!v37)
                    {
LABEL_44037d:
                        v40 = NULL;
                        goto LABEL_4403d9;
                    }
                    else
                    {
                        v39 = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)))
                        {
                            do
                            {
                                if (v39->field_0->padding_0 == v37)
                                {
                                    v40 = v39->field_0;
                                    goto LABEL_4403d5;
                                }
                            } while ((v39 = (st_43fde0_9 *)v39->field_4, v39));
                        }
                        v40 = *((int *)(g_4ce8cc + 8935104));
                        if (!*((int *)(g_4ce8cc + 8935104)))
                            goto LABEL_44037d;
                        while (*((int *)v40->padding_0) != v37)
                        {
                            v40 = v40->field_4;
                            if (!v40)
                                goto LABEL_4403d9;
                        }
                        v40 = (st_43fde0_8 *)v40->padding_0;
LABEL_4403d5:
                        if (v40)
                            goto LABEL_4403e3;
LABEL_4403d9:
                        *((st_43fde0_2 **)&idx[712]) = NULL;
LABEL_4403e3:
                        v41 = &v40->field_10;
                        if (v41)
                        {
                            do
                            {
                                if (*((short *)(*((int *)v41) + 1002)) == v38 || v38 == -0x1 && v42 != v40)
                                {
                                    v42 = *((int *)v41);
                                    v43 = v42;
                                    goto LABEL_44040f;
                                }
                            } while ((v41 = (unsigned int)*((int *)(v41 + 4)), v41));
                        }
                        v43 = NULL;
LABEL_44040f:
                        if (v43->field_494)
                            v43->field_494();
                        v43->field_3c4 = 30;
                        sub_455630(v43);
                        v44 = (int)idx[712];
                        v45 = v36 + 11;
                        if (!v44)
                        {
LABEL_440441:
                            v47 = NULL;
                            goto LABEL_440490;
                        }
                        else
                        {
                            v46 = *((int *)(g_4ce8cc + 8935096));
                            if (*((int *)(g_4ce8cc + 8935096)))
                            {
                                do
                                {
                                    if (v46->field_0->padding_0 == v44)
                                    {
                                        v47 = v46->field_0;
                                        goto LABEL_44048c;
                                    }
                                } while ((v46 = (st_43fde0_9 *)v46->field_4, v46));
                            }
                            v47 = *((int *)(g_4ce8cc + 8935104));
                            if (!*((int *)(g_4ce8cc + 8935104)))
                                goto LABEL_440441;
                            while (*((int *)v47->padding_0) != v44)
                            {
                                v47 = v47->field_4;
                                if (!v47)
                                    goto LABEL_440490;
                            }
                            v47 = (st_43fde0_8 *)v47->padding_0;
LABEL_44048c:
                            if (v47)
                                continue;
LABEL_440490:
                            *((st_43fde0_2 **)&idx[712]) = NULL;
                        }
                    }
                    v48 = &v47->field_10;
                    if (v48)
                    {
                        do
                        {
                            if (*((short *)(*((int *)v48) + 1002)) == v45 || v45 == -0x1 && v49 != v47)
                            {
                                v49 = *((int *)v48);
                                v50 = v49;
                                goto LABEL_4404c0;
                            }
                        } while ((v48 = (unsigned int)*((int *)(v48 + 4)), v48));
                    }
                    v50 = 0;
LABEL_4404c0:
                    if (*((int *)(v50 + 1172)))
                        (*((int *)(v50 + 1172)))();
                    *((unsigned short *)(v50 + 964)) = 30;
                    sub_455630(v50);
                    v36 += 1;
                } while (v36 < (int)idx[40]);
            }
            v51 = v36 + 1;
            if (v51 < 8)
            {
                v52 = v51 + 11;
                do
                {
                    v53 = (int)idx[712];
                    v54 = v52 - 8;
                    if (!v53)
                    {
LABEL_44050d:
                        v56 = NULL;
                        goto LABEL_440559;
                    }
                    else
                    {
                        v55 = *((int *)(g_4ce8cc + 8935096));
                        if (*((int *)(g_4ce8cc + 8935096)))
                        {
                            do
                            {
                                if (v55->field_0->padding_0 == v53)
                                {
                                    v56 = v55->field_0;
                                    goto LABEL_440555;
                                }
                            } while ((v55 = (st_43fde0_9 *)v55->field_4, v55));
                        }
                        v56 = *((int *)(g_4ce8cc + 8935104));
                        if (!*((int *)(g_4ce8cc + 8935104)))
                            goto LABEL_44050d;
                        while (*((int *)v56->padding_0) != v53)
                        {
                            v56 = v56->field_4;
                            if (!v56)
                                goto LABEL_440559;
                        }
                        v56 = (st_43fde0_2 *)v56->padding_0;
LABEL_440555:
                        if (v56)
                            goto LABEL_440563;
LABEL_440559:
                        *((st_43fde0_2 **)&idx[712]) = NULL;
LABEL_440563:
                        v57 = &v56->field_10;
                        if (v57)
                        {
                            do
                            {
                                if (*((short *)&v57->padding_0[1002]) == v54 || v54 == -0x1 && v58 != v56)
                                {
                                    v58 = (st_43fde0_0 *)v57->padding_0;
                                    v59 = v58;
                                    goto LABEL_44058f;
                                }
                            } while ((v57 = (st_43fde0_2 *)v57->field_4, v57));
                        }
                        v59 = NULL;
LABEL_44058f:
                        if (v59->field_494)
                            v59->field_494();
                        v59->field_3c4 = 31;
                        sub_455630(v59);
                        v60 = (int)idx[712];
                        if (!v60)
                        {
LABEL_4405be:
                            v62 = NULL;
                            goto LABEL_440609;
                        }
                        else
                        {
                            v61 = *((int *)(g_4ce8cc + 8935096));
                            if (*((int *)(g_4ce8cc + 8935096)))
                            {
                                do
                                {
                                    if (v61->field_0->padding_0 == v60)
                                    {
                                        v62 = v61->field_0;
                                        goto LABEL_440605;
                                    }
                                } while ((v61 = (st_43fde0_9 *)v61->field_4, v61));
                            }
                            v62 = *((int *)(g_4ce8cc + 8935104));
                            if (!*((int *)(g_4ce8cc + 8935104)))
                                goto LABEL_4405be;
                            while (*((int *)v62->padding_0) != v60)
                            {
                                v62 = v62->field_4;
                                if (!v62)
                                    goto LABEL_440609;
                            }
                            v62 = (st_43fde0_8 *)v62->padding_0;
LABEL_440605:
                            if (v62)
                                continue;
LABEL_440609:
                            *((st_43fde0_2 **)&idx[712]) = NULL;
                        }
                    }
                    v63 = &v62->field_10;
                    if (v63)
                    {
                        do
                        {
                            if (*((short *)(*((int *)v63) + 1002)) == v52 || v52 == -0x1 && v64 != v62)
                            {
                                v64 = *((int *)v63);
                                v65 = v64;
                                goto LABEL_440647;
                            }
                        } while ((v63 = (unsigned int)*((int *)(v63 + 4)), v63));
                    }
                    v65 = NULL;
LABEL_440647:
                    if (v65->field_494)
                        v65->field_494();
                    v65->field_3c4 = 31;
                    sub_455630(v65);
                    v52 += 1;
                } while (v52 < 19);
            }
            if (!(g_4b451c->field_1e9d0 & 16) && !(g_4b451c->field_1e9d1 & 16) && !(g_4b451c->field_1e9d2 & 16) && !(g_4b451c->field_1e9d3 & 16) && !(g_4b451c->field_1e9d4 & 16) && !(g_4b451c->field_1e9d5 & 16))
            {
                v66 = (int)idx[712];
                index = g_4ce8cc;
                if (!v66)
                {
LABEL_4406d7:
                    v71 = NULL;
                    goto LABEL_44071e;
                }
                else
                {
                    v68 = *((int *)(g_4ce8cc + 8935096));
                    if (*((int *)(g_4ce8cc + 8935096)))
                    {
                        do
                        {
                            v69 = v68->field_0;
                            if (*((int *)&v69->padding_0[0]) == v66)
                                goto LABEL_440718;
                        } while ((v68 = (st_43fde0_9 *)v68->field_4, v68));
                    }
                    v70 = *((int *)(g_4ce8cc + 8935104));
                    if (!*((int *)(g_4ce8cc + 8935104)))
                        goto LABEL_4406d7;
                }
                while (1)
                {
                    v69 = v70->field_0;
                    if (*((int *)&v69->padding_0[0]) != v66)
                    {
                        v70 = v70->field_4;
                        if (!v70)
                        {
                            v71 = NULL;
                            goto LABEL_44071e;
                        }
                    }
                    else
                    {
LABEL_440718:
                        v71 = v69;
                        if (v71)
                            break;
LABEL_44071e:
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                        break;
                    }
                }
                v72 = &v71->field_10;
                if (v72)
                {
                    do
                    {
                        v73 = *((int *)&v72->padding_0[0]);
                        if (v73->field_3ea == 4)
                            goto LABEL_440744;
                    } while ((v72 = (st_43fde0_66 *)*((int *)&v72->padding_0[4]), v72));
                }
                v73 = NULL;
LABEL_440744:
                if (v73->field_494)
                {
                    v73->field_494();
                    index = g_4ce8cc;
                }
                v73->field_3c4 = 29;
                v74 = (int)idx[712];
                if (!v74)
                {
LABEL_440773:
                    v76 = NULL;
                    goto LABEL_4407b3;
                }
                else
                {
                    v75 = *((int *)(index + 8935096));
                    if (*((int *)(index + 8935096)))
                    {
                        do
                        {
                            if (v75->field_0->padding_0 == v74)
                            {
                                v76 = v75->field_0;
                                goto LABEL_4407af;
                            }
                        } while ((v75 = (st_43fde0_9 *)v75->field_4, v75));
                    }
                    v77 = *((int *)(index + 8935104));
                    if (!*((int *)(index + 8935104)))
                        goto LABEL_440773;
                }
                while (1)
                {
                    if (v77->field_0->padding_0 != v74)
                    {
                        v77 = v77->field_4;
                        if (!v77)
                        {
                            v76 = NULL;
                            goto LABEL_4407b3;
                        }
                    }
                    else
                    {
                        v76 = v77->field_0;
LABEL_4407af:
                        if (v76)
                            break;
LABEL_4407b3:
                        *((st_43fde0_2 **)&idx[712]) = NULL;
                        break;
                    }
                }
                v78 = &v76->field_10;
                if (v78)
                {
                    do
                    {
                        if (*((short *)(*((int *)v78) + 1002)) == 12)
                        {
                            v79 = *((int *)v78);
                            goto LABEL_4407d9;
                        }
                    } while ((v78 = (unsigned int)*((int *)(v78 + 4)), v78));
                }
                v79 = NULL;
LABEL_4407d9:
                if (v79->field_494)
                    v79->field_494();
                v79->field_3c4 = 29;
                return 1;
            }
        }
        break;
    case 3:
        return 1;
    default:
        return 1;
    }
}
