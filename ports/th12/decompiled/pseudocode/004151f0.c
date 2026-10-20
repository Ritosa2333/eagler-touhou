// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4151f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4151f0_1 {
    struct st_4151f0_0 *field_0;
    struct st_4151f0_1 *field_4;
} st_4151f0_1;

typedef struct st_4151f0_0 {
    char padding_0[9800];
    unsigned int field_2648;
    char padding_264c[8];
    unsigned int field_2654;
    unsigned int field_2658;
    char field_265c;
    char padding_265d[155];
    unsigned int field_26f8;
} st_4151f0_0;

typedef struct st_4151f0_2 {
    char padding_0[104];
    struct st_4151f0_1 *field_68;
} st_4151f0_2;

extern st_4151f0_2 *g_4b43dc;

void sub_4151f0(unsigned int a0, unsigned int a1, unsigned int a2)
{
    st_4151f0_1 *iter;  // eax
    unsigned int v8;  // esi
    unsigned int v17;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v9;  // edi
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // edi
    int v30;  // ebp
    int v31;  // edx
    unsigned int v33;  // ebp
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int *v39;  // ecx
    unsigned int v10;  // ftop
    st_4151f0_0 *idx;  // esi
    unsigned int v12;  // ftop
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x1c]
    unsigned int v4;  // [bp-0x10]
    st_4151f0_1 *v5;  // [bp-0xc], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x4]

    v6 = a0;
    iter = g_4b43dc->field_68;
    if (!iter)
        return;
    v5 = v8;
    v4 = v9;
    do
    {
        idx = iter->field_0;
        v12 = v10 - 1;
        if (/* unsupported instruction */)
        {
            v13 = v12 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v13 = v12 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
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
        iter = iter->field_4;
        v5 = iter;
        if (idx->field_26f8 & 0x800)
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
            v17 = v15 - 1;
            if (!((char)((((unsigned short)v17 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
                v15 = v17 + 1;
                if (!((char)((((unsigned short)v15 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    v19 = v15 - 1;
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
                    v15 = v20 + 1;
                    if (!((char)((((unsigned short)v15 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    {
                        v22 = v15 - 1;
                        if (/* unsupported instruction */)
                        {
                            v23 = v22 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v23 = v22 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        v24 = v23 - 1;
                        if (/* unsupported instruction */)
                        {
                            v25 = v24 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v25 = v24 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v27 = v25 + 2;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v28 = v27 + 1;
                        if (!((char)((((unsigned short)v27 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                        {
                            if (idx->field_265c & 1)
                            {
                                v29 = idx->field_2658;
                                idx->field_2654 = idx->field_2654 - 99999;
                                v30 = idx->field_2654 - v29 * 7;
                                v31 = (int)((unsigned int)(2454267027 * v30 >> 32) + v30) >> 2;
                                iter = v5;
                                idx->field_2648 = ((unsigned int)(v31) >> 31) + v31 + v29;
                            }
                            else
                            {
                                idx->field_2648 = idx->field_2648 - 99999;
                            }
                            if (/* unsupported instruction */)
                                v3 = /* unsupported instruction */;
                            else
                                v3 = nan;
                            if (/* unsupported instruction */)
                            {
                                v2 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v10 = v28 + 1;
                            }
                            else
                            {
                                v2 = nan;
                                /* unsupported instruction */
                                v10 = v28 + 1;
                            }
                            if (!sub_41c780() && v33)
                            {
                                v34 = v10 - 1;
                                if (/* unsupported instruction */)
                                {
                                    v35 = v34 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v35 = v34 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                if (/* unsupported instruction */)
                                {
                                    v1 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v36 = v35 + 1;
                                }
                                else
                                {
                                    v1 = nan;
                                    /* unsupported instruction */
                                    v36 = v35 + 1;
                                }
                                v37 = v36 - 1;
                                if (/* unsupported instruction */)
                                {
                                    v38 = v37 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v38 = v37 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                if (/* unsupported instruction */)
                                {
                                    v0 = /* unsupported instruction */;
                                    /* unsupported instruction */
                                    v10 = v38 + 1;
                                }
                                else
                                {
                                    v0 = nan;
                                    /* unsupported instruction */
                                    v10 = v38 + 1;
                                }
                                sub_4273f0(9, v39, v0, v1);
                                continue;
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
                v15 = v17 + 1;
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v28 = v15 + 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = v28 + 1;
    } while (iter);
    return;
}
