// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40a250; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40a250_2 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_40a250_2;

typedef struct st_40a250_4 {
    char padding_0[16];
    struct st_40a250_0 *field_10;
} st_40a250_4;

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

typedef struct st_40a250_0 {
    unsigned int field_0;
    unsigned int field_4;
    char field_8;
    char padding_9[963];
    unsigned short field_3cc;
    char padding_3ce[206];
    struct st_40a250_1 *field_49c;
    char padding_4a0[24];
    struct st_40a250_0 *field_4b8;
    unsigned int field_4bc;
    unsigned int field_4c0;
    unsigned int field_4c4;
    char padding_4c8[12];
    unsigned int field_4d4;
    unsigned int field_4d8;
    unsigned int field_4dc;
    unsigned int field_4e0;
    unsigned int field_4e4;
    unsigned int field_4e8;
    unsigned int field_4ec;
    char padding_4f0[4];
    unsigned int field_4f4;
    unsigned int field_4f8;
    unsigned int field_4fc;
    unsigned int field_500;
    char padding_504[4];
    unsigned int field_508;
    char padding_50c[20];
    unsigned int field_520;
    struct st_40a250_2 *field_524;
    unsigned int field_528;
    unsigned int field_52c;
    char padding_530[2];
    unsigned short field_532;
    char padding_534[12];
    unsigned int field_540;
    unsigned int field_544;
    unsigned int field_548;
    struct st_40a250_2 *field_54c;
    unsigned int field_550;
    char field_554;
    char padding_555[1183];
    unsigned short field_9f4;
    unsigned short field_9f6;
    char field_9f8;
    char padding_9f9[1329];
    unsigned short field_f2a;
} st_40a250_0;

typedef struct st_40a250_1 {
    unsigned int field_0;
} st_40a250_1;

extern unsigned int g_4af348[4];
extern unsigned int g_4af34c[4];
extern unsigned int g_4b0bb0[4];
extern char g_4b2ed0;

