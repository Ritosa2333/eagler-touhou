// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x414f40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_414f40_1 {
    struct st_414f40_0 *field_0;
    struct st_414f40_1 *field_4;
} st_414f40_1;

typedef struct st_414f40_0 {
    char padding_0[4212];
    char field_1074;
    char padding_1075[5587];
    unsigned int field_2648;
    char padding_264c[8];
    unsigned int field_2654;
    unsigned int field_2658;
    char field_265c;
    char padding_265d[155];
    unsigned int field_26f8;
} st_414f40_0;

typedef struct st_414f40_2 {
    char padding_0[104];
    struct st_414f40_1 *field_68;
} st_414f40_2;

extern st_414f40_2 *g_4b43dc;

void sub_414f40(unsigned int a0, unsigned int a1, unsigned int a2)
{
    st_414f40_1 *iter;  // eax
    unsigned int v4;  // ftop
    int v13;  // edi
    int v14;  // edx
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v18;  // 4148
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v22;  // ftop
    unsigned int v5;  // ftop
    unsigned int v23;  // ftop
    unsigned int v25;  // 4148
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v29;  // ftop
    unsigned int v6;  // ftop
    st_414f40_0 *idx;  // ecx
    unsigned int v8;  // ftop
    unsigned int v9;  // ftop
    unsigned int v11;  // 4501
    unsigned int v12;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    iter = g_4b43dc->field_68;
    if (!iter)
        return;
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
    do
    {
        idx = iter->field_0;
        iter = iter->field_4;
        if (idx->field_26f8 & 0x800)
        {
            v8 = v6 - 1;
            if (/* unsupported instruction */)
            {
                v9 = v8 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v9 = v8 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            /* unsupported instruction */
            v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
            v6 = v9 + 1;
            v11 = _ccall(10, 13, (char)((((unsigned short)v6 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v11 & 1))
            {
                if (idx->field_265c & 1)
                {
                    v12 = idx->field_2658;
                    idx->field_2654 = idx->field_2654 - 99999;
                    v13 = idx->field_2654 - v12 * 7;
                    v14 = (int)((unsigned int)(2454267027 * v13 >> 32) + v13) >> 2;
                    idx->field_2648 = ((unsigned int)(v14) >> 31) + v14 + v12;
                }
                else
                {
                    idx->field_2648 = idx->field_2648 - 99999;
                }
                v15 = v6 - 1;
                if (/* unsupported instruction */)
                {
                    v16 = v15 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v16 = v15 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = v16 + 1;
                v18 = _ccall(10, 13, (char)((((unsigned short)v6 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v18 & 1)
                {
                    v19 = v6 - 1;
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
                    v6 = v20 + 1;
                    if ((((unsigned short)v6 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                    {
                        v22 = v6 - 1;
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
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v6 = v23 + 1;
                        v25 = _ccall(10, 13, (char)((((unsigned short)v6 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (v25 & 1)
                        {
                            v26 = v6 - 1;
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
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v6 = v27 + 1;
                            if ((((unsigned short)v6 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1 && a2)
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
                                sub_4273f0(9, &idx->field_1074, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                                v29 = v6 - 0;
                                if (/* unsupported instruction */)
                                {
                                    v6 = v29 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v6 = v29 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                            }
                        }
                    }
                }
            }
        }
    } while (iter);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return;
}
