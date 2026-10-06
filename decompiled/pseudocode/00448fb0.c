// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x448fb0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_448fb0_5 {
    char padding_0[36];
    unsigned int field_24;
    unsigned int field_28;
    char padding_2c[652];
    int field_2b8;
    char padding_2bc[22216];
    unsigned int field_5984;
    char padding_5988[8];
    unsigned int field_5990;
    char padding_5994[228];
    unsigned int field_5a78;
    char padding_5a7c[4];
    struct st_448fb0_1 *field_5a80;
} st_448fb0_5;

typedef struct st_448fb0_2 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    int field_14;
} st_448fb0_2;

typedef struct st_448fb0_1 {
    char padding_0[28];
    struct st_448fb0_0 *field_1c;
} st_448fb0_1;

typedef struct st_448fb0_0 {
    char padding_0[12];
    char field_c;
    char padding_d[79];
    unsigned int field_5c;
    unsigned int field_60;
    unsigned int field_64;
} st_448fb0_0;

typedef struct st_448fb0_3 {
    char padding_0[12];
    char field_c;
    char padding_d[79];
    unsigned int field_5c;
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
} st_448fb0_3;

typedef struct st_448fb0_6 {
    char padding_0[102272];
    unsigned int field_18f80;
    char padding_18f84[16];
    unsigned int field_18f94;
} st_448fb0_6;

extern unsigned int g_4aee20[4];
extern unsigned int g_4aee38[4];
extern unsigned int g_4aee88[4];
extern unsigned int g_4aeea8;
extern st_448fb0_6 *g_4b43b8;
extern st_448fb0_1 *g_4b4518;

