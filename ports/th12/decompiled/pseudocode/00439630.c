// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x439630; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_439630_2 {
    char padding_0[16];
    int field_10;
    char padding_14[2628];
    unsigned int field_a58;
    unsigned int field_a5c;
    unsigned int field_a60;
    char padding_a64[4];
    unsigned int field_a68;
    unsigned int field_a6c;
    unsigned int field_a70;
    unsigned int field_a74;
    char padding_a78[8];
    unsigned int field_a80;
    unsigned int field_a84;
    unsigned int field_a88;
    unsigned int field_a8c;
    char padding_a90[12];
    char field_a9c;
    char padding_a9d[3];
    unsigned int field_aa0;
    unsigned int field_aa4;
    unsigned int field_aa8;
    char padding_aac[12];
    unsigned int field_ab8;
    char padding_abc[4];
    unsigned int field_ac0;
    unsigned int field_ac4;
    char padding_ac8[4];
    struct st_439630_0 *field_acc;
} st_439630_2;

typedef struct st_439630_0 {
    char padding_0[2];
    unsigned short field_2;
    char padding_4[24];
    char field_1c;
    char field_1d;
    unsigned short field_1e;
    char padding_20[2];
    unsigned short field_22;
    struct st_439630_1 *field_24;
} st_439630_0;

typedef struct st_439630_1 {
    unsigned int field_0;
} st_439630_1;

typedef struct st_439630_6 {
    struct st_439630_5 *field_0;
    struct st_439630_6 *field_4;
} st_439630_6;

typedef struct st_439630_5 {
    char padding_0[44];
    unsigned int field_2c;
    char padding_30[1024];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[64];
    unsigned int field_47c;
} st_439630_5;

extern char g_4b2ed0;
extern unsigned int g_4ce8cc;

