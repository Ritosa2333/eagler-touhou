// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x425c20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_425c20_1 {
    char padding_0[2432];
    unsigned int field_980;
} st_425c20_1;

typedef struct st_425c20_0 {
    char padding_0[1168];
    char field_490;
    char padding_491[31];
    char field_4b0;
    char field_4b1;
    char padding_4b2[1170];
    char field_944;
    char padding_945[55];
    int field_97c;
    unsigned int field_980;
    unsigned int field_984;
    unsigned int field_988;
    unsigned int field_98c;
    unsigned int field_990;
    char padding_994[4];
    unsigned int field_998;
    char padding_99c[4];
    int field_9a0;
    char padding_9a4[16];
    int field_9b4;
    char padding_9b8[12];
    unsigned int field_9c4;
    int field_9c8;
    char padding_9cc[4];
    unsigned int field_9d0;
    unsigned int field_9d4;
    unsigned int field_9d8;
    unsigned int field_9dc;
    int field_9e0;
    unsigned int field_9e4;
} st_425c20_0;

typedef struct st_425c20_2 {
    char padding_0[27952];
    unsigned int field_6d30;
} st_425c20_2;

extern int g_4b0c48;
extern char g_4b0c4c;
extern char g_4b0c50;
extern char g_4b0c58;
extern unsigned int g_4b0ccc;
extern char g_4b0cd0;
extern unsigned int g_4b43c8;
extern st_425c20_2 *g_4b43e4;
extern st_425c20_1 *g_4b4514;
extern unsigned int g_4b4534;
extern unsigned int g_4d49d0;

