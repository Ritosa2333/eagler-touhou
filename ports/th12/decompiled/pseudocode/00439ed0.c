// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x439ed0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_439ed0_2 {
    char padding_0[16];
    unsigned int field_10;
    char padding_14[2588];
    unsigned int field_a30;
    char padding_a34[35];
    char field_a57;
    char field_a58;
    char padding_a59[23];
    char field_a70;
    char padding_a71[3];
    unsigned int field_a74;
    char padding_a78[12];
    unsigned int field_a84;
    char padding_a88[24];
    unsigned int field_aa0;
    struct st_439ed0_2 *field_aa4;
    char padding_aa8[8];
    unsigned int field_ab0;
    unsigned int field_ab4;
    int field_ab8;
    char padding_abc[16];
    struct st_439ed0_0 *field_acc;
    char field_ad0;
    char padding_ad1[23];
    char field_ae8;
} st_439ed0_2;

typedef struct st_439ed0_8 {
    char padding_0[35216];
    unsigned int field_8990;
    char padding_8994[64];
    unsigned int field_89d4;
    char padding_89d8[16];
    unsigned int field_89e8;
    int field_89ec;
    char padding_89f0[4];
    unsigned int field_89f4;
    unsigned int field_89f8;
} st_439ed0_8;

typedef struct st_439ed0_1 {
    unsigned int field_0;
} st_439ed0_1;

typedef struct st_439ed0_0 {
    char padding_0[29];
    char field_1d;
    char padding_1e[2];
    unsigned short field_20;
    char padding_22[14];
    struct st_439ed0_1 *field_30;
} st_439ed0_0;

typedef struct st_439ed0_7 {
    char padding_0[44];
    unsigned int field_2c;
    char padding_30[1100];
    unsigned int field_47c;
} st_439ed0_7;

typedef struct st_439ed0_6 {
    char padding_0[60];
    unsigned int field_3c;
} st_439ed0_6;

extern int g_4b0c44;
extern st_439ed0_6 *g_4b43c4;
extern st_439ed0_2 *g_4b4514;

