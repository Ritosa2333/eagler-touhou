// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45d710; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45d710_0 {
    char padding_0[172];
    unsigned int field_ac;
} st_45d710_0;

typedef struct st_45d710_2 {
    unsigned int field_0;
} st_45d710_2;

typedef struct st_45d710_1 {
    char padding_0[268];
    struct st_45d710_2 *field_10c;
    char padding_110[60];
    struct st_45d710_2 *field_14c;
    char padding_150[20];
    struct st_45d710_2 *field_164;
} st_45d710_1;

typedef struct st_45d710_5 {
    struct st_45d710_1 *field_0;
} st_45d710_5;

extern char g_4b5642;
extern char g_4b5647;
extern unsigned int g_4ce8cc;
extern st_45d710_5 *g_4ce8f0;

unsigned int sub_45d710(unsigned int a0, unsigned int a1, st_45d710_0 *idx, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, int a7)
{
    unsigned int v12;  // ebx
    unsigned int v13;  // esi
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    void* iter;  // esi
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v15;  // edi
    st_45d710_0 *v16;  // edi
    unsigned int v17;  // ebx
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    int v21;  // eax
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x34]
    unsigned int v2;  // [bp-0x30]
    unsigned int v3;  // [bp-0x2c]
    unsigned int v4;  // [bp-0x28]
    unsigned int v5;  // [bp-0x24]
    unsigned int i;  // [bp-0x24]
    unsigned int v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0x14]
    unsigned int v9;  // [bp-0x10]
    unsigned int v10;  // [bp-0x8]
    int v11;  // [bp-0x4], Other Possible Types: unsigned int

    v11 = a0;
    v10 = v12;
    v9 = v13;
    iter = *((int *)&idx[50767].padding_0[96]);
    v8 = v15;
    v16 = &idx[50767].padding_0[96];
    v17 = a7 * 20;
    if (iter + v17 + 20 >= v16)
        return 0;
    v19 = v18 - 1;
    if (/* unsupported instruction */)
    {
        v20 = v19 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v20 = v19 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v21 = a7 + 1;
    if (/* unsupported instruction */)
    {
        idx = /* unsupported instruction */;
        /* unsupported instruction */
        v22 = v20 + 1;
    }
    else
    {
        idx = nan;
        /* unsupported instruction */
        v22 = v20 + 1;
    }
    v23 = v22 - 1;
    if (/* unsupported instruction */)
    {
        v24 = v23 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v24 = v23 - 1;
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
        a6 = /* unsupported instruction */;
        /* unsupported instruction */
        v25 = v24 + 1;
    }
    else
    {
        a6 = nan;
        /* unsupported instruction */
        v25 = v24 + 1;
    }
    if (v21 > 0)
    {
        v11 = v21;
        do
        {
            i = v5;
            v26 = v25 - 1;
            if (/* unsupported instruction */)
            {
                v27 = v26 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v27 = v26 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v4 = /* unsupported instruction */;
                /* unsupported instruction */
                v28 = v27 + 1;
            }
            else
            {
                v4 = nan;
                /* unsupported instruction */
                v28 = v27 + 1;
            }
            v29 = v28 - 1;
            if (/* unsupported instruction */)
            {
                v30 = v29 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v30 = v29 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v3 = /* unsupported instruction */;
                /* unsupported instruction */
                v31 = v30 + 1;
            }
            else
            {
                v3 = nan;
                /* unsupported instruction */
                v31 = v30 + 1;
            }
            sub_45d890();
            v32 = v31 - 1;
            if (/* unsupported instruction */)
            {
                v33 = v32 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v33 = v32 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            *((unsigned int *)&iter[16]) = idx;
            iter += 20;
            *((int *)((char *)iter - 20)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 16)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 12)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            *((int *)((char *)iter - 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v34 = v33 + 1;
            sub_464640();
            if (/* unsupported instruction */)
            {
                v7 = /* unsupported instruction */;
                /* unsupported instruction */
                v25 = v34 + 1;
            }
            else
            {
                v7 = nan;
                /* unsupported instruction */
                v25 = v34 + 1;
            }
            v5 = i - 1;
        } while (i != 1);
    }
    if ((&g_4b5647)[g_4ce8cc])
    {
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 4, 3);
        v0 = 0;
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 1, 3);
        (&g_4b5647)[g_4ce8cc] = 0;
    }
    if (*((char *)(idx + &g_4b5642)) != 1)
    {
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 6, 0);
        g_4ce8f0->field_0->field_10c(g_4ce8f0, 0, 3, 0);
        *((char *)(idx + &g_4b5642)) = 1;
    }
    g_4ce8f0->field_0->field_164(g_4ce8f0, 0x44);
    g_4ce8f0->field_0->field_14c(g_4ce8f0, 3, v0, *((int *)&v16->padding_0[0]), 20);
    *((unsigned int *)&v16->padding_0[0]) = *((int *)&v16->padding_0[0]) + v17 + 20;
    idx->field_ac = idx->field_ac + 1;
    return 0;
}
