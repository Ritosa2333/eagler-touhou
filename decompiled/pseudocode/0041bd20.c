// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41bd20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41bd20_1 {
    char padding_0[20];
    struct st_41bd20_2 *field_14;
} st_41bd20_1;

typedef struct st_41bd20_2 {
    unsigned int field_0;
} st_41bd20_2;

typedef struct st_41bd20_0 {
    char padding_0[572];
    int field_23c;
    char padding_240[4312];
    unsigned short field_1318;
    unsigned short field_131a;
    int field_131c;
    int field_1320;
    unsigned int field_1324;
    unsigned int field_1328;
    char padding_132c[4];
    unsigned int field_1330;
    char padding_1334[476];
    unsigned short field_1510;
    unsigned short field_1512;
    unsigned short field_1514;
    char padding_1516[2];
    unsigned int field_1518;
    unsigned int field_151c;
    unsigned int field_1520;
} st_41bd20_0;

typedef struct st_41bd20_3 {
    struct st_41bd20_1 *field_0;
    char padding_4[4];
    struct st_41bd20_3 *field_8;
    char padding_c[68];
    unsigned int field_50;
    unsigned int field_54;
    unsigned int field_58;
} st_41bd20_3;

typedef struct st_41bd20_4 {
    char padding_0[24];
    struct st_41bd20_3 *field_18;
    char padding_1c[1136];
    struct st_41bd20_3 *field_48c;
} st_41bd20_4;

extern st_41bd20_4 *g_4b44f4;

unsigned int sub_41bd20(st_41bd20_0 *idx)
{
    st_41bd20_0 *v8;  // ebp
    int v9;  // edi
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // fc3210
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    int v10;  // ebp
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    int v11;  // ebx
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
    st_41bd20_3 **idx1;  // eax
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // ftop
    unsigned int v52;  // ftop
    unsigned int v53;  // fc3210
    unsigned int v54;  // eax
    st_41bd20_3 *index;  // ebx
    unsigned int v14;  // esi
    unsigned int v15;  // ftop
    unsigned int v0;  // [bp-0x3c]
    unsigned int v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x34]
    int v3;  // [bp-0x30]
    int v4;  // [bp-0x2c]
    unsigned int v5;  // [bp-0x24]
    unsigned int v6;  // [bp-0x20]

    v8 = &idx->field_1318;
    sub_477420(v8, 0, 532, v9, v10, v11);
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_1328 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    *((unsigned short *)&v8->padding_0[0]) = 7;
    idx->field_1514 = 1;
    idx->field_1510 = 1;
    idx1 = g_4b44f4;
    idx->field_131a = 0;
    idx->field_1512 = 1;
    idx->field_151c = 22;
    idx->field_1520 = 40;
    idx->field_1518 = 131;
    index = g_4b44f4->field_18;
    g_4b44f4->field_48c = index;
    if (!index)
        return 0;
    v2 = v14;
    while (1)
    {
        if (*((int *)&index->padding_c[52]) >= idx->field_23c)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v54 = index->field_50;
            v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v5 = index->field_54;
            /* unsupported instruction */
            v54 = index->field_50;
            v6 = index->field_58;
            sub_41c580((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v16 = v15 - 1;
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
            if (/* unsupported instruction */)
            {
                v18 = CmpF(/* unsupported instruction */, 0) * 0x100 & 0x4500;
                /* unsupported instruction */
                v15 = v17 + 1;
            }
            else
            {
                v18 = CmpF(nan, 0) * 0x100 & 0x4500;
                /* unsupported instruction */
                v15 = v17 + 1;
            }
            if (!((char)((((unsigned short)v15 & 7) * 0x800 | (unsigned short)v18 & 0x4700) >> 8) & 65))
            {
                do
                {
                    idx->field_131c = v3;
                    idx->field_1320 = v4;
                    idx->field_1324 = v54;
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
                    if (/* unsupported instruction */)
                    {
                        v2 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v21 = v20 + 1;
                    }
                    else
                    {
                        v2 = nan;
                        /* unsupported instruction */
                        v21 = v20 + 1;
                    }
                    v1 = sub_464440();
                    v22 = v21 - 1;
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
                        v1 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v24 = v23 + 1;
                    }
                    else
                    {
                        v1 = nan;
                        /* unsupported instruction */
                        v24 = v23 + 1;
                    }
                    v25 = v24 - 1;
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
                        v1 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v27 = v26 + 1;
                    }
                    else
                    {
                        v1 = nan;
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
                        idx->field_1328 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v30 = v29 + 1;
                    }
                    else
                    {
                        idx->field_1328 = nan;
                        /* unsupported instruction */
                        v30 = v29 + 1;
                    }
                    v2 = sub_464440();
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
                        v2 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v33 = v32 + 1;
                    }
                    else
                    {
                        v2 = nan;
                        /* unsupported instruction */
                        v33 = v32 + 1;
                    }
                    v34 = v33 - 1;
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
                        idx->field_1330 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v36 = v35 + 1;
                    }
                    else
                    {
                        idx->field_1330 = nan;
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
                        v4 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v42 = v41 + 1;
                    }
                    else
                    {
                        v4 = nan;
                        /* unsupported instruction */
                        v42 = v41 + 1;
                    }
                    v43 = v42 - 1;
                    if (/* unsupported instruction */)
                    {
                        v44 = v43 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v44 = v43 - 1;
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
                        v54 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v45 = v44 + 1;
                    }
                    else
                    {
                        v54 = nan;
                        /* unsupported instruction */
                        v45 = v44 + 1;
                    }
                    sub_40b5e0();
                    v46 = v45 - 1;
                    if (/* unsupported instruction */)
                    {
                        v47 = v46 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v47 = v46 - 1;
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
                        v0 = /* unsupported instruction */;
                        /* unsupported instruction */
                        v48 = v47 + 1;
                    }
                    else
                    {
                        v0 = nan;
                        /* unsupported instruction */
                        v48 = v47 + 1;
                    }
                    v49 = v48 - 1;
                    if (/* unsupported instruction */)
                    {
                        v50 = v49 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v50 = v49 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    v51 = v50 - 1;
                    if (/* unsupported instruction */)
                    {
                        v52 = v51 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v52 = v51 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    v53 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v15 = v52 + 2;
                } while (!((char)((((unsigned short)v15 & 7) * 0x800 | (unsigned short)v53 & 0x4700) >> 8) & 65));
            }
            index->field_0->field_14(0, 0);
            idx1 = g_4b44f4;
        }
        if (!idx1[291])
            return 0;
        index = idx1[291]->field_8;
        idx1[291] = index;
        if (!index)
            return 0;
    }
}
