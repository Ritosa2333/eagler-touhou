// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x448690; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_448690_2 {
    char padding_0[24];
    unsigned int field_18;
    char field_1c;
    char field_1d;
} st_448690_2;

typedef struct st_448690_1 {
    char padding_0[24];
    unsigned int field_18;
    char padding_1c[1];
    char field_1d;
} st_448690_1;

typedef struct st_448690_0 {
    char padding_0[102272];
    unsigned int field_18f80;
    char padding_18f84[16];
    unsigned int field_18f94;
} st_448690_0;

extern unsigned int g_4aee60[4];
extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;
extern unsigned int g_4b0ca8;
extern st_448690_0 *g_4b43b8;
extern unsigned int g_4b451c;

unsigned int sub_448690(unsigned int *a0)
{
    unsigned int *v16;  // ecx
    unsigned int v17;  // ftop
    unsigned int iter;  // edi
    unsigned int v27;  // ebp
    unsigned int v28;  // eax
    unsigned int v29;  // ecx
    st_448690_1 *v30;  // ecx
    unsigned int *idx;  // eax
    st_448690_2 *v32;  // ecx
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    st_448690_2 *v35;  // ecx
    unsigned int v18;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ftop
    unsigned int *v44;  // ebp
    unsigned int v45;  // ftop
    unsigned int v19;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // ftop
    unsigned int v52;  // ftop
    unsigned int v53;  // ftop
    unsigned int v54;  // ftop
    unsigned int v55;  // ftop
    st_448690_0 *index;  // esi
    st_448690_0 *v56;  // esi
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v61;  // edi
    unsigned int v62;  // ftop
    unsigned int v63;  // ftop
    unsigned int v64;  // ftop
    unsigned int v65;  // ftop
    unsigned int v21;  // ftop
    unsigned int v66;  // ftop
    unsigned int v67;  // ftop
    unsigned int v68;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    int v24;  // eax
    unsigned int v25;  // ftop
    unsigned int v0;  // [bp-0x50]
    st_448690_2 *v1;  // [bp-0x4c]
    unsigned int v2;  // [bp-0x48]
    unsigned int v3;  // [bp-0x44]
    unsigned int v4;  // [bp-0x40]
    unsigned int v5;  // [bp-0x3c]
    unsigned int v6;  // [bp-0x38]
    st_448690_1 *v7;  // [bp-0x34], Other Possible Types: unsigned int
    unsigned int v8;  // [bp-0x30]
    unsigned int *v9;  // [bp-0x2c], Other Possible Types: unsigned int
    st_448690_2 *v10;  // [bp-0x28], Other Possible Types: double, unsigned long
    unsigned int *v11;  // [bp-0x24], Other Possible Types: unsigned int
    int v12;  // [bp-0x10]
    unsigned int v13;  // [bp-0xc]
    unsigned int v14;  // [bp-0x8]
    unsigned int v15;  // [bp-0x4]

    v16 = a0;
    if (v16[9] != 2)
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
    index = g_4b43b8;
    if (/* unsupported instruction */)
    {
        v13 = /* unsupported instruction */;
        /* unsupported instruction */
        v21 = v19 + 1;
    }
    else
    {
        v13 = nan;
        /* unsupported instruction */
        v21 = v19 + 1;
    }
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
    v24 = 0xff;
    if (/* unsupported instruction */)
    {
        v14 = /* unsupported instruction */;
        /* unsupported instruction */
        v25 = v23 + 1;
    }
    else
    {
        v14 = nan;
        /* unsupported instruction */
        v25 = v23 + 1;
    }
    iter = 0;
    v27 = g_4b0ca8 * 280;
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    g_4b43b8->field_18f94 = 1;
    v12 = 0xff;
    while (1)
    {
        if (v16[5730])
            index->field_18f80 = (v24 * 0x100 | v24) * 0x100 | 0xff0000ff;
        else
            index->field_18f80 = (-(0 < v16[10] - iter) & 0xff404041) - 1;
        v28 = (g_4b0c94 + g_4b0c90 * 2) * 17908;
        v29 = v28 + v27;
        v30 = v29 + g_4b451c;
        if (*((int *)(v29 + g_4b451c + 40)) || *((int *)&v30[1].padding_0[14]))
        {
            idx = sub_46d8e4(v28 + g_4b451c + 8 + v27 + 32);
            v32 = (g_4b0c94 + g_4b0c90 * 2) * 17908 + v27 + g_4b451c;
            v33 = v25 - 1;
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
            v10 = v32;
            v35 = v32 + 1;
            if (/* unsupported instruction */)
            {
                v10 = /* unsupported instruction */;
                /* unsupported instruction */
                v36 = v34 + 1;
            }
            else
            {
                v10 = (double)nan;
                /* unsupported instruction */
                v36 = v34 + 1;
            }
            v9 = g_4aee60[v32->field_1c];
            v8 = idx[1];
            v7 = idx[2];
            v6 = idx[3];
            v5 = idx[4] + 1;
            v4 = idx[5] + 1900;
            v3 = *(&v35->padding_0[0] - 1);
            v2 = *((int *)(&v35->padding_0[0] - 6));
            v1 = v35;
            iter += 1;
            v0 = iter;
            sub_4015c0("%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%");
        }
        else
        {
            v37 = v25 - 1;
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
            iter += 1;
            if (/* unsupported instruction */)
            {
                v10 = /* unsupported instruction */;
                /* unsupported instruction */
                v36 = v38 + 1;
            }
            else
            {
                v10 = (double)nan;
                /* unsupported instruction */
                v36 = v38 + 1;
            }
            v9 = v30->field_1d;
            v8 = v30->field_18;
            v7 = v30 + 1;
            v6 = iter;
            sub_4015c0("%2d  %s  %9ld%d  ----/--/-- --:--  Stage -  ---%%");
        }
        v39 = v36 - 1;
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
        /* unsupported instruction */
        /* unsupported instruction */
        v27 += 28;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v12 -= 16;
        v14 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v43 = v42 + 1;
        if (v12 <= 95)
            break;
        index = g_4b43b8;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v25 = v43 + 1;
        v24 = v12;
        v16 = a0;
    }
    v44 = a0;
    if (v44[5730])
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return 1;
    }
    v45 = v43 - 1;
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
        v13 = /* unsupported instruction */;
        /* unsupported instruction */
        v47 = v46 + 1;
    }
    else
    {
        v13 = nan;
        /* unsupported instruction */
        v47 = v46 + 1;
    }
    v11 = v44 + 5726;
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
    g_4b43b8->field_18f80 = 0xffffffff;
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
        v48 = v47 + 1;
    }
    else
    {
        v14 = nan;
        /* unsupported instruction */
        v48 = v47 + 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    sub_4015c0("%s");
    a0 = v44[5729] * 9;
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
        v51 = v50 + 1;
    }
    else
    {
        v13 = nan;
        /* unsupported instruction */
        v51 = v50 + 1;
    }
    if (v44[5729] == 8)
    {
        v52 = v51 - 1;
        if (/* unsupported instruction */)
        {
            v53 = v52 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v53 = v52 - 1;
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
            v51 = v53 + 1;
        }
        else
        {
            v13 = nan;
            /* unsupported instruction */
            v51 = v53 + 1;
        }
    }
    g_4b43b8->field_18f80 = 0xffffff00;
    sub_4015c0("_");
    v54 = v51 - 1;
    if (/* unsupported instruction */)
    {
        v55 = v54 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v55 = v54 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    v56 = g_4b43b8;
    if (/* unsupported instruction */)
    {
        v13 = /* unsupported instruction */;
        /* unsupported instruction */
        v57 = v55 + 1;
    }
    else
    {
        v13 = nan;
        /* unsupported instruction */
        v57 = v55 + 1;
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
        v14 = /* unsupported instruction */;
        /* unsupported instruction */
        v60 = v59 + 1;
    }
    else
    {
        v14 = nan;
        /* unsupported instruction */
        v60 = v59 + 1;
    }
    g_4b43b8->field_18f80 = 0xffffffff;
    /* unsupported instruction */
    /* unsupported instruction */
    v61 = 0;
    v15 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    while (1)
    {
        v56->field_18f80 = (-(0 < v44[5732] - v61) & 0xff808180) - 0x100;
        if (v61 < 88)
            v11 = *((char *)(v61 + 4853384));
        else
            v11 = (v61 == 88 ? 129 : (unsigned int)((v61 != 89) + 127));
        sub_4015c0("%c");
        if (v61 % 13 == 12)
        {
            v62 = v60 - 1;
            if (/* unsupported instruction */)
            {
                v63 = v62 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v63 = v62 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v13 = /* unsupported instruction */;
                /* unsupported instruction */
                v64 = v63 + 1;
            }
            else
            {
                v13 = nan;
                /* unsupported instruction */
                v64 = v63 + 1;
            }
            v65 = v64 - 1;
            if (/* unsupported instruction */)
            {
                v66 = v65 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v66 = v65 - 1;
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
                v60 = v66 + 1;
            }
            else
            {
                v14 = nan;
                /* unsupported instruction */
                v60 = v66 + 1;
            }
        }
        else
        {
            v67 = v60 - 1;
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
                v60 = v68 + 1;
            }
            else
            {
                v13 = nan;
                /* unsupported instruction */
                v60 = v68 + 1;
            }
        }
        v61 += 1;
        if (v61 >= 91)
            break;
        v56 = g_4b43b8;
    }
    g_4b43b8->field_18f80 = 0xffffffff;
    return 1;
}
