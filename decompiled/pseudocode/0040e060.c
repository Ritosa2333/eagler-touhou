// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40e060; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40e060_9 {
    char padding_0[1644];
    char field_66c;
} st_40e060_9;

typedef struct st_40e060_10 {
    char padding_0[1776];
    int field_6f0;
} st_40e060_10;

typedef struct st_40e060_6 {
    char padding_0[16];
    struct st_40e060_5 *field_10;
    struct st_40e060_6 *field_14;
} st_40e060_6;

typedef struct st_40e060_5 {
    char padding_0[1002];
    unsigned short field_3ea;
    char padding_3ec[24];
    int field_404;
} st_40e060_5;

typedef struct st_40e060_11 {
    char padding_0[109224];
    int field_1aaa8;
} st_40e060_11;

typedef struct st_40e060_4 {
    char padding_0[4212];
    unsigned int field_1074;
    unsigned int field_1078;
    unsigned int field_107c;
} st_40e060_4;

typedef struct st_40e060_1 {
    char field_0;
} st_40e060_1;

typedef struct st_40e060_3 {
    char padding_0[28];
    struct st_40e060_4 *field_1c;
} st_40e060_3;

typedef struct st_40e060_0 {
    char padding_0[20];
    unsigned int field_14;
    struct st_40e060_1 *field_18;
    unsigned int field_1c;
    int field_20;
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
    char padding_30[4];
    unsigned int field_34;
    char field_38;
    char padding_39[63];
    unsigned int field_78;
    unsigned int field_7c;
    unsigned int field_80;
    unsigned int field_84;
    int field_88;
    char padding_8c[4];
    unsigned int field_90;
    char padding_94[24];
    unsigned int field_ac;
    unsigned int field_b0;
    unsigned int field_b4;
} st_40e060_0;

typedef struct st_40e060_12 {
    char padding_0[60];
    unsigned int field_3c;
} st_40e060_12;

typedef struct st_40e060_2 {
    char padding_0[102324];
    int field_18fb4;
} st_40e060_2;

extern unsigned int g_40e5bc[4];
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern char g_4b2ed0;
extern st_40e060_2 *g_4b43b8;
extern st_40e060_12 *g_4b43c4;
extern unsigned int g_4b43c8;
extern st_40e060_0 *g_4b43cc;
extern st_40e060_3 *g_4b43dc;
extern unsigned int g_4b4518;
extern unsigned int g_4b451c;
extern unsigned int g_4cee70;

