// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x439b10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_439b10_2 {
    char padding_0[2652];
    int field_a5c;
    char padding_a60[16];
    unsigned int field_a70;
    char padding_a74[12];
    unsigned int field_a80;
    char padding_a84[4];
    unsigned int field_a88;
    unsigned int field_a8c;
    char padding_a90[12];
    char field_a9c;
    char padding_a9d[3];
    unsigned int field_aa0;
    unsigned int field_aa4;
    int field_aa8;
    char padding_aac[4];
    unsigned int field_ab0;
    char padding_ab4[24];
    struct st_439b10_0 *field_acc;
} st_439b10_2;

typedef struct st_439b10_1 {
    unsigned int field_0;
} st_439b10_1;

typedef struct st_439b10_0 {
    char padding_0[29];
    char field_1d;
    char padding_1e[10];
    struct st_439b10_1 *field_28;
} st_439b10_0;

typedef struct st_439b10_4 {
    struct st_439b10_3 *field_0;
    struct st_439b10_4 *field_4;
} st_439b10_4;

typedef struct st_439b10_5 {
    unsigned int field_0;
    struct st_439b10_5 *field_4;
} st_439b10_5;

typedef struct st_439b10_8 {
    char padding_0[20];
    struct st_439b10_4 *field_14;
    unsigned int field_18;
    char padding_1c[1120];
    unsigned int field_47c;
} st_439b10_8;

typedef struct st_439b10_3 {
    char padding_0[44];
    unsigned int field_2c;
    char padding_30[1024];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[64];
    unsigned int field_47c;
} st_439b10_3;

extern unsigned int g_4ce8cc;