unsigned int sub_40a250(st_40aa60_1 *a0, void* index, int a2, unsigned int a3, unsigned int a4)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // esi
    unsigned int v17;  // ftop
    unsigned short v18;  // ftop
    unsigned int v19;  // fc3210
    unsigned int v20;  // 4208
    unsigned short v21;  // ftop
    unsigned short v22;  // ftop
    unsigned int v23;  // fc3210
    unsigned int v24;  // 4159
    unsigned int v26;  // 4377
    unsigned int v9;  // edi
    unsigned int v27;  // eax
    unsigned int v28;  // eax
    unsigned int v29;  // ecx
    unsigned int v30;  // edx
    unsigned int v31;  // ecx
    void* i;  // esi
    unsigned int iter;  // edi
    unsigned int v34;  // ecx
    unsigned int v35;  // eax
    unsigned int *v36;  // eax
    st_40a250_0 *iter1;  // ebx
    void* v37;  // ebp
    unsigned int idx;  // esi
    st_40a250_0 *v39;  // ebx
    st_40a250_4 *idx1;  // ebp
    int node;  // edx
    unsigned int v12;  // ftop
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned short v15;  // si
    unsigned int v16;  // ftop
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x4]

    v3 = v7;
    v2 = v8;
    v1 = v9;
    iter1 = *((int *)&a0->padding_8[8]);
    node = 0;
    while (iter1->field_532)
    {
        iter1 = &iter1->field_9f8;
        if (iter1->field_532 == 5)
            iter1 = &a0->padding_8[92];
        if (iter1->field_532)
        {
            iter1 = &iter1->field_9f8;
            if (iter1->field_532 == 5)
                iter1 = &a0->padding_8[92];
            if (iter1->field_532)
            {
                iter1 = &iter1->field_9f8;
                if (iter1->field_532 == 5)
                    iter1 = &a0->padding_8[92];
                if (iter1->field_532)
                {
                    iter1 = &iter1->field_9f8;
                    if (iter1->field_532 == 5)
                        iter1 = &a0->padding_8[92];
                    if (iter1->field_532)
                    {
                        iter1 = &iter1->field_9f8;
                        if (iter1->field_532 == 5)
                            iter1 = &a0->padding_8[92];
                        if (node + 5 >= 2000)
                            return 1;
                    }
                    else
                    {
                        node += 4;
                        break;
                    }
                }
                else
                {
                    node += 3;
                    break;
                }
            }
            else
            {
                node += 2;
                break;
            }
        }
        else
        {
            node += 1;
            break;
        }
    }
    if (node >= 2000)
        return 1;
    /* unsupported instruction */
    /* unsupported instruction */
    index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v13 = v12;
    if ((short)index[506] > 1)
    {
        v14 = v13 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = (short)index[506];
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
    }
    else
    {
        v14 = v13 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v15 = (short)index[508];
    v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v16 = v14 + 1;
    switch (v15)
    {
    case 0: case 1:
        if ((char)index[504] & 1)
        {
            a2 = (a2 + 1) / 2;
            v17 = v16 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            a2 /= 2;
            v17 = v16 - 1;
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
        }
        /* unsupported instruction */
        /* unsupported instruction */
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v16 = v17 + 1;
        if ((char)a2 & 1)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        if (!v15)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        break;
    case 6:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        a2 = sub_464440();
        /* unsupported instruction */
        /* unsupported instruction */
        if (a2 < NULL)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        break;
    case 7:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        a4 = sub_464440();
        /* unsupported instruction */
        /* unsupported instruction */
        if (a4 < NULL)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        a4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (short)index[504];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        break;
    case 8:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        a2 = sub_464440();
        /* unsupported instruction */
        /* unsupported instruction */
        if (a2 < NULL)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        a2 = sub_464440();
        /* unsupported instruction */
        /* unsupported instruction */
        if (a2 < NULL)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        break;
    case 2:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    case 3:
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (short)index[504];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        break;
    case 4:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    case 5:
        a4 = (short)index[504];
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
        index = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        break;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    iter1->field_4d4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_464640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    a0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    iter1->field_4d8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    v18 = v16 - 1;
    /* unsupported instruction */
    /* unsupported instruction */
    iter1->field_4bc = (int)index[4];
    iter1->field_4c0 = (int)index[8];
    iter1->field_4c4 = (int)index[12];
    v19 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (int)index[32]) * 0x100 & 0x4500;
    v20 = _ccall(10, 13, (char)(((v18 & 7) * 0x800 | (unsigned short)v19 & 0x4700) >> 8) & 0x44, 0, 0);
    if (v20 & 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_40d640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        iter1->field_4bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        iter1->field_4c0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v21 = v18 + 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v21 = v18 + 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    iter1->field_4c4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v22 = v21;
    v23 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&a0->padding_8[88])) * 0x100 & 0x4500;
    v24 = _ccall(10, 13, (char)(((v22 & 7) * 0x800 | (unsigned short)v23 & 0x4700) >> 8) & 5, 0, 0);
    if (!(v24 & 1))
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
        /* unsupported instruction */
        /* unsupported instruction */
        v26 = _ccall(10, 13, (char)(((v22 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
        if (!(v26 & 1))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            return 0xffffffff;
        }
    }
    iter1->field_0 = iter1->field_0 | 1;
    iter1->field_532 = 1;
    v27 = iter1->field_4f4;
    if (!((char)iter1->field_4f4 & 1))
    {
        iter1->field_4ec = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        iter1->field_4e8 = 0;
        iter1->field_4e4 = 0xfff0bdc1;
        *((char **)&iter1->padding_4f0[0]) = &g_4b2ed0;
        iter1->field_4f4 = v27 | 1;
    }
    iter1->field_4ec = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    iter1->field_4e8 = 0;
    iter1->field_4e4 = 0xffffffff;
    v28 = iter1->field_508;
    if (!((char)iter1->field_508 & 1))
    {
        iter1->field_500 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        iter1->field_4fc = 0;
        iter1->field_4f8 = 0xfff0bdc1;
        *((char **)&iter1->padding_504[0]) = &g_4b2ed0;
        iter1->field_508 = v28 | 1;
    }
    iter1->field_500 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    iter1->field_4fc = 0;
    /* unsupported instruction */
    iter1->field_4f8 = 0xffffffff;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    iter1->field_4 = 0;
    sub_40d640((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    iter1->field_528 = (int)index[0x200];
    iter1->field_9f6 = (short)index[2];
    v29 = iter1->field_0 & 0xfffffff3;
    iter1->field_9f4 = *((short *)index);
    iter1->field_540 = v30;
    iter1->field_0 = v29 | 2;
    sub_402520((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    *((void* *)&iter1->padding_4a0[20]) = sub_40d430;
    iter1->field_4b8 = iter1;
    sub_454ee0();
    iter1->field_0 = iter1->field_0 & 0xffffffef;
    v31 = iter1->field_0;
    switch (g_4af34c[52 * *((short *)index)])
    {
    case 0:
        iter1->field_524 = (short)index[2] * 2 + 4;
        break;
    case 1:
        iter1->field_524 = g_4b0bb0[(short)index[2]];
        break;
    case 2:
        iter1->field_524 = 0xffffffff;
        iter1->field_0 = v31 | 16;
        break;
    case 3:
        iter1->field_524 = 0x10;
        break;
    case 4:
        iter1->field_524 = 0x6;
        break;
    }
    iter1->field_54c = g_4af348[52 * *((short *)index)];
    iter1->field_544 = (int)index[520];
    iter1->field_520 = 10;
    i = index + 36;
    iter = &iter1->field_550;
    /* unsupported instruction */
    /* unsupported instruction */
    v34 = 108;
    v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    iter1->field_4e0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    iter1->field_4dc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    iter1->field_52c = (int)index[0x200];
    iter1->field_528 = 0;
    v35 = (int)index[524];
    for (iter1->field_548 = v35; v34; i += 4)
    {
        v34 -= 1;
        *((int *)iter) = *((int *)i);
        iter += 4;
    }
    v36 = &iter1->field_49c->field_0;
    v37 = index + v35 * 24;
    idx = &iter1->field_8;
    if ((int)v37[52] == 2)
    {
        if (v36)
            v36();
        *((unsigned short *)(idx + 964)) = (short)v37[44] + 7;
        iter1->field_532 = 2;
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
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        iter1->field_4bc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        iter1->field_4c0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        iter1->field_4c4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        iter1->field_548 = iter1->field_548 + 1;
    }
    else
    {
        if (v36)
            v36();
        *((unsigned short *)(idx + 964)) = 2;
    }
    sub_40aa60(iter1);
    sub_455630(&iter1->field_8);
    v39 = &iter1->field_9f8;
    if (v39->field_532 == 5)
        idx1->field_10 = idx1 + 5;
    else
        idx1->field_10 = v39;
    return 0;
}
