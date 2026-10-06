// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4150c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4150c0_1 {
    struct st_4150c0_0 *field_0;
    struct st_4150c0_1 *field_4;
} st_4150c0_1;

typedef struct st_4150c0_0 {
    char padding_0[9800];
    unsigned int field_2648;
    char padding_264c[8];
    unsigned int field_2654;
    unsigned int field_2658;
    char field_265c;
    char padding_265d[155];
    unsigned int field_26f8;
} st_4150c0_0;

typedef struct st_4150c0_2 {
    char padding_0[104];
    struct st_4150c0_1 *field_68;
} st_4150c0_2;

extern st_4150c0_2 *g_4b43dc;

void sub_4150c0(unsigned int a0, unsigned int a1)
{
    st_4150c0_1 *iter;  // eax
    unsigned int v11;  // ebx
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v23;  // 4175
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // 4192
    unsigned int v28;  // ftop
    unsigned int v12;  // esi
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v33;  // edi
    int v34;  // ebp
    int v35;  // edx
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v13;  // edi
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ecx
    unsigned int v14;  // ftop
    st_4150c0_0 *idx;  // esi
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x38]
    unsigned int v3;  // [bp-0x34]
    unsigned int v4;  // [bp-0x30]
    unsigned int v5;  // [bp-0x2c]
    unsigned int v6;  // [bp-0x14]
    unsigned int v7;  // [bp-0x10]
    unsigned int v8;  // [bp-0xc]
    unsigned int v9;  // [bp-0x4]

    iter = g_4b43dc->field_68;
    if (!iter)
        return;
    v9 = v11;
    v8 = v12;
    v7 = v13;
    do
    {
        idx = iter->field_0;
        v16 = v14 - 1;
        if (/* unsupported instruction */)
        {
            v17 = v16 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v17 = v16 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
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
        iter = iter->field_4;
        if (idx->field_26f8 & 0x800)
        {
            v20 = v19 - 1;
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v19 = v21 + 1;
            v23 = _ccall(10, 13, (char)((((unsigned short)v19 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v23 & 1))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v25 = v19;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v26 = v25 + 1;
                v27 = _ccall(10, 13, (char)((((unsigned short)v25 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (!(v27 & 1))
                {
                    v28 = v26 - 1;
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
                    v30 = v29 - 1;
                    if (/* unsupported instruction */)
                    {
                        v31 = v30 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v31 = v30 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v26 = v31 + 2;
                    if (!((char)((((unsigned short)v26 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    {
                        if (idx->field_265c & 1)
                        {
                            v33 = idx->field_2658;
                            idx->field_2654 = idx->field_2654 - 99999;
                            v34 = idx->field_2654 - v33 * 7;
                            v35 = (int)((unsigned int)(2454267027 * v34 >> 32) + v34) >> 2;
                            idx->field_2648 = ((unsigned int)(v35) >> 31) + v35 + v33;
                        }
                        else
                        {
                            idx->field_2648 = idx->field_2648 - 99999;
                        }
                        if (/* unsupported instruction */)
                            v5 = /* unsupported instruction */;
                        else
                            v5 = nan;
                        if (/* unsupported instruction */)
                        {
                            v4 = /* unsupported instruction */;
                            /* unsupported instruction */
                            v14 = v26 + 1;
                        }
                        else
                        {
                            v4 = nan;
                            /* unsupported instruction */
                            v14 = v26 + 1;
                        }
                        if (!sub_41c780() && v6)
                        {
                            v37 = v14 - 1;
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
                                v3 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v39 = v38 + 1;
                            }
                            else
                            {
                                v3 = nan;
                                /* unsupported instruction */
                                v39 = v38 + 1;
                            }
                            v40 = v39 - 1;
                            if (/* unsupported instruction */)
                            {
                                v41 = v40 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            else
                            {
                                v41 = v40 - 1;
                                /* unsupported instruction */
                                /* unsupported instruction */
                            }
                            if (/* unsupported instruction */)
                            {
                                v2 = /* unsupported instruction */;
                                /* unsupported instruction */
                                v14 = v41 + 1;
                            }
                            else
                            {
                                v2 = nan;
                                /* unsupported instruction */
                                v14 = v41 + 1;
                            }
                            v1 = v42;
                            v0 = 9;
                            sub_4273f0();
                            continue;
                        }
                    }
                }
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v26 = v19 + 1;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v14 = v26 + 1;
    } while (iter);
    return;
}
