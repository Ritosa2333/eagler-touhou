// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x434900; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_434900_2 {
    char padding_0[1444];
    int field_5a4;
} st_434900_2;

typedef struct st_434900_1 {
    char padding_0[30];
    char field_1e;
} st_434900_1;

typedef struct st_434900_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[12];
    int field_14;
    char padding_18[32];
    int field_38;
    int field_3c;
    unsigned int field_40;
    char padding_44[196];
    unsigned int field_108;
    char padding_10c[4];
    unsigned int field_110;
    unsigned int field_114;
    unsigned int field_118;
    char padding_11c[196];
    unsigned int field_1e0;
    char padding_1e4[4];
    int field_1e8;
    char padding_1ec[4];
    int field_1f0;
    unsigned int field_1f4;
    unsigned int field_1f8;
    char padding_1fc[8];
    unsigned int field_204;
    char padding_208[196];
    char field_2cc;
    char field_2cd;
} st_434900_0;

typedef struct st_434900_3 {
    char padding_0[104];
    int field_68;
} st_434900_3;

typedef struct st_434900_4 {
    char padding_0[28];
    struct st_434900_3 *field_1c;
} st_434900_4;

extern char g_4aedb0;
extern char g_4b0c44;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern unsigned int g_4b0cb4;
extern char g_4b0ce0;
extern st_434900_4 *g_4b4518;
extern unsigned int g_4b451c;
extern unsigned int g_4b452c;
extern unsigned int g_4cee40;
extern char g_4d48c0;
extern void g_4d48c4;