unsigned int sub_439630(unsigned int *idx, unsigned int a1, st_439630_0 *a2)
{
    unsigned int v9;  // esi
    st_439630_2 *v10;  // ebx
    unsigned short v19;  // ftop
    unsigned int v20;  // ecx
    unsigned int v23;  // ecx
    int v24;  // edx
    unsigned int v25;  // edi
    unsigned int v26;  // ecx
    st_439630_2 *iter;  // edi
    st_439630_6 *node;  // eax
    st_439630_5 *index;  // esi
    st_439630_6 *iter1;  // eax
    unsigned int v30;  // eax
    int v12;  // eax
    unsigned int v13;  // eax
    unsigned short v14;  // ftop
    unsigned short v15;  // ftop
    unsigned short v16;  // ftop
    unsigned int v17;  // ecx
    unsigned int v0;  // [bp-0x54]
    unsigned int v1;  // [bp-0x2c]
    unsigned int v2;  // [bp-0x28]
    unsigned int v3;  // [bp-0x24]
    unsigned int v4;  // [bp-0x20]
    unsigned int v5;  // [bp-0x18]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]
    int v8;  // [bp-0x4], Other Possible Types: unsigned int

    v5 = v9;
    iter = &v10->field_a58;
    if (a2->field_1d == 2 && *((int *)&v10[18].padding_14[380 + 4 * a2->field_1c]))
        return 0;
    v12 = 0;
    while (*((int *)&iter->padding_14[52]))
    {
        iter = &iter->padding_14[100];
        if (v12 + 1 >= 0x100)
            return 0;
    }
    v13 = iter->field_10;
    /* unsupported instruction */
    /* unsupported instruction */
    *((unsigned int *)&iter->padding_14[52]) = 1;
    *((st_439630_0 **)&iter->padding_14[96]) = a2;
    if (!((char)v13 & 1))
    {
        *((int *)&iter->padding_0[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&iter->padding_0[4]) = 0;
        *((unsigned int *)&iter->padding_0[0]) = 0xfff0bdc1;
        *((char **)&iter->padding_0[12]) = &g_4b2ed0;
        iter->field_10 = v13 | 1;
    }
    *((int *)&iter->padding_0[8]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    *((unsigned int *)&iter->padding_0[4]) = 0;
    *((unsigned int *)&iter->padding_0[0]) = 0xffffffff;
    *((int *)&iter->padding_14[76]) = a2->field_2;
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&iter->padding_14[84]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&iter->padding_14[88]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    v15 = v14 - 1;
    if (!a2->field_1c)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v16 = v15 + 1;
        *((unsigned int *)&iter->padding_14[0]) = *(idx);
        *((unsigned int *)&iter->padding_14[4]) = idx[1];
        v17 = idx[2];
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
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned int *)&iter->padding_14[0]) = v6;
        v7 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        *((unsigned int *)&iter->padding_14[4]) = v7;
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v16 = v15 + 1;
        v17 = v8;
    }
    *((unsigned int *)&iter->padding_14[8]) = v17;
    if (a2->field_1d == 2)
        *((unsigned int *)&v10[18].padding_14[380 + 4 * a2->field_1c]) = 1;
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&iter->padding_14[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v19 = v16;
    if (!(((v19 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x408f400000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
    {
        if (!a2->field_1c)
            goto LABEL_43982d;
        a2 = sub_464440();
        /* unsupported instruction */
        /* unsupported instruction */
        if (a2 < 0)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v4 = a2->field_1c * 228;
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
        a2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v3 = v20;
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        *((int *)&iter->padding_14[28]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v8 = sub_464440((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        if (v8 < 0)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&iter->padding_14[24]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!(((v19 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x408f180000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
        {
            v4 = v17;
            if (!a2->field_1c)
                goto LABEL_43982e;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
LABEL_43982d:
            v4 = v17;
LABEL_43982e:
            /* unsupported instruction */
            /* unsupported instruction */
        }
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v3 = v20;
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        *((int *)&iter->padding_14[28]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    if (!(iter->padding_14[48] & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_465390((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&iter->padding_14[20]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = v23;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&iter->padding_14[32]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        v1 = v20;
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
        *((int *)&iter->padding_14[28]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v24 = v10->field_10;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&iter->padding_14[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    *((int *)&iter->padding_14[4]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    sub_4615a0(v24, &v6, a2->field_1e + 5, 0);
    v26 = v25;
    *((unsigned int *)&iter->padding_14[56]) = v25;
    if (!v26)
    {
LABEL_4398da:
        index = NULL;
        goto LABEL_43991e;
    }
    else
    {
        node = *((int *)(g_4ce8cc + 8935096));
        if (*((int *)(g_4ce8cc + 8935096)))
        {
            iter = iter;
            do
            {
                index = node->field_0;
                if (*((int *)&index->padding_0[0]) == v26)
                    goto LABEL_43991a;
            } while ((node = (st_439630_6 *)node->field_4, node));
        }
        iter1 = *((int *)(g_4ce8cc + 8935104));
        if (!*((int *)(g_4ce8cc + 8935104)))
            goto LABEL_4398da;
    }
    while (1)
    {
        if (*((int *)&iter1->field_0->padding_0[0]) != v26)
        {
            iter1 = iter1->field_4;
            if (!iter1)
            {
                index = NULL;
                goto LABEL_43991e;
            }
        }
        else
        {
            index = iter1->field_0;
LABEL_43991a:
            if (index)
                break;
LABEL_43991e:
            *((unsigned int *)&iter->padding_14[56]) = 0;
            break;
        }
    }
    v30 = index->field_47c;
    if (index->field_47c & 0x20000000)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        index->field_2c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        index->field_47c = v30 | 4;
    }
    if (a2->field_1d == 2)
    {
        sub_4615a0(v10->field_10, &v25, 8, 0);
        *((unsigned int *)&iter->padding_14[60]) = v1;
    }
    else
    {
        *((unsigned int *)&iter->padding_14[60]) = 0;
    }
    if (a2->field_24)
        a2->field_24(v2);
    if (a2->field_22 >= 0)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = v26;
        /* unsupported instruction */
        sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index->field_430 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index->field_434 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index->field_438 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    return 0;
}
