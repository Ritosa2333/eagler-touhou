// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x42c260; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_42c260_0 {
    char padding_0[8];
    struct st_42c260_0 *field_8;
    unsigned int field_c;
    unsigned int field_10;
    char padding_14[84];
    unsigned int field_68;
    char padding_6c[1024];
    unsigned int field_46c;
    char padding_470[20];
    unsigned int field_484;
    char padding_488[468];
    unsigned int field_65c;
    char padding_660[960];
    unsigned int field_a20;
    char padding_a24[184];
    unsigned int field_adc;
} st_42c260_0;

typedef struct st_42c260_1 {
    char padding_0[24];
    struct st_42c260_0 *field_18;
    char padding_1c[1136];
    struct st_42c260_0 *field_48c;
} st_42c260_1;

extern st_42c260_1 *g_4b44f4;

unsigned int sub_42c260(void)
{
    st_42c260_0 **index;  // ecx
    unsigned int v15;  // esi
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ecx
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    st_42c260_0 *idx;  // esi
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
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
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v48;  // ftop
    unsigned int v49;  // ftop
    unsigned int v50;  // ftop
    unsigned int v51;  // ftop
    unsigned int v52;  // ftop
    unsigned int v17;  // ftop
    unsigned int v53;  // ftop
    unsigned int v54;  // ftop
    unsigned int v55;  // ftop
    unsigned int v56;  // ftop
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v62;  // ftop
    unsigned int v18;  // ftop
    unsigned int v63;  // 4188
    unsigned int v64;  // ftop
    unsigned int v65;  // ftop
    unsigned int v66;  // ftop
    unsigned int v67;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v0;  // [bp-0x5c]
    unsigned int v1;  // [bp-0x58]
    unsigned int v2;  // [bp-0x54]
    unsigned int v3;  // [bp-0x50]
    unsigned int v4;  // [bp-0x4c]
    unsigned int v5;  // [bp-0x48]
    unsigned int v6;  // [bp-0x44]
    unsigned int v7;  // [bp-0x40]
    unsigned int v8;  // [bp-0x3c]
    unsigned int v9;  // [bp-0x38]
    unsigned int v10;  // [bp-0x34]
    unsigned int v11;  // [bp-0x30]
    unsigned int v12;  // [bp-0x14]

    index = g_4b44f4;
    v12 = v15;
    idx = g_4b44f4->field_18;
    g_4b44f4->field_48c = idx;
    if (!idx)
        return 0;
    while (1)
    {
        if (idx->field_10 == 1 && idx->field_c == 3)
        {
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
                v10 = /* unsupported instruction */;
                /* unsupported instruction */
                v20 = v19 + 1;
            }
            else
            {
                v10 = nan;
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
                v8 = /* unsupported instruction */;
                /* unsupported instruction */
                v23 = v22 + 1;
            }
            else
            {
                v8 = nan;
                /* unsupported instruction */
                v23 = v22 + 1;
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
            if (/* unsupported instruction */)
            {
                v7 = /* unsupported instruction */;
                /* unsupported instruction */
                v26 = v25 + 1;
            }
            else
            {
                v7 = nan;
                /* unsupported instruction */
                v26 = v25 + 1;
            }
            sub_42e880();
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
            v6 = v29;
            if (/* unsupported instruction */)
            {
                v9 = /* unsupported instruction */;
                /* unsupported instruction */
                v30 = v28 + 1;
            }
            else
            {
                v9 = nan;
                /* unsupported instruction */
                v30 = v28 + 1;
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
            if (/* unsupported instruction */)
            {
                v10 = /* unsupported instruction */;
                /* unsupported instruction */
                v33 = v32 + 1;
            }
            else
            {
                v10 = nan;
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
                v11 = /* unsupported instruction */;
                /* unsupported instruction */
                v36 = v35 + 1;
            }
            else
            {
                v11 = nan;
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
                v8 = /* unsupported instruction */;
                /* unsupported instruction */
                v39 = v38 + 1;
            }
            else
            {
                v8 = nan;
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
                v6 = /* unsupported instruction */;
                /* unsupported instruction */
                v42 = v41 + 1;
            }
            else
            {
                v6 = nan;
                /* unsupported instruction */
                v42 = v41 + 1;
            }
            sub_4646e0();
            if (/* unsupported instruction */)
            {
                idx->field_46c = /* unsupported instruction */;
                /* unsupported instruction */
                v43 = v42 + 1;
            }
            else
            {
                idx->field_46c = nan;
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
                v5 = /* unsupported instruction */;
                /* unsupported instruction */
                v46 = v45 + 1;
            }
            else
            {
                v5 = nan;
                /* unsupported instruction */
                v46 = v45 + 1;
            }
            v47 = v46 - 1;
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
            if (/* unsupported instruction */)
            {
                v4 = /* unsupported instruction */;
                /* unsupported instruction */
                v49 = v48 + 1;
            }
            else
            {
                v4 = nan;
                /* unsupported instruction */
                v49 = v48 + 1;
            }
            v50 = v49 - 1;
            if (/* unsupported instruction */)
            {
                v51 = v50 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v51 = v50 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            if (/* unsupported instruction */)
            {
                v3 = /* unsupported instruction */;
                /* unsupported instruction */
                v17 = v51 + 1;
            }
            else
            {
                v3 = nan;
                /* unsupported instruction */
                v17 = v51 + 1;
            }
            if (sub_437d00() != 1 && (*((int *)&idx->padding_14[4]) < idx->field_484 - 60 || idx->field_c != 3))
            {
                v52 = v17 - 1;
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
                idx->field_adc = idx->field_adc & 0xfffeffff;
                if (/* unsupported instruction */)
                {
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v54 = v53 + 1;
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                    v54 = v53 + 1;
                }
                idx->field_65c = 0;
                v55 = v54 - 1;
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
                    v1 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v57 = v56 + 1;
                }
                else
                {
                    v1 = nan;
                    /* unsupported instruction */
                    v57 = v56 + 1;
                }
                sub_42e770();
                if (/* unsupported instruction */)
                {
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v58 = v57 + 1;
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                    v58 = v57 + 1;
                }
                v59 = v58 - 1;
                if (/* unsupported instruction */)
                {
                    v60 = v59 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v60 = v59 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v62 = v60;
                v63 = _ccall(10, 13, (char)((((unsigned short)v62 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fa921fb60000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (v63 & 1)
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
                v0 = v29;
                if (/* unsupported instruction */)
                {
                    idx->field_68 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v64 = v62 + 1;
                }
                else
                {
                    idx->field_68 = nan;
                    /* unsupported instruction */
                    v64 = v62 + 1;
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
                    v0 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v67 = v66 + 1;
                }
                else
                {
                    v0 = nan;
                    /* unsupported instruction */
                    v67 = v66 + 1;
                }
                sub_4646e0();
                if (/* unsupported instruction */)
                {
                    idx->field_68 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v17 = v67 + 1;
                }
                else
                {
                    idx->field_68 = nan;
                    /* unsupported instruction */
                    v17 = v67 + 1;
                }
            }
            else
            {
                idx->field_adc = idx->field_adc | 0x10000;
                idx->field_a20 = 0xffff4040;
            }
            index = g_4b44f4;
        }
        if (!index[291])
            return 0;
        idx = index[291]->field_8;
        index[291] = idx;
        if (!idx)
            return 0;
    }
}
