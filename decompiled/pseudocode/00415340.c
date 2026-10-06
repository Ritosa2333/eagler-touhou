// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x415340; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_415340_1 {
    struct st_415340_0 *field_0;
    struct st_415340_1 *field_4;
} st_415340_1;

typedef struct st_415340_0 {
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
} st_415340_0;

typedef struct st_415340_2 {
    char padding_0[104];
    struct st_415340_1 *field_68;
} st_415340_2;

extern st_415340_2 *g_4b43dc;

void sub_415340(unsigned int a0, unsigned int a1, unsigned int a2)
{
    st_415340_1 *iter;  // eax
    unsigned int v6;  // ftop
    unsigned int v16;  // 4501
    unsigned int v17;  // esi
    int v18;  // edi
    int v19;  // edx
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v23;  // 4148
    unsigned int v24;  // ftop
    unsigned int v7;  // ftop
    unsigned int v25;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v30;  // 4148
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v34;  // ftop
    unsigned int v8;  // ftop
    unsigned int v9;  // ebx
    unsigned int v10;  // esi
    unsigned int v11;  // edi
    st_415340_0 *idx;  // ecx
    unsigned int v13;  // ftop
    unsigned int v14;  // ftop
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x4]

    iter = g_4b43dc->field_68;
    if (!iter)
        return;
    v7 = v6 - 1;
    if (/* unsupported instruction */)
    {
        v8 = v7 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v8 = v7 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v3 = v9;
    v1 = v10;
    v0 = v11;
    do
    {
        idx = iter->field_0;
        iter = iter->field_4;
        if (idx->field_26f8 & 0x800)
        {
            v13 = v8 - 1;
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
            v8 = v14 + 1;
            v16 = _ccall(10, 13, (char)((((unsigned short)v8 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
            if (!(v16 & 1))
            {
                if (idx->field_265c & 1)
                {
                    v17 = idx->field_2658;
                    idx->field_2654 = idx->field_2654 - 99999;
                    v18 = idx->field_2654 - v17 * 7;
                    v19 = (int)((unsigned int)(2454267027 * v18 >> 32) + v18) >> 2;
                    idx->field_2648 = ((unsigned int)(v19) >> 31) + v19 + v17;
                }
                else
                {
                    idx->field_2648 = idx->field_2648 - 99999;
                }
                v20 = v8 - 1;
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
                v8 = v21 + 1;
                v23 = _ccall(10, 13, (char)((((unsigned short)v8 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0xc068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                if (v23 & 1)
                {
                    v24 = v8 - 1;
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
                    v8 = v25 + 1;
                    if ((((unsigned short)v8 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x4068000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1)
                    {
                        v27 = v8 - 1;
                        if (/* unsupported instruction */)
                        {
                            v28 = v27 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        else
                        {
                            v28 = v27 - 1;
                            /* unsupported instruction */
                            /* unsupported instruction */
                        }
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v8 = v28 + 1;
                        v30 = _ccall(10, 13, (char)((((unsigned short)v8 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
                        if (v30 & 1)
                        {
                            v31 = v8 - 1;
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
                            /* unsupported instruction */
                            /* unsupported instruction */
                            /* unsupported instruction */
                            v8 = v32 + 1;
                            if ((((unsigned short)v8 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x407c000000000000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1 && v4)
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
                                v34 = v8 - 0;
                                if (/* unsupported instruction */)
                                {
                                    v8 = v34 - 1;
                                    /* unsupported instruction */
                                    /* unsupported instruction */
                                }
                                else
                                {
                                    v8 = v34 - 1;
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
