// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41bb40; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41bb40_3 {
    struct st_41bb40_1 *field_0;
    struct st_41bb40_3 *field_4;
    struct st_41bb40_3 *field_8;
    char padding_c[116];
    unsigned int field_80;
} st_41bb40_3;

typedef struct st_41bb40_2 {
    unsigned int field_0;
} st_41bb40_2;

typedef struct st_41bb40_1 {
    char padding_0[4];
    struct st_41bb40_2 *field_4;
} st_41bb40_1;

typedef struct st_41bb40_5 {
    char padding_0[8];
    unsigned int field_8;
    char padding_c[1096];
    unsigned int field_454;
    unsigned int field_458;
} st_41bb40_5;

typedef struct st_41bb40_4 {
    char padding_0[24];
    struct st_41bb40_5 *field_18;
    char padding_1c[1096];
    struct st_41bb40_3 *field_464;
    unsigned int field_468;
    unsigned int field_46c;
} st_41bb40_4;

typedef struct st_41bb40_0 {
    char padding_0[100];
    char field_64;
    char padding_65[1211];
    unsigned int field_520;
    unsigned int field_524;
    unsigned int field_528;
    char padding_52c[106];
    unsigned short field_596;
    char padding_598[252];
    unsigned int field_694;
    char padding_698[964];
    char field_a5c;
} st_41bb40_0;

extern st_41bb40_0 *g_4b43c8;
extern st_41bb40_4 *g_4b44f4;

unsigned int sub_41bb40(void)
{
    unsigned int node;  // esi
    unsigned int v26;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // edi
    unsigned int v37;  // ftop
    int v38;  // ebp
    unsigned int v39;  // ebp
    st_41bb40_3 *idx;  // eax
    st_41bb40_3 *v42;  // ecx
    unsigned int v43;  // ftop
    unsigned int *iter;  // eax
    unsigned int v27;  // ftop
    unsigned int v45;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v28;  // ftop
    st_41bb40_4 *index;  // ebx
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    int v0;  // [bp-0x204]
    int v1;  // [bp-0x200]
    int v2;  // [bp-0x1f8]
    unsigned int v3;  // [bp-0x1f0]
    unsigned int v4;  // [bp-0x1f0]
    char v5;  // [bp-0x1ec], Other Possible Types: unsigned int
    unsigned int v6;  // [bp-0x1e8]
    unsigned int v7;  // [bp-0x1e4]
    unsigned int v8;  // [bp-0x1e0]
    unsigned int v9;  // [bp-0x1dc]
    unsigned int v10;  // [bp-0x1d8]
    unsigned int v11;  // [bp-0x1d4]
    unsigned int v12;  // [bp-0x1d0]
    unsigned int v13;  // [bp-0x1cc]
    unsigned short v14;  // [bp-0x1c8]
    unsigned short v15;  // [bp-0x1c6]
    unsigned int v16;  // [bp-0x1c0]
    unsigned int v17;  // [bp-0x1c0]
    char v18;  // [bp-0x1bc]
    unsigned int v19;  // [bp-0x1b4]
    unsigned int v20;  // [bp-0x1ac]
    unsigned int v21;  // [bp-0xc]
    unsigned int v22;  // [bp-0x8]
    char v23;  // [bp-0x4]

    node = &g_4b43c8->field_64;
    sub_477420(&v5, 0, 488, v0, v1, &v23, v2);
    sub_477420(&v18, 0, 432);
    v27 = v26 - 1;
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
    index = g_4b44f4;
    v3 = 2000;
    do
    {
        v4 = v3;
        if (*((char *)node) & 1 && *((short *)(node + 1330)) == 1 && *((int *)(node + 1584)) == 1)
        {
            v30 = v28 - 1;
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
            if (/* unsupported instruction */)
            {
                v8 = /* unsupported instruction */;
                /* unsupported instruction */
                v32 = v31 + 1;
            }
            else
            {
                v8 = nan;
                /* unsupported instruction */
                v32 = v31 + 1;
            }
            v33 = v32 - 1;
            if (/* unsupported instruction */)
            {
                v34 = v33 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v34 = v33 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v17 = v16 | 1;
            if (/* unsupported instruction */)
            {
                v13 = /* unsupported instruction */;
                /* unsupported instruction */
                v35 = v34 + 1;
            }
            else
            {
                v13 = nan;
                /* unsupported instruction */
                v35 = v34 + 1;
            }
            v5 = *((int *)(node + 1212));
            if (/* unsupported instruction */)
                v10 = /* unsupported instruction */;
            else
                v10 = nan;
            v6 = *((int *)(node + 1216));
            if (/* unsupported instruction */)
                v9 = /* unsupported instruction */;
            else
                v9 = nan;
            v7 = *((int *)(node + 1220));
            /* unsupported instruction */
            /* unsupported instruction */
            v11 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v36 = &index->field_468;
            v12 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v37 = v35;
            v14 = 7;
            v15 = 6;
            v21 = 19;
            v22 = 0xffffffff;
            v20 = 0x200;
            v19 = 1200;
            if (*((int *)v36) >= 0x100)
            {
                v38 = 0;
            }
            else
            {
                index->field_46c = index->field_46c + 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v39 = &index->field_46c;
                if (index->field_46c < 0x10000)
                    *((unsigned int *)v39) = 0x10000;
                idx = (!sub_46c9ea(4004) ? NULL : sub_428520());
                idx->field_80 = *((int *)v39);
                v42 = index->field_464;
                idx->field_4 = v42;
                v42->field_8 = idx;
                *((unsigned int *)v36) = *((int *)v36) + 1;
                index->field_464 = idx;
                idx->field_0->field_4(&v5);
                v43 = v37 - 0;
                if (/* unsupported instruction */)
                {
                    v37 = v43 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v37 = v43 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v38 = *((int *)v39);
                index = g_4b44f4;
            }
            iter = index->field_18;
            if (v38 && index->field_18)
            {
                do
                {
                    if (iter[32] == v38)
                        goto LABEL_41bce8;
                } while ((iter = (unsigned int *)iter[2], iter));
            }
            iter = 0;
LABEL_41bce8:
            v45 = v37 - 1;
            if (/* unsupported instruction */)
            {
                v46 = v45 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v46 = v45 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                iter[277] = /* unsupported instruction */;
                /* unsupported instruction */
                v47 = v46 + 1;
            }
            else
            {
                iter[277] = nan;
                /* unsupported instruction */
                v47 = v46 + 1;
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
                iter[278] = /* unsupported instruction */;
                /* unsupported instruction */
                v28 = v49 + 1;
            }
            else
            {
                iter[278] = nan;
                /* unsupported instruction */
                v28 = v49 + 1;
            }
        }
    } while ((node += 2552, v3 = v4 - 1, v4 != 1));
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    return 0;
}
