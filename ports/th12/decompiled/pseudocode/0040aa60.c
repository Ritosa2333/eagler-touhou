// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40aa60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40aa60_3 {
    char padding_0[1368];
    char field_558[2];
    char field_559;
    char field_55a;
    char padding_55b[1];
    struct st_40aa60_0 *field_55c;
    char padding_560[16];
    unsigned short field_570;
    char padding_572[2];
    unsigned int field_574;
} st_40aa60_3;

typedef struct st_40aa60_0 {
    unsigned int field_0;
    char field_1;
    char padding_2[2];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_40aa60_0;

typedef struct st_40aa60_1 {
    struct st_40aa60_0 *field_0;
    struct st_40aa60_0 *field_4;
    char padding_8[964];
    unsigned short field_3cc;
    char padding_3ce[182];
    unsigned int field_484;
    char padding_488[20];
    struct st_40aa60_0 *field_49c;
    char padding_4a0[24];
    struct st_40aa60_1 *field_4b8;
    unsigned int field_4bc;
    unsigned int field_4c0;
    unsigned int field_4c4;
    char padding_4c8[12];
    unsigned int field_4d4;
    unsigned int field_4d8;
    unsigned int field_4dc;
    unsigned int field_4e0;
    char padding_4e4[40];
    struct st_40aa60_0 *field_50c;
    char padding_510[20];
    struct st_40aa60_0 *field_524;
    unsigned int field_528;
    char padding_52c[28];
    struct st_40aa60_0 *field_548;
    char padding_54c[4];
    unsigned int field_550;
    char padding_554[464];
    unsigned int field_724;
    char padding_728[32];
    unsigned int field_748;
    unsigned int field_74c;
    char padding_750[12];
    struct st_40aa60_0 *field_75c;
    char padding_760[28];
    unsigned int field_77c;
    unsigned int field_780;
    char padding_784[12];
    struct st_40aa60_0 *field_790;
    char padding_794[28];
    unsigned int field_7b0;
    unsigned int field_7b4;
    char padding_7b8[12];
    struct st_40aa60_0 *field_7c4;
    struct st_40aa60_0 *field_7c8;
    unsigned int field_7cc;
    char padding_7d0[20];
    unsigned int field_7e4;
    char padding_7e8[16];
    unsigned int field_7f8;
    struct st_40aa60_0 *field_7fc;
    struct st_40aa60_0 *field_800;
    char padding_804[176];
    unsigned int field_8b4;
    unsigned int field_8b8;
    char padding_8bc[12];
    struct st_40aa60_0 *field_8c8;
    char padding_8cc[28];
    unsigned int field_8e8;
    char padding_8ec[4];
    unsigned int field_8f0;
    unsigned int field_8f4;
    unsigned int field_8f8;
    struct st_40aa60_0 *field_8fc;
    unsigned int field_900;
    char padding_904[4];
    unsigned int field_908;
    unsigned int field_90c;
    unsigned int field_910;
    char padding_914[4];
    struct st_40aa60_0 *field_918;
    unsigned int field_91c;
    unsigned int field_920;
    char padding_924[8];
    unsigned int field_92c;
    struct st_40aa60_0 *field_930;
    char padding_934[28];
    unsigned int field_950;
    unsigned int field_954;
    char padding_958[12];
    struct st_40aa60_0 *field_964;
    char padding_968[48];
    struct st_40aa60_0 *field_998;
    char padding_99c[12];
    unsigned int field_9a8;
    unsigned int field_9ac;
    unsigned int field_9b0;
    unsigned int field_9b4;
    unsigned int field_9b8;
    unsigned int field_9bc;
    unsigned int field_9c0;
    unsigned int field_9c4;
    unsigned int field_9c8;
    unsigned int field_9cc;
    unsigned int field_9d0;
    unsigned int field_9d4;
    char padding_9d8[20];
    struct st_40aa60_0 *field_9ec;
    unsigned int field_9f0;
    short field_9f4;
    short field_9f6;
} st_40aa60_1;

extern char g_400000;
extern unsigned int g_4af34c[4];
extern st_40aa60_0 g_4b0bb0;
extern char g_4b2ed0;
extern unsigned int g_4ce8d0;
extern unsigned int g_4ce8d4;
extern unsigned int g_4ce8d8;

st_40aa60_0 * sub_40aa60(st_40aa60_1 *idx)
{
    unsigned int v24;  // ebx
    unsigned int v25;  // esi
    unsigned int v33;  // ecx
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // 4197
    unsigned int v38;  // ftop
    unsigned int v40;  // ftop
    unsigned int v42;  // ftop
    unsigned int v26;  // edi
    unsigned int v43;  // ftop
    unsigned int v44;  // 4167
    unsigned int v45;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    st_40aa60_0 *v50;  // eax
    unsigned short v51;  // cx
    unsigned short v52;  // dx
    st_40aa60_3 *idx1;  // edi
    st_40aa60_0 *v54;  // ax
    st_40aa60_1 *i;  // esi
    unsigned int v56;  // ecx
    unsigned int *iter;  // edi
    st_40aa60_1 *idx2;  // esi
    st_40aa60_0 *v59;  // eax
    unsigned int v61;  // ftop
    unsigned int v62;  // ftop
    unsigned int v27;  // ftop
    unsigned int v64;  // ftop
    unsigned int v66;  // edx
    unsigned int v67;  // ecx
    unsigned int v68;  // edx
    int v28;  // esi
    st_40aa60_0 *v75;  // eax
    unsigned int v29;  // ftop
    st_40aa60_0 *v30;  // eax
    unsigned int v31;  // edx
    st_40aa60_3 *index;  // edi
    unsigned int v0;  // [bp-0x240]
    unsigned int v1;  // [bp-0x23c]
    unsigned int v2;  // [bp-0x234]
    unsigned int v3;  // [bp-0x230]
    char *v4;  // [bp-0x22c]
    unsigned int v5;  // [bp-0x228]
    unsigned int v6;  // [bp-0x224]
    unsigned short v7;  // [bp-0x220]
    unsigned short v8;  // [bp-0x21e]
    unsigned int v9;  // [bp-0x21c]
    unsigned int v10;  // [bp-0x218]
    unsigned int v11;  // [bp-0x214]
    unsigned int v12;  // [bp-0x210]
    unsigned int v13;  // [bp-0x20c]
    unsigned int v14;  // [bp-0x208]
    unsigned int v15;  // [bp-0x204]
    char v16;  // [bp-0x1fc]
    st_40aa60_0 *v17;  // [bp-0x28]
    unsigned short v18;  // [bp-0x26]
    unsigned short v19;  // [bp-0x24]
    unsigned int v20;  // [bp-0x20]
    unsigned int v21;  // [bp-0x14]
    char v22;  // [bp-0x4]

    v5 = v24;
    v4 = &v22;
    v3 = v25;
    v2 = v26;
    if (idx->field_548 >= 18)
        return v75;
    v28 = idx->field_548;
    /* unsupported instruction */
    /* unsupported instruction */
    v29 = v27 - 2;
    /* unsupported instruction */
    /* unsupported instruction */
    v30 = v28 * 3;
    v31 = *((int *)((char *)idx + 0x8 * v30 + 1376));
    index = (char *)idx + 0x8 * v30 + 1360;
    if (v31 && (*((int *)&index->padding_0[20]) || !idx->field_528) && (v30 = (st_40aa60_0 *)idx->field_528, !(v30 & v31)))
    {
        if (v31 == 0x80000000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v27 = v29 + 2;
            idx->field_548 = v28 + 1;
        }
        v33 = v31;
        if (v33 <= 0x4000)
        {
            if (v33 == 0x4000)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v27 = v29 + 2;
                v1 = v33;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v30 = sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                idx->field_548 = (char *)&idx->field_548->field_0 + 1;
            }
            else if (v33 > 0x100)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v27 = v29 + 2;
                if (v33 <= 0x800)
                {
                    if (v33 != 0x800)
                    {
                        if (v33 != 0x200)
                        {
                            if (v33 == 0x400)
                            {
                                idx->field_528 = v30 | v31;
                                sub_4067e0(*((int *)&index->padding_0[8]));
                                idx->field_998 = *((int *)&index->padding_0[12]);
                                idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            }
                            else
                            {
                                idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            }
                        }
                        else
                        {
                            idx->field_4 = *((int *)&index->padding_0[8]);
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                        }
                    }
                    else
                    {
                        idx->field_9f4 = *((short *)&index->padding_0[8]);
                        idx->field_9f6 = *((short *)&index->padding_0[12]);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        idx->field_4e0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        idx->field_4dc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        sub_402520();
                        *((void* *)&idx->padding_4a0[20]) = sub_40d430;
                        idx->field_4b8 = idx;
                        sub_454ee0();
                        idx->field_0 = idx->field_0 & 0xffffffef;
                        v30 = idx->field_0;
                        switch (g_4af34c[52 * idx->field_9f4])
                        {
                        case 0:
                            idx->field_524 = idx->field_9f6 * 2 + 4;
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            break;
                        case 1:
                            idx->field_524 = (&g_4b0bb0.field_0)[idx->field_9f6];
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            break;
                        case 2:
                            v30 |= 16;
                            idx->field_524 = 0xffffffff;
                            idx->field_0 = v30;
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            break;
                        case 3:
                            idx->field_524 = 0x10;
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            break;
                        case 4:
                            idx->field_524 = 0x6;
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            break;
                        case 5:
                            idx->field_524 = 0xc;
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            break;
                        default:
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                            break;
                        }
                    }
                }
                else
                {
                    if (v33 != 0x1000)
                    {
                        if (v33 == 0x2000)
                        {
                            v30 = sub_40c8b0();
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                        }
                        else
                        {
                            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                        }
                    }
                    else
                    {
                        idx->field_528 = v30 | v31;
                        v30 = sub_4067e0(*((int *)&index->padding_0[8]));
                        idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                    }
                }
            }
            else if (v33 != 0x100)
            {
                switch (v33)
                {
                case 1:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_528 = v30 | 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v27 = v29 + 2;
                    v30 = sub_4067e0(0);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_724 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                    break;
                case 4:
                    idx->field_528 = v30 | 4;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_748 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v35 = v29;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v36 = v35 + 1;
                    v37 = _ccall(10, 13, (char)((((unsigned short)v35 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                    if (!(v37 & 1))
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v38 = v36 - 0;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v38 = v36 + 1;
                        if (!((((unsigned short)v38 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                        {
                            sub_4377a0();
                        }
                        else
                        {
                            v38 -= 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                    }
                    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_74c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    sub_4067e0(0);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    idx->field_75c = *((int *)&index->padding_0[8]);
                    v40 = v38 - 0;
                    /* unsupported instruction */
                    /* unsupported instruction */
                case 8:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_528 = v30 | 8;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_77c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v27 = v29 + 2;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_780 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    sub_4067e0(0);
                    idx->field_790 = *((int *)&index->padding_0[8]);
                    if (!idx->field_548)
                    {
                        idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                        break;
                    }
                    else if (*((int *)&idx->padding_52c[24]) >= 0)
                    {
                        v30 = sub_453d90();
                        idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                        break;
                    }
                    else
                    {
                        idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                        break;
                    }
                case 16: case 32: case 64:
                    idx->field_528 = v30 | v31;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v42 = v29;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v43 = v42 + 1;
                    v44 = _ccall(10, 13, (char)((((unsigned short)v42 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                    if (!(v44 & 1))
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v45 = v43 - 0;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v45 = v43 + 1;
                        if (!((((unsigned short)v45 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
                        {
                            sub_4377a0();
                        }
                        else
                        {
                            v45 -= 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                    }
                    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_7b4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v48 = v45 + 1;
                    if (!((char)((((unsigned short)v48 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc08f380000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    {
                        v49 = v48 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v49 = v48 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v27 = v49 + 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_7b0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v30 = sub_4067e0(0);
                    idx->field_7c4 = *((int *)&index->padding_0[8]);
                    idx->field_7c8 = *((int *)&index->padding_0[12]);
                    idx->field_7cc = 0;
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                    break;
                default:
LABEL_40b4f7:
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v27 = v29 + 2;
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                    break;
                }
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_528 = v30 | v31;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v27 = v29 + 2;
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_7e4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v30 = *((int *)&index->padding_0[8]);
                idx->field_7fc = v30;
                idx->field_7f8 = 0;
                idx->field_800 = *((int *)&index->padding_0[12]);
                idx->field_548 = (char *)&idx->field_548->field_0 + 1;
            }
        }
        if (v33 <= 0x1000000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v27 = v29 + 2;
            if (v33 == 0x1000000)
            {
                idx->field_9f4 = *((short *)&index->padding_0[8]);
                idx->field_9f6 = *((short *)&index->padding_0[12]);
                idx2 = idx->padding_8;
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_4e0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                idx->field_4dc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                sub_402520();
                *((void* *)&idx->padding_4a0[20]) = sub_40d430;
                idx->field_4b8 = idx;
                sub_454ee0();
                idx->field_0 = idx->field_0 & 0xffffffef;
                v59 = idx->field_0;
                switch (g_4af34c[52 * idx->field_9f4])
                {
                case 0:
                    idx->field_524 = idx->field_9f6 * 2 + 4;
                    break;
                case 1:
                    idx->field_524 = (&g_4b0bb0.field_0)[idx->field_9f6];
                    break;
                case 2:
                    idx->field_524 = 0xffffffff;
                    idx->field_0 = v59 | 16;
                    break;
                case 3:
                    idx->field_524 = 0x10;
                    break;
                case 4:
                    idx->field_524 = 0x6;
                    break;
                case 5:
                    idx->field_524 = 0xc;
                    break;
                }
                v30 = *((int *)&idx2->padding_488[12]);
                if (v30)
                    v30 = v30();
                *((unsigned short *)&idx2->padding_8[956]) = 2;
                idx->field_548 = (char *)&idx->field_548->field_0 + 1;
            }
            else if (v33 <= 0x200000)
            {
                if (v33 == 0x200000)
                {
                    idx->field_50c = *((int *)&index->padding_0[8]);
                    idx->field_548 = v28 + 1;
                }
                else if (v33 == 0x20000)
                {
                    idx->field_528 = v30 | v31;
                    v30 = sub_4067e0(*((int *)&index->padding_0[8]));
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                }
                else if (v33 == 0x40000)
                {
                    idx->field_528 = v30 | v31;
                    v30 = sub_4067e0(*((int *)&index->padding_0[8]));
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                }
                else if (v33 == 0x80000)
                {
                    sub_409400();
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v9 = idx->field_4bc;
                    v10 = idx->field_4c0;
                    v50 = *((int *)&index->padding_0[8]);
                    v11 = idx->field_4c4;
                    v51 = index->padding_0[10];
                    v19 = v50 >> 24 & 127;
                    v52 = index->padding_0[9];
                    idx1 = &index->padding_0[24];
                    v21 = v50 & 0xff;
                    v54 = (st_40aa60_0 *)*((short *)(&idx1->padding_0[0] - 12));
                    idx->field_548 = v28 + 1;
                    v7 = v51;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v8 = v52;
                    v13 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    v18 = *((short *)&idx1->padding_0[8]);
                    v17 = v54;
                    v20 = *((int *)&idx1->padding_0[12]);
                    i = &idx->field_550;
                    v56 = 108;
                    for (iter = &v16; v56; i = &i->field_4)
                    {
                        v56 -= 1;
                        *(iter) = i->field_0;
                        iter += 1;
                    }
                    v30 = sub_40b5e0();
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                    if (v50 & 0x80000000)
                    {
                        v30 = sub_40c8b0();
                        idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                    }
                }
                else
                {
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                }
            }
            else
            {
                if (v33 == &g_400000)
                {
                    idx->field_548 = *((int *)&index->padding_0[8]);
                }
                else if (v33 == 0x800000)
                {
                    idx->field_528 = v30 | 0x800000;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_8b4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    idx->field_8b8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                    /* unsupported instruction */
                    sub_4067e0(0);
                    idx->field_8c8 = *((int *)&index->padding_0[8]);
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                }
                else
                {
                    idx->field_548 = (char *)&idx->field_548->field_0 + 1;
                }
            }
        }
        if (v33 > 0x8000000)
            goto LABEL_0x40b450;
        if (v33 == 0x8000000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_528 = v30 | 0x8000000;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v27 = v29 + 2;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            sub_40d640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_92c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_920 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_91c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx->field_930 = *((int *)&index->padding_0[8]);
            v30 = idx->field_918;
            if (!(*((char *)&v30) & 1))
            {
                v30 |= 1;
                idx->field_910 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                idx->field_90c = 0;
                idx->field_908 = 0xfff0bdc1;
                *((char **)&idx->padding_914[0]) = &g_4b2ed0;
                idx->field_918 = v30;
            }
            idx->field_910 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx->field_90c = 0;
            idx->field_908 = 0xffffffff;
            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
        }
        if (v33 == 0x2000000)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_528 = v30 | 0x2000000;
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_8f0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_8f4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v27 = v29 + 2;
            if (*((int *)&index->padding_0[12]) & 0x100)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_8f0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_8f4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_8f8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_8e8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_8f8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            idx->field_8fc = *((int *)&index->padding_0[8]);
            idx->field_900 = *((int *)&index->padding_0[12]) & 0xff;
            sub_4067e0(0);
            v66 = idx->field_4c0;
            idx->field_9a8 = idx->field_4bc;
            v67 = idx->field_4c4;
            idx->field_9ac = v66;
            idx->field_9b4 = idx->field_8f0;
            v68 = idx->field_8f8;
            idx->field_9b0 = v67;
            idx->field_9b8 = idx->field_8f4;
            idx->field_9bc = v68;
            idx->field_9c0 = g_4ce8d0;
            idx->field_9c4 = g_4ce8d4;
            idx->field_9c8 = g_4ce8d8;
            idx->field_9cc = g_4ce8d0;
            idx->field_9d0 = g_4ce8d4;
            idx->field_9d4 = g_4ce8d8;
            idx->field_9ec = *((int *)&index->padding_0[8]);
            idx->field_9f0 = *((int *)&index->padding_0[12]) & 0xff;
            v30 = sub_406340(0);
            idx->field_548 = (char *)&idx->field_548->field_0 + 1;
        }
        if (v33 != 0x4000000)
            goto LABEL_40b4f7;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v61 = v29 + 1;
        if (!((((unsigned short)v61 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            sub_4377a0();
            /* unsupported instruction */
            v62 = v61 + 2;
            sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v64 = v61 + 1;
            if ((char)((((unsigned short)v64 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 1)
                goto LABEL_40b22a;
            v62 = v64 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        idx->field_4d8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v64 = v62 + 1;
LABEL_40b22a:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v27 = v64;
        if (!((((unsigned short)v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc08ef00000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            idx->field_4d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v30 = sub_40d640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        idx->field_548 = (char *)&idx->field_548->field_0 + 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return v30;
}