unsigned int sub_425c20(st_425c20_0 *index)
{
    int v36;  // ebx
    unsigned int v37;  // esi
    unsigned int v135;  // 4150
    unsigned int v137;  // ftop
    unsigned int v139;  // ftop
    unsigned int v141;  // ftop
    unsigned int v142;  // ftop
    unsigned int v143;  // 4162
    unsigned int v145;  // 4153
    unsigned int *idx;  // eax
    unsigned int v147;  // ecx
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // 4549
    unsigned int v52;  // ftop
    unsigned int v53;  // ftop
    unsigned int v54;  // fc3210
    unsigned int v38;  // edi
    unsigned int v55;  // 4143
    unsigned int v56;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // 4543
    st_425c20_0 *idx1;  // esi
    st_425c20_0 *v63;  // ebx
    st_425c20_0 *iter;  // edi
    st_425c20_0 *idx2;  // esi
    st_425c20_0 *v67;  // esi
    unsigned int v69;  // ftop
    st_425c20_2 *v70;  // ecx
    unsigned int v73;  // 4295
    int v74;  // eax
    unsigned int v75;  // ftop
    unsigned int v76;  // fc3210
    unsigned int v77;  // ftop
    unsigned int v78;  // fc3210
    unsigned int v80;  // 4146
    unsigned int v81;  // fc3210
    unsigned int v82;  // 4131
    unsigned int v83;  // fc3210
    unsigned int node;  // ftop
    unsigned int v84;  // ftop
    unsigned int v85;  // fc3210
    unsigned int v86;  // fc3210
    unsigned int v87;  // ftop
    unsigned int v88;  // 4146
    unsigned int v90;  // 4134
    unsigned int v92;  // 4145
    unsigned int v41;  // eax
    unsigned int v96;  // eax
    st_425c20_0 *v97;  // esi
    unsigned int v99;  // ftop
    unsigned int v101;  // 4299
    st_425c20_1 *v102;  // ecx
    unsigned int v103;  // ftop
    unsigned int v42;  // 4098
    unsigned int v105;  // ftop
    unsigned int v106;  // 4417
    unsigned int v109;  // 4150
    unsigned int v111;  // 4150
    int v113;  // eax
    unsigned int v43;  // 4107
    unsigned int v115;  // fc3210
    unsigned int v116;  // eax
    int v117;  // esi
    unsigned int v118;  // eax
    int v119;  // edx
    unsigned int v120;  // edx
    int v121;  // eax
    int v122;  // esi
    unsigned int v123;  // eax
    int v44;  // ebx
    int v124;  // esi
    unsigned int v125;  // ebx
    unsigned int v126;  // eax
    int v127;  // esi
    unsigned int v128;  // eax
    unsigned int v133;  // 4150
    unsigned int v0;  // [bp-0xc4]
    unsigned int v1;  // [bp-0xbc]
    st_425c20_1 *v2;  // [bp-0xb8]
    unsigned int v3;  // [bp-0xb4]
    unsigned int v4;  // [bp-0xb0]
    unsigned int v5;  // [bp-0xac]
    st_425c20_1 *v6;  // [bp-0xa8], Other Possible Types: int, unsigned int
    st_425c20_2 *v7;  // [bp-0xa4]
    unsigned int v8;  // [bp-0xa0]
    unsigned int v9;  // [bp-0x9c]
    unsigned int v10;  // [bp-0x98]
    int i;  // [bp-0x94]
    unsigned int v12;  // [bp-0x90]
    unsigned int v13;  // [bp-0x8c]
    unsigned int v14;  // [bp-0x88]
    unsigned int v15;  // [bp-0x84]
    unsigned int v16;  // [bp-0x80]
    unsigned int v17;  // [bp-0x7c]
    unsigned int v18;  // [bp-0x78]
    unsigned int v19;  // [bp-0x70]
    unsigned int v20;  // [bp-0x6c]
    unsigned int v21;  // [bp-0x68]
    unsigned int v22;  // [bp-0x64]
    unsigned int v23;  // [bp-0x60]
    unsigned int v24;  // [bp-0x5c]
    unsigned int v25;  // [bp-0x58]
    unsigned int v26;  // [bp-0x54]
    char v27;  // [bp-0x48]
    char v28;  // [bp-0x44]
    unsigned int v29;  // [bp-0x40]
    unsigned int v30;  // [bp-0x3c]
    unsigned int v31;  // [bp-0x38]
    unsigned int v32;  // [bp-0x24]
    unsigned int v33;  // [bp-0x20]
    unsigned int v34;  // [bp-0x18]
    unsigned int v35;  // [bp-0x14]

    i = v36;
    v10 = v37;
    v9 = v38;
    iter = &index->padding_0[20];
    *((unsigned int *)&index[2647].padding_0[516]) = 0;
    *((unsigned int *)&index[2647].padding_0[508]) = 0;
    v13 = 0;
    do
    {
        v41 = *((int *)&iter->padding_9a4[12]);
        if (!v41)
            continue;
        if (v41 == 5)
        {
            v42 = *((int *)&iter->padding_9b8[8]);
            *((unsigned int *)&iter->padding_9b8[8]) = *((int *)&iter->padding_9b8[8]) - 1;
            v43 = _ccall(8, 3, v42, 0xffffffff, 0);
            if (v43 & 1)
            {
                v44 = iter->field_9b4;
                *((unsigned int *)&iter->padding_9a4[12]) = 2;
                v10 = *((int *)(g_4b43c8 + 5106652));
                sub_402520();
                iter->padding_491[12] = 16;
                iter->padding_491[11] = 16;
                sub_454d10(iter, v44 + 165);
                goto LABEL_426f53;
            }
        }
        if (v41 == 1)
        {
            if (*((int *)(g_4b4534 + 44)) && sub_4271c0())
                goto LABEL_426879;
            if (*((int *)&g_4b4514[1].padding_0[164]) != 2 && *((int *)&g_4b4514[1].padding_0[164]) != 4)
            {
                if (iter->field_98c < 40)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4b4514->field_980) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                        goto LABEL_425d1b;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), g_4b4514->field_980) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    goto LABEL_425d28;
            }
