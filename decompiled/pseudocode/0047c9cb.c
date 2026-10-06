// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c9cb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47c9cb_0 {
    char padding_0[12];
    char field_c;
} st_47c9cb_0;

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_47c9cb_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[20];
    char field_24;
} st_47c9cb_1;

typedef struct st_47c9cb_2 {
    char padding_0[112];
    unsigned int field_70;
} st_47c9cb_2;

extern char g_49d620;
extern char g_49d640;
extern void* g_4ad3d8;
extern void* g_4ad3dc;
extern st_47c9cb_1 g_4adbb0;
extern int g_4ade88;
extern int g_4ade90;
extern int g_4ade94;
extern st_47c9cb_1 g_4d6320;

int sub_47c9cb(void)
{
    unsigned int v42;  // ebx
    char *v43;  // ebx
    char v52;  // dl
    char *v53;  // ebx
    unsigned int v54;  // eax
    char *v55;  // eax
    unsigned int v56;  // eax
    unsigned int v57;  // eax
    unsigned int v58;  // eax
    char v59;  // al
    char *v60;  // eax
    unsigned int v61;  // eax
    unsigned int v44;  // esi
    unsigned int v62;  // eax
    unsigned int v64;  // eax
    unsigned int v65;  // eax
    void* v66;  // eax
    void* v67;  // eax
    void* v68;  // esi
    void* v69;  // eax
    void* v70;  // edi
    unsigned int v45;  // edi
    void* iter;  // ecx
    void* iter1;  // eax
    void* iter2;  // eax
    unsigned int v77;  // eax
    void* v78;  // edi
    void* v79;  // eax
    void* v80;  // edx
    void* v46;  // edi
    void* v82;  // edi
    void* v83;  // eax
    void* v84;  // esi
    int v86;  // ecx
    int v87;  // ecx
    int v88;  // ebx
    void* v89;  // esi
    void* v47;  // edx
    unsigned int v48;  // eax
    st_47c9cb_1 *v49;  // ecx
    st_47c9cb_1 *v50;  // eax
    char v51;  // 4105
    int v0;  // [bp-0x2ac]
    int v1;  // [bp-0x2a8]
    int v2;  // [bp-0x2a4]
    int v3;  // [bp-0x2a0]
    void* v4;  // [bp-0x29c], Other Possible Types: int, unsigned int
    unsigned int v5;  // [bp-0x288]
    unsigned int v6;  // [bp-0x284]
    unsigned int v7;  // [bp-0x280]
    void* v8;  // [bp-0x27c]
    void* v9;  // [bp-0x278]
    char v10;  // [bp-0x274], Other Possible Types: unsigned int
    int v11;  // [bp-0x270]
    void* v12;  // [bp-0x268]
    st_47c9cb_0 *v13;  // [bp-0x264]
    unsigned int v14;  // [bp-0x260]
    unsigned int v15;  // [bp-0x25c]
    void* v16;  // [bp-0x258]
    st_46d3b4_0 v17;  // [bp-0x254]
    st_47c9cb_2 *idx;  // [bp-0x24c]
    char v19;  // [bp-0x248]
    unsigned int v20;  // [bp-0x244]
    char *v21;  // [bp-0x240]
    unsigned int v22;  // [bp-0x23c]
    void* v23;  // [bp-0x238]
    unsigned int v24;  // [bp-0x234]
    char v25;  // [bp-0x230]
    char v26;  // [bp-0x22f]
    int v27;  // [bp-0x22c]
    void* v28;  // [bp-0x224], Other Possible Types: int
    void* node;  // [bp-0x220]
    void* v30;  // [bp-0x21c]
    void* v31;  // [bp-0x21c]
    char v32;  // [bp-0x215]
    unsigned int v33;  // [bp-0x214]
    unsigned int v34;  // [bp-0x210]
    unsigned int v35;  // [bp-0x210]
    char v36;  // [bp-0x12]
    st_47c9cb_0 *v38;  // [bp+0x4]
    char *v39;  // [bp+0x8]
    unsigned int *v40;  // [bp+0xc]
    void* v41;  // [bp+0x10]

    v7 = v42;
    v43 = v39;
    v6 = v44;
    v5 = v45;
    v46 = v41;
    v13 = v38;
    v14 = 0;
    v33 = 0;
    v23 = NULL;
    v30 = NULL;
    v24 = 0;
    v15 = 0;
    v22 = 0;
    sub_46d3b4(&v17, v47, v40);
    if (v13)
    {
        if (!(v13->field_c & 64))
        {
            v48 = sub_47c1da(v13);
            if (v48 != 0xffffffff && v48 != 0xfffffffe)
                v49 = (v48 & 31) * 64 + (&g_4d6320.field_0)[(int)(v48) >> 5];
            else
                v49 = &g_4adbb0.field_0;
            if (!(v49->field_24 & 127))
            {
                if (v48 != 0xffffffff && v48 != 0xfffffffe)
                    v50 = (v48 & 31) * 64 + (&g_4d6320.field_0)[(int)(v48) >> 5];
                else
                    v50 = &g_4adbb0.field_0;
                if (!(v50->field_24 & 128))
                    goto LABEL_47cacf;
            }
        }
        else
        {
LABEL_47cacf:
            if (v43)
            {
                v51 = *(v43);
                v52 = v51;
                v27 = 0;
                v28 = NULL;
                v20 = 0;
                v16 = NULL;
                v32 = v51;
                if (!v51)
                {
LABEL_47d551:
                    if (!v19)
                        return;
                    idx->field_70 = idx->field_70 & 0xfffffffd;
                    return;
                }
                while (1)
                {
                    v53 = v43 + 1;
                    v54 = 0;
                    v21 = v53;
                    if (v27 >= NULL)
                    {
                        if (v52 - 32 <= 88)
                            v54 = *(&(&g_49d620)[v52]) & 15;
                        v4 = 8;
                        v20 = (&g_49d640)[9 * v54 + v20] >> 4;
                        if (v20 == 8)
                            break;
                        v4 = 7;
                        switch (v20)
                        {
                        case 1:
                            v30 = 0xffffffff;
                            v11 = 0;
                            v15 = 0;
                            v23 = NULL;
                            v24 = 0;
                            v33 = 0;
                            v22 = 0;
                            goto LABEL_47d50a;
                        case 2:
                            v55 = v52;
                            v56 = v55 - 32;
                            if (v55 != 0x20)
                            {
                                v57 = v56 - 3;
                                if (v56 == 3)
                                {
                                    v33 |= 128;
                                    goto LABEL_47d50a;
                                }
                                else if (v57 != 8)
                                {
                                    v58 = v57 - 9;
                                    if (v58 == 1)
                                    {
                                        v33 |= 4;
                                        goto LABEL_47d50a;
                                    }
                                    else if (v58 == 4)
                                    {
                                        v33 |= 8;
                                        goto LABEL_47d50a;
                                    }
                                }
                                else
                                {
                                    v33 |= 1;
                                    goto LABEL_47d50a;
                                }
                            }
                            else
                            {
                                v33 |= 2;
                                goto LABEL_47d50a;
                            }
                        case 3:
                            if (v52 == 42)
                            {
                                v46 += 4;
                                v23 = *((int *)((char *)v46 - 4));
                                if (*((int *)((char *)v46 - 4)) < NULL)
                                {
                                    v33 |= 4;
                                    v23 = -(v23);
                                    goto LABEL_47d50a;
                                }
                            }
                            else
                            {
                                v23 = v23 * 10 + v52 - 48;
                                goto LABEL_47d50a;
                            }
                        case 4:
                            v30 = NULL;
                            goto LABEL_47d50a;
                        case 5:
                            if (v52 == 42)
                            {
                                v46 += 4;
                                v30 = *((int *)((char *)v46 - 4));
                                if (*((int *)((char *)v46 - 4)) < NULL)
                                {
                                    v30 = 0xffffffff;
                                    goto LABEL_47d50a;
                                }
                            }
                            else
                            {
                                v30 = v30 * 10 + v52 - 48;
                                goto LABEL_47d50a;
                            }
                        case 7:
                            v60 = v52;
                            if (v60 > 0x64)
                            {
                                if (v60 > 0x70)
                                {
                                    if (v60 != 0x73)
                                    {
                                        v77 = v60 - 116;
                                        if (v77 != 1)
                                        {
                                            if (v77 != 4)
                                                break;
                                            v14 = 39;
LABEL_47d1d8:
                                            v28 = 16;
                                            if ((char)v33 & 128)
                                            {
                                                v25 = 48;
                                                v26 = (char)v14 + 81;
                                                v24 = 2;
                                            }
LABEL_47d05a:
                                            if (v33 & 0x8000 || v33 & 0x1000)
                                            {
                                                v78 = v46 + 8;
                                                v79 = *((int *)((char *)v78 - 8));
                                                v80 = *((int *)((char *)v78 - 4));
                                            }
                                            else
                                            {
                                                v78 = v46 + 4;
                                                if ((char)v33 & 32)
                                                {
                                                    v46 = v78;
                                                    v79 = (!((char)v33 & 64) ? *((short *)((char *)v78 - 4)) : *((short *)((char *)v78 - 4)));
                                                    v80 = v79 >> 31;
                                                    goto LABEL_47d252;
                                                }
                                                else
                                                {
                                                    v79 = *((int *)((char *)v78 - 4));
                                                    v80 = (!((char)v33 & 64) ? NULL : v79 >> 31);
                                                }
                                            }
                                            v46 = v78;
LABEL_47d252:
                                            if ((char)v33 & 64 && v80 <= NULL && (v80 < NULL || v79 < NULL))
                                            {
                                                v80 = -(v80 + (NULL < v79));
                                                v33 |= 0x100;
                                                v79 = -(v79);
                                            }
                                            v82 = v79;
                                            if (!(v33 & 0x9000))
                                                v80 = NULL;
                                            if (v30 < NULL)
                                            {
                                                v30 = 0x1;
                                            }
                                            else
                                            {
                                                v33 &= 0xfffffff7;
                                                if (v30 > 0x200)
                                                    v30 = 0x200;
                                            }
                                            v83 = v82;
                                            if (!v83 && !v80)
                                                v24 &= v83 | v80;
                                            v84 = &v36 + 1;
                                            while (1)
                                            {
                                                v31 = v30 - 1;
                                                if (v30 <= NULL && !v82 && !v80)
                                                    break;
                                                v87 = v86 + 48;
                                                v12 = v80;
                                                v82 = sub_47c890(v82, v80, v28, v28 >> 31);
                                                v80 = v47;
                                                if (v87 > 57)
                                                    v87 += v14;
                                                *((char *)v84) = v87;
                                                v84 -= 1;
                                                v30 = v31;
                                            }
                                            v28 = &v36 + 1 - v84;
                                            node = v84 + 1;
                                            v30 = v31;
                                            if (!(v33 & 0x200) || v28 && !(v30 = v31, *((char *)node) != 48))
                                                break;
                                            node -= 1;
                                            *((char *)node) = 48;
                                            v67 = v28 + 1;
                                            v30 = v31;
LABEL_47d37f:
                                            v28 = v67;
                                            if (v15)
                                            {
LABEL_47d4ee:
                                                if (v16)
                                                {
                                                    sub_46c941(v16);
                                                    v16 = NULL;
                                                    goto LABEL_47d50a;
                                                }
                                            }
                                            if ((char)v33 & 64)
                                            {
                                                if (v33 & 0x100)
                                                {
                                                    v25 = 45;
                                                }
                                                else if ((char)v33 & 1)
                                                {
                                                    v25 = 43;
                                                }
                                                else
                                                {
                                                    if (!((char)v33 & 2))
                                                        goto LABEL_47d3ce;
                                                    v25 = 32;
                                                }
                                                v24 = 1;
                                            }
LABEL_47d3ce:
                                            v88 = v23 - v28 - v24;
                                            if (!((char)v33 & 12))
                                                sub_47c958(32, v88, v13);
                                            sub_47c97e(v24);
                                            if ((char)v33 & 8 && !((char)v33 & 4))
                                                sub_47c958(48, v88, v13);
                                            if (v22 && v28 > NULL)
                                            {
                                                v12 = v28;
                                                do
                                                {
                                                    v12 -= 1;
                                                    v89 = node + 2;
                                                    if (sub_47c503(&v10, &v36 + 2, 6, *((short *)node)) || !v10)
                                                    {
                                                        v27 = -0x1;
                                                        break;
                                                    }
                                                } while ((sub_47c97e(v10), node = v89, v12));
                                            }
                                            else
                                            {
                                                sub_47c97e(v28);
                                            }
                                            if (v27 >= NULL && (char)v33 & 4)
                                            {
                                                sub_47c958(32, v88, v13);
                                                goto LABEL_47d4ee;
                                            }
                                        }
LABEL_47d050:
                                        v28 = 10;
                                        goto LABEL_47d05a;
                                    }
LABEL_47ce78:
                                    iter = v30;
                                    if (iter == 0xffffffff)
                                        iter = 0x7fffffff;
                                    v46 += 4;
                                    node = *((int *)((char *)v46 - 4));
                                    if (v33 & 2064)
                                    {
                                        if (!node)
                                            node = g_4ad3dc;
                                        v22 = 1;
                                        for (iter1 = node; iter && (iter -= 1, *((short *)iter1)); iter1 += 2);
                                        v67 = (int)(iter1 - node) >> 1;
                                        goto LABEL_47d37f;
                                    }
                                    else
                                    {
                                        if (!node)
                                            node = g_4ad3d8;
                                        for (iter2 = node; iter && (iter -= 1, *((char *)iter2)); iter2 += 1);
                                        v67 = iter2 - node;
                                        goto LABEL_47d37f;
                                    }
                                }
                                if (v60 == 0x70)
                                {
                                    v30 = 0x8;
LABEL_47d1ac:
                                    v14 = 7;
                                    goto LABEL_47d1d8;
                                }
                                if (v60 < 0x65)
                                    break;
                                if (v60 > 0x67)
                                {
                                    if (v60 == 0x69)
                                    {
LABEL_47d049:
                                        v33 |= 64;
                                        goto LABEL_47d050;
                                    }
                                    if (v60 != 0x6e)
                                    {
                                        if (v60 != 0x6f)
                                            break;
                                        v28 = 8;
                                        if ((char)v33 & 128)
                                        {
                                            v33 |= 0x200;
                                            goto LABEL_47d05a;
                                        }
                                    }
                                    v46 += 4;
                                    if (!sub_47c381())
                                        goto LABEL_47ca36;
                                    if ((char)v33 & 32)
                                        *((unsigned short *)*((int *)v46)) = v27;
                                    else
                                        *((int *)*((int *)v46)) = v27;
                                    v15 = 1;
                                    goto LABEL_47d4ee;
                                }
LABEL_47ce09:
                                v33 |= 64;
                                v4 = &v34;
                                node = &v34;
                                v12 = 0x200;
                                if (v30 < NULL)
                                {
                                    v30 = 0x6;
                                }
                                else if (v30)
                                {
                                    if (v30 > 0x200)
                                        v30 = 0x200;
                                    if (v30 > 0xa3)
                                    {
                                        v68 = v30 + 349;
                                        v69 = sub_4777ce(v68);
                                        v52 = v32;
                                        v16 = v69;
                                        v4 = 8;
                                        if (v69)
                                        {
                                            node = v69;
                                            v12 = v68;
                                            v4 = v69;
                                        }
                                        else
                                        {
                                            v30 = 0xa3;
                                        }
                                    }
                                }
                                else if (v52 == 103)
                                {
                                    v30 = 0x1;
                                }
                                v70 = v46 + 8;
                                v8 = *((int *)((char *)v70 - 8));
                                v9 = *((int *)((char *)v70 - 4));
                                v46 = v70;
                                sub_4751de(g_4ade88, &v8, v4, v12, v52, v30, v11, &v17)(&v8, v4, v12, v52, v30, v11, &v17);
                                if (v33 & 128 && !v30)
                                    sub_4751de(g_4ade94, v4, &v17)(v4, &v17);
                                if (v32 == 103 && !(v33 & 128))
                                    sub_4751de(g_4ade90, v4, &v17)(v4, &v17);
                                if (*((char *)v4) == 45)
                                {
                                    v33 |= 0x100;
                                    v4 += 1;
                                    node = v4;
                                }
                                goto LABEL_47cfab;
                            }
                            if (v60 == 0x64)
                                goto LABEL_47d049;
                            if (v60 <= 0x53)
                            {
                                if (v60 == 0x53 && !(v33 & 2096))
                                {
                                    v33 |= 0x800;
                                    goto LABEL_47ce78;
                                }
                                if (v60 != 0x41)
                                {
                                    v61 = v60 - 66;
                                    if (v61 != 1)
                                    {
                                        v62 = v61 - 2;
                                        if (v62 != 1 && v62 != 3)
                                            break;
                                    }
                                    else if (!(v33 & 2096))
                                    {
                                        v33 |= 0x800;
                                        goto LABEL_47ceeb;
                                    }
                                }
                                v11 = 1;
                                v32 = v52 + 32;
                                v52 += 32;
                                goto LABEL_47ce09;
                            }
                            else
                            {
                                if (v60 == 0x58)
                                    goto LABEL_47d1ac;
                                v64 = v60 - 89;
                                v65 = v64 - 1;
                                if (v64 == 1)
                                {
                                    v66 = *((int *)v46);
                                    v46 += 4;
                                    if (v66 && (int)v66[4])
                                    {
                                        v67 = *((short *)v66);
                                        node = (int)v66[4];
                                        if (v33 & 0x800)
                                        {
                                            v67 = &v67;
                                            v22 = 1;
                                            goto LABEL_47d37f;
                                        }
                                        else
                                        {
                                            v22 = 0;
                                            goto LABEL_47d37f;
                                        }
                                    }
                                    else
                                    {
                                        node = g_4ad3d8;
                                        v4 = g_4ad3d8;
                                    }
LABEL_47cfab:
                                    v67 = sub_475860(v4);
                                    goto LABEL_47d37f;
                                }
                                if (v65 == 7)
                                    goto LABEL_47ce09;
                                if (v65 != 9)
                                    break;
LABEL_47ceeb:
                                v46 += 4;
                                if (v33 & 2064)
                                {
                                    v35 = v34;
                                    if (sub_47c503(&v28, &v35, 0x200, *((short *)((char *)v46 - 4))))
                                    {
                                        v15 = 1;
                                        v35 = v34;
                                    }
                                }
                                else
                                {
                                    v35 = _INSERT(v34, 0, *((char *)v46 - 4));
                                    v28 = 0x1;
                                }
                                node = &v35;
                                break;
                            }
                        case 6:
                            if (v52 != 73)
                            {
                                if (v52 == 104)
                                {
                                    v33 |= 32;
                                    goto LABEL_47d50a;
                                }
                                else if (v52 != 108)
                                {
                                    if (v52 == 0x77)
                                    {
                                        v33 |= 0x800;
                                        goto LABEL_47d50a;
                                    }
                                }
                                else
                                {
                                    if (*(v53) == 108)
                                    {
                                        v33 |= 0x1000;
                                        v21 = v53 + 1;
                                        goto LABEL_47d50a;
                                    }
                                    else
                                    {
                                        v33 |= 16;
                                        goto LABEL_47d50a;
                                    }
                                }
                            }
                            v59 = *(v53);
                            if (*(v53) == 54 && v53[1] == 52)
                            {
                                v33 |= 0x8000;
                                v21 = v53 + 2;
                                goto LABEL_47d50a;
                            }
                            if (v59 == 0x33 && v53[1] == 50)
                            {
                                v33 &= 0xffff7fff;
                                v21 = v53 + 2;
                                goto LABEL_47d50a;
                            }
                            if (v59 == 100 || v59 == 105 || v59 == 111 || v59 == 117 || v59 == 120 || v59 == 88)
                                goto LABEL_47d50a;
                            v20 = 0;
                        case 0:
                            v22 = 0;
                            if (sub_47c5a3(v52, &v17) && (sub_47c925(), v21 = v53 + 1, !*(v53)))
                                goto LABEL_47ca36;
                            sub_47c925();
                            goto LABEL_47d50a;
                        default:
LABEL_47d50a:
                            v43 = v21;
                            v52 = *(v43);
                            v32 = v52;
                            if (!v52)
                                goto LABEL_47d529;
                            v46 = v46;
                            continue;
                        }
                    }
                    else
                    {
LABEL_47d529:
                        if (v20 && v20 != 7)
                        {
                            v4 = 0;
                            v3 = 0;
                            v2 = 0;
                            v1 = 0;
                            *((unsigned int *)sub_46e96f()) = 22;
                            v0 = 0;
                            goto LABEL_47ca48;
                        }
                        goto LABEL_47d551;
                    }
                }
            }
        }
    }
LABEL_47ca36:
    *((unsigned int *)sub_46e96f()) = 22;
    v4 = 0;
    v3 = 0;
    v2 = 0;
    v1 = 0;
    v0 = 0;
LABEL_47ca48:
    sub_470f2d(v0, v1, v2, v3, v4);
    if (v19)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return;
}
