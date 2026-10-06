// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46a970; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46a970_1 {
    char padding_0[64];
    int field_40;
    char padding_44[4];
    int field_48;
    char padding_4c[1208];
    struct st_46a970_0 *field_504;
    unsigned int field_508;
    unsigned int field_50c;
    unsigned int field_510;
} st_46a970_1;

typedef struct st_46a970_0 {
    char padding_0[4];
    int field_4;
    char padding_8[8];
    unsigned int field_10;
} st_46a970_0;

typedef struct st_46a970_2 {
    char padding_0[2428];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
    char padding_988[30932];
    unsigned int field_825c;
} st_46a970_2;

extern st_46a970_2 *g_4b4514;

unsigned int sub_46a970(void)
{
    st_46a970_1 *idx;  // edi
    int v7;  // esi
    unsigned int v16;  // ecx
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int iter;  // ebx
    unsigned int v25;  // ftop
    int v8;  // ebp
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int idx1;  // edx
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    int v9;  // ebx
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    int v10;  // ecx
    unsigned int v46;  // edx
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int *index;  // esi
    int i;  // ebp
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x1c]
    unsigned int v4;  // [bp-0xc]

    index = sub_461920(idx->field_40, v7, v8, v9, v10);
    i = 0;
    if (!index)
        idx->field_40 = 0;
    sub_406f60(40);
    if (!index)
    {
        if (idx->field_504)
        {
            sub_402870();
            sub_46ca4f(idx->field_504);
        }
        /* unsupported instruction */
        /* unsupported instruction */
        idx->field_504 = NULL;
        g_4b4514->field_825c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_461970(idx->field_48);
        return 0xffffffff;
    }
    else
    {
        v14 = v13 - 1;
        if (/* unsupported instruction */)
        {
            v15 = v14 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v15 = v14 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v16 = g_4b4514->field_97c;
        if (/* unsupported instruction */)
        {
            g_4b4514->field_825c = /* unsupported instruction */;
            /* unsupported instruction */
            v17 = v15 + 1;
        }
        else
        {
            g_4b4514->field_825c = nan;
            /* unsupported instruction */
            v17 = v15 + 1;
        }
        v18 = v17 - 1;
        if (/* unsupported instruction */)
        {
            v19 = v18 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v19 = v18 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        idx->field_508 = v16;
        idx->field_50c = g_4b4514->field_980;
        idx->field_510 = g_4b4514->field_984;
        if (/* unsupported instruction */)
        {
            v3 = /* unsupported instruction */;
            /* unsupported instruction */
            v20 = v19 + 1;
        }
        else
        {
            v3 = nan;
            /* unsupported instruction */
            v20 = v19 + 1;
        }
        index[268] = g_4b4514->field_97c;
        v21 = v20 - 1;
        if (/* unsupported instruction */)
        {
            v22 = v21 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v22 = v21 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
            v23 = v22 + 1;
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
            v23 = v22 + 1;
        }
        index[269] = g_4b4514->field_980;
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        index[270] = g_4b4514->field_984;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_410020();
        iter = idx->field_504->field_10;
        if (idx->field_504->field_4 > NULL)
        {
            do
            {
                *((unsigned int *)(iter + idx->field_504->field_4 * 28 + 16)) = 0xff7070a0;
                *((unsigned int *)(iter + idx->field_504->field_4 * 56 + 16)) = 0xff7070a0;
                v3 = sub_464440();
                v25 = v23 - 1;
                if (/* unsupported instruction */)
                {
                    v26 = v25 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v26 = v25 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                if (sub_464440() < NULL)
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v27 = v26 + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v27 = v26 + 1;
                }
                v28 = v27 - 1;
                if (/* unsupported instruction */)
                {
                    v29 = v28 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v29 = v28 - 1;
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v30 = v29 + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v30 = v29 + 1;
                }
                v31 = v30 - 1;
                if (/* unsupported instruction */)
                {
                    v32 = v31 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v32 = v31 - 1;
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
                idx1 = idx->field_504->field_4 * 7;
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v34 = v32 + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v34 = v32 + 1;
                }
                v35 = v34 - 1;
                if (/* unsupported instruction */)
                {
                    v36 = v35 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v36 = v35 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                if (/* unsupported instruction */)
                {
                    *((int *)(iter + idx1 * 4)) = /* unsupported instruction */;
                    /* unsupported instruction */
                    v37 = v36 + 1;
                }
                else
                {
                    *((unsigned int *)(iter + idx1 * 4)) = nan;
                    /* unsupported instruction */
                    v37 = v36 + 1;
                }
                v3 = sub_464440();
                v38 = v37 - 1;
                if (/* unsupported instruction */)
                {
                    v39 = v38 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v39 = v38 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                if (sub_464440() < NULL)
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
                i += 1;
                if (/* unsupported instruction */)
                {
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v40 = v39 + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v40 = v39 + 1;
                }
                iter += 28;
                v41 = v40 - 1;
                if (/* unsupported instruction */)
                {
                    v42 = v41 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v42 = v41 - 1;
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v43 = v42 + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v43 = v42 + 1;
                }
                v44 = v43 - 1;
                if (/* unsupported instruction */)
                {
                    v45 = v44 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v45 = v44 - 1;
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
                v46 = idx->field_504->field_4 * 7;
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v47 = v45 + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v47 = v45 + 1;
                }
                v48 = v47 - 1;
                if (/* unsupported instruction */)
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
                if (/* unsupported instruction */)
                {
                    *((int *)(iter + v46 * 8 - 28)) = /* unsupported instruction */;
                    /* unsupported instruction */
                    v23 = v49 + 1;
                }
                else
                {
                    *((unsigned int *)(iter + v46 * 8 - 28)) = nan;
                    /* unsupported instruction */
                    v23 = v49 + 1;
                }
            } while (i < idx->field_504->field_4);
        }
        sub_46abe0();
        return 0;
    }
}
