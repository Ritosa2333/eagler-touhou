// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41fcc0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41fcc0_7 {
    char padding_0[1432];
    unsigned int field_598;
} st_41fcc0_7;

typedef struct st_41fcc0_9 {
    char padding_0[1088];
    unsigned int field_440;
} st_41fcc0_9;

typedef struct st_41fcc0_5 {
    char padding_0[10];
    char field_a;
} st_41fcc0_5;

typedef struct st_41fcc0_3 {
    char padding_0[56];
    int field_38;
} st_41fcc0_3;

typedef struct st_41fcc0_6 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[8];
    struct st_41fcc0_5 *field_1c;
} st_41fcc0_6;

typedef struct st_41fcc0_1 {
    char padding_0[16];
    int field_10;
} st_41fcc0_1;

typedef struct st_41fcc0_4 {
    char padding_0[116];
    unsigned int field_74;
} st_41fcc0_4;

typedef struct st_41fcc0_0 {
    char padding_0[27876];
    int field_6ce4;
    char padding_6ce8[48];
    unsigned int field_6d18;
    char padding_6d1c[40];
    int field_6d44;
    unsigned int field_6d48;
} st_41fcc0_0;

typedef struct st_41fcc0_2 {
    char padding_0[72];
    int field_48;
    int field_4c;
} st_41fcc0_2;

extern unsigned int g_420e04[4];
extern unsigned int g_4b0c48;
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0c98;
extern unsigned int g_4b0ca0;
extern unsigned int g_4b0ca8;
extern char g_4b0cb0;
extern unsigned int g_4b0cb8;
extern unsigned int g_4b0cc0;
extern char g_4b0ce0;
extern unsigned int g_4b2ed0;
extern unsigned int g_4b2f84[4];
extern unsigned int g_4b2f90[4];
extern unsigned int g_4b3070[4];
extern char g_4b30d0;
extern char g_4b30dc;
extern st_41fcc0_2 *g_4b43dc;
extern st_41fcc0_0 *g_4b43e4;
extern st_41fcc0_4 *g_4b44e8;
extern st_41fcc0_1 *g_4b4514;
extern st_41fcc0_6 *g_4b4518;
extern unsigned int g_4b451c;
extern st_41fcc0_3 *g_4b452c;
extern unsigned int g_4d49d0;
extern unsigned int g_4d49dc;