int sub_40e060(void)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // edi
    st_40e060_9 *iter;  // esi
    char *iter1;  // edx
    char j;  // 4103
    st_40e060_10 *index;  // ecx
    st_40e060_11 *idx;  // esi
    char *iter2;  // ecx
    char k;  // 4102
    unsigned int v24;  // esi
    int v25;  // edx
    int v26;  // ecx
    char *v9;  // eax
    unsigned int idx1;  // eax
    unsigned int *idx2;  // eax
    st_40e060_6 *v29;  // eax
    st_40e060_6 *v30;  // eax
    st_40e060_5 *v31;  // eax
    int v32;  // ebx
    st_40e060_6 *v33;  // eax
    st_40e060_6 *node;  // eax
    st_40e060_5 *v35;  // eax
    unsigned int v36;  // eax
    char *v10;  // ebx
    unsigned int v11;  // eax
    unsigned int v12;  // edx
    unsigned int v13;  // ecx
    char *v14;  // eax
    char i;  // 4102
    unsigned int v16;  // eax
    char *v0;  // [bp-0x4c]
    unsigned int v1;  // [bp-0x38]
    char *v2;  // [bp-0x24]
    unsigned int v3;  // [bp-0x18]
    unsigned int v4;  // [bp-0xc]
    char v5;  // [bp-0x4]

    /* unsupported instruction */
    /* unsupported instruction */
    v4 = v7;
    v3 = v8;
    v10 = v9;
    v11 = g_4b43cc->field_34;
    if (!((char)v11 & 1))
    {
        if (/* unsupported instruction */)
            g_4b43cc->field_2c = /* unsupported instruction */;
        else
            g_4b43cc->field_2c = nan;
        g_4b43cc->field_28 = 0;
        g_4b43cc->field_24 = 0xfff0bdc1;
        *((char **)&g_4b43cc->padding_30[0]) = &g_4b2ed0;
        g_4b43cc->field_34 = v11 | 1;
    }
    v12 = &g_4b43cc->field_38;
    if (/* unsupported instruction */)
    {
        g_4b43cc->field_2c = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b43cc->field_2c = nan;
        /* unsupported instruction */
    }
    g_4b43cc->field_28 = 0;
    g_4b43cc->field_24 = 0xffffffff;
    g_4b43cc->field_78 = v13;
    v14 = v10;
    do
    {
        i = *(v14);
        *((char *)(v12 - v10 + v14)) = *(v14);
        v14 += 1;
    } while (i);
    g_4b43cc->field_7c = g_4b43cc->field_7c & 0xffffffe7 | 3;
    if (*((int *)(g_4b4518 + 16)) != 1)
    {
        v16 = v13 * 144;
        iter = (g_4b0c94 + g_4b0c90 * 2) * 17908 + v16 + g_4b451c + 1644;
        iter1 = v10;
        do
        {
            j = *(iter1);
            iter->padding_0[0] = *(iter1);
            iter1 += 1;
            iter = &iter->padding_0[1];
        } while (j);
        index = (g_4b0c94 + g_4b0c90 * 2) * 17908 + v16 + g_4b451c + 1644;
        if (*((int *)&index->padding_0[132]) < 99999)
            *((unsigned int *)&index->padding_0[132]) = *((int *)&index->padding_0[132]) + 1;
        idx = v16 + g_4b451c + 109092;
        iter2 = v10;
        do
        {
            k = *(iter2);
            *(iter2 + &idx->padding_0[0] - v10) = *(iter2);
            iter2 += 1;
        } while (k);
        if (*((int *)&idx->padding_0[132]) < 99999)
            *((unsigned int *)&idx->padding_0[132]) = *((int *)&idx->padding_0[132]) + 1;
    }
    g_4b43cc->field_7c = g_4b43cc->field_7c & 0xffffffdf;
    if (g_4b43c4->field_3c)
        g_4b43cc->field_7c = g_4b43cc->field_7c | 32;
    g_4b43cc->field_7c = g_4b43cc->field_7c & 0xffffffbf;
    g_4b43cc->field_90 = 1;
    v2 = &v5;
    sub_4615a0(g_4b43b8->field_18fb4, &v5, 1, 0);
    g_4b43cc->field_14 = v24;
    v1 = g_4cee70;
    sub_4615a0(g_4cee70, &v24, 74, 0);
    g_4b43cc->field_18 = &v5;
    v25 = g_4b43b8->field_18fb4;
    sub_4615a0(v25, &v2, 2, 0);
    g_4b43cc->field_1c = &v24;
    v0 = &g_4b43cc->field_18->field_0;
    if (!sub_461920(g_4b43cc->field_18))
        g_4b43cc->field_18 = NULL;
    sub_460800(0xffffff, 0, 0, 0, v10);
    sub_453d90();
    sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v1, 127, 0);
    v26 = v25;
    g_4b43cc->field_20 = v26;
    idx1 = &g_4b43dc->field_1c->field_1074;
    g_4b43cc->field_ac = g_4b43dc->field_1c->field_1074;
    g_4b43cc->field_b0 = *((int *)(idx1 + 4));
    g_4b43cc->field_b4 = *((int *)(idx1 + 8));
    idx2 = sub_461920(v26);
    if (idx2)
    {
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            idx2[268] = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx2[268] = nan;
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            idx2[269] = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx2[269] = nan;
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            idx2[270] = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx2[270] = nan;
            /* unsupported instruction */
        }
    }
    v29 = sub_461920(g_4b43cc->field_20);
    if (!v29)
        g_4b43cc->field_20 = 0;
    v30 = &v29->field_10;
    if (v30)
    {
        do
        {
            if (*((short *)(*((int *)&v30->padding_0[0]) + 1002)) == 125)
            {
                v31 = *((int *)&v30->padding_0[0]);
                goto LABEL_40e318;
            }
        } while ((v30 = (st_40e060_6 *)*((int *)&v30->padding_0[4]), v30));
    }
    v31 = NULL;
LABEL_40e318:
    v32 = v25;
    v31->field_404 = v32;
    v33 = sub_461920(g_4b43cc->field_20);
    if (!v33)
        g_4b43cc->field_20 = 0;
    node = &v33->field_10;
    if (node)
    {
        do
        {
            if (*((short *)(*((int *)&node->padding_0[0]) + 1002)) == 126)
            {
                v35 = *((int *)&node->padding_0[0]);
                goto LABEL_40e358;
            }
        } while ((node = (st_40e060_6 *)*((int *)&node->padding_0[4]), node));
    }
    v35 = NULL;
LABEL_40e358:
    v35->field_404 = v32;
    g_4b43cc->field_88 = v32;
    v36 = (g_4b0ca8 + g_4b0cb0) * 2000000;
    g_4b43cc->field_80 = v36;
    g_4b43cc->field_84 = v36;
    if (v36 >= 100000000)
        g_4b43cc->field_84 = 0x5f5e0ff;
    sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v0, 137, 0);
    if (g_4b0cb0 - 1 > 6)
        return;
    goto *((void *)(*((int *)((char *)&g_40e5bc[g_4b0cb0] - 4))));
}
