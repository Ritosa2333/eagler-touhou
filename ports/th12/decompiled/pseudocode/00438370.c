// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x438370; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_438370_0 {
    char padding_0[2600];
    unsigned int field_a28;
    char padding_a2c[4];
    unsigned int field_a30;
    unsigned int field_a34;
    unsigned int field_a38;
    char padding_a3c[4];
    unsigned int field_a40;
    char padding_a44[47548];
    unsigned int field_c400;
    unsigned int field_c404;
    unsigned int field_c408;
    char padding_c40c[4];
    unsigned int field_c410;
} st_438370_0;

typedef struct st_438370_1 {
    struct st_438370_2 *field_0;
    struct st_438370_1 *field_4;
} st_438370_1;

typedef struct st_438370_3 {
    unsigned int field_0;
    struct st_438370_3 *field_4;
} st_438370_3;

typedef struct st_438370_4 {
    char padding_0[96];
    unsigned int field_60;
} st_438370_4;

typedef struct st_438370_2 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
} st_438370_2;

typedef struct st_438370_5 {
    char padding_0[40];
    int field_28;
    char padding_2c[80];
    unsigned int field_7c;
    unsigned int field_80;
} st_438370_5;

typedef struct st_438370_6 {
    char padding_0[60];
    unsigned int field_3c;
} st_438370_6;

extern char g_4b2ed0;
extern st_438370_6 *g_4b43c4;
extern unsigned int g_4b43c8;
extern st_438370_5 *g_4b43cc;
extern st_438370_4 *g_4b44e8;
extern unsigned int g_4ce8cc;

void sub_438370(void)
{
    st_438370_0 *idx;  // esi
    int v18;  // edi
    unsigned int v27;  // eax
    unsigned int *index;  // eax
    unsigned int v20;  // ebp
    unsigned int v21;  // ebx
    st_438370_1 *iter;  // eax
    unsigned int idx1;  // eax
    st_438370_3 *node;  // eax
    unsigned int v25;  // eax
    unsigned int v26;  // eax
    st_438370_0 *v0;  // [bp-0x60]
    unsigned int v1;  // [bp-0x5c]
    unsigned int v2;  // [bp-0x48]
    unsigned int v3;  // [bp-0x48]
    unsigned int v4;  // [bp-0x44]
    unsigned int v5;  // [bp-0x40]
    unsigned int v6;  // [bp-0x3c]
    unsigned int v7;  // [bp-0x34]
    char *v8;  // [bp-0x30]
    unsigned int v9;  // [bp-0x2c]
    unsigned int v10;  // [bp-0x28]
    unsigned int v11;  // [bp-0x18]
    char v12;  // [bp-0x14]
    unsigned int v13;  // [bp-0xc]
    unsigned int v14;  // [bp-0x8]
    unsigned int v15;  // [bp-0x4]

    idx->field_a28 = 4;
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
    v10 = 0;
    if (/* unsupported instruction */)
    {
        v13 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v13 = nan;
        /* unsupported instruction */
    }
    v9 = 79;
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
    v8 = &v12;
    v7 = *((int *)(g_4b43c8 + 5106652));
    if (/* unsupported instruction */)
    {
        v14 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v14 = nan;
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
        v15 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v15 = nan;
        /* unsupported instruction */
    }
    sub_4615a0();
    index = sub_461920(v18);
    if (index)
    {
        index[268] = v20;
        index[269] = v21;
        index[270] = v11;
    }
    v10 = 32;
    do
    {
        v3 = v2;
        sub_4615a0(*((int *)(g_4b43c8 + 5106652)), &v7, 80, 0);
        if (v4)
        {
            iter = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    if (*((int *)&iter->field_0->padding_0[0]) == v4)
                    {
                        idx1 = iter->field_0;
                        goto LABEL_438475;
                    }
                } while ((iter = (st_438370_1 *)iter->field_4, iter));
            }
            if (*((int *)(g_4ce8cc + 8935104)))
            {
                for (node = *((int *)(g_4ce8cc + 8935104)); *((int *)node->field_0) != v4; node = node->field_4);
                idx1 = node->field_0;
LABEL_438475:
                if (idx1)
                {
                    *((unsigned int *)(idx1 + 1072)) = v5;
                    *((unsigned int *)(idx1 + 1076)) = v6;
                    *((unsigned int *)(idx1 + 1080)) = v11;
                }
            }
        }
    } while ((v2 = v3 - 1, v3 != 1));
    v25 = idx->field_a40;
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)idx->field_a40 & 1))
    {
        if (/* unsupported instruction */)
            idx->field_a38 = /* unsupported instruction */;
        else
            idx->field_a38 = nan;
        idx->field_a34 = 0;
        idx->field_a30 = 0xfff0bdc1;
        *((char **)&idx->padding_a3c[0]) = &g_4b2ed0;
        idx->field_a40 = v25 | 1;
    }
    if (/* unsupported instruction */)
        idx->field_a38 = /* unsupported instruction */;
    else
        idx->field_a38 = nan;
    idx->field_a34 = 0;
    idx->field_a30 = 0xffffffff;
    if (g_4b44e8 && !(g_4b44e8->field_60 & 0x200))
        sub_453d90();
    v26 = idx->field_c410;
    if (!((char)idx->field_c410 & 1))
    {
        if (/* unsupported instruction */)
        {
            idx->field_c408 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            idx->field_c408 = nan;
            /* unsupported instruction */
        }
        idx->field_c404 = 0;
        idx->field_c400 = 0xfff0bdc1;
        *((char **)&idx->padding_c40c[0]) = &g_4b2ed0;
        idx->field_c410 = v26 | 1;
    }
    else
    {
        /* unsupported instruction */
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
    v1 = 0;
    if (/* unsupported instruction */)
    {
        idx->field_c408 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        idx->field_c408 = nan;
        /* unsupported instruction */
    }
    idx->field_c404 = 6;
    idx->field_c400 = 5;
    v0 = &idx->padding_0[20];
    sub_454d10();
    v27 = g_4b43cc->field_7c;
    if (!((char)v27 & 1))
    {
        return;
    }
    else if (g_4b43cc->field_28 >= 60)
    {
        g_4b43cc->field_80 = 0;
        g_4b43cc->field_7c = v27 & 0xffffffdd;
        return;
    }
    else
    {
        if (!g_4b43c4->field_3c)
            return;
        g_4b43cc->field_7c = v27 | 32;
        return;
    }
}