unsigned int sub_439b10(unsigned int a0, unsigned int a1, st_439b10_2 *a2)
{
    unsigned int v10;  // ebx
    unsigned int v11;  // esi
    unsigned int v18;  // ftop
    unsigned int v19;  // ecx
    st_439b10_4 *node;  // eax
    unsigned int index;  // esi
    st_439b10_5 *iter;  // eax
    unsigned int v23;  // ftop
    unsigned int v25;  // 4159
    unsigned int v26;  // ftop
    unsigned int v12;  // edi
    unsigned int v28;  // ftop
    unsigned int v29;  // 4168
    unsigned int v30;  // ftop
    unsigned int v32;  // 4147
    unsigned int v34;  // ftop
    unsigned int v36;  // 4147
    st_439b10_2 *iter1;  // edi
    unsigned int v38;  // ftop
    unsigned int v40;  // 4147
    unsigned int v41;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // 4156
    unsigned int v45;  // ftop
    unsigned int v47;  // 4147
    st_439b10_0 *v14;  // eax
    unsigned int v49;  // ftop
    unsigned int v51;  // 4147
    unsigned int v53;  // ftop
    unsigned int v55;  // ftop
    unsigned int v56;  // 4159
    unsigned int v15;  // ecx
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v62;  // 4159
    unsigned short v64;  // ftop
    unsigned short v66;  // ftop
    unsigned int v67;  // 4143
    unsigned int v68;  // fc3210
    unsigned short v69;  // ftop
    unsigned int v70;  // fc3210
    unsigned int *idx;  // eax
    unsigned int v72;  // eax
    st_439b10_8 *idx1;  // eax
    st_439b10_4 *v74;  // eax
    st_439b10_4 *v75;  // eax
    unsigned int v16;  // ftop
    unsigned int v17;  // ecx
    unsigned int v0;  // [bp-0x64]
    unsigned int v1;  // [bp-0x60]
    unsigned int v2;  // [bp-0x50]
    unsigned int v3;  // [bp-0x4c]
    unsigned int v4;  // [bp-0x48]
    unsigned int v5;  // [bp-0x44]
    unsigned int v6;  // [bp-0x44]
    unsigned int v7;  // [bp-0x40]
    st_439b10_2 *v8;  // [bp-0x3c]
    unsigned int v9;  // [bp-0x38]

    v5 = v10;
    v3 = v11;
    v2 = v12;
    v8 = &a2->padding_0[2648];
    iter1 = &v8->padding_0[32];
    v9 = 0x100;
    do
    {
        v6 = v5;
        if (!*((int *)&iter1->padding_0[40]))
            continue;
        v14 = *((int *)&iter1->padding_0[84]);
        *((unsigned int *)&iter1->padding_0[56]) = 0;
        v15 = a0;
        if (v14->field_28)
            v14->field_28();
        if (!(iter1->padding_0[36] & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_465390((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter1->padding_0[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v16 = v18;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = v15;
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter1->padding_0[20]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v0 = v17;
            /* unsupported instruction */
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            *((int *)&iter1->padding_0[16]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v16 = v18 + 2;
        }
        sub_464db0();
        v19 = *((int *)&iter1->padding_0[44]);
        if (v19)
        {
            node = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                v19 = v19;
                do
                {
                    index = node->field_0;
                    if (*((int *)index) == v19)
                        goto LABEL_439be6;
                } while ((node = (st_439b10_4 *)node->field_4, node));
            }
            iter = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_439bea;
            while (1)
            {
                index = iter->field_0;
                if (*((int *)index) == v19)
                    break;
                iter = iter->field_4;
                if (!iter)
                    goto LABEL_439bea;
            }
LABEL_439be6:
            if (!index)
                goto LABEL_439bea;
            if (*((char *)(*((int *)&iter1->padding_0[84]) + 29)) == 2)
            {
LABEL_439cfd:
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
                *((int *)(index + 1072)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                *((int *)(index + 1076)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)(index + 1080)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if (*((int *)&iter1->padding_0[48]))
                {
                    idx = sub_461920(*((int *)&iter1->padding_0[48]));
                    if (!idx)
                    {
                        *((int *)&iter1->padding_0[48]) = 0;
                    }
                    else
                    {
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
                        /* unsupported instruction */
                        idx[268] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx[269] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx[270] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
LABEL_439d52:
                        v72 = *((int *)(index + 1148));
                        if (*((int *)(index + 1148)) & 0x20000000)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            *((int *)(index + 44)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                            /* unsupported instruction */
                            *((unsigned int *)(index + 1148)) = v72 | 4;
                        }
                        sub_464a80();
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                goto LABEL_439d52;
            }
            sub_45cdf0();
            if (*((int *)(&iter1->padding_0[0] - 28)) < 10)
            {
LABEL_439cf7:
                goto LABEL_439cfd;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v23 = v16 - 2;
            /* unsupported instruction */
            /* unsupported instruction */
            v25 = _ccall(10, 13, (char)((((unsigned short)v23 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (v25 & 1)
            {
                v26 = v23 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v28 = v26 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v29 = _ccall(10, 13, (char)((((unsigned short)v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v29 & 1)
                {
                    v30 = v28 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v32 = _ccall(10, 13, (char)((((unsigned short)v30 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                    if (v32 & 1)
                    {
                        /* unsupported instruction */
                        v28 = v30 + 1;
                        if ((((unsigned short)v28 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407d000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            goto LABEL_439cf5;
                        }
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v28 = v30 + 1;
                    }
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v28 = v23 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v34 = v28 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v36 = _ccall(10, 13, (char)((((unsigned short)v34 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (v36 & 1)
            {
                /* unsupported instruction */
                v38 = v34 + 1;
                if (!((char)((((unsigned short)v38 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                    goto LABEL_439da5;
                v34 = v38 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v40 = _ccall(10, 13, (char)((((unsigned short)v34 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v40 & 1)
                {
                    v41 = v34 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v43 = v41 + 1;
                    v44 = _ccall(10, 13, (char)((((unsigned short)v41 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                    if (v44 & 1)
                        goto LABEL_439cef;
                    goto LABEL_439dab;
                }
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v38 = v34 + 1;
LABEL_439da5:
            v43 = v38 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_439dab:
            v45 = v43 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            v47 = _ccall(10, 13, (char)((((unsigned short)v45 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (v47 & 1)
            {
                /* unsupported instruction */
                v49 = v45 + 1;
                if (!((char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                    goto LABEL_439ddf;
                v45 = v49 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v51 = _ccall(10, 13, (char)((((unsigned short)v45 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (!(v51 & 1))
                    goto LABEL_439ddd;
                /* unsupported instruction */
                v49 = v45 + 1;
                if (!((char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                    goto LABEL_439ddf;
            }
            else
            {
LABEL_439ddd:
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v49 = v45 + 1;
LABEL_439ddf:
                v53 = v49 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v55 = v53 + 1;
                v56 = _ccall(10, 13, (char)((((unsigned short)v53 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v56 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v58 = v55 + 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v59 = v58 + 1;
                    if ((((unsigned short)v58 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                    {
                        v60 = v59 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v59 = v60 + 1;
                        v62 = _ccall(10, 13, (char)((((unsigned short)v60 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (v62 & 1)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v64 = (unsigned short)v59 + 2;
                            if ((char)(((v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 1)
                                goto LABEL_439cf7;
                            goto LABEL_439e21;
                        }
                    }
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v59 = v55 + 2;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v64 = (unsigned short)v59 + 2;
LABEL_439e21:
                if (*((char *)(*((int *)&iter1->padding_0[84]) + 29)) == 3)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v66 = v64;
                    v67 = _ccall(10, 13, (char)(((v66 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc06c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                    if (v67 & 1)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v68 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x406c000000000000) * 0x100 & 0x4500;
                        /* unsupported instruction */
                        v69 = v66;
                        if (((v69 & 7) * 0x800 | (unsigned short)v68 & 0x4700) >> 8 & 1 && !(/* unsupported instruction */
, /* unsupported instruction */
, v70 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (unsigned long long)(unsigned int)*((int *)((char *)iter1 - 8))) * 0x100 & 0x4500, /* unsupported instruction */
, !((char)(((v69 & 7) * 0x800 | (short)(CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (unsigned long long)(unsigned int)*((int *)((char *)iter1 - 8)))) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)))
                            goto LABEL_439cf7;
                    }
                }
                sub_461a70(*((int *)&iter1->padding_0[44]));
                *((unsigned int *)&iter1->padding_0[44]) = 0;
                *((unsigned int *)&iter1->padding_0[40]) = 0;
                continue;
            }
LABEL_439cef:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_439cf5:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_439cf7;
        }
        else
        {
LABEL_439bea:
            *((unsigned int *)&iter1->padding_0[40]) = 0;
            idx1 = sub_461920(*((int *)&iter1->padding_0[48]));
            if (idx1)
            {
                idx1->field_47c = idx1->field_47c | 0x10000000;
                if (!idx1->field_18)
                {
                    v74 = idx1->field_14;
                    if (idx1->field_14)
                    {
                        do
                        {
                            v75 = v74;
                            v75->field_0->field_47c = v75->field_0->field_47c | 0x10000000;
                            v74 = v75->field_4;
                        } while (v75->field_4);
                    }
                }
            }
            *((unsigned int *)&iter1->padding_0[44]) = 0;
            *((int *)&iter1->padding_0[48]) = 0;
        }
    } while ((iter1 += 120, v5 = v6 - 1, v4 += 120, v6 != 1));
    return 0;
}
