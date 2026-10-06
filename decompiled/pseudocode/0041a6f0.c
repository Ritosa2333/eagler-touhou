// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41a6f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41a6f0_0 {
    char padding_0[4780];
    unsigned int field_12ac;
    int field_12b0;
    unsigned int field_12b4;
    char padding_12b8[4];
    unsigned int field_12bc;
    char padding_12c0[5000];
    int field_2648;
    char padding_264c[4];
    int field_2650;
    char padding_2654[4];
    int field_2658;
    char padding_265c[156];
    unsigned int field_26f8;
    char padding_26fc[16];
    int field_270c;
    int field_2710;
    unsigned int field_2714;
} st_41a6f0_0;

typedef struct st_41a6f0_1 {
    char padding_0[27960];
    int field_6d38;
    unsigned int field_6d3c;
} st_41a6f0_1;

typedef struct st_41a6f0_4 {
    char padding_0[24];
    unsigned int field_18;
} st_41a6f0_4;

typedef struct st_41a6f0_2 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_41a6f0_2;

typedef struct st_41a6f0_3 {
    char padding_0[60];
    unsigned int field_3c;
} st_41a6f0_3;

extern char g_4b2ed0;
extern st_41a6f0_3 *g_4b43c4;
extern st_41a6f0_2 *g_4b43cc;
extern st_41a6f0_4 *g_4b43dc;
extern st_41a6f0_1 *g_4b43e4;

unsigned int sub_41a6f0(st_41a6f0_0 *idx)
{
    unsigned int v3;  // esi
    unsigned int v4;  // eax
    int v13;  // ebx
    unsigned int v14;  // edi
    int v15;  // edx
    unsigned int v16;  // eax
    int v17;  // edx
    int v18;  // edx
    unsigned int v19;  // esi
    unsigned int v20;  // eax
    unsigned int v21;  // eax
    unsigned int v22;  // eax
    st_41a6f0_0 *v5;  // edx
    unsigned int v23;  // edx
    st_41a6f0_0 *index;  // eax
    unsigned int v7;  // esi
    st_41a6f0_0 *v8;  // eax
    unsigned int v9;  // ebx
    unsigned int v10;  // ebp
    st_41a6f0_0 *v11;  // ebp
    int v12;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    v3 = idx->field_2648;
    idx->field_2650 = v3;
    idx->field_2658 = 0;
    v4 = 0;
    v5 = &idx->field_270c;
    do
    {
        if (*((int *)&v5->padding_0[0]) >= 0)
        {
            index = &idx->padding_0[16 * v4];
            idx->field_2650 = v3 - index->field_270c;
            idx->field_2658 = index->field_270c;
            if (v3 <= index->field_270c)
            {
                /* unsupported instruction */
                /* unsupported instruction */
                idx->field_2648 = index->field_270c;
                index->field_270c = 0xffffffff;
                v23 = idx->field_12bc;
                if (!((char)idx->field_12bc & 1))
                {
                    if (/* unsupported instruction */)
                        idx->field_12b4 = /* unsupported instruction */;
                    else
                        idx->field_12b4 = nan;
                    idx->field_12b0 = 0;
                    idx->field_12ac = 0xfff0bdc1;
                    *((char **)&idx->padding_12b8[0]) = &g_4b2ed0;
                    idx->field_12bc = v23 | 1;
                }
                idx->field_12b0 = 0;
                if (/* unsupported instruction */)
                {
                    idx->field_12b4 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    idx->field_12b4 = nan;
                    /* unsupported instruction */
                }
                idx->field_12ac = 0xffffffff;
                idx->field_26f8 = idx->field_26f8 & 0xff7fffff;
                return index->field_2714;
            }
        }
    } while ((v4 += 1, v5 += 16, v4 < 8));
    v7 = 0;
    v8 = &idx->field_2710;
    while (*((int *)(&v8->padding_0[0] - 4)) < 0 || *((int *)&v8->padding_0[0]) <= 0)
    {
        v8 = &v8->padding_0[16];
        if (v7 + 1 >= 8)
            return 0;
    }
    v1 = v9;
    v0 = v10;
    v11 = &(&idx->field_2710)[4 * v7];
    v12 = *((int *)&v11->padding_0[0]) - idx->field_12b0;
    v13 = v12 / 60;
    v14 = (int)(v12 % 60) * 100;
    v15 = (int)((unsigned int)(2290649225 * v14 >> 32) + v14) >> 5;
    v16 = ((unsigned int)(v15) >> 31) + v15;
    v17 = 99;
    if (v13 <= 99)
    {
        v18 = v13;
        v17 = v18;
    }
    g_4b43e4->field_6d38 = v17;
    if (v18 > 99)
        v16 = 99;
    g_4b43e4->field_6d3c = v16;
    if (idx->field_12b0 < *((int *)&v11->padding_0[0]))
        return 0;
    /* unsupported instruction */
    /* unsupported instruction */
    v19 = v7 * 16;
    idx->field_2648 = *((int *)&idx->padding_0[v19 + 9996]);
    *((unsigned int *)&idx->padding_0[v19 + 9996]) = 0xffffffff;
    v20 = idx->field_12bc;
    if (!((char)v20 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_12b4 = /* unsupported instruction */;
        else
            idx->field_12b4 = nan;
        idx->field_12b0 = 0;
        idx->field_12ac = 0xfff0bdc1;
        *((char **)&idx->padding_12b8[0]) = &g_4b2ed0;
        idx->field_12bc = v20 | 1;
    }
    if (/* unsupported instruction */)
    {
        idx->field_12b4 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_12b4 = nan;
        /* unsupported instruction */
    }
    idx->field_12b0 = 0;
    idx->field_12ac = 0xffffffff;
    idx->field_26f8 = idx->field_26f8 | 0x800000;
    v21 = g_4b43cc->field_7c;
    if (!((char)v21 & 8))
    {
        if ((char)v21 & 1)
        {
            if (g_4b43cc->field_28 >= 60)
            {
                g_4b43cc->field_80 = 0;
                v22 = v21 & 0xffffffdd;
            }
            else
            {
                if (!g_4b43c4->field_3c)
                    goto LABEL_41a8eb;
                v22 = v21 | 32;
            }
            g_4b43cc->field_7c = v22;
        }
LABEL_41a8eb:
        g_4b43dc->field_18 = 0;
    }
    return *((int *)&idx[1].padding_0[v19]);
}