unsigned int sub_448fb0(st_448fb0_5 *a0)
{
    st_448fb0_5 *idx;  // eax
    unsigned int v17;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    st_448fb0_0 *idx1;  // esi
    st_448fb0_2 *idx2;  // eax
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ecx
    unsigned int v34;  // ftop
    st_448fb0_5 *v35;  // ebp
    unsigned int v18;  // ftop
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
    unsigned int v19;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int *v49;  // esi
    unsigned int v50;  // ftop
    unsigned int v51;  // ftop
    unsigned int v52;  // ftop
    unsigned int v53;  // ftop
    unsigned int v54;  // edi
    unsigned int v55;  // ftop
    unsigned int v20;  // ftop
    unsigned int v56;  // ftop
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v61;  // ftop
    unsigned int *v62;  // eax
    unsigned int v63;  // ftop
    unsigned int v64;  // ftop
    unsigned int v21;  // ftop
    unsigned int *v65;  // esi
    unsigned int v66;  // ftop
    unsigned int v67;  // ftop
    unsigned int v68;  // ftop
    int iter;  // edi
    unsigned int v70;  // ftop
    st_448fb0_5 *v71;  // ebp
    st_448fb0_3 *v72;  // esi
    st_448fb0_2 *index;  // eax
    unsigned int v74;  // ftop
    unsigned int v22;  // ftop
    unsigned int v75;  // ftop
    unsigned int v76;  // ftop
    unsigned int v77;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    int v0;  // [bp-0x50], Other Possible Types: unsigned int
    st_448fb0_3 *v1;  // [bp-0x4c], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x48]
    unsigned int v3;  // [bp-0x44]
    unsigned int v4;  // [bp-0x40]
    unsigned int v5;  // [bp-0x3c]
    unsigned int v6;  // [bp-0x38]
    unsigned int *v7;  // [bp-0x34]
    unsigned int *v8;  // [bp-0x30]
    unsigned int *v9;  // [bp-0x2c], Other Possible Types: unsigned int
    double v10;  // [bp-0x28], Other Possible Types: unsigned int, unsigned long
    st_448fb0_5 *v11;  // [bp-0x24], Other Possible Types: int, unsigned int
    unsigned int v12;  // [bp-0x10]
    unsigned int v13;  // [bp-0xc]
    unsigned int v14;  // [bp-0x8]
    unsigned int v15;  // [bp-0x4]

    idx = a0;
    if (idx->field_24 != 2)
    {
        if (idx->field_24 != 3)
            return 1;
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
        if (/* unsupported instruction */)
        {
            v13 = /* unsupported instruction */;
            /* unsupported instruction */
            v20 = v19 + 1;
        }
        else
        {
            v13 = nan;
            /* unsupported instruction */
            v20 = v19 + 1;
        }
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
            v14 = /* unsupported instruction */;
            /* unsupported instruction */
            v23 = v22 + 1;
        }
        else
        {
            v14 = nan;
            /* unsupported instruction */
            v23 = v22 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v24 = v23;
        if (idx->field_2b8 < 10)
        {
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v12 = idx->field_5a78 * 15 + 80;
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
            v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v24 = v28 + 2;
        }
        idx1 = g_4b4518->field_1c;
        idx2 = sub_46d8e4(&idx1->field_c);
        v31 = v24 - 1;
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
        v10 = v33;
        if (/* unsupported instruction */)
        {
            v10 = /* unsupported instruction */;
            /* unsupported instruction */
            v34 = v32 + 1;
        }
        else
        {
            v10 = (double)nan;
            /* unsupported instruction */
            v34 = v32 + 1;
        }
        v9 = g_4aeea8;
        v8 = g_4aee38[idx1->field_64];
        v7 = g_4aee20[2 * idx1->field_5c + idx1->field_60];
        v6 = idx2->field_4;
        v5 = idx2->field_8;
        v4 = idx2->field_c;
        v3 = idx2->field_10 + 1;
        v2 = idx2->field_14 % 100;
        v1 = "        ";
        v0 = idx->field_5a78 + 1;
        sub_4015c0("No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%");
        v35 = a0;
        if (v35->field_2b8 < 10)
            goto LABEL_44932c;
        v36 = v34 - 1;
        if (/* unsupported instruction */)
        {
            v37 = v36 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v37 = v36 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (/* unsupported instruction */)
        {
            v13 = /* unsupported instruction */;
            /* unsupported instruction */
            v38 = v37 + 1;
        }
        else
        {
            v13 = nan;
            /* unsupported instruction */
            v38 = v37 + 1;
        }
        v39 = v38 - 1;
        if (/* unsupported instruction */)
        {
            v40 = v39 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v40 = v39 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v11 = &v35->padding_2bc[22204];
        if (/* unsupported instruction */)
        {
            v14 = /* unsupported instruction */;
            /* unsupported instruction */
            v41 = v40 + 1;
        }
        else
        {
            v14 = nan;
            /* unsupported instruction */
            v41 = v40 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4b43b8->field_18f80 = 0xffffffff;
        sub_4015c0("%s");
        a0 = v35->field_5984 * 9;
        v42 = v41 - 1;
        if (/* unsupported instruction */)
        {
            v43 = v42 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v43 = v42 - 1;
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
            v13 = /* unsupported instruction */;
            /* unsupported instruction */
            v44 = v43 + 1;
        }
        else
        {
            v13 = nan;
            /* unsupported instruction */
            v44 = v43 + 1;
        }
        if (v35->field_5984 == 8)
        {
            v45 = v44 - 1;
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
                v13 = /* unsupported instruction */;
                /* unsupported instruction */
                v44 = v46 + 1;
            }
            else
            {
                v13 = nan;
                /* unsupported instruction */
                v44 = v46 + 1;
            }
        }
        g_4b43b8->field_18f80 = 0xffffff00;
        sub_4015c0("_");
        v47 = v44 - 1;
        if (/* unsupported instruction */)
        {
            v48 = v47 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v48 = v47 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v49 = g_4b43b8;
        if (/* unsupported instruction */)
        {
            v13 = /* unsupported instruction */;
            /* unsupported instruction */
            v50 = v48 + 1;
        }
        else
        {
            v13 = nan;
            /* unsupported instruction */
            v50 = v48 + 1;
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
        if (/* unsupported instruction */)
        {
            v14 = /* unsupported instruction */;
            /* unsupported instruction */
            v53 = v52 + 1;
        }
        else
        {
            v14 = nan;
            /* unsupported instruction */
            v53 = v52 + 1;
        }
        g_4b43b8->field_18f80 = 0xffffffff;
        /* unsupported instruction */
        /* unsupported instruction */
        v54 = 0;
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        while (1)
        {
            v49[25568] = (-(0 < v35->field_5990 - v54) & 0xff808180) - 0x100;
            v11 = (v54 < 88 ? *((char *)(v54 + 4853384)) : (v54 == 88 ? 129 : (unsigned int)((v54 != 89) + 127)));
            sub_4015c0("%c");
            if (v54 % 13 == 12)
            {
                v55 = v53 - 1;
                if (/* unsupported instruction */)
                {
                    v56 = v55 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v56 = v55 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                if (/* unsupported instruction */)
                {
                    v13 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v57 = v56 + 1;
                }
                else
                {
                    v13 = nan;
                    /* unsupported instruction */
                    v57 = v56 + 1;
                }
                v58 = v57 - 1;
                if (/* unsupported instruction */)
                {
                    v59 = v58 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v59 = v58 - 1;
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
                    v14 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v53 = v59 + 1;
                }
                else
                {
                    v14 = nan;
                    /* unsupported instruction */
                    v53 = v59 + 1;
                }
            }
            else
            {
                v60 = v53 - 1;
                if (/* unsupported instruction */)
                {
                    v61 = v60 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v61 = v60 - 1;
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
                    v13 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v53 = v61 + 1;
                }
                else
                {
                    v13 = nan;
                    /* unsupported instruction */
                    v53 = v61 + 1;
                }
            }
            v54 += 1;
            if (v54 >= 91)
                break;
            v49 = g_4b43b8;
        }
        v62 = g_4b43b8;
        g_4b43b8->field_18f80 = 0xffffffff;
    }
    else
    {
        v63 = v17 - 1;
        if (/* unsupported instruction */)
        {
            v64 = v63 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v64 = v63 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        v65 = g_4b43b8;
        if (/* unsupported instruction */)
        {
            v13 = /* unsupported instruction */;
            /* unsupported instruction */
            v66 = v64 + 1;
        }
        else
        {
            v13 = nan;
            /* unsupported instruction */
            v66 = v64 + 1;
        }
        g_4b43b8->field_18f94 = 1;
        v67 = v66 - 1;
        if (/* unsupported instruction */)
        {
            v68 = v67 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v68 = v67 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        iter = 0;
        if (/* unsupported instruction */)
        {
            v14 = /* unsupported instruction */;
            /* unsupported instruction */
            v70 = v68 + 1;
        }
        else
        {
            v14 = nan;
            /* unsupported instruction */
            v70 = v68 + 1;
        }
        v71 = &idx->field_5a80;
        /* unsupported instruction */
        /* unsupported instruction */
        v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        while (1)
        {
            v65[25568] = (-(0 < idx->field_28 - iter) & 0xff808180) - 0x100;
            if (*((int *)&v71->padding_0[0]))
            {
                v72 = *((int *)(*((int *)&v71->padding_0[0]) + 28));
                index = sub_46d8e4(&v72->field_c);
                v74 = v70 - 1;
                if (/* unsupported instruction */)
                {
                    v75 = v74 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v75 = v74 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v10 = v33;
                if (/* unsupported instruction */)
                {
                    v10 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v70 = v75 + 1;
                }
                else
                {
                    v10 = (double)nan;
                    /* unsupported instruction */
                    v70 = v75 + 1;
                }
                v9 = g_4aee88[v72->field_68];
                v8 = g_4aee38[v72->field_64];
                v7 = g_4aee20[2 * v72->field_5c + v72->field_60];
                v6 = index->field_4;
                v5 = index->field_8;
                v4 = index->field_c;
                v3 = index->field_10 + 1;
                iter += 1;
                v2 = index->field_14 % 100;
                v1 = v72;
                v0 = iter;
                sub_4015c0("No.%.2d %s %.2d/%.2d/%.2d %.2d:%.2d %s %s %s %2.1f%%");
            }
            else
            {
                iter += 1;
                v11 = iter;
                sub_4015c0("No.%.2d -------- --/--/-- --:-- ------- ------- --- ---%%");
            }
            v76 = v70 - 1;
            if (/* unsupported instruction */)
            {
                v77 = v76 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v77 = v76 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v71 = &v71->padding_0[4];
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
                v14 = /* unsupported instruction */;
                /* unsupported instruction */
                v70 = v77 + 1;
                if (iter >= 25)
                    break;
            }
            else
            {
                v14 = nan;
                /* unsupported instruction */
                v70 = v77 + 1;
                if (iter >= 25)
                    break;
            }
            v65 = g_4b43b8;
            idx = a0;
        }
LABEL_44932c:
        v62 = g_4b43b8;
    }
    v62[25573] = 0;
    v62[25568] = 0xffffffff;
    return 1;
}
