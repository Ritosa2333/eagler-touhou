// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4463c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4463c0_0 {
    char padding_0[36];
    unsigned int field_24;
    int field_28;
    char padding_2c[652];
    int field_2b8;
} st_4463c0_0;

typedef struct st_4463c0_1 {
    char padding_0[102292];
    unsigned int field_18f94;
} st_4463c0_1;

extern unsigned int g_4aee60[4];
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern st_4463c0_1 *g_4b43b8;
extern unsigned int g_4b451c;

unsigned int sub_4463c0(st_4463c0_0 *a0)
{
    unsigned int v4;  // ftop
    unsigned int v5;  // ftop
    unsigned int v14;  // ftop
    int v15;  // edi
    unsigned int v16;  // ftop
    unsigned int v17;  // edx
    unsigned int v18;  // edx
    unsigned int v19;  // eax
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v6;  // ftop
    unsigned int v7;  // esi
    st_4463c0_1 *idx;  // esi
    unsigned int v9;  // ftop
    unsigned int v10;  // ftop
    unsigned int v11;  // ftop
    unsigned int v12;  // ftop
    unsigned int v13;  // ftop
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    if (a0->field_24 - 2 > 1)
        return 1;
    v5 = v4 - 1;
    if (/* unsupported instruction */)
    {
        v6 = v5 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v6 = v5 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v0 = v7;
    idx = g_4b43b8;
    if (/* unsupported instruction */)
        v1 = /* unsupported instruction */;
    else
        v1 = nan;
    if (/* unsupported instruction */)
    {
        v2 = /* unsupported instruction */;
        /* unsupported instruction */
        v9 = v6 + 1;
    }
    else
    {
        v2 = nan;
        /* unsupported instruction */
        v9 = v6 + 1;
    }
    g_4b43b8->field_18f94 = 1;
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if (a0->field_2b8 >= 10 || a0->field_24 == 3)
    {
        v10 = v9 - 1;
        if (/* unsupported instruction */)
        {
            v11 = v10 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v11 = v10 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
            v12 = v11 + 1;
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
            v12 = v11 + 1;
        }
        v13 = v12 - 1;
        if (/* unsupported instruction */)
        {
            v14 = v13 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v14 = v13 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v15 = 1;
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
            v16 = v14 + 1;
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
            v16 = v14 + 1;
        }
        while (1)
        {
            if (a0->field_28 != v15 - 1)
            {
                *((unsigned int *)&idx->padding_0[102272]) = 0xff808080;
            }
            else if (*((char *)((g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + (v15 + g_4b0ca8 * 6) * 8 + 1449)))
            {
                if (a0->field_24 == 3)
                {
                    v17 = a0->field_2b8 & 0x80000003;
                    if ((a0->field_2b8 & 0x80000003) < 0)
                        v17 = (v17 - 1 | 0xfffffffc) + 1;
                    if (v17 >= 2)
                    {
                        *((unsigned int *)&idx->padding_0[102272]) = 0xff000000;
                        goto LABEL_4464b5;
                    }
                }
                *((unsigned int *)&idx->padding_0[102272]) = 0xffffff00;
            }
            else
            {
                *((unsigned int *)&idx->padding_0[102272]) = 0xffdfdfdf;
            }
LABEL_4464b5:
            v18 = (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c;
            v19 = v15 + g_4b0ca8 * 6;
            if (!*((char *)(v18 + v19 * 8 + 1449)))
                sub_4015c0("%s  ---------", g_4aee60[v15]);
            else
                sub_4015c0("%s  %.8d0", g_4aee60[v15], *((int *)(v18 + v19 * 8 + 1444)));
            v20 = v16 - 1;
            if (/* unsupported instruction */)
            {
                v21 = v20 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v21 = v20 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            idx = g_4b43b8;
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
            v15 += 1;
            if (/* unsupported instruction */)
            {
                v2 = /* unsupported instruction */;
                /* unsupported instruction */
                v16 = v21 + 1;
                if (v15 >= 7)
                    break;
            }
            else
            {
                v2 = nan;
                /* unsupported instruction */
                v16 = v21 + 1;
                if (v15 >= 7)
                    break;
            }
        }
    }
    *((unsigned int *)&idx->padding_0[102272]) = 0xffffffff;
    idx->field_18f94 = 0;
    return 1;
}