int sub_439ed0(st_439ed0_2 *a0, int a1)
{
    st_439ed0_2 *v22;  // ebx
    st_439ed0_2 *v23;  // ebx
    unsigned int v32;  // ftop
    unsigned int v34;  // 4167
    unsigned int v38;  // 4133
    unsigned int v39;  // ftop
    unsigned int v41;  // 4139
    st_439ed0_2 *v24;  // esi
    st_439ed0_0 *v42;  // edx
    unsigned int v43;  // esi
    st_439ed0_2 *v44;  // edx
    st_439ed0_0 *v45;  // eax
    st_439ed0_2 *v46;  // edi
    unsigned int v47;  // edx
    st_439ed0_7 *idx;  // eax
    unsigned int v49;  // ftop
    st_439ed0_8 *node;  // edi
    unsigned int v51;  // esi
    unsigned int iter;  // edi
    unsigned int v52;  // esi
    unsigned int v53;  // fc3210
    unsigned int v54;  // ftop
    unsigned int v55;  // 4144
    unsigned int v57;  // ftop
    unsigned int v58;  // 4215
    unsigned int v61;  // 4215
    unsigned int v26;  // ftop
    unsigned int v64;  // ftop
    unsigned int v65;  // ftop
    unsigned int v66;  // 4643
    unsigned int v68;  // ftop
    unsigned int v69;  // 4219
    unsigned int v71;  // ftop
    unsigned int v27;  // ftop
    unsigned int v72;  // ftop
    unsigned int v73;  // 4268
    unsigned int v75;  // 4210
    int v77;  // esi
    unsigned int v78;  // edx
    st_439ed0_0 *v28;  // edx
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v0;  // [bp-0x78]
    st_439ed0_2 *v1;  // [bp-0x6c]
    st_439ed0_2 *v2;  // [bp-0x60]
    st_439ed0_2 *v3;  // [bp-0x5c], Other Possible Types: int
    st_439ed0_8 *v4;  // [bp-0x58]
    unsigned int v5;  // [bp-0x54]
    st_439ed0_2 *v6;  // [bp-0x50], Other Possible Types: unsigned int
    st_439ed0_2 *v7;  // [bp-0x4c]
    st_439ed0_2 *v8;  // [bp-0x4c]
    unsigned int v9;  // [bp-0x48]
    unsigned int v10;  // [bp-0x44]
    unsigned int v11;  // [bp-0x40]
    unsigned int v12;  // [bp-0x3c]
    unsigned int v13;  // [bp-0x38]
    unsigned int v14;  // [bp-0x34]
    unsigned int v15;  // [bp-0x30]
    unsigned int v16;  // [bp-0x2c]
    unsigned int v17;  // [bp-0x28]
    unsigned int v18;  // [bp-0x1c]
    unsigned int v19;  // [bp-0x18]
    unsigned int v20;  // [bp-0x10]
    unsigned int v21;  // [bp-0xc]

    v3 = v22;
    v23 = NULL;
    v6 = g_4b4514;
    v5 = 0;
    if (*((int *)&g_4b4514->padding_a34[0]) == g_4b4514->field_a30)
        return 0;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v24 = &g_4b4514->field_a58;
    /* unsupported instruction */
    /* unsupported instruction */
    v8 = v24;
    /* unsupported instruction */
    /* unsupported instruction */
    iter = &v24->padding_14[4];
    v9 = 0x100;
    /* unsupported instruction */
    /* unsupported instruction */
    v20 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v21 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v18 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
    v27 = v26 - 3;
    /* unsupported instruction */
    /* unsupported instruction */
    while (1)
    {
        if (*((int *)(iter + 48)) && *((int *)(iter + 48)) != 2)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v28 = *((int *)(iter + 92));
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v16 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v29 = v27;
            if (v28->field_1d == 2)
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
                v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                v17 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v30 = v29 + 1;
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v30 = v29 + 1;
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v32 = v30;
            if ((char)((((unsigned short)v32 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v34 = _ccall(10, 13, (char)((((unsigned short)v32 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v34 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if ((char)((((unsigned short)v32 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        if ((char)((((unsigned short)v32 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65)
                        {
                            if (v28->field_1d != 2)
                            {
                                v38 = _ccall(10, 13, (char)((((unsigned short)v32 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                if (v38 & 1)
                                {
                                    if (v28->field_1d == 2)
                                        goto LABEL_43a041;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                    v39 = v32 + 2;
LABEL_43a054:
                                    if (!v28->field_30 || !(v1 = a0, v1 = a0, v28->field_30(v1)))
                                    {
                                        v42 = *((int *)(iter + 92));
                                        *((unsigned int *)(iter + 64)) = 1;
                                        if (v42->field_1d == 2)
                                        {
                                            if (!*((int *)(iter + 0x44)))
                                            {
                                                v1 = *((int *)(iter + 52));
                                                sub_461970(*((int *)(iter + 52)));
                                                v23 = v3;
                                                *((unsigned int *)(iter + 0x44)) = 1;
                                            }
                                        }
                                        else
                                        {
                                            if (v42->field_1d != 4)
                                                goto LABEL_43a0b2;
                                        }
                                        if (!sub_407700(4))
                                            goto LABEL_43a0b9;
LABEL_43a0b2:
                                        v23 = &v23->padding_0[*((int *)(iter + 72))];
                                        v2 = v23;
LABEL_43a0b9:
                                        if (*((char *)(*((int *)(iter + 92)) + 29)) != 2)
                                        {
                                            v1 = v1;
                                            if (*((char *)(*((int *)(iter + 92)) + 29)) != 4)
                                            {
                                                v43 = iter + 52;
                                                sub_461c50();
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                v44 = *((int *)v43);
                                                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                                /* unsupported instruction */
                                                sub_461a70(v44);
                                                *((st_439ed0_2 **)v43) = NULL;
                                                v45 = *((int *)(iter + 92));
                                                v0 = 0;
                                                if (v45->field_1d != 3)
                                                {
                                                    sub_4615a0(v2->field_10, &v5, v45->field_20 + 5);
                                                    *((st_439ed0_2 **)v43) = v46;
                                                }
                                                else if (g_4b43c4->field_3c)
                                                {
                                                    sub_4615a0(v2->field_10, &v6, v45->field_20 + 6);
                                                    *((st_439ed0_2 **)v43) = v2;
                                                    v47 = 1431655766 * *((int *)(iter + 72)) >> 32;
                                                    v44 = &v44->padding_0[v47 + (v47 >> 31)];
                                                }
                                                else
                                                {
                                                    sub_4615a0(v2->field_10, &v8, v45->field_20 + 5);
                                                    *((st_439ed0_2 **)v43) = v3;
                                                }
                                                idx = sub_461c50();
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                idx->field_47c = idx->field_47c | 4;
                                                idx->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                *((int *)(iter + 4)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                *((unsigned int *)(iter + 48)) = 2;
                                                /* unsupported instruction */
                                                /* unsupported instruction */
                                                *((int *)(iter + 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                                                /* unsupported instruction */
                                                sub_422c10();
                                                v24 = v1;
                                                v23 = v44;
                                                goto LABEL_43a1dd;
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
LABEL_43a041:
                                /* unsupported instruction */
                                /* unsupported instruction */
                                v39 = v32 + 2;
                                v41 = _ccall(10, 13, (char)((((unsigned short)v39 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                                if (v41 & 1)
                                    goto LABEL_43a054;
                                goto LABEL_43a1dd;
                            }
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
            v32 = v27 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v39 = v32 + 2;
LABEL_43a1dd:
        v24 = &v24->padding_14[100];
        iter += 120;
        v7 = (char *)v8 - 1;
        v6 = v24;
        if (v8 == 1)
            break;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v27 = v39 - 3;
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
        v8 = v7;
    }
    v49 = v39 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = &v23->padding_0[sub_406de0(a1)];
    node = v4->padding_89d8;
    v51 = 128;
    do
    {
        v52 = v51;
        if (!((char)*((int *)&node->padding_0[32]) & 1) || *((int *)&node->padding_0[0]) != *((int *)(&node->padding_0[0] - 4)) && !(*((int *)&node->padding_0[0]) % *((int *)&node->padding_0[28])))
            continue;
        if (!((char)*((int *)&node->padding_0[32]) & 2))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v53 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)(&node->padding_0[0] - 72))) * 0x100 & 0x4500;
            /* unsupported instruction */
            v54 = v49;
            v55 = _ccall(10, 13, (char)((((unsigned short)v54 & 7) * 0x800 | (unsigned short)v53 & 0x4700) >> 8) & 0x44, 0, 0);
            if (!(v55 & 1))
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
                v57 = v54 - 1;
                v58 = _ccall(10, 13, (char)((((unsigned short)v57 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v58 & 1)
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v49 = v57 + 1;
                    if (!((char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                        continue;
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
                    v57 = v49 - 1;
                    v61 = _ccall(10, 13, (char)((((unsigned short)v57 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                    if (v61 & 1)
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v49 = v57 + 1;
                        if (!((char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                            continue;
                        goto LABEL_43a3d1;
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
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4938c0();
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4939f0();
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
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
                v64 = v54 - 3;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v65 = v64 + 1;
                v66 = _ccall(10, 13, (char)((((unsigned short)v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (!(v66 & 1))
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v49 = v65 + 2;
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
                /* unsupported instruction */
                /* unsupported instruction */
                v68 = v65 + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v49 = v68 + 1;
                v69 = _ccall(10, 13, (char)((((unsigned short)v68 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (!(v69 & 1))
                    continue;
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
                v71 = v49 - 3;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v72 = v71 + 1;
                v73 = _ccall(10, 13, (char)((((unsigned short)v71 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v73 & 1)
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
                    v49 = v72 + 2;
                    v75 = _ccall(10, 13, (char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                    if (!(v75 & 1))
                        continue;
                    goto LABEL_43a3d1;
                }
                else
                {
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v57 = v72 + 1;
                }
            }
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v49 = v57 + 1;
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!((char)((((unsigned short)v49 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                continue;
LABEL_43a3d1:
            v3 += *((int *)&node->padding_0[16]);
            *((int *)&node->padding_0[20]) = *((int *)&node->padding_0[20]) + *((int *)&node->padding_0[16]);
            if (*((int *)&node->padding_0[24]) <= *((int *)&node->padding_0[20]))
                *((unsigned int *)&node->padding_0[16]) = 0;
        }
    } while ((node += 116, v51 = v52 - 1, v52 != 1));
    v77 = v3;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (v77 >= 80)
    {
        v3 = 80;
        v77 = v3;
    }
    else if (!v77)
    {
        return v77;
    }
    v78 = 1717986919 * ((int)(1717986919 * v77 >> 0x22) + ((unsigned int)((int)(1717986919 * v77 >> 0x22)) >> 31) + 10) >> 0x22;
    g_4b0c44 = g_4b0c44 + (v78 >> 31) + v78;
    if (g_4b0c44 < 1000000000)
        return v77;
    g_4b0c44 = 0x3b9ac9ff;
    return v77;
}
