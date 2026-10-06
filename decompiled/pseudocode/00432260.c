// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x432260; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_432260_2 {
    char padding_0[16];
    struct st_432260_1 *field_10;
    struct st_432260_2 *field_14;
} st_432260_2;

typedef struct st_432260_4 {
    char padding_0[12];
    int field_c;
    unsigned int field_10;
    int field_14;
} st_432260_4;

typedef struct st_432260_13 {
    char padding_0[24];
    int field_18;
    char field_1c;
    char field_1d;
} st_432260_13;

typedef struct st_432260_6 {
    char padding_0[28];
    struct st_432260_5 *field_1c;
} st_432260_6;

typedef struct st_432260_11 {
    char padding_0[24];
    int field_18;
    char padding_1c[1];
    char field_1d;
    char padding_1e[14];
    unsigned int field_2c;
} st_432260_11;

typedef struct st_432260_7 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[12];
    int field_14;
    char padding_18[32];
    unsigned int field_38;
    char padding_3c[432];
    int field_1ec;
    char padding_1f0[8];
    unsigned int field_1f8;
    char padding_1fc[8];
    struct st_432260_6 *field_204;
} st_432260_7;

typedef struct st_432260_1 {
    char padding_0[956];
    unsigned int field_3bc;
    char padding_3c0[42];
    unsigned short field_3ea;
} st_432260_1;

typedef struct st_432260_5 {
    char padding_0[12];
    char field_c;
    char padding_d[79];
    unsigned int field_5c;
    unsigned int field_60;
    unsigned int field_64;
    unsigned int field_68;
} st_432260_5;

typedef struct st_432260_3 {
    char padding_0[956];
    unsigned int field_3bc;
} st_432260_3;

typedef struct st_432260_0 {
    char padding_0[102272];
    unsigned int field_18f80;
    char padding_18f84[16];
    unsigned int field_18f94;
} st_432260_0;

extern unsigned int g_4aee20[4];
extern unsigned int g_4aee4c[4];
extern unsigned int g_4aee60[4];
extern unsigned int g_4aee88[4];
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern unsigned int g_4b0cb0;
extern st_432260_0 *g_4b43b8;
extern st_432260_6 *g_4b4518;
extern unsigned int g_4b451c;
extern st_432260_3 *g_4ceaac;