LABEL_425d1b:
            if (!g_4b43e4->field_6d30)
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
                v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&iter->padding_945[39]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&iter->padding_945[43]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&iter->padding_945[47]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                iter->field_97c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v48 = node - 2;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v50 = v48 + 1;
                v51 = _ccall(10, 13, (char)((((unsigned short)v48 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (!(v51 & 1))
                {
                    *((int *)&iter->padding_945[0x33]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v52 = v50 + 1;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v52 = v50 + 1;
                }
                v53 = v52 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v54 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), iter->field_97c) * 0x100 & 0x4500;
                v55 = _ccall(10, 13, (char)((((unsigned short)v53 & 7) * 0x800 | (unsigned short)v54 & 0x4700) >> 8) & 5, 0, 0);
                if (!(v55 & 1))
                {
                    iter->field_97c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v56 = v53 + 1;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v56 = v53 + 1;
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                node = v56;
                if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407d800000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    *((unsigned int *)&iter->padding_9a4[12]) = 0;
                    continue;
                }
                goto LABEL_4260af;
            }
LABEL_425d28:
            v47 = node - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            goto LABEL_425eab;
        }
        else if (v41 == 2)
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
            v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter->padding_945[39]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter->padding_945[43]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&iter->padding_945[47]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            iter->field_97c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v59 = node;
            v60 = _ccall(10, 13, (char)((((unsigned short)v59 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v60 & 1))
            {
                v47 = v59 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                node = v59;
                if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407d800000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    *((unsigned int *)&iter->padding_9a4[12]) = 0;
                    g_4b0ccc = g_4b0ccc - 4;
                    if (g_4b0ccc > 0x400)
                    {
                        g_4b0ccc = 0x400;
                        goto LABEL_426f53;
                    }
                    else
                    {
                        if (g_4b0ccc < -0x400)
                        {
                            g_4b0ccc = 0xfffffc00;
                            goto LABEL_426f53;
                        }
                    }
                }
            }
LABEL_425eab:
            *((int *)&iter->padding_9b8[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            node = v47 + 1;
            *((unsigned int *)&iter->padding_9a4[12]) = 3;
            goto LABEL_425f24;
        }
        else if (v41 == 3)
        {
LABEL_425f24:
            if (*((int *)(g_4b4534 + 44)) && sub_4271c0())
                goto LABEL_426879;
            /* unsupported instruction */
            /* unsupported instruction */
            idx1 = &iter->padding_945[39];
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v63 = &iter->padding_945[0x33];
            sub_4377a0();
            /* unsupported instruction */
            sub_427d00((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx1->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx1->padding_0[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx1->padding_0[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            node += 1;
            if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_9b8[4])) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&iter->padding_9b8[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                if (*((int *)&g_4b4514[1].padding_0[164]) != 4)
                    goto LABEL_4260af;
                goto LABEL_42609b;
            }
        }
        else
        {
            if (v41 == 4)
            {
                if (*((int *)(g_4b4534 + 44)) && sub_4271c0())
                    goto LABEL_426879;
                /* unsupported instruction */
                /* unsupported instruction */
                idx2 = &iter->padding_945[39];
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v63 = &iter->padding_945[0x33];
                sub_4377a0();
                /* unsupported instruction */
                sub_427d00((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
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
                v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx2->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx2->padding_0[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&idx2->padding_0[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                node += 1;
                if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_9b8[4])) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&iter->padding_9b8[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                }
                if (*((int *)&g_4b4514[1].padding_0[164]) != 4)
                    goto LABEL_4260af;
                goto LABEL_42609b;
            }
            if (v41 == 8)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                v67 = &iter->padding_945[39];
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v63 = &iter->padding_945[0x33];
                sub_4377a0();
                /* unsupported instruction */
                sub_427d00((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v25 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v67->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v67->padding_0[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v67->padding_0[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                node += 1;
                if (iter->field_98c >= 60)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), iter->field_984) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        iter->field_984 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                    }
                }
                if (*((int *)&g_4b4514[1].padding_0[164]) != 4)
                    goto LABEL_4260af;
LABEL_42609b:
                v69 = node - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)&v63->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                goto LABEL_42609f;
            }
            else if (v41 == 6)
            {
                if (*((int *)(g_4b4534 + 44)) && sub_4271c0())
                    goto LABEL_426879;
                v70 = g_4b43e4;
                if (g_4b43e4->field_6d30)
                    sub_4067e0(10000);
                if (iter->field_9b4 != 13 && iter->field_9b4 != 14 && iter->field_9b4 != 15)
                {
LABEL_426375:
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
                    v22 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v23 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v24 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&iter->padding_945[39]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&iter->padding_945[43]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&iter->padding_945[47]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v75 = node;
                    if (iter->field_98c >= 1200)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        node = v75;
                        v92 = _ccall(10, 13, (char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc06a000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (v92 & 1)
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x406a000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                                goto LABEL_42655b;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_945[43])) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                                goto LABEL_42655b;
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            if (!((char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407d000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 1))
                                goto LABEL_42655b;
                            goto LABEL_42658a;
                        }
                        else
                        {
LABEL_42655b:
                            *((unsigned int *)&iter->padding_9a4[12]) = 0;
                            g_4b0ccc = g_4b0ccc - 4;
                            if (g_4b0ccc > 0x400)
                            {
                                g_4b0ccc = 0x400;
                            }
                            else if (g_4b0ccc < -0x400)
                            {
                                g_4b0ccc = 0xfffffc00;
                            }
                            sub_461a70(*((int *)&iter->padding_945[35]));
                            *((int *)&iter->padding_945[35]) = 0;
                            continue;
                        }
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v76 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_945[39])) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    v77 = v75 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((((unsigned short)v75 & 7) * 0x800 | (unsigned short)v76 & 0x4700) >> 8 & 1 || (v78 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (unsigned long long)(unsigned int)*((int *)&iter->padding_945[0x33])) * 0x100 & 0x4500, (char)((((unsigned short)v77 & 7) * 0x800 | (short)(CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (unsigned long long)(unsigned int)*((int *)&iter->padding_945[0x33]))) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v80 = _ccall(10, 13, (char)((((unsigned short)v77 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_945[39])) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (v80 & 1 || (v81 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (unsigned long long)(unsigned int)*((int *)&iter->padding_945[0x33])) * 0x100 & 0x4500, v82 = (unsigned int)_ccall(10, 13, (unsigned int)((char)((((unsigned short)v77 & 7) * 0x800 | (unsigned short)v81 & 0x4700) >> 8) & 5), 0, 0), v82 & 1))
                            goto LABEL_426435;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&iter->padding_945[0x33]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
LABEL_426435:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v83 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_945[43])) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    v84 = v77;
                    if ((((unsigned short)v84 & 7) * 0x800 | (unsigned short)v83 & 0x4700) >> 8 & 1 || !(v85 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (unsigned long long)(unsigned int)iter->field_97c) * 0x100 & 0x4500, !((char)((((unsigned short)v84 & 7) * 0x800 | (short)(CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (unsigned long long)(unsigned int)iter->field_97c)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)))
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v86 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_945[43])) * 0x100 & 0x4500;
                        /* unsupported instruction */
                        v87 = v84;
                        v88 = _ccall(10, 13, (char)((((unsigned short)v87 & 7) * 0x800 | (unsigned short)v86 & 0x4700) >> 8) & 65, 0, 0);
                        if (!(v88 & 1))
                        {
                            /* unsupported instruction */
                            node = v87 + 1;
                            v90 = _ccall(10, 13, (char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), iter->field_97c) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                            if (v90 & 1)
                                goto LABEL_42658a;
                        }
                        else
                        {
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            node = v87 + 1;
LABEL_42658a:
                            if (*((int *)&g_4b0c58) >= 3)
                            {
                                sub_461a70(*((int *)&iter->padding_945[35]));
                                *((int *)&iter->padding_945[35]) = 0;
                                goto LABEL_4260af;
                            }
                            else
                            {
                                switch (iter->field_9b4)
                                {
                                case 10: case 13:
                                    if (*((int *)&g_4b0c4c) == 1)
                                    {
                                        if (*((int *)&g_4b0c50) == 1 && !*((int *)&iter->padding_945[35]))
                                        {
                                            sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v21, 211, 0);
                                            *((unsigned int *)&iter->padding_945[35]) = v18;
                                            sub_461970(v18);
                                            goto LABEL_4260af;
                                        }
                                    }
                                    else
                                    {
                                        if (*((int *)&g_4b0c4c) == 2)
                                        {
                                            if (*((int *)&g_4b0c50) == 3)
                                                goto LABEL_426658;
                                        }
                                        else
                                        {
                                            if (*((int *)&g_4b0c4c) == 3 && *((int *)&g_4b0c50) == 2)
                                            {
LABEL_426658:
                                                if (*((int *)&iter->padding_945[35]))
                                                    goto LABEL_4260af;
                                                sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v27, 211, 0);
                                                v96 = v25;
                                                goto LABEL_426832;
                                            }
                                        }
                                    }
                                    sub_461a70(*((int *)&iter->padding_945[35]));
                                    *((int *)&iter->padding_945[35]) = 0;
                                    goto LABEL_4260af;
                                case 11: case 14:
                                    if (*((int *)&g_4b0c4c) == 2)
                                    {
                                        if (*((int *)&g_4b0c50) == 2 && !*((int *)&iter->padding_945[35]))
                                        {
                                            sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v14, 211, 0);
                                            *((unsigned int *)&iter->padding_945[35]) = v10;
                                            sub_461970(v10);
                                            goto LABEL_4260af;
                                        }
                                    }
                                    else
                                    {
                                        if (*((int *)&g_4b0c4c) == 1)
                                        {
                                            if (*((int *)&g_4b0c50) == 3)
                                                goto LABEL_42672b;
                                        }
                                        else
                                        {
                                            if (*((int *)&g_4b0c4c) == 3 && *((int *)&g_4b0c50) == 1)
                                            {
LABEL_42672b:
                                                if (*((int *)&iter->padding_945[35]))
                                                    goto LABEL_4260af;
                                                sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v25, 211, 0);
                                                v96 = v21;
                                                goto LABEL_426832;
                                            }
                                        }
                                    }
                                    sub_461a70(*((int *)&iter->padding_945[35]));
                                    *((int *)&iter->padding_945[35]) = 0;
                                    goto LABEL_4260af;
                                case 12: case 15:
                                    if (*((int *)&g_4b0c4c) == 3)
                                    {
                                        if (*((int *)&g_4b0c50) == 3 && !*((int *)&iter->padding_945[35]))
                                        {
                                            sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v18, 211, 0);
                                            *((unsigned int *)&iter->padding_945[35]) = v14;
                                            sub_461970(v14);
                                            goto LABEL_4260af;
                                        }
                                    }
                                    else
                                    {
                                        if (*((int *)&g_4b0c4c) == 1)
                                        {
                                            if (*((int *)&g_4b0c50) == 2)
                                                goto LABEL_4267fa;
                                        }
                                        else
                                        {
                                            if (*((int *)&g_4b0c4c) == 2 && *((int *)&g_4b0c50) == 1)
                                            {
LABEL_4267fa:
                                                if (*((int *)&iter->padding_945[35]))
                                                    goto LABEL_4260af;
                                                sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v28, 211, 0);
                                                v96 = v26;
LABEL_426832:
                                                *((unsigned int *)&iter->padding_945[35]) = v96;
                                                sub_461970(v96);
                                                goto LABEL_4260af;
                                            }
                                        }
                                    }
                                    sub_461a70(*((int *)&iter->padding_945[35]));
                                    *((int *)&iter->padding_945[35]) = 0;
                                    goto LABEL_4260af;
                                default:
LABEL_4260af:
                                    v102 = g_4b4514;
                                    if (*((int *)&g_4b4514[1].padding_0[164]) == 2)
                                    {
LABEL_426ecb:
                                        if (iter->padding_0[1148] & 1)
                                            sub_455630(iter);
                                        if (iter->padding_4b2[1150] & 1)
                                            sub_455630(&iter->padding_4b2[2]);
                                        if (*((int *)&iter->padding_945[35]))
                                        {
                                            idx = sub_461920(*((int *)&iter->padding_945[35]));
                                            if (idx)
                                            {
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
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                idx[269] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                idx[270] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                                /* unsupported instruction */
                                            }
                                        }
                                        sub_464a80();
                                        *((unsigned int *)&index[2647].padding_0[508]) = *((int *)&index[2647].padding_0[508]) + 1;
                                        continue;
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
                                    /* unsupported instruction */
                                    v32 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v33 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v34 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v35 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v103 = node - 2;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v105 = v103 - 2;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v106 = _ccall(10, 13, (char)((((unsigned short)v103 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                    if (v106 & 1)
                                    {
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        if (!((char)((((unsigned short)v105 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                                            goto LABEL_426dd8;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v109 = _ccall(10, 13, (char)((((unsigned short)v105 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                        if (!(v109 & 1))
                                            goto LABEL_426dd8;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v111 = _ccall(10, 13, (char)((((unsigned short)v105 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                        if (!(v111 & 1))
                                            goto LABEL_426dd8;
                                    }
                                    else
                                    {
LABEL_426dd8:
                                        if (*((int *)&iter->padding_9a4[12]) == 4 || *((int *)&iter->padding_9a4[12]) == 3 || *((int *)&iter->padding_9a4[12]) == 6 || *((int *)&iter->padding_9a4[12]) == 7 || *((int *)&iter->padding_9a4[12]) == 8)
                                        {
LABEL_426ec3:
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v137 = v105 + 1;
LABEL_426ec5:
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v139 = v137 + 1;
LABEL_426ec7:
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v142 = v139 + 1;
LABEL_426ec9:
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            node = v142 + 1;
                                            goto LABEL_426ecb;
                                        }
                                        if (g_4d49d0 & 8)
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            if ((char)((((unsigned short)v105 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                                            {
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                if ((char)((((unsigned short)v105 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                                                {
                                                    /* unsupported instruction */
                                                    /* unsupported instruction */
                                                    /* unsupported instruction */
                                                    v133 = _ccall(10, 13, (char)((((unsigned short)v105 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                                    if (v133 & 1)
                                                    {
                                                        /* unsupported instruction */
                                                        /* unsupported instruction */
                                                        /* unsupported instruction */
                                                        v135 = _ccall(10, 13, (char)((((unsigned short)v105 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                                        if (v135 & 1)
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
                                                            node = v105 + 4;
LABEL_426ea2:
                                                            /* unsupported instruction */
                                                            /* unsupported instruction */
                                                            *((unsigned int *)&iter->padding_9a4[12]) = 4;
                                                            /* unsupported instruction */
                                                            /* unsupported instruction */
                                                            *((int *)&iter->padding_9b8[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                                            /* unsupported instruction */
                                                            goto LABEL_426ecb;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        if (g_4d49d0 & 8)
                                            goto LABEL_426ec3;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v137 = v105 + 1;
                                        if (!((char)((((unsigned short)v105 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                                            goto LABEL_426ec5;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v139 = v137 + 1;
                                        if (!((char)((((unsigned short)v137 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                                            goto LABEL_426ec7;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v141 = v139;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v142 = v141 + 1;
                                        v143 = _ccall(10, 13, (char)((((unsigned short)v141 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                        if (!(v143 & 1))
                                            goto LABEL_426ec9;
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        node = v142 + 1;
                                        v145 = _ccall(10, 13, (char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                        if (!(v145 & 1))
                                            goto LABEL_426ecb;
                                        goto LABEL_426ea2;
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
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    node = v105 + 4;
                                    switch (iter->field_9b4)
                                    {
                                    case 1:
                                        sub_426ff0();
                                        break;
                                    case 2:
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v115 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v10) * 0x100 & 0x4500;
                                        /* unsupported instruction */
                                        if ((((unsigned short)node & 7) * 0x800 | (unsigned short)v115 & 0x4700) >> 8 & 1 && *((int *)&iter->padding_9a4[12]) != 3)
                                        {
                                            v116 = sub_421880();
                                            node -= 1;
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v117 = (int)(v116 * 3 + ((int)(v116 * 3) >> 31 & 3)) >> 2;
                                            v118 = sub_4931e0();
                                            v119 = (int)((unsigned int)(2443359173 * (v117 * (v118 - 128)) >> 32) + v117 * (v118 - 128)) >> 8;
                                            v120 = 1717986919 * (v117 - (((unsigned int)(v119) >> 31) + v119)) >> 0x22;
                                            v3 = ((v120 >> 31) + v120) * 10;
                                            if (v3 <= 0)
                                                v3 = 10;
                                            v6 = 0xffffffff;
                                            goto LABEL_426d8b;
                                        }
                                        else
                                        {
                                            v121 = sub_421880();
                                            goto LABEL_426d65;
                                        }
                                    case 3:
                                        if (g_4b0c48 >= *((int *)&g_4b0cd0))
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v6 = g_4b4514;
                                            /* unsupported instruction */
                                            sub_427ca0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                            sub_43e250(&iter->padding_945[39], -0xbf00c0);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v3 = v147;
                                            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                        }
                                        if (!sub_422d70())
                                            break;
                                        sub_4385b0(g_4b4514);
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v1 = v147;
                                        /* unsupported instruction */
                                        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                        sub_43e250(&iter->padding_945[39], -0xc0);
                                        v113 = g_4b0ccc + 24;
                                        break;
                                    case 4:
                                        sub_422d30();
                                        v113 = g_4b0ccc + 0x100;
                                        break;
                                    case 5:
                                        sub_422e10();
                                        v113 = g_4b0ccc + 0x100;
                                        break;
                                    case 6:
                                        sub_422ce0();
                                        v113 = g_4b0ccc + 0x100;
                                        break;
                                    case 7:
                                        sub_422dd0();
                                        v113 = g_4b0ccc + 0x100;
                                        break;
                                    case 8:
                                        if (g_4b0c48 >= *((int *)&g_4b0cd0))
                                        {
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v6 = g_4b0c48;
                                            /* unsupported instruction */
                                            sub_427ca0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                            sub_43e250(&iter->padding_945[39], -0xbf00c0);
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v3 = v147;
                                            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                            /* unsupported instruction */
                                            sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                        }
                                        if (!sub_422d70())
                                            break;
                                        sub_4385b0(g_4b4514);
                                        sub_43e250(&iter->padding_945[39], -0xc0);
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v0 = v147;
                                        /* unsupported instruction */
                                        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                        v113 = g_4b0ccc + 12;
                                        break;
                                    case 9:
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v6 = g_4b4514;
                                        /* unsupported instruction */
                                        sub_427ca0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                        v5 = 10;
                                        goto LABEL_426d9a;
                                    case 10: case 13:
                                        sub_4270b0();
                                        break;
                                    case 11: case 14:
                                        sub_4270b0();
                                        break;
                                    case 12: case 15:
                                        sub_4270b0();
                                        break;
                                    case 16: case 17: case 18:
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v6 = g_4b4514;
                                        /* unsupported instruction */
                                        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                        switch (iter->field_9c8)
                                        {
                                        case -1:
                                            v122 = 0;
                                            if (iter->padding_9cc > 0)
                                            {
                                                do
                                                {
                                                    sub_426ff0();
                                                    v122 += 1;
                                                } while (v122 < iter->padding_9cc);
                                            }
                                            v123 = sub_421880();
                                            v9 = iter->field_9d0 * v123;
                                            node -= 1;
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            break;
                                        case 0:
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v2 = v102;
                                            /* unsupported instruction */
                                            *((unsigned int *)&iter->padding_9a4[12]) = 0;
                                            sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                            sub_461970(*((int *)&iter->padding_945[35]));
                                            continue;
                                        case 1:
                                            v124 = 0;
                                            if (iter->padding_9cc > 0)
                                            {
                                                do
                                                {
                                                    sub_426ff0();
                                                    v124 += 1;
                                                } while (v124 < iter->padding_9cc);
                                            }
                                            v125 = sub_421880();
                                            v9 = (unsigned int)(v125 * iter->padding_9cc);
                                            node -= 1;
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            v126 = sub_4931e0();
                                            v3 = v126 + v125 * iter->field_9d0;
                                            goto LABEL_426d7d;
                                        case 2: case 3:
                                            v127 = 0;
                                            if (iter->padding_9cc > 0)
                                            {
                                                do
                                                {
                                                    sub_426ff0();
                                                    v127 += 1;
                                                } while (v127 < iter->padding_9cc);
                                            }
                                            v128 = sub_421880();
                                            v9 = iter->field_9d0 * v128;
                                            node -= 1;
                                            /* unsupported instruction */
                                            /* unsupported instruction */
                                            break;
                                        default:
                                            break;
                                        }
                                        /* unsupported instruction */
                                        /* unsupported instruction */
                                        v121 = sub_4931e0();
LABEL_426d65:
                                        v3 = (((unsigned int)((int)(1717986919 * v121 >> 0x22)) >> 31) + (int)(1717986919 * v121 >> 0x22)) * 10;
LABEL_426d7d:
                                        if (v3 <= 0)
                                            v3 = 10;
                                        v5 = 0xffffff00;
LABEL_426d8b:
                                        sub_43e250(&iter->padding_945[39]);
                                        v3 = v3;
LABEL_426d9a:
                                        sub_40e730(v3);
                                        break;
                                    default:
                                        break;
                                    }
                                    g_4b0ccc = v113;
                                    if (v113 > 0x400)
                                    {
                                        g_4b0ccc = 0x400;
                                        break;
                                    }
                                    else if (v113 < -0x400)
                                    {
                                        g_4b0ccc = 0xfffffc00;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        node = v84 + 1;
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    iter->field_97c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    goto LABEL_42658a;
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
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                i = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v73 = _ccall(10, 13, (char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x40ac200000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (!(v73 & 1))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v7 = v70;
                    /* unsupported instruction */
                    sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v6 = v147;
                    /* unsupported instruction */
                    sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    if (iter->field_9a0 == 90)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v5 = v147;
                        /* unsupported instruction */
                        sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    }
                    if (!iter->field_9c4)
                    {
                        sub_421a10();
                        iter->field_9c4 = 1;
                    }
                }
                else if (iter->field_9c4)
                {
                    if (iter->field_9a0 >= 90)
                        sub_40d5d0();
                    else
                        sub_4219e0();
                    iter->field_9c4 = 0;
                }
                if (iter->field_9a0 == 90)
                {
                    sub_40d5d0();
                }
                else if (iter->field_9a0 >= 150)
                {
                    sub_4067e0(0);
                    v74 = (int)((iter->field_9b4 - 12) % 3) + 13;
                    iter->field_9b4 = v74;
                    v3 = v74 + 165;
                    sub_4061e0(*((int *)(g_4b43c8 + 5106652)), v74 + 165);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v1 = v147;
                    /* unsupported instruction */
                    sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    sub_461a70(*((int *)&iter->padding_945[35]));
                    *((int *)&iter->padding_945[35]) = 0;
LABEL_42636a:
                    sub_464a80();
                    goto LABEL_426375;
                }
                goto LABEL_42636a;
            }
            else
            {
                if (v41 != 7)
                    goto LABEL_4260af;
LABEL_426879:
                if (!*((int *)(g_4b4534 + 44)))
                {
                    v69 = node - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&iter->padding_945[0x33]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
LABEL_42609f:
                    iter->field_97c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    node = v69 + 1;
                    *((unsigned int *)&iter->padding_9a4[12]) = 1;
                    goto LABEL_4260af;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v97 = &iter->padding_945[39];
                    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    sub_44ac00();
                    /* unsupported instruction */
                    sub_427d00((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
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
                    v29 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v30 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v31 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&v97->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&v97->padding_0[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    *((int *)&v97->padding_0[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v99 = node + 1;
                    if (!((char)((((unsigned short)v99 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&iter->padding_9b8[4])) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
                        *((int *)&iter->padding_9b8[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
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
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    node = v99;
                    v101 = _ccall(10, 13, (char)((((unsigned short)node & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x43a20000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                    if (v101 & 1)
                        goto LABEL_4260af;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v4 = v147;
                    /* unsupported instruction */
                    *((unsigned int *)&iter->padding_9a4[12]) = 0;
                    sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    sub_44aca0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                }
            }
        }
LABEL_426f53:
        iter = &iter->field_9d8;
        i += 1;
    } while (i < 2664);
    return 1;
}
