// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x433bb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_433bb0_3 {
    char padding_0[1444];
    int field_5a4;
} st_433bb0_3;

typedef struct st_433bb0_0 {
    char padding_0[30];
    char field_1e;
} st_433bb0_0;

typedef struct st_433bb0_1 {
    char padding_0[104];
    unsigned int field_68;
} st_433bb0_1;

typedef struct st_433bb0_2 {
    char padding_0[28];
    struct st_433bb0_1 *field_1c;
} st_433bb0_2;

extern char g_4b0c44;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern char g_4b0ce0;
extern st_433bb0_2 *g_4b4518;
extern unsigned int g_4b451c;
extern unsigned int g_4cee40;
extern char g_4d48c0;
extern void g_4d48c4;

int sub_433bb0(unsigned int a0, unsigned int a1, void* idx)
{
    unsigned int v18;  // 4101
    unsigned int v19;  // eax
    void* v20;  // esi
    unsigned int v21;  // ecx
    int *v22;  // eax
    unsigned int v23;  // eax
    unsigned int *v24;  // eax
    unsigned int v25;  // ecx
    unsigned int v26;  // 4101
    int v27;  // esi
    void* iter;  // edi
    int v30;  // eax
    unsigned int v31;  // eax
    void* v32;  // esi
    unsigned int v33;  // eax
    void* v34;  // edi
    unsigned int v35;  // 4101
    unsigned int v36;  // 4098
    void* node;  // esi
    unsigned int v11;  // eax
    unsigned int v38;  // eax
    unsigned int iter1;  // eax
    char v40;  // 4102
    char v41;  // 4101
    unsigned int v42;  // cc_dep1
    unsigned int v43;  // cc_dep2
    char v44;  // 4103
    unsigned int v45;  // eax
    unsigned int v46;  // 4111
    unsigned int v47;  // 4120
    st_433bb0_3 *v12;  // eax
    int v48;  // eax
    void* iter2;  // esi
    int v50;  // edi
    int v51;  // edi
    unsigned int v52;  // ebx
    unsigned int v53;  // ecx
    unsigned int v54;  // eax
    void* idx1;  // esi
    unsigned int v56;  // edx
    unsigned int v57;  // edx
    int v13;  // eax
    unsigned int v58;  // ecx
    unsigned int idx2;  // eax
    int v60;  // eax
    int v61;  // eax
    unsigned int v62;  // eax
    void* v63;  // esi
    unsigned int v64;  // eax
    void* v65;  // eax
    char v66;  // 4102
    unsigned int v67;  // eax
    unsigned int v14;  // 4101
    unsigned int v68;  // eax
    int v69;  // eax
    unsigned int v70;  // eax
    void* v71;  // esi
    unsigned int v72;  // edx
    unsigned int v73;  // edx
    unsigned int index;  // eax
    int v75;  // eax
    int v76;  // eax
    unsigned int v77;  // eax
    unsigned int v15;  // eax
    void* v78;  // eax
    st_433bb0_0 *v79;  // esi
    void* v80;  // edx
    char v81;  // 4107
    void* v82;  // eax
    char v83;  // 4104
    void* v84;  // esi
    int v85;  // eax
    unsigned int v86;  // 4101
    unsigned int v87;  // eax
    unsigned int v16;  // eax
    unsigned int v88;  // eax
    unsigned int v89;  // eax
    unsigned int v90;  // eax
    unsigned int v91;  // eax
    int v17;  // eax
    unsigned int v0;  // [bp-0x78]
    unsigned int v1;  // [bp-0x74]
    void* v4;  // [bp-0x60], Other Possible Types: unsigned int
    char v5;  // [bp-0x58]
    char v6;  // [bp-0x54]
    char v7;  // [bp-0x48]

    switch ((int)idx[4])
    {
    case 12:
        if ((int)idx[20] < 10)
        {
            _security_check_cookie();
            return v91;
        }
        else if (*((int *)&g_4d48c4) & 524289)
        {
            sub_453d90(a0, 7);
            *((unsigned int *)&idx[4]) = 13;
            sub_461970((int)idx[488]);
            sub_4067e0(0);
            _security_check_cookie();
            return v11;
        }
        else
        {
            _security_check_cookie();
            return v91;
        }
    case 13:
        if ((int)idx[20] >= 10)
        {
            sub_431b30();
            if (g_4b0ce0 & 16)
            {
                v12 = (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + (g_4b0cb0 + g_4b0ca8 * 6) * 8 + 1444;
                if (*((int *)&g_4b0c44) > *((int *)&v12->padding_0[0]))
                    *((int *)&v12->padding_0[0]) = *((int *)&g_4b0c44);
                *((int *)&idx[488]) = *((int *)sub_4357c0(&v7, 111));
                *((unsigned int *)&idx[4]) = 14;
                *((int *)&idx[64]) = 4;
                *((unsigned int *)&idx[264]) = 1;
                v13 = (int)idx[64];
                if (v13)
                {
                    v14 = _ccall(14, 15, v13, 0, 0);
                    if (v14 & 1)
                        *((unsigned int *)&idx[56]) = v13 - 1;
                    else
                        *((unsigned int *)&idx[56]) = 0;
                }
                else
                {
                    *((unsigned int *)&idx[56]) = 0;
                }
                sub_461970((int)idx[488]);
                sub_461970((int)idx[488]);
                _security_check_cookie();
                return v15;
            }
            else
            {
                sub_433a30();
                sub_4067e0(0);
                if (!(int)idx[504])
                {
                    *((unsigned int *)&idx[4]) = 18;
                    sub_461d80();
                    _security_check_cookie();
                    return v16;
                }
                *((int *)&idx[488]) = *((int *)sub_4357c0(&v5, 111));
                *((unsigned int *)&idx[4]) = 14;
                *((int *)&idx[64]) = 4;
                *((unsigned int *)&idx[264]) = 1;
                v17 = (int)idx[64];
                if (v17)
                {
                    v18 = _ccall(14, 15, v17, 0, 0);
                    if (v18 & 1)
                        *((unsigned int *)&idx[56]) = v17 - 1;
                    else
                        *((unsigned int *)&idx[56]) = 0;
                }
                else
                {
                    *((unsigned int *)&idx[56]) = 0;
                }
                sub_461970((int)idx[488]);
                sub_461970((int)idx[488]);
                _security_check_cookie();
                return v19;
            }
        }
        else
        {
            _security_check_cookie();
            return v91;
        }
    case 14:
        v20 = idx + 56;
        *((int *)&v20[4]) = (int)idx[56];
        if (g_4d48c4 & 16 || g_4d48c0 & 16)
            sub_464970(-0x1);
        if (g_4d48c4 & 32 || g_4d48c0 & 32)
            sub_464970(1);
        if ((int)v20[4] != *((int *)v20))
        {
            sub_461970((int)idx[488]);
            sub_453d90(v21, 10);
        }
        if (*((int *)&g_4d48c4) & 524289)
        {
            v22 = sub_462060();
            sub_461970(*(v22));
            sub_453d90(v21, 7);
            *((unsigned int *)&idx[4]) = 19;
            sub_4067e0(0);
            sub_431b30(0);
            v23 = (int)idx[56] - 1;
            if ((int)idx[56] == 1)
            {
LABEL_433ea8:
                *((unsigned int *)&idx[4]) = 19;
                sub_4067e0(0);
                sub_453d90(v21, 7);
            }
            else if (v23 == 1)
            {
                *((unsigned int *)&idx[4]) = 16;
                sub_4067e0(0);
                sub_461d80(0);
                v24 = sub_464900();
                *((int *)&idx[64]) = 25;
                *((unsigned int *)&idx[264]) = 1;
                v25 = v24[2];
                if (v25)
                {
                    v26 = _ccall(14, 15, v25, 0, 0);
                    if (v26 & 1)
                        *(v24) = v25 - 1;
                    else
                        *(v24) = 0;
                }
                else
                {
                    *(v24) = 0;
                }
                v27 = 1;
                iter = idx + 516;
                do
                {
                    sub_46cbbb(&v4, "th12_%.2d.rpy", v27);
                    *((unsigned int *)iter) = sub_43b6f0(&v4);
                    v27 += 1;
                    iter += 4;
                } while (v27 <= 25);
            }
            else if (v23 == 2)
            {
                goto LABEL_433ea8;
            }
        }
        if (!(*((int *)&g_4d48c4) & 258))
        {
            _security_check_cookie();
            return v91;
        }
        sub_453d90(a0, 9);
        if ((int)idx[56] == 1)
        {
            _security_check_cookie();
            return v91;
        }
        v30 = (int)idx[64];
        if (!v30)
        {
            *((unsigned int *)&idx[56]) = 1;
        }
        else if (v30 <= 1)
        {
            *((unsigned int *)&idx[56]) = v30 - 1;
        }
        else
        {
            *((unsigned int *)&idx[56]) = 1;
        }
        sub_461970((int)idx[488]);
        _security_check_cookie();
        return v31;
    case 15:
        _security_check_cookie();
        return v91;
    case 16:
        if ((int)idx[20] >= 10)
        {
            v32 = idx + 56;
            *((int *)&v32[4]) = (int)idx[56];
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0x1);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(1);
            if ((int)v32[4] != *((int *)v32))
                sub_453d90((int)v32[4], 10);
            if (*((int *)&g_4d48c4) & 524289)
            {
                *((unsigned int *)&idx[4]) = 0x11;
                sub_4067e0(0);
                v33 = (int)idx[280];
                v34 = idx + 272;
                if (v33)
                {
                    v35 = _ccall(14, 15, v33, 0, 0);
                    if (v35 & 1)
                        *((unsigned int *)v34) = v33 - 1;
                    else
                        *((unsigned int *)v34) = 0;
                }
                else
                {
                    *((unsigned int *)v34) = 0;
                }
                v36 = (int)idx[500];
                *((unsigned int *)&idx[280]) = 91;
                *((unsigned int *)&idx[480]) = 1;
                if (v36 && !(g_4b0ce0 & 16))
                {
                    sub_46d5b7(&g_4b4518->field_1c->padding_0[12]);
                    g_4b4518->field_1c->field_68 = 8;
                }
                else
                {
                    sub_46d5b7(&g_4b4518->field_1c->padding_0[12]);
                    g_4b4518->field_1c->field_68 = g_4b0cb0;
                }
                node = idx + 716;
                v38 = g_4b451c + 125376;
                iter1 = v38;
                do
                {
                    v40 = *((char *)iter1);
                    *((char *)node - v38 + iter1) = *((char *)iter1);
                    iter1 += 1;
                } while (v40);
                *((int *)&idx[496]) = 0;
                a0 = "        ";
                while (1)
                {
                    v41 = *((char *)node);
                    v42 = v41;
                    v43 = *((char *)a0);
                    if (v41 != *((char *)a0))
                        break;
                    if (v41)
                    {
                        v44 = (char)node[1];
                        v42 = v44;
                        v43 = *((char *)(a0 + 1));
                        if (v44 != *((char *)(a0 + 1)))
                            break;
                        node += 2;
                        a0 += 2;
                        if (!v44)
                            goto LABEL_43411c;
                    }
                    else
                    {
LABEL_43411c:
                        v45 = 0;
                        goto LABEL_434125;
                    }
                }
                v46 = _ccall(4, v42, v43, 0);
                v47 = _ccall(12, 0, v46 & 1, v46 & 1);
                v45 = -(v46 & 1) + 1 - (v47 & 1);
LABEL_434125:
                if (v45)
                    sub_464970(-0x1);
                v48 = 8;
                do
                {
                } while (*(v48 + (char *)idx + 715) == 32 && (v48 = (int)(v48 - 1), v48 > 0));
                *((int *)&idx[496]) = v48;
                break;
            }
            else if (*((int *)&g_4d48c4) & 258)
            {
                *((unsigned int *)&idx[4]) = 14;
                v4 = idx + 16;
                sub_4067e0(0);
                sub_461d40(0);
                sub_464940();
                *((int *)&idx[64]) = 4;
                *((unsigned int *)&idx[264]) = 1;
                sub_461970((int)idx[488]);
                iter2 = idx + 516;
                v50 = 25;
                do
                {
                    v51 = v50;
                    v52 = *((int *)iter2);
                    if (v52)
                    {
                        sub_43b450(v52);
                        sub_46ca4f(v52);
                    }
                } while ((*((unsigned int *)iter2) = 0, iter2 += 4, v50 = (int)(v51 - 1), v51 != 1));
                *((unsigned int *)&idx[4]) = 14;
                sub_4067e0(v50);
                sub_453d90(v53, v50 + 9);
                _security_check_cookie();
                return v54;
            }
            else
            {
                _security_check_cookie();
                return v91;
            }
        }
        else
        {
            _security_check_cookie();
            return v91;
        }
    case 17:
        if ((int)idx[20] >= 10)
        {
            idx1 = idx + 272;
            *((int *)&idx1[4]) = (int)idx[272];
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0xd);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(13);
            if (g_4d48c4 & 64 || g_4d48c0 & 64)
            {
                v56 = 1321528399 * *((int *)idx1) >> 0x22;
                v1 = (*((int *)idx1) == ((v56 >> 31) + v56) * 13 ? 12 : 0xffffffff);
                sub_464970((*((int *)idx1) == ((v56 >> 31) + v56) * 13 ? 12 : 0xffffffff));
            }
            if (g_4d48c4 & 128 || g_4d48c0 & 128)
            {
                v57 = 1321528399 * *((int *)idx1) >> 0x22;
                v0 = (*((int *)idx1) - ((v57 >> 31) + v57) * 13 == 12 ? 0xfffffff4 : 1);
                sub_464970((*((int *)idx1) - ((v57 >> 31) + v57) * 13 == 12 ? 0xfffffff4 : 1));
            }
            v58 = (int)idx1[4];
            if (v58 != *((int *)idx1))
                sub_453d90(v58, 10);
            if (!(*((int *)&g_4d48c4) & 524289))
            {
LABEL_434354:
                if (!(*((int *)&g_4d48c4) & 258))
                {
                    _security_check_cookie();
                    return v91;
                }
                sub_453d90(v58, 9);
                if ((int)idx[496])
                {
                    v69 = (int)idx[496] - 1;
                    *((int *)&idx[496]) = v69;
                    *(v69 + (char *)idx + 716) = 32;
                    _security_check_cookie();
                    return v70;
                }
                *((unsigned int *)&idx[4]) = 16;
                sub_4067e0(0);
                _security_check_cookie();
                return v68;
            }
            idx2 = *((int *)idx1);
            if (idx2 < 88)
            {
                v58 = (int)idx[496];
                if (v58 < 8)
                {
                    *(v58 + (char *)idx + 716) = *((char *)(idx2 + 4853384));
                    goto LABEL_43432f;
                }
                else
                {
                    *(v58 + (char *)idx + 715) = *((char *)(idx2 + 4853384));
                }
            }
            else if (idx2 == 88)
            {
                v60 = (int)idx[496];
                if (v60 < 8)
                {
                    *(v60 + (char *)idx + 716) = 32;
LABEL_43432f:
                    *((unsigned int *)&idx[496]) = (int)idx[496] + 1;
                    if ((int)idx[496] >= 8)
                        sub_40f790();
                }
                else
                {
                    *(v60 + (char *)idx + 715) = 32;
                }
            }
            else if (idx2 == 89)
            {
                if ((int)idx[496])
                {
                    v61 = (int)idx[496] - 1;
                    *((int *)&idx[496]) = v61;
                    *(v61 + (char *)idx + 716) = 32;
                    sub_453d90(v58, 9);
                    _security_check_cookie();
                    return v62;
                }
                _security_check_cookie();
                return v91;
            }
            else
            {
                if (idx2 == 90)
                {
                    sub_453d90(v58, 18);
                    sub_46cbbb(&v6, "th12_%.2d.rpy", (int)idx[56] + 1);
                    sub_43b7c0();
                    v63 = idx + 716;
                    sub_43bc10(0);
                    v64 = sub_43b6f0(&v5);
                    *((unsigned int *)((char *)idx + 4 * idx[56] + 516)) = v64;
                    *((unsigned int *)&idx[4]) = 16;
                    sub_4067e0(0);
                    v65 = v63;
                    do
                    {
                        v66 = *((char *)v65);
                        *((char *)(g_4b451c + 125376 - v63 + v65)) = *((char *)v65);
                        v65 += 1;
                    } while (v66);
                    _security_check_cookie();
                    return v67;
                }
            }
            sub_453d90(v58, 7);
            goto LABEL_434354;
        }
        else
        {
            _security_check_cookie();
            return v91;
        }
    case 18:
        if ((int)idx[20] < 10)
        {
            _security_check_cookie();
            return v91;
        }
        if (!(int)idx[504])
        {
            v71 = idx + 272;
            *((int *)&v71[4]) = (int)idx[272];
            if (g_4d48c4 & 16 || g_4d48c0 & 16)
                sub_464970(-0xd);
            if (g_4d48c4 & 32 || g_4d48c0 & 32)
                sub_464970(13);
            if (g_4d48c4 & 64 || g_4d48c0 & 64)
            {
                v72 = 1321528399 * *((int *)v71) >> 0x22;
                v1 = (*((int *)v71) == ((v72 >> 31) + v72) * 13 ? 12 : 0xffffffff);
                sub_464970((*((int *)v71) == ((v72 >> 31) + v72) * 13 ? 12 : 0xffffffff));
            }
            if (g_4d48c4 & 128 || g_4d48c0 & 128)
            {
                v73 = 1321528399 * *((int *)v71) >> 0x22;
                v0 = (*((int *)v71) - ((v73 >> 31) + v73) * 13 == 12 ? 0xfffffff4 : 1);
                sub_464970((*((int *)v71) - ((v73 >> 31) + v73) * 13 == 12 ? 0xfffffff4 : 1));
            }
            a0 = (int)v71[4];
            if (a0 != *((int *)v71))
                sub_453d90(a0, 10);
        }
        if (*((int *)&g_4d48c4) & 524289)
        {
            if ((int)idx[504])
                goto LABEL_4346eb;
            index = (int)idx[272];
            if (index < 88)
            {
                a0 = (int)idx[496];
                if (a0 < 8)
                {
                    *(a0 + (char *)idx + 716) = *((char *)(index + 4853384));
                    goto LABEL_4345ff;
                }
                else
                {
                    *(a0 + (char *)idx + 715) = *((char *)(index + 4853384));
                    break;
                }
            }
            else if (index == 88)
            {
                v75 = (int)idx[496];
                if (v75 < 8)
                {
                    *(v75 + (char *)idx + 716) = 32;
LABEL_4345ff:
                    *((unsigned int *)&idx[496]) = (int)idx[496] + 1;
                    if ((int)idx[496] >= 8)
                    {
                        sub_40f790();
                        break;
                    }
                }
                else
                {
                    *(v75 + (char *)idx + 715) = 32;
                    break;
                }
            }
            else if (index == 89)
            {
                if ((int)idx[496])
                {
                    v76 = (int)idx[496] - 1;
                    *((int *)&idx[496]) = v76;
                    *(v76 + (char *)idx + 716) = 32;
                    break;
                }
                else
                {
                    _security_check_cookie();
                    return v91;
                }
            }
            else
            {
                if (index == 90)
                {
                    v78 = idx + 716;
                    v79 = (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + ((int)idx[56] + g_4b0ca8 * 10) * 28 + 30;
                    v80 = v78;
                    do
                    {
                        v81 = *((char *)v80);
                        v79->padding_0[0] = *((char *)v80);
                        v80 += 1;
                        v79 = &v79->padding_0[1];
                    } while (v81);
                    v82 = v78;
                    do
                    {
                        v83 = *((char *)v82);
                        *((char *)(g_4b451c + 125376 - v78 + v82)) = *((char *)v82);
                        v82 += 1;
                    } while (v83);
                    goto LABEL_4346eb;
                }
            }
        }
        else
        {
            if (!(*((int *)&g_4d48c4) & 258))
            {
                _security_check_cookie();
                return v91;
            }
            else if ((int)idx[56] >= 0)
            {
                if ((int)idx[496])
                {
                    sub_453d90(a0, 9);
                    *((unsigned int *)&idx[496]) = (int)idx[496] - 1;
                    *(idx[496] + (char *)idx + 716) = 32;
                    _security_check_cookie();
                    return v88;
                }
                _security_check_cookie();
                return v91;
            }
LABEL_4346eb:
            v84 = idx + 488;
            *((int *)v84) = *((int *)sub_4357c0(&ebp, 111));
            sub_461d40(&ebp, 111);
            *((unsigned int *)&idx[4]) = 14;
            *((int *)&idx[64]) = 4;
            *((unsigned int *)&idx[264]) = 1;
            v85 = (int)idx[64];
            if (v85)
            {
                v86 = _ccall(14, 15, v85, 0, 0);
                if (v86 & 1)
                    *((unsigned int *)&idx[56]) = v85 - 1;
                else
                    *((unsigned int *)&idx[56]) = 0;
            }
            else
            {
                *((unsigned int *)&idx[56]) = 0;
            }
            sub_461970(*((int *)v84));
            sub_461970(*((int *)v84));
            _security_check_cookie();
            return v87;
        }
        sub_453d90(a0, 7);
        _security_check_cookie();
        return v77;
    case 19:
        if ((int)idx[20] >= 12)
        {
            sub_4339c0();
            *((unsigned int *)&idx[4]) = 27;
            switch ((int)idx[56])
            {
            case 0:
                g_4cee40 = (!(int)idx[500]) * 4 + 10;
                return v89;
            case 1: case 2:
                sub_40f720();
                return v90;
            case 3:
                g_4cee40 = 10;
                _security_check_cookie();
                return v91;
            default:
                _security_check_cookie();
                return v91;
            }
        }
        else
        {
            _security_check_cookie();
            return v91;
        }
    default:
        _security_check_cookie();
        return v91;
    }
}