unsigned int sub_432260(st_432260_7 *idx)
{
    unsigned int *index;  // esi
    int v13;  // edi
    unsigned int v21;  // ftop
    int node;  // edi
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    st_432260_5 *v26;  // esi
    st_432260_4 *v27;  // eax
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int *v30;  // esi
    int v14;  // esi
    st_432260_4 *v31;  // eax
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    int v15;  // ebp
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ftop
    int iter;  // edi
    unsigned int v45;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ecx
    unsigned int v50;  // eax
    int v16;  // ebx
    unsigned int v51;  // edx
    st_432260_11 *v52;  // edx
    st_432260_4 *v53;  // eax
    st_432260_13 *v54;  // ecx
    unsigned int v55;  // ftop
    unsigned int v56;  // ftop
    st_432260_2 *v17;  // eax
    st_432260_2 *iter1;  // eax
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v0;  // [bp-0x3c]
    st_432260_7 *v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x34]
    unsigned int v3;  // [bp-0x30]
    unsigned int v4;  // [bp-0x2c]
    unsigned int v5;  // [bp-0x24]
    unsigned int v6;  // [bp-0x20]
    unsigned int v7;  // [bp-0x18]
    int v8;  // [bp-0x14], Other Possible Types: unsigned int
    int v9;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0xc]
    st_432260_7 *v11;  // [bp-0x4], Other Possible Types: unsigned int

    index = g_4b43b8;
    g_4b43b8->field_18f94 = 1;
    if (!sub_461920(idx->field_1ec, v13, v14, v15, v16))
    {
        idx->field_1ec = 0;
    }
    else
    {
        v17 = sub_461920(idx->field_1ec);
        if (!v17)
            idx->field_1ec = 0;
        iter1 = &v17->field_10;
        if (iter1)
        {
            do
            {
                if (*((short *)(*((int *)&iter1->padding_0[0]) + 1002)) == 79 && *((int *)&iter1->padding_0[0]))
                {
                    g_4ceaac->field_3bc = *((int *)(*((int *)&iter1->padding_0[0]) + 956)) | 0xff000000;
                    break;
                }
            } while ((iter1 = (st_432260_2 *)*((int *)&iter1->padding_0[4]), iter1));
        }
    }
    switch (idx->field_4)
    {
    case 9: case 16: case 23:
        v19 = v32 - 1;
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
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
            v21 = v20 + 1;
        }
        else
        {
            v8 = nan;
            /* unsupported instruction */
            v21 = v20 + 1;
        }
        node = 0;
        v23 = v21 - 1;
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
        v11 = &idx->field_204;
        if (/* unsupported instruction */)
        {
            v9 = /* unsupported instruction */;
            /* unsupported instruction */
            v25 = v24 + 1;
        }
        else
        {
            v9 = nan;
            /* unsupported instruction */
            v25 = v24 + 1;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        while (1)
        {
            index[25568] = (-(0 < idx->field_38 - node) & 0xff808180) - 0x100;
            if (v11->padding_0)
            {
                v26 = *((int *)&v11->padding_0[28]);
                v27 = sub_46d8e4(&v26->field_c);
                node += 1;
                sub_4015c0("No.%.2d %s %.2d/%.2d/%.2d %s %s %s", node, v26, v27->field_14 % 100, v27->field_10 + 1, v27->field_c, g_4aee20[2 * v26->field_5c + v26->field_60], g_4aee4c[v26->field_64], g_4aee88[v26->field_68]);
            }
            else
            {
                node += 1;
                sub_4015c0("No.%.2d -------- --/--/-- ------- - St-", node);
            }
            v28 = v25 - 1;
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
            v11 = &v11->field_4;
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
            index = g_4b43b8;
            if (/* unsupported instruction */)
            {
                v9 = /* unsupported instruction */;
                /* unsupported instruction */
                v25 = v29 + 1;
                if (node >= 25)
                    break;
            }
            else
            {
                v9 = nan;
                /* unsupported instruction */
                v25 = v29 + 1;
                if (node >= 25)
                    break;
            }
        }
        goto LABEL_4326ad;
    case 10: case 17: case 24:
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v11 = idx->field_38;
        v7 = idx->field_14;
        if (idx->field_14 < 10)
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
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
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
        }
        else if (/* unsupported instruction */)
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
            v9 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v9 = nan;
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
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v8 = nan;
            /* unsupported instruction */
        }
        v2 = v8;
        v3 = v9;
        v1 = idx;
        v4 = v10;
        sub_4320f0();
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
            v5 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v5 = nan;
            /* unsupported instruction */
        }
        v30 = g_4b4518->field_1c;
        v0 = v30 + 3;
        v31 = sub_46d8e4();
        sub_4015c0("No.%.2d          %.2d/%.2d/%.2d %s %s %s", idx->field_38 + 1, v31->field_14 % 100, v31->field_10 + 1, v31->field_c, g_4aee20[2 * v30[23] + v30[24]], g_4aee4c[v30[25]], g_4aee88[g_4b0cb0]);
        index = g_4b43b8;
        break;
    case 18: case 25:
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
        if (/* unsupported instruction */)
        {
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
            v35 = v34 + 1;
        }
        else
        {
            v8 = nan;
            /* unsupported instruction */
            v35 = v34 + 1;
        }
        v4 = "            Score Ranking!!";
        /* unsupported instruction */
        /* unsupported instruction */
        v10 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v11 = idx->field_38;
        /* unsupported instruction */
        /* unsupported instruction */
        v9 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_4015c0();
        v36 = v35 - 1;
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
            v9 = /* unsupported instruction */;
            /* unsupported instruction */
            v38 = v37 + 1;
        }
        else
        {
            v9 = nan;
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
        if (/* unsupported instruction */)
        {
            v8 = /* unsupported instruction */;
            /* unsupported instruction */
            v41 = v40 + 1;
            if (idx->field_1f8)
                goto LABEL_432562;
LABEL_432541:
            sub_4320f0(idx, v8, v9, v10);
        }
        else
        {
            v8 = nan;
            /* unsupported instruction */
            v41 = v40 + 1;
            if (!idx->field_1f8)
                goto LABEL_432541;
LABEL_432562:
            v11 = 0xffffffff;
        }
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
        iter = 0;
        if (/* unsupported instruction */)
        {
            v5 = /* unsupported instruction */;
            /* unsupported instruction */
            v45 = v43 + 1;
        }
        else
        {
            v5 = nan;
            /* unsupported instruction */
            v45 = v43 + 1;
        }
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
            v6 = /* unsupported instruction */;
            /* unsupported instruction */
            v48 = v47 + 1;
        }
        else
        {
            v6 = nan;
            /* unsupported instruction */
            v48 = v47 + 1;
        }
        while (1)
        {
            g_4b43b8->field_18f80 = (-(0 < iter - v8) & 0xff808180) - 0x100;
            v49 = (g_4b0c94 + g_4b0c90 * 2) * 17908;
            v50 = (iter + g_4b0ca8 * 10) * 28;
            v51 = v50 + v49;
            v52 = v51 + g_4b451c;
            if (*((int *)(v51 + g_4b451c + 40)) || v52->field_2c)
            {
                v53 = sub_46d8e4(v50 + v49 + g_4b451c + 8 + 32);
                v54 = (g_4b0c94 + g_4b0c90 * 2) * 17908 + g_4b451c + (iter + g_4b0ca8 * 10) * 28;
                iter += 1;
                sub_4015c0("%2d %s %.9ld%d %.2d/%.2d/%.2d %s", iter, v54 + 1, v54->field_18, v54->field_1d, v53->field_14 % 100, v53->field_10 + 1, v53->field_c, g_4aee60[v54->field_1c]);
            }
            else
            {
                iter += 1;
                sub_4015c0("%2d %s %.9ld%d --/--/-- Stage -", iter, v52->padding_1e, v52->field_18, v52->field_1d);
            }
            v55 = v48 - 1;
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
                v6 = /* unsupported instruction */;
                /* unsupported instruction */
                v48 = v56 + 1;
                if (iter >= 10)
                    break;
            }
            else
            {
                v6 = nan;
                /* unsupported instruction */
                v48 = v56 + 1;
                if (iter >= 10)
                    break;
            }
        }
        index = g_4b43b8;
LABEL_4326ad:
        index[25568] = 0xffffffff;
        break;
    }
    index[25573] = 0;
    return 1;
}
