// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4329a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4329a0_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[12];
    int field_14;
    char padding_18[32];
    unsigned int field_38;
    unsigned int field_3c;
    int field_40;
    char padding_44[196];
    unsigned int field_108;
    unsigned int field_10c;
    unsigned int field_110;
    unsigned int field_114;
    unsigned int field_118;
    char padding_11c[196];
    unsigned int field_1e0;
    char padding_1e4[4];
    unsigned int field_1e8;
    int field_1ec;
    int field_1f0;
    unsigned int field_1f4;
    char padding_1f8[4];
    unsigned int field_1fc;
    char padding_200[4];
    unsigned int field_204;
    char padding_208[196];
    char field_2cc;
    char field_2cd;
} st_4329a0_0;

typedef struct st_4329a0_1 {
    unsigned int field_0;
    char padding_4[4];
    int field_8;
} st_4329a0_1;

typedef struct st_4329a0_2 {
    char padding_0[104];
    unsigned int field_68;
} st_4329a0_2;

typedef struct st_4329a0_4 {
    char padding_0[125376];
    char field_1e9c0;
} st_4329a0_4;

typedef struct st_4329a0_3 {
    char padding_0[28];
    struct st_4329a0_2 *field_1c;
} st_4329a0_3;

extern unsigned int g_4b0cb0;
extern char g_4b0ce0;
extern unsigned int g_4b44e8;
extern st_4329a0_3 *g_4b4518;
extern st_4329a0_4 *g_4b451c;
extern unsigned int g_4cee40;
extern char g_4d48c0;
extern void g_4d48c4;