unsigned int sub_41fcc0(void* idx)
{
    unsigned int v15;  // ebx
    unsigned int v16;  // esi
    int v23;  // eax
    unsigned int v24;  // eax
    int v25;  // eax
    int v26;  // eax
    int v27;  // esi
    int v28;  // eax
    int v29;  // esi
    int v30;  // eax
    int v31;  // eax
    int v32;  // esi
    unsigned int v17;  // edi
    int v33;  // eax
    int v34;  // esi
    int v35;  // eax
    st_41fcc0_9 *v36;  // eax
    st_41fcc0_9 *v37;  // eax
    st_41fcc0_9 *v38;  // eax
    st_41fcc0_9 *v39;  // eax
    void* v40;  // ebx
    void* v41;  // edi
    st_41fcc0_9 *v42;  // eax
    unsigned int v18;  // eax
    st_41fcc0_9 *v43;  // eax
    void* v44;  // ebx
    void* v45;  // edi
    st_41fcc0_9 *v46;  // eax
    st_41fcc0_9 *v47;  // eax
    void* v48;  // ebx
    void* v49;  // edi
    st_41fcc0_9 *v50;  // eax
    st_41fcc0_9 *v51;  // eax
    void* v52;  // ebx
    void* v53;  // edi
    st_41fcc0_9 *v54;  // eax
    st_41fcc0_9 *v55;  // eax
    void* v56;  // ebx
    void* v57;  // edi
    st_41fcc0_9 *v58;  // eax
    st_41fcc0_9 *v59;  // eax
    void* v60;  // ebx
    void* v61;  // edi
    st_41fcc0_9 *v62;  // eax
    unsigned int v19;  // ftop
    st_41fcc0_9 *v63;  // eax
    unsigned int v64;  // ecx
    int v65;  // eax
    int v66;  // eax
    unsigned int index;  // eax
    unsigned int iter;  // esi
    unsigned int v69;  // edx
    st_41fcc0_7 *v70;  // eax
    unsigned int v71;  // eax
    unsigned int v72;  // esi
    unsigned int v73;  // edx
    st_41fcc0_7 *v74;  // eax
    unsigned int v75;  // ftop
    void* v76;  // edx
    unsigned int v77;  // edx
    unsigned int *v78;  // ecx
    unsigned short v80;  // ftop
    void* v20;  // eax
    unsigned int v21;  // ecx
    unsigned int v22;  // eax
    unsigned int v0;  // [bp-0x60]
    unsigned int v1;  // [bp-0x5c]
    unsigned int v2;  // [bp-0x58]
    unsigned int v3;  // [bp-0x54]
    unsigned int v4;  // [bp-0x50]
    unsigned int v5;  // [bp-0x4c]
    char v6;  // [bp-0x48], Other Possible Types: unsigned int
    char v7;  // [bp-0x44]
    char v8;  // [bp-0x40]
    char v9;  // [bp-0x3c]
    unsigned int *v10;  // [bp-0x38]
    char v11;  // [bp-0xc]
    int v12;  // [bp-0x4]
    int v13;  // [bp-0x4]
    int node;  // [bp+0x0]

    v5 = v15;
    v3 = v16;
    v2 = v17;
    if ((int)idx[140] > 0)
        *((unsigned int *)&idx[140]) = (int)idx[140] - 1;
    if ((char)idx[144] & 1 && g_4d49d0 & 0x200)
    {
        v18 = (int)idx[40];
        idx = *((short *)(int)idx[100]);
        if (!((char)v18 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)&idx[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            *((unsigned int *)&idx[28]) = 0;
            *((unsigned int *)&idx[24]) = 0xfff0bdc1;
            *((unsigned int **)&idx[36]) = &g_4b2ed0;
            *((unsigned int *)&idx[40]) = v18 | 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&idx[28]) = idx;
        *((unsigned int *)&idx[24]) = idx - 1;
        *((int *)&idx[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    if ((int)idx[28] >= *((short *)(int)idx[100]))
    {
        while (1)
        {
            v20 = (int)idx[100];
            v21 = (char)v20[2];
            switch (v21)
            {
            case 0:
                if (g_4b0cb8)
                    g_4b0cc0 = 0;
                g_4b0cb8 = 0;
                return 0xffffffff;
            case 1:
                sub_4615a0(g_4b4514->field_10, &v7, g_4b2f84[g_4b0c90], 0);
                *((unsigned int *)&idx[64]) = v3;
                continue;
            case 2:
                if (*((int *)&g_4b0cb0) == 5)
                {
                    if (*((int *)idx) >= 2)
                    {
                        sub_4615a0(g_4b43dc->field_4c, &v8, 10, 0);
                        *((unsigned int *)&idx[0x44]) = v4;
                        continue;
                    }
                }
                else
                {
                    if (*((int *)&g_4b0cb0) == 7 && *((int *)idx) >= 2)
                    {
                        sub_4615a0(g_4b43dc->field_4c, &v9, 13, 0);
                        *((unsigned int *)&idx[0x44]) = v5;
                        continue;
                    }
                }
                sub_4615a0(g_4b43dc->field_48, &v10, g_4b2f90[*((int *)&g_4b0cb0)], 0);
                *((unsigned int *)&idx[0x44]) = v6;
                continue;
            case 3:
                sub_4615a0(g_4b43e4->field_6d44, &v6, 52, 0);
                *((unsigned int *)&idx[72]) = v2;
                continue;
            case 4:
                sub_461970((int)idx[64], v2);
                *((int *)&idx[64]) = 0;
                continue;
            case 5:
                sub_461970((int)idx[0x44], v2);
                v2 = (int)idx[92];
                *((unsigned int *)&idx[0x44]) = 0;
                sub_461970((int)idx[92], v3);
                continue;
            case 6:
                sub_461970((int)idx[72], v2);
                v2 = (int)idx[76];
                sub_461970((int)idx[76], v3);
                v3 = (int)idx[80];
                sub_461970((int)idx[80], v4);
                v4 = (int)idx[84];
                sub_461970((int)idx[84], v5);
                v5 = (int)idx[88];
                sub_461970((int)idx[88], v6);
                continue;
            case 7:
                sub_461970((int)idx[0x44], v2);
                sub_461970((int)idx[64], v3);
                sub_461970((int)idx[72], v4);
                v40 = idx + 76;
                *((unsigned int *)&idx[156]) = 0;
                sub_461dd0(v5);
                v41 = idx + 80;
                sub_461dd0();
                v4 = *((int *)v40);
                v42 = sub_461920(*((int *)v40));
                if (!v42)
                    *((unsigned int *)v40) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v42->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v3 = *((int *)v41);
                v43 = sub_461920(*((int *)v41));
                if (!v43)
                    *((unsigned int *)v41) = 0;
                v43->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v44 = idx + 84;
                sub_461dd0();
                v45 = idx + 88;
                sub_461dd0();
                v2 = *((int *)v44);
                v46 = sub_461920(*((int *)v44));
                if (!v46)
                    *((unsigned int *)v44) = 0;
                v46->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v47 = sub_461920(*((int *)v45));
                if (!v47)
                    *((unsigned int *)v45) = 0;
                v47->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&idx[144]) = (int)idx[144] & 0xfffffffd;
                *((unsigned int *)&idx[148]) = 0;
                *((unsigned int *)&idx[152]) = 0;
                continue;
            case 8:
                sub_461970((int)idx[64], v2);
                sub_461970((int)idx[0x44], v3);
                sub_461970((int)idx[72], v4);
                v48 = idx + 76;
                *((unsigned int *)&idx[156]) = 1;
                sub_461dd0(v5);
                v49 = idx + 80;
                sub_461dd0();
                v4 = *((int *)v48);
                v50 = sub_461920(*((int *)v48));
                if (!v50)
                    *((unsigned int *)v48) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v50->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v3 = *((int *)v49);
                v51 = sub_461920(*((int *)v49));
                if (!v51)
                    *((unsigned int *)v49) = 0;
                v51->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v52 = idx + 84;
                sub_461dd0();
                v53 = idx + 88;
                sub_461dd0();
                v2 = *((int *)v52);
                v54 = sub_461920(*((int *)v52));
                if (!v54)
                    *((unsigned int *)v52) = 0;
                v54->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v55 = sub_461920(*((int *)v53));
                if (!v55)
                    *((unsigned int *)v53) = 0;
                v55->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&idx[144]) = (int)idx[144] & 0xfffffffd;
                *((unsigned int *)&idx[148]) = 0;
                *((unsigned int *)&idx[152]) = 0;
                continue;
            case 9:
                sub_461970((int)idx[0x44], v2);
                sub_461970((int)idx[64], v3);
                sub_461970((int)idx[72], v4);
                v56 = idx + 76;
                *((unsigned int *)&idx[156]) = 2;
                sub_461dd0(v5);
                v57 = idx + 80;
                sub_461dd0();
                v4 = *((int *)v56);
                v58 = sub_461920(*((int *)v56));
                if (!v58)
                    *((unsigned int *)v56) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v58->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v3 = *((int *)v57);
                v59 = sub_461920(*((int *)v57));
                if (!v59)
                    *((unsigned int *)v57) = 0;
                v59->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v60 = idx + 84;
                sub_461dd0();
                v61 = idx + 88;
                sub_461dd0();
                v2 = *((int *)v60);
                v62 = sub_461920(*((int *)v60));
                if (!v62)
                    *((unsigned int *)v60) = 0;
                v62->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                v63 = sub_461920(*((int *)v61));
                if (!v63)
                    *((unsigned int *)v61) = 0;
                v63->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                *((unsigned int *)&idx[144]) = (int)idx[144] | 2;
                *((unsigned int *)&idx[148]) = 0;
                *((unsigned int *)&idx[152]) = 0;
                continue;
            case 10:
                *((unsigned int *)&idx[144]) = (int)idx[144] ^ ((char)v20[4] ^ (int)idx[144]) & 1;
                continue;
            case 11:
                if ((int)idx[48] <= 0)
                    sub_4067e0((int)v20[4]);
                /* unsupported instruction */
                /* unsupported instruction */
                v0 = v21;
                /* unsupported instruction */
                sub_464a20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                if (!(!(g_4d49dc & 524289) && (int)idx[48] > 0))
                {
                    sub_453d90();
                    sub_4067e0(0);
                    *((unsigned int *)&idx[148]) = 0;
                    *((unsigned int *)&idx[152]) = 0;
                    goto LABEL_420ced;
                }
                else if (!((char)idx[144] & 1))
                {
                    return 0;
                }
                else if (g_4d49d0 & 0x200)
                {
                    sub_4067e0(0);
                    *((unsigned int *)&idx[148]) = 0;
                    *((unsigned int *)&idx[152]) = 0;
                    goto LABEL_420ced;
                }
                else
                {
                    return 0;
                }
            case 12:
                *((int *)&idx[140]) = 1;
                continue;
            case 13:
                idx = 0;
                v13 = v12;
                do
                {
                    v10 = *((int *)(4 * node + 8 * g_4b0c90 + 4 * g_4b0c90 + 4927444)) + *((int *)((int)idx[100] + 4));
                    if (sub_461920(*((int *)sub_462060())))
                        sub_454b80();
                } while ((v13 = (int)(v13 + 1), v13 < 3));
            case 14:
                v64 = *((int *)&g_4b0cb0);
                if (*((int *)&g_4b0cb0) == 5)
                {
                    if (*((int *)idx) >= 2)
                    {
                        v65 = 0;
                        idx = 0;
                        do
                        {
                            if (*((int *)&(&g_4b30d0)[v65]) >= 0)
                            {
                                sub_462060(*((int *)((int)idx[100] + 4)) + *((int *)&(&g_4b30d0)[v65]));
                                sub_461ea0(*((int *)((int)idx[100] + 4)) + *((int *)&(&g_4b30d0)[v65]));
                                v65 = v12;
                            }
                        } while ((v65 = (int)(v65 + 4), node = v65, v65 < 12));
                    }
                }
                else
                {
                    if (*((int *)&g_4b0cb0) == 7 && *((int *)idx) >= 2)
                    {
                        v66 = 0;
                        idx = 0;
                        do
                        {
                            if (*((int *)&(&g_4b30dc)[v66]) >= 0)
                            {
                                sub_462060(*((int *)((int)idx[100] + 4)) + *((int *)&(&g_4b30dc)[v66]));
                                sub_461ea0(*((int *)((int)idx[100] + 4)) + *((int *)&(&g_4b30dc)[v66]));
                                v66 = v12;
                            }
                        } while ((v66 = (int)(v66 + 4), node = v66, v66 < 12));
                    }
                }
                idx = 0;
                do
                {
                    index = (node + v64 * 2 + v64) * 2;
                    if (*((int *)(2 * index + (char *)&g_4b3070[0])) >= 0)
                    {
                        v10 = *((int *)((int)idx[100] + 4)) + *((int *)(2 * index + (char *)&g_4b3070[0]));
                        if (sub_461920(*((int *)sub_462060())))
                            sub_454b80();
                        v64 = *((int *)&g_4b0cb0);
                    }
                } while ((node = (int)(node + 1), node < 3));
            case 15:
                v22 = sub_461920((int)idx[76]);
                if (!v22)
                    *((unsigned int *)&idx[76]) = v22;
                v23 = sub_420e40();
                sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, v23);
                sub_461970((int)idx[76], (int)idx[76]);
                continue;
            case 16:
                v24 = sub_461920((int)idx[80]);
                if (!v24)
                    *((unsigned int *)&idx[80]) = v24;
                v25 = sub_420e40();
                sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, v25);
                sub_461970((int)idx[80], (int)idx[80]);
                continue;
            case 17:
                if (!(int)idx[148])
                {
                    if (!(int)idx[152])
                    {
                        sub_461c50(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, " ");
                        sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, " ");
                        sub_461c50(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, " ");
                        sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, " ");
                        sub_461c50(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 1, " ");
                        sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 1, " ");
                        sub_461c50(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 1, " ");
                        sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 1, " ");
                        *((unsigned int *)&idx[152]) = 1;
                        sub_461970((int)idx[76], v2);
                        v2 = (int)idx[80];
                        sub_461970((int)idx[80], v3);
                        v3 = (int)idx[84];
                        sub_461970((int)idx[84], v4);
                        v4 = (int)idx[88];
                        sub_461970((int)idx[88], v5);
                    }
                    v26 = sub_420e40();
                    if (*((char *)v26) == 124)
                    {
                        v27 = v26 + 1;
                        v28 = sub_46d143(v27);
                        v29 = sub_46d1a0(v27, 44) + 1;
                        idx = sub_46d143(v29);
                        v30 = sub_46d1a0(v29, 44);
                        sub_461c50(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, v28, idx, v30 + 1);
                        sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, v28, idx, v30 + 1);
                        sub_461970((int)idx[84], v2);
                        goto LABEL_420ced;
                    }
                    else
                    {
                        sub_461c50(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, v26);
                        sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, v26);
                        sub_461970((int)idx[76], v2);
                        *((unsigned int *)&idx[148]) = (int)idx[148] + 1;
                        goto LABEL_420ced;
                    }
                }
                else
                {
                    v31 = sub_420e40();
                    if (*((char *)v31) == 124)
                    {
                        v32 = v31 + 1;
                        v33 = sub_46d143(v32);
                        v34 = sub_46d1a0(v32, 44) + 1;
                        idx = sub_46d143(v34);
                        v35 = sub_46d1a0(v34, 44);
                        sub_461c50(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, v33, idx, v35 + 1);
                        sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, v33, idx, v35 + 1);
                        sub_461970((int)idx[88], v2);
                        goto LABEL_420ced;
                    }
                    else
                    {
                        sub_461c50(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, v31);
                        sub_460760(*((int *)((char *)idx + 4 * idx[156] + 160)), 0, 0, 0, v31);
                        sub_461970((int)idx[80], v2);
                        *((unsigned int *)&idx[148]) = 0;
                        *((unsigned int *)&idx[152]) = 0;
                        goto LABEL_420ced;
                    }
                }
            case 18:
                sub_461970((int)idx[76], v2);
                v2 = (int)idx[80];
                sub_461970((int)idx[80], v3);
                v3 = (int)idx[84];
                sub_461970((int)idx[84], v4);
                v4 = (int)idx[88];
                sub_461970((int)idx[88], v5);
                continue;
            case 19:
                sub_430150(1, g_4b452c->field_38);
                sub_4615a0(g_4b43e4->field_6ce4, &v11, 2, 0);
                continue;
            case 20:
                if (*((int *)&g_4b0cb0) - 1 <= 6)
                    goto *((void *)(*((int *)((char *)&g_420e04[*((int *)&g_4b0cb0)] - 4))));
                break;
            case 21:
                sub_421350();
                sub_41d020();
                sub_406ed0();
                if (!g_4b44e8->field_74 && g_4b0ca8 != 4)
                {
                    *((char *)((g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + (*((int *)&g_4b0cb0) + g_4b0ca8 * 6) * 8 + 1449)) = 1;
                    *((char *)((g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + (*((int *)&g_4b0cb0) + g_4b0ca8 * 6) * 8 + 1448)) = 1;
                }
                if (g_4b0ce0 & 16)
                {
                    sub_433870();
                    continue;
                }
                else
                {
                    if (!g_4b44e8->field_74 || !(g_4b4518->field_1c->field_a & 1))
                    {
                        if (*((int *)&g_4b0cb0) == 6)
                        {
                            g_4b43e4->field_6d18 = g_4b43e4->field_6d18 | 32;
                            iter = sub_421880() * 100;
                            switch (g_4b0ca8)
                            {
                            case 0:
                                iter += ((g_4b0ca0 + g_4b0c98 * 2) * 25 + g_4b0c48) * 10000;
                                break;
                            case 1:
                                iter += ((g_4b0ca0 + g_4b0c98 * 2) * 50 + g_4b0c48) * 10000;
                                break;
                            case 2:
                                iter += ((g_4b0ca0 + g_4b0c98 * 2) * 100 + g_4b0c48) * 10000;
                                break;
                            case 3:
                                iter += ((g_4b0ca0 + g_4b0c98 * 2) * 150 + g_4b0c48) * 10000;
                                break;
                            case 4:
                                iter += (g_4b0c98 * 100 + g_4b0c48) * 400000;
                                break;
                            }
                            sub_40e730(iter);
                            g_4b43e4->field_6d48 = g_4b43e4->field_6d48 + iter;
                            if (g_4b4518->field_10 != 1)
                            {
                                g_4b43e4->field_6d18 = g_4b43e4->field_6d18 | 16;
                                *((unsigned int *)&g_4b43e4[1].padding_0[0]) = 0;
                                v69 = (g_4b0c94 + g_4b0c90 * 2) * 4477 + g_4b0ca8;
                                v70 = g_4b451c + v69 * 4 + 1432;
                                if (*((int *)(g_4b451c + v69 * 4 + 1432)) < 99999)
                                {
                                    *((unsigned int *)&v70->padding_0[0]) = *((int *)&v70->padding_0[0]) + 1;
                                    goto LABEL_420ced;
                                }
                            }
                        }
                        else if (*((int *)&g_4b0cb0) == 7)
                        {
                            g_4b43e4->field_6d18 = g_4b43e4->field_6d18 | 32;
                            v71 = sub_421880();
                            v72 = (v71 + ((g_4b0ca0 + g_4b0c98 * 2) * 150 + g_4b0c48) * 100) * 100;
                            sub_40e730(v72);
                            g_4b43e4->field_6d48 = g_4b43e4->field_6d48 + v72;
                            if (g_4b4518->field_10 != 1)
                            {
                                sub_433870();
                                v73 = (g_4b0c94 + g_4b0c90 * 2) * 4477 + g_4b0ca8;
                                v74 = g_4b451c + v73 * 4 + 1432;
                                if (*((int *)(g_4b451c + v73 * 4 + 1432)) < 99999)
                                {
                                    *((unsigned int *)&v74->padding_0[0]) = *((int *)&v74->padding_0[0]) + 1;
                                    goto LABEL_420ced;
                                }
                            }
                        }
                        else
                        {
                            sub_40f720();
                            sub_4217e0();
                            continue;
                        }
                    }
                    sub_432850();
                    continue;
                }
            case 22:
                v1 = v21;
                if (*((int *)&g_4b0cb0) != 6)
                {
                    v75 = v19 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    break;
                }
                else
                {
                    v75 = v19 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    break;
                }
            case 23:
                sub_461970((int)idx[64], v2);
                continue;
            case 24:
                sub_461970((int)idx[0x44], v2);
                continue;
            case 25:
                v36 = sub_461920((int)idx[76]);
                if (!v36)
                    *((unsigned int *)&idx[76]) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v36->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v37 = sub_461920((int)idx[80]);
                if (!v37)
                    *((unsigned int *)&idx[80]) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v37->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v38 = sub_461920((int)idx[84]);
                if (!v38)
                    *((unsigned int *)&idx[84]) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v38->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v39 = sub_461920((int)idx[88]);
                if (!v39)
                    *((unsigned int *)&idx[88]) = 0;
                /* unsupported instruction */
                /* unsupported instruction */
                v39->field_440 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                continue;
            case 26:
                *((unsigned int *)&idx[144]) = (int)idx[144] | 2;
                continue;
            case 27:
                v75 = v19 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = v21;
                /* unsupported instruction */
                v19 = v75 + 1;
                sub_430270((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                goto LABEL_420ced;
            default:
LABEL_420ced:
                v76 = *((char *)((int)idx[100] + 3)) + (int)idx[100] + 4;
                *((void* *)&idx[100]) = v76;
                if ((int)idx[28] < *((short *)v76))
                    goto LABEL_420d09;
                else
                    continue;
            }
        }
    }
LABEL_420d09:
    v77 = (int)idx[28];
    v78 = (int)idx[36];
    *((unsigned int *)&idx[24]) = v77;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v80 = v19;
    if (!((char)(((v80 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v80 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v78)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&idx[28]) = v77 + 1;
            *((int *)&idx[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            return 0;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&idx[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((unsigned int *)&idx[28]) = sub_4931e0();
    return 0;
}