int sub_434900(st_434900_0 *idx)
{
    st_434900_0 *v20;  // esi
    int *v21;  // eax
    unsigned int *v22;  // eax
    unsigned int v23;  // ecx
    unsigned int v24;  // 4101
    int v25;  // esi
    st_434900_0 *iter;  // edi
    unsigned int v27;  // eax
    unsigned int v28;  // 4101
    unsigned int v29;  // eax
    unsigned int v12;  // edi
    st_434900_0 *v30;  // esi
    unsigned int v31;  // eax
    st_434900_0 *v32;  // edi
    unsigned int v33;  // 4101
    unsigned int v34;  // 4098
    st_434900_0 *node;  // esi
    unsigned int v36;  // eax
    unsigned int iter1;  // eax
    char v38;  // 4102
    char *v39;  // ecx
    unsigned int v13;  // eax
    char v40;  // 4101
    unsigned int v41;  // cc_dep1
    unsigned int v42;  // cc_dep2
    char v43;  // 4103
    unsigned int v44;  // eax
    unsigned int v45;  // 4111
    unsigned int v46;  // 4120
    int v47;  // eax
    st_434900_0 *iter2;  // esi
    int v49;  // ebx
    st_434900_2 *v14;  // eax
    int v50;  // ebx
    unsigned int v51;  // edi
    unsigned int v52;  // eax
    st_434900_0 *v53;  // esi
    unsigned int v54;  // edx
    unsigned int v55;  // edx
    unsigned int idx1;  // eax
    int v57;  // ecx
    int v58;  // eax
    int v59;  // eax
    unsigned int v15;  // 4101
    unsigned int v60;  // eax
    st_434900_0 *v61;  // esi
    unsigned int v62;  // eax
    st_434900_0 *v63;  // eax
    char v64;  // 4104
    unsigned int v65;  // eax
    unsigned int v66;  // eax
    int v67;  // eax
    unsigned int v68;  // eax
    st_434900_0 *v69;  // esi
    unsigned int v16;  // eax
    unsigned int v70;  // edx
    unsigned int v71;  // edx
    unsigned int index;  // eax
    int v73;  // ecx
    int v74;  // eax
    int v75;  // eax
    unsigned int v76;  // eax
    st_434900_0 *v77;  // eax
    st_434900_1 *v78;  // esi
    st_434900_0 *v79;  // edx
    unsigned int v17;  // eax
    char v80;  // 4103
    st_434900_0 *v81;  // eax
    char v82;  // 4102
    st_434900_0 *v83;  // esi
    unsigned int v84;  // 4101
    unsigned int v85;  // eax
    unsigned int v86;  // eax
    unsigned int v87;  // eax
    unsigned int v88;  // eax
    unsigned int v18;  // 4101
    unsigned int v19;  // eax
    char *v0;  // [bp-0x7c]
    unsigned int v1;  // [bp-0x78]
    unsigned int v2;  // [bp-0x74]
    unsigned int v3;  // [bp-0x70]
    int v4;  // [bp-0x6c]
    st_434900_0 *v6;  // [bp-0x60]
    char v7;  // [bp-0x58]
    char v8;  // [bp-0x54]
    char v9;  // [bp-0x48]

    switch (idx->field_4)
    {
    case 20:
        if (idx->field_14 >= 10 && *((int *)&g_4d48c4) & 524289)
        {
            sub_453d90();
            idx->field_4 = 21;
            sub_461970(idx->field_1e8, v12);
            sub_4067e0(0);
            return v13;
        }
        break;
    case 21:
        if (idx->field_14 >= 10)
        {
            sub_431b30();
            if (g_4b0ce0 & 16)
            {
                v14 = (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + (g_4b0cb0 + g_4b0ca8 * 6) * 8 + 1444;
                if (*((int *)&g_4b0c44) > *((int *)&v14->padding_0[0]))
                    *((int *)&v14->padding_0[0]) = *((int *)&g_4b0c44);
                idx->field_1e8 = *((int *)sub_4357c0(&v9, -(0 < idx->field_1f4) + 117));
                idx->field_4 = 22;
                idx->field_40 = 3;
                idx->field_108 = 1;
                if (1)
                {
                    v15 = _ccall(14, 15, 3, 0, 0);
                    if (v15 & 1)
                        idx->field_38 = 2;
                    else
                        idx->field_38 = 0;
                }
                else
                {
                    idx->field_38 = 0;
                }
                sub_461970(idx->field_1e8, &v9);
                sub_461970(idx->field_1e8, -(0 < idx->field_1f4) + 117);
                return v16;
            }
            else
            {
                sub_433a30();
                sub_4067e0(0);
                if (!idx->field_1f8)
                {
                    idx->field_4 = 25;
                    sub_461d80();
                    _security_check_cookie();
                    return v17;
                }
                idx->field_1e8 = *((int *)sub_4357c0(&v7, 111));
                idx->field_4 = 14;
                idx->field_40 = 3;
                idx->field_108 = 1;
                if (1)
                {
                    v18 = _ccall(14, 15, 3, 0, 0);
                    if (v18 & 1)
                        idx->field_38 = 2;
                    else
                        idx->field_38 = 0;
                }
                else
                {
                    idx->field_38 = 0;
                }
                sub_461970(idx->field_1e8, &v7);
                sub_461970(idx->field_1e8, 111);
                _security_check_cookie();
                return v19;
            }
        }
        break;
    case 22:
        v20 = &idx->field_38;
        v20->field_4 = idx->field_38;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
        {
            v4 = -0x1;
            sub_464970(-0x1);
        }
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
        {
            v3 = 1;
            sub_464970(1);
        }
        if (v20->field_4 != v20->padding_0)
        {
            sub_461970(idx->field_1e8, v3);
            sub_453d90(v4);
        }
        if (*((int *)&g_4d48c4) & 524289)
        {
            v21 = sub_462060();
            sub_461970(*(v21), v3);
            sub_453d90(v4);
            idx->field_4 = 26;
            sub_4067e0(0);
            sub_431b30();
            if (!idx->field_38)
            {
LABEL_434bfa:
                idx->field_4 = 26;
                sub_4067e0(0);
                sub_453d90(0);
            }
            else if (idx->field_38 == 1)
            {
                idx->field_4 = 23;
                sub_4067e0(0);
                sub_461d80(0);
                v22 = sub_464900();
                idx->field_40 = 25;
                idx->field_108 = 1;
                v23 = v22[2];
                if (v23)
                {
                    v24 = _ccall(14, 15, v23, 0, 0);
                    if (v24 & 1)
                        *(v22) = v23 - 1;
                    else
                        *(v22) = 0;
                }
                else
                {
                    *(v22) = 0;
                }
                v25 = 1;
                iter = &idx->field_204;
                do
                {
                    sub_46cbbb(&v8, "th12_%.2d.rpy", v25);
                    v0 = &v8;
                    *((unsigned int *)&iter->padding_0[0]) = sub_43b6f0(&v8);
                    v25 += 1;
                    iter = &iter->field_4;
                } while (v25 <= 25);
            }
            else if (idx->field_38 == 2)
            {
                goto LABEL_434bfa;
            }
        }
        if (*((int *)&g_4d48c4) & 258)
        {
            sub_453d90();
            if (idx->field_38)
            {
                v27 = idx->field_40;
                if (v27)
                {
                    v28 = _ccall(14, 15, v27, 0, 0);
                    if (v28 & 1)
                        idx->field_38 = v27 - 1;
                    else
                        idx->field_38 = 0;
                }
                else
                {
                    idx->field_38 = 0;
                }
                sub_461970(idx->field_1e8, v0);
                _security_check_cookie();
                return v29;
            }
        }
        break;
    case 23:
        if (idx->field_14 >= 10)
        {
            v30 = &idx->field_38;
            v30->field_4 = idx->field_38;
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0x1);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(1);
            if (v30->field_4 != v30->padding_0)
                sub_453d90();
            if (*((int *)&g_4d48c4) & 524289)
            {
                idx->field_4 = 24;
                sub_4067e0(0);
                v31 = idx->field_118;
                v32 = &idx->field_110;
                if (v31)
                {
                    v33 = _ccall(14, 15, v31, 0, 0);
                    if (v33 & 1)
                        *((unsigned int *)&v32->padding_0[0]) = v31 - 1;
                    else
                        *((unsigned int *)&v32->padding_0[0]) = 0;
                }
                else
                {
                    *((unsigned int *)&v32->padding_0[0]) = 0;
                }
                v34 = idx->field_1f4;
                idx->field_118 = 91;
                idx->field_1e0 = 1;
                if (v34 && !(g_4b0ce0 & 16))
                {
                    sub_46d5b7(&g_4b4518->field_1c->padding_0[12]);
                    g_4b4518->field_1c->field_68 = 8;
                }
                else
                {
                    sub_46d5b7(&g_4b4518->field_1c->padding_0[12]);
                    g_4b4518->field_1c->field_68 = g_4b0cb0;
                }
                node = &idx->field_2cc;
                v36 = g_4b451c + 125376;
                iter1 = v36;
                do
                {
                    v38 = *((char *)iter1);
                    node->padding_0[iter1 + -1 * v36] = *((char *)iter1);
                    iter1 += 1;
                } while (v38);
                idx->field_1f0 = 0;
                v39 = "        ";
                while (1)
                {
                    v40 = node->padding_0[0];
                    v41 = v40;
                    v42 = *(v39);
                    if (v40 != *(v39))
                        break;
                    if (v40)
                    {
                        v43 = node->padding_0[1];
                        v41 = v43;
                        v42 = v39[1];
                        if (v43 != v39[1])
                            break;
                        node = &node->padding_0[2];
                        v39 += 2;
                        if (!v43)
                            goto LABEL_434e5c;
                    }
                    else
                    {
LABEL_434e5c:
                        v44 = 0;
                        goto LABEL_434e65;
                    }
                }
                v45 = _ccall(4, v41, v42, 0);
                v46 = _ccall(12, 0, v45 & 1, v45 & 1);
                v44 = -(v45 & 1) + 1 - (v46 & 1);
LABEL_434e65:
                if (v44)
                    sub_464970(-0x1);
                v47 = 8;
                do
                {
                } while (idx->padding_208[195 + v47] == 32 && (v47 = (int)(v47 - 1), v47 > 0));
                idx->field_1f0 = v47;
                break;
            }
            else if (*((int *)&g_4d48c4) & 258)
            {
                idx->field_4 = 22;
                v6 = &idx->padding_8[8];
                sub_4067e0(0);
                sub_461d40(0);
                sub_464940();
                idx->field_40 = 3;
                idx->field_108 = 1;
                sub_461970(idx->field_1e8, 0);
                iter2 = &idx->field_204;
                v49 = 25;
                do
                {
                    v50 = v49;
                    v51 = (unsigned int)iter2->padding_0;
                    if (v51)
                    {
                        sub_43b450(v51);
                        sub_46ca4f(v51);
                    }
                } while ((*((unsigned int *)&iter2->padding_0[0]) = 0, iter2 += 4, v49 = (int)(v50 - 1), v50 != 1));
                idx->field_4 = 22;
                sub_4067e0(v49);
                sub_453d90(v49);
                _security_check_cookie();
                return v52;
            }
        }
        break;
    case 24:
        if (idx->field_14 >= 10)
        {
            v53 = &idx->field_110;
            v53->field_4 = idx->field_110;
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0xd);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(13);
            if (g_4d48c4 & 64 || g_4d48c0 & 64)
            {
                v54 = (unsigned int)(1321528399 * v53->padding_0 >> 0x22);
                v2 = (v53->padding_0 == ((v54 >> 31) + v54) * 13 ? 12 : 0xffffffff);
                sub_464970((v53->padding_0 == ((v54 >> 31) + v54) * 13 ? 12 : 0xffffffff));
            }
            if (g_4d48c4 & 128 || g_4d48c0 & 128)
            {
                v55 = (unsigned int)(1321528399 * v53->padding_0 >> 0x22);
                v1 = (v53->padding_0 - ((v55 >> 31) + v55) * 13 == 12 ? 0xfffffff4 : 1);
                sub_464970((v53->padding_0 - ((v55 >> 31) + v55) * 13 == 12 ? 0xfffffff4 : 1));
            }
            if (v53->field_4 != v53->padding_0)
                sub_453d90();
            if (*((int *)&g_4d48c4) & 524289)
            {
                idx1 = (unsigned int)v53->padding_0;
                if (idx1 < 88)
                {
                    v57 = idx->field_1f0;
                    if (v57 < 8)
                    {
                        (&idx->field_2cc)[v57] = *((char *)(idx1 + 4853384));
                        goto LABEL_43506c;
                    }
                    else
                    {
                        idx->padding_208[195 + v57] = *((char *)(idx1 + 4853384));
                        goto LABEL_435087;
                    }
                }
                if (idx1 == 88)
                {
                    v58 = idx->field_1f0;
                    if (v58 < 8)
                    {
                        (&idx->field_2cc)[v58] = 32;
LABEL_43506c:
                        idx->field_1f0 = idx->field_1f0 + 1;
                        if (idx->field_1f0 >= 8)
                            sub_40f790();
                    }
                    else
                    {
                        idx->padding_208[195 + v58] = 32;
                        goto LABEL_435087;
                    }
                    goto LABEL_435087;
                }
                else if (idx1 != 89)
                {
                    if (idx1 == 90)
                    {
                        sub_453d90();
                        sub_46cbbb(&v8, "th12_%.2d.rpy", idx->field_38 + 1);
                        sub_43b7c0();
                        v61 = &idx->field_2cc;
                        sub_43bc10(&v8, v61, 0);
                        v62 = sub_43b6f0(&v8);
                        (&idx->field_204)[idx->field_38] = v62;
                        idx->field_4 = 23;
                        sub_4067e0(0);
                        v63 = v61;
                        do
                        {
                            v64 = v63->padding_0[0];
                            *((char *)(v63 + g_4b451c - (char *)v61 + 125376)) = v63->padding_0[0];
                            v63 = &v63->padding_0[1];
                        } while (v64);
                        _security_check_cookie();
                        return v65;
                    }
LABEL_435087:
                    sub_453d90();
                    goto LABEL_435091;
                }
                else if (idx->field_1f0)
                {
                    v59 = idx->field_1f0 - 1;
                    idx->field_1f0 = v59;
                    (&idx->field_2cc)[v59] = 32;
                    sub_453d90();
                    _security_check_cookie();
                    return v60;
                }
            }
            else
            {
LABEL_435091:
                if (*((int *)&g_4d48c4) & 258)
                {
                    sub_453d90();
                    if (idx->field_1f0)
                    {
                        v67 = idx->field_1f0 - 1;
                        idx->field_1f0 = v67;
                        (&idx->field_2cc)[v67] = 32;
                        _security_check_cookie();
                        return v68;
                    }
                    idx->field_4 = 23;
                    sub_4067e0(0);
                    _security_check_cookie();
                    return v66;
                }
            }
        }
        break;
    case 25:
        if (idx->field_14 >= 10)
        {
            if (!idx->field_1f8)
            {
                v69 = &idx->field_110;
                v69->field_4 = idx->field_110;
                if (g_4d48c4 & 16 || g_4d48c0 & 16)
                    sub_464970(-0xd);
                if (g_4d48c4 & 32 || g_4d48c0 & 32)
                    sub_464970(13);
                if (g_4d48c4 & 64 || g_4d48c0 & 64)
                {
                    v70 = (unsigned int)(1321528399 * v69->padding_0 >> 0x22);
                    v2 = (v69->padding_0 == ((v70 >> 31) + v70) * 13 ? 12 : 0xffffffff);
                    sub_464970((v69->padding_0 == ((v70 >> 31) + v70) * 13 ? 12 : 0xffffffff));
                }
                if (g_4d48c4 & 128 || g_4d48c0 & 128)
                {
                    v71 = (unsigned int)(1321528399 * v69->padding_0 >> 0x22);
                    v1 = (v69->padding_0 - ((v71 >> 31) + v71) * 13 == 12 ? 0xfffffff4 : 1);
                    sub_464970((v69->padding_0 - ((v71 >> 31) + v71) * 13 == 12 ? 0xfffffff4 : 1));
                }
                if (v69->field_4 != v69->padding_0)
                    sub_453d90();
            }
            if (*((int *)&g_4d48c4) & 524289)
            {
                if (idx->field_1f8)
                    goto LABEL_43542c;
                index = idx->field_110;
                if (index < 88)
                {
                    v73 = idx->field_1f0;
                    if (v73 < 8)
                    {
                        (&idx->field_2cc)[v73] = *((char *)(index + 4853384));
                        goto LABEL_435341;
                    }
                    else
                    {
                        idx->padding_208[195 + v73] = *((char *)(index + 4853384));
                        break;
                    }
                }
                if (index == 88)
                {
                    v74 = idx->field_1f0;
                    if (v74 < 8)
                    {
                        (&idx->field_2cc)[v74] = 32;
LABEL_435341:
                        idx->field_1f0 = idx->field_1f0 + 1;
                        if (idx->field_1f0 >= 8)
                        {
                            sub_40f790();
                            break;
                        }
                    }
                    else
                    {
                        idx->padding_208[195 + v74] = 32;
                        break;
                    }
                }
                if (index == 89)
                {
                    if (idx->field_1f0)
                    {
                        v75 = idx->field_1f0 - 1;
                        idx->field_1f0 = v75;
                        (&idx->field_2cc)[v75] = 32;
                        break;
                    }
                }
                else
                {
                    if (index == 90)
                    {
                        v77 = &idx->field_2cc;
                        v78 = (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + (idx->field_38 + g_4b0ca8 * 10) * 28 + 30;
                        v79 = v77;
                        do
                        {
                            v80 = v79->padding_0[0];
                            v78->padding_0[0] = v79->padding_0[0];
                            v79 = &v79->padding_0[1];
                            v78 = &v78->padding_0[1];
                        } while (v80);
                        v81 = v77;
                        do
                        {
                            v82 = v81->padding_0[0];
                            *((char *)(v81 + g_4b451c - (char *)v77 + 125376)) = v81->padding_0[0];
                            v81 = &v81->padding_0[1];
                        } while (v82);
                        goto LABEL_43542c;
                    }
                    sub_453d90();
                    _security_check_cookie();
                    return v76;
                }
            }
            else if (*((int *)&g_4d48c4) & 258)
            {
                if (idx->field_38 < 0)
                {
LABEL_43542c:
                    v83 = &idx->field_1e8;
                    *((int *)&v83->padding_0[0]) = *((int *)sub_4357c0(&ebp, 116));
                    sub_461d40(&ebp, 116);
                    idx->field_4 = 22;
                    idx->field_40 = 3;
                    idx->field_108 = 1;
                    if (1)
                    {
                        v84 = _ccall(14, 15, 3, 0, 0);
                        if (v84 & 1)
                            idx->field_38 = 2;
                        else
                            idx->field_38 = 0;
                    }
                    else
                    {
                        idx->field_38 = 0;
                    }
                    sub_461970(v83->padding_0, &ebp);
                    sub_461970(v83->padding_0, 116);
                    _security_check_cookie();
                    return v85;
                }
                else if (idx->field_1f0)
                {
                    sub_453d90();
                    idx->field_1f0 = idx->field_1f0 - 1;
                    (&idx->field_2cc)[idx->field_1f0] = 32;
                    _security_check_cookie();
                    return v86;
                }
            }
        }
        break;
    case 26:
        if (idx->field_14 >= 12)
        {
            sub_4339c0();
            idx->field_4 = 27;
            if (!(idx->field_38 && idx->field_38 != 1))
            {
                sub_40f720();
            }
            else if (idx->field_38 == 2)
            {
                if (g_4b0cb0 >= 7)
                {
                    g_4b452c = &g_4aedb0;
                    g_4b0cb0 = 7;
                    g_4b0cb4 = 7;
                }
                g_4cee40 = 10;
                return v87;
            }
        }
        break;
    default:
        _security_check_cookie();
        return v88;
    }
    goto LABEL_0x43557a;
}