int sub_4329a0(st_4329a0_0 *idx)
{
    int v9;  // eax
    unsigned int v10;  // 4101
    int *v19;  // eax
    int *v20;  // eax
    int v21;  // eax
    int *v22;  // eax
    int v23;  // eax
    st_4329a0_1 *v24;  // eax
    int v25;  // ecx
    unsigned int v26;  // esi
    st_4329a0_0 *v27;  // esi
    int *v28;  // eax
    unsigned int v11;  // esi
    int *v29;  // eax
    int *v30;  // eax
    int v31;  // eax
    int v32;  // eax
    unsigned int v33;  // 4101
    unsigned int v34;  // eax
    st_4329a0_0 *idx1;  // edi
    unsigned int v36;  // eax
    st_4329a0_0 *v37;  // edi
    unsigned int v38;  // 4101
    unsigned int v12;  // eax
    unsigned int v39;  // 4098
    st_4329a0_0 *iter;  // esi
    unsigned int v41;  // eax
    unsigned int node;  // eax
    char v43;  // 4104
    char *v44;  // ecx
    char v45;  // 4101
    unsigned int v46;  // cc_dep1
    unsigned int v47;  // cc_dep2
    char v48;  // 4103
    int v13;  // eax
    unsigned int v49;  // eax
    unsigned int v50;  // 4111
    unsigned int v51;  // 4120
    int v52;  // eax
    unsigned int v53;  // eax
    unsigned int v54;  // eax
    st_4329a0_0 *iter1;  // ebx
    int v56;  // eax
    unsigned int v57;  // eax
    st_4329a0_0 *v58;  // esi
    unsigned int v14;  // esi
    unsigned int v59;  // edx
    unsigned int v60;  // edx
    unsigned int index;  // eax
    int v62;  // ecx
    int v63;  // eax
    int v64;  // eax
    unsigned int v65;  // eax
    st_4329a0_0 *v66;  // esi
    unsigned int v67;  // eax
    st_4329a0_0 *iter2;  // eax
    unsigned int v15;  // eax
    char v69;  // 4104
    unsigned int v70;  // eax
    unsigned int v71;  // eax
    int v72;  // eax
    unsigned int v73;  // eax
    unsigned int v74;  // esi
    unsigned int v75;  // eax
    unsigned int v76;  // esi
    int v77;  // ebx
    unsigned int v78;  // eax
    st_4329a0_0 *v16;  // esi
    unsigned int *v79;  // eax
    unsigned int v80;  // ecx
    unsigned int v81;  // 4101
    int v82;  // esi
    st_4329a0_0 *v83;  // ebp
    unsigned int v84;  // eax
    unsigned int v85;  // eax
    unsigned int v86;  // esi
    unsigned int v87;  // ebx
    unsigned int v88;  // eax
    int *v17;  // eax
    unsigned int v89;  // esi
    unsigned int v90;  // ebx
    int v91;  // edi
    unsigned int v92;  // eax
    unsigned int v93;  // eax
    int *v18;  // eax
    unsigned int v0;  // [bp-0x8c]
    unsigned int v1;  // [bp-0x88]
    unsigned int v2;  // [bp-0x84]
    unsigned int v3;  // [bp-0x80]
    unsigned int v4;  // [bp-0x74]
    unsigned int v5;  // [bp-0x74]
    st_4329a0_0 *v6;  // [bp-0x70]
    char v7;  // [bp-0x54]
    char v8;  // [bp-0x4c]

    switch (idx->field_4)
    {
    case 1:
        if (idx->field_14 >= 10)
        {
            idx->field_4 = 3;
            idx->field_40 = 4;
            if (*((int *)(g_4b44e8 + 116)))
            {
                *((unsigned int *)&idx->padding_44[132 + 4 * idx->field_10c]) = 2;
                idx->field_10c = idx->field_10c + 1;
            }
            idx->field_108 = 1;
            v9 = idx->field_40;
            if (v9)
            {
                v10 = _ccall(14, 15, v9, 0, 0);
                if (v10 & 1)
                    idx->field_38 = v9 - 1;
                else
                    idx->field_38 = 0;
            }
            else
            {
                idx->field_38 = 0;
            }
            sub_461970(idx->field_1e8, v11);
            idx->field_1fc = 0;
            _security_check_cookie();
            return v12;
        }
        break;
    case 2:
        if (idx->field_14 >= 10)
        {
            idx->field_4 = 3;
            idx->field_40 = 4;
            *((unsigned int *)&idx->padding_44[132 + 4 * idx->field_10c]) = 2;
            idx->field_10c = idx->field_10c + 1;
            *((unsigned int *)&idx->padding_44[132 + 4 * idx->field_10c]) = 0;
            idx->field_10c = idx->field_10c + 1;
            idx->field_108 = 1;
            v13 = idx->field_40;
            if (!v13)
            {
                idx->field_38 = 1;
            }
            else if (v13 <= 1)
            {
                idx->field_38 = v13 - 1;
            }
            else
            {
                idx->field_38 = 1;
            }
            sub_461970(idx->field_1e8, v14);
            idx->field_1fc = 1;
            _security_check_cookie();
            return v15;
        }
        break;
    case 3:
        v16 = &idx->field_38;
        v16->field_4 = idx->field_38;
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
        {
            v3 = 0xffffffff;
            sub_464970(-0x1);
        }
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
        {
            v2 = 1;
            sub_464970(1);
        }
        if (v16->field_4 != v16->padding_0)
        {
            v1 = idx->field_1e8;
            sub_461970(idx->field_1e8, v2);
            sub_453d90(v3);
        }
        if (*((int *)&g_4d48c4) & 524289)
        {
            sub_453d90();
            switch (v16->padding_0)
            {
            case 0:
                sub_461970(idx->field_1ec, v2);
                v2 = idx->field_1e8;
                sub_461970(idx->field_1e8, v3);
                idx->field_4 = 4;
                break;
            case 1:
                v17 = sub_462060();
                sub_461970(*(v17), v2);
                idx->field_4 = -(0 < *((int *)(g_4b44e8 + 116))) + 5;
                break;
            case 2:
                v18 = sub_462060();
                sub_461970(*(v18), v2);
                idx->field_4 = 7;
                break;
            case 3:
                v19 = sub_462060();
                sub_461970(*(v19), v2);
                idx->field_4 = 5;
                idx->field_4 = -(0 < *((int *)(g_4b44e8 + 116))) + 5;
                break;
            }
            v1 = 0;
            sub_4067e0(0);
        }
        if (!idx->field_1fc && *((int *)&g_4d48c4) & 0x200000)
        {
            sub_453d90();
            v20 = sub_462060();
            sub_461970(*(v20), v1);
            v1 = 0;
            idx->field_4 = 4;
            sub_4067e0(0);
            v21 = idx->field_40;
            if (!v21)
            {
                idx->field_38 = 3;
            }
            else if (v21 <= 3)
            {
                idx->field_38 = v21 - 1;
            }
            else
            {
                idx->field_38 = 3;
            }
            sub_461970(idx->field_1ec, 0);
        }
        if (*((int *)&g_4d48c4) & 0x10000)
        {
            sub_453d90();
            v22 = sub_462060();
            sub_461970(*(v22), v1);
            v1 = 0;
            idx->field_4 = 4;
            sub_4067e0(0);
            v23 = idx->field_40;
            if (!v23)
            {
                idx->field_38 = 1;
            }
            else if (v23 <= 1)
            {
                idx->field_38 = v23 - 1;
            }
            else
            {
                idx->field_38 = 1;
            }
        }
        if (!idx->field_1fc)
            goto LABEL_432f38;
        break;
    case 4:
        if (idx->field_14 >= 12)
        {
            v85 = idx->field_38;
            idx->field_4 = 0;
            if (!v85)
            {
                sub_432960();
            }
            else if (v85 == 1)
            {
                sub_461970(idx->field_1ec, v89);
                sub_461970(idx->field_1e8, v90);
                sub_40f720(v91);
                _security_check_cookie();
                return v92;
            }
            else if (v85 == 3)
            {
                sub_461970(idx->field_1ec, v86);
                sub_461970(idx->field_1e8, v87);
                g_4cee40 = (unsigned int)((*((int *)(g_4b44e8 + 116))) + 10);
                _security_check_cookie();
                return v88;
            }
        }
        _security_check_cookie();
        return v93;
    case 5: case 7:
        if (idx->field_14 >= 20)
        {
            if (idx->field_14 == 20)
            {
                v24 = sub_464900();
                idx->field_40 = 2;
                idx->field_108 = 1;
                v25 = v24->field_8;
                if (!v25)
                {
                    v24->field_0 = 1;
                }
                else if (v25 <= 1)
                {
                    v24->field_0 = v25 - 1;
                }
                else
                {
                    v24->field_0 = 1;
                }
                v3 = idx->field_1e8;
                sub_461970(idx->field_1e8, v26);
            }
            if (idx->field_14 >= 30)
            {
                if (idx->field_14 == 30)
                {
                    v3 = idx->field_1e8;
                    sub_461970(idx->field_1e8, v26);
                }
                v27 = &idx->field_38;
                v27->field_4 = idx->field_38;
                if (g_4d48c4 & 16 || g_4d48c0 & 16)
                {
                    v3 = 0xffffffff;
                    sub_464970(-0x1);
                }
                if (g_4d48c4 & 32 || g_4d48c0 & 32)
                {
                    v2 = 1;
                    sub_464970(1);
                }
                if (v27->field_4 != v27->padding_0)
                {
                    v1 = idx->field_1e8;
                    sub_461970(idx->field_1e8, v2);
                    sub_453d90(v3);
                }
                if (*((int *)&g_4d48c4) & 524289)
                {
                    if (v27->padding_0)
                    {
                        if (v27->padding_0 != 1)
                            goto LABEL_432ea8;
                        v28 = sub_462060();
                        sub_461970(*(v28), v2);
                        idx->field_4 = 6;
                    }
                    else
                    {
                        v29 = sub_462060();
                        sub_461970(*(v29), v2);
                        idx->field_4 = (idx->field_4 == 7) * 2 + 6;
                    }
                    sub_453d90();
LABEL_432ea8:
                    v1 = 0;
                    sub_4067e0(0);
                }
                if (*((int *)&g_4d48c4) & 258)
                {
                    sub_453d90();
                    if (!idx->field_38)
                    {
                        v31 = idx->field_40;
                        if (v31)
                        {
                            if (v31 <= 1)
                                idx->field_38 = v31 - 1;
                            else
                                idx->field_38 = 1;
                        }
                        else
                        {
                            idx->field_38 = 1;
                        }
                        sub_461970(idx->field_1e8, v1);
                    }
                    else if (idx->field_38 == 1)
                    {
                        v30 = sub_462060();
                        sub_461970(*(v30), v1);
                        v1 = 0;
                        idx->field_4 = idx->field_38 + 5;
                        sub_4067e0(0);
                    }
                }
LABEL_432f38:
                if (*((int *)&g_4d48c4) & 0x100)
                {
                    v32 = idx->field_40;
                    if (v32)
                    {
                        v33 = _ccall(14, 15, v32, 0, 0);
                        if (v33 & 1)
                            idx->field_38 = v32 - 1;
                        else
                            idx->field_38 = 0;
                    }
                    else
                    {
                        idx->field_38 = 0;
                    }
                    sub_461970(idx->field_1ec, v1);
                    sub_461970(idx->field_1e8, v2);
                    idx->field_4 = 4;
                    sub_4067e0(0);
                    _security_check_cookie();
                    return v34;
                }
            }
        }
        break;
    case 6:
        if (idx->field_14 >= 20)
        {
            if (!idx->field_38)
            {
                sub_461970(idx->field_1e8, v76);
                idx->field_4 = 4;
                sub_464940(v77);
            }
            else if (idx->field_38 == 1)
            {
                sub_464940();
                sub_461970(idx->field_1e8, v74);
                idx->field_4 = 3;
                sub_4067e0(0);
                return v75;
            }
            sub_4067e0(0);
            _security_check_cookie();
            return v78;
        }
        break;
    case 8:
        if (idx->field_14 >= 20)
        {
            idx->field_4 = 9;
            sub_4067e0(0);
            sub_461d80(0);
            v79 = sub_464900();
            idx->field_40 = 25;
            idx->field_108 = 1;
            v80 = v79[2];
            if (v80)
            {
                v81 = _ccall(14, 15, v80, 0, 0);
                if (v81 & 1)
                    *(v79) = v80 - 1;
                else
                    *(v79) = 0;
            }
            else
            {
                *(v79) = 0;
            }
            v82 = 1;
            v83 = &idx->field_204;
            do
            {
                sub_46cbbb(&v8, "th12_%.2d.rpy", v82);
                *((unsigned int *)&v83->padding_0[0]) = sub_43b6f0(&v8);
                v82 += 1;
                v83 = &v83->field_4;
            } while (v82 <= 25);
            _security_check_cookie();
            return v84;
        }
        break;
    case 9:
        if (idx->field_14 >= 10)
        {
            idx1 = &idx->field_38;
            idx1->field_4 = idx->field_38;
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0x1);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(1);
            if (idx1->field_4 != idx1->padding_0)
                sub_453d90();
            if (*((int *)&g_4d48c4) & 524289)
            {
                idx->field_4 = 10;
                sub_4067e0(0);
                v36 = idx->field_118;
                v37 = &idx->field_110;
                if (v36)
                {
                    v38 = _ccall(14, 15, v36, 0, 0);
                    if (v38 & 1)
                        *((unsigned int *)&v37->padding_0[0]) = v36 - 1;
                    else
                        *((unsigned int *)&v37->padding_0[0]) = 0;
                }
                else
                {
                    *((unsigned int *)&v37->padding_0[0]) = 0;
                }
                v39 = idx->field_1f4;
                idx->field_118 = 91;
                idx->field_1e0 = 1;
                if (v39 && !(g_4b0ce0 & 16))
                {
                    sub_46d5b7(&g_4b4518->field_1c->padding_0[12]);
                    g_4b4518->field_1c->field_68 = 8;
                }
                else
                {
                    sub_46d5b7(&g_4b4518->field_1c->padding_0[12]);
                    g_4b4518->field_1c->field_68 = g_4b0cb0;
                }
                iter = &idx->field_2cc;
                v41 = &g_4b451c->field_1e9c0;
                node = v41;
                do
                {
                    v43 = *((char *)node);
                    iter->padding_0[node + -1 * v41] = *((char *)node);
                    node += 1;
                } while (v43);
                idx->field_1f0 = 0;
                v44 = "        ";
                while (1)
                {
                    v45 = iter->padding_0[0];
                    v46 = v45;
                    v47 = *(v44);
                    if (v45 != *(v44))
                        break;
                    if (v45)
                    {
                        v48 = iter->padding_0[1];
                        v46 = v48;
                        v47 = v44[1];
                        if (v48 != v44[1])
                            break;
                        iter = &iter->padding_0[2];
                        v44 += 2;
                        if (!v48)
                            goto LABEL_4330fc;
                    }
                    else
                    {
LABEL_4330fc:
                        v49 = 0;
                        goto LABEL_433105;
                    }
                }
                v50 = _ccall(4, v46, v47, 0);
                v51 = _ccall(12, 0, v50 & 1, v50 & 1);
                v49 = -(v50 & 1) + 1 - (v51 & 1);
LABEL_433105:
                if (v49)
                    sub_464970(-0x1);
                v52 = 8;
                do
                {
                } while (idx->padding_208[195 + v52] == 32 && (v52 = (int)(v52 - 1), v52 > 0));
                idx->field_1f0 = v52;
                sub_453d90();
                _security_check_cookie();
                return v53;
            }
            else if (*((int *)&g_4d48c4) & 258)
            {
                idx->field_4 = 14;
                v6 = &idx->padding_8[8];
                sub_4067e0(0);
                sub_464940(0);
                v54 = idx->field_1e8;
                idx->field_108 = 1;
                idx->field_40 = 4;
                sub_461970(v54, 0);
                iter1 = &idx->field_204;
                v4 = 25;
                do
                {
                    v5 = v4;
                    sub_43b7c0();
                    *((unsigned int *)&iter1->padding_0[0]) = 0;
                    iter1 = &iter1->field_4;
                    v4 = v5 - 1;
                } while (v5 != 1);
                idx->field_4 = 4;
                v56 = *((int *)&idx1->padding_8[0]);
                if (!v56)
                {
                    *((unsigned int *)&idx1->padding_0[0]) = 1;
                }
                else if (v56 <= 1)
                {
                    *((unsigned int *)&idx1->padding_0[0]) = v56 - 1;
                }
                else
                {
                    *((unsigned int *)&idx1->padding_0[0]) = 1;
                }
                sub_4067e0(0);
                sub_453d90(0);
                _security_check_cookie();
                return v57;
            }
        }
        break;
    case 10:
        if (idx->field_14 >= 10)
        {
            v58 = &idx->field_110;
            v58->field_4 = idx->field_110;
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0xd);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(13);
            if (g_4d48c4 & 64 || g_4d48c0 & 64)
            {
                v59 = (unsigned int)(1321528399 * v58->padding_0 >> 0x22);
                v1 = (v58->padding_0 == ((v59 >> 31) + v59) * 13 ? 12 : 0xffffffff);
                sub_464970((v58->padding_0 == ((v59 >> 31) + v59) * 13 ? 12 : 0xffffffff));
            }
            if (g_4d48c4 & 128 || g_4d48c0 & 128)
            {
                v60 = (unsigned int)(1321528399 * v58->padding_0 >> 0x22);
                v0 = (v58->padding_0 - ((v60 >> 31) + v60) * 13 == 12 ? 0xfffffff4 : 1);
                sub_464970((v58->padding_0 - ((v60 >> 31) + v60) * 13 == 12 ? 0xfffffff4 : 1));
            }
            if (v58->field_4 != v58->padding_0)
                sub_453d90();
            if (*((int *)&g_4d48c4) & 524289)
            {
                index = (unsigned int)v58->padding_0;
                if (index < 88)
                {
                    v62 = idx->field_1f0;
                    if (v62 < 8)
                    {
                        (&idx->field_2cc)[v62] = *((char *)(index + 4853384));
                        goto LABEL_433320;
                    }
                    else
                    {
                        idx->padding_208[195 + v62] = *((char *)(index + 4853384));
                        goto LABEL_43333b;
                    }
                }
                if (index == 88)
                {
                    v63 = idx->field_1f0;
                    if (v63 < 8)
                    {
                        (&idx->field_2cc)[v63] = 32;
LABEL_433320:
                        idx->field_1f0 = idx->field_1f0 + 1;
                        if (idx->field_1f0 >= 8)
                            sub_40f790();
                    }
                    else
                    {
                        idx->padding_208[195 + v63] = 32;
                        goto LABEL_43333b;
                    }
                    goto LABEL_43333b;
                }
                else if (index != 89)
                {
                    if (index == 90)
                    {
                        sub_453d90();
                        sub_46cbbb(&v7, "th12_%.2d.rpy", idx->field_38 + 1);
                        sub_43b7c0();
                        v66 = &idx->field_2cc;
                        sub_43bc10(&v7, v66, 1);
                        v67 = sub_43b6f0(&v7);
                        (&idx->field_204)[idx->field_38] = v67;
                        idx->field_4 = 9;
                        sub_4067e0(0);
                        iter2 = v66;
                        do
                        {
                            v69 = iter2->padding_0[0];
                            *(&iter2->padding_0[0] + &g_4b451c->padding_0[0] - &v66->padding_0[0] + 125376) = iter2->padding_0[0];
                            iter2 = &iter2->padding_0[1];
                        } while (v69);
                        _security_check_cookie();
                        return v70;
                    }
LABEL_43333b:
                    sub_453d90();
                    goto LABEL_433345;
                }
                else if (idx->field_1f0)
                {
                    v64 = idx->field_1f0 - 1;
                    idx->field_1f0 = v64;
                    (&idx->field_2cc)[v64] = 32;
                    sub_453d90();
                    _security_check_cookie();
                    return v65;
                }
            }
            else
            {
LABEL_433345:
                if (*((int *)&g_4d48c4) & 258)
                {
                    sub_453d90();
                    if (idx->field_1f0)
                    {
                        v72 = idx->field_1f0 - 1;
                        idx->field_1f0 = v72;
                        (&idx->field_2cc)[v72] = 32;
                        _security_check_cookie();
                        return v73;
                    }
                    idx->field_4 = 9;
                    sub_4067e0(0);
                    _security_check_cookie();
                    return v71;
                }
            }
        }
        break;
    default:
        _security_check_cookie();
        return v93;
    }
}
