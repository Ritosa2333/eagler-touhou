// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46a300; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46a300_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    int field_c;
    char padding_10[16];
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    unsigned int field_2c;
} st_46a300_0;

typedef struct st_46a300_1 {
    char padding_0[1072];
    unsigned int field_430;
    unsigned int field_434;
    unsigned int field_438;
    char padding_43c[60];
    struct st_46a300_0 *field_478;
} st_46a300_1;

unsigned int sub_46a300(st_46a300_1 *index)
{
    st_46a300_0 *idx;  // edi
    int v12;  // eax
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v13;  // ecx
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned short v33;  // ftop
    unsigned short v34;  // ftop
    unsigned short v35;  // ftop
    unsigned short v36;  // ftop
    unsigned short v37;  // ftop
    unsigned short v38;  // ftop
    unsigned short v39;  // ftop
    unsigned short v40;  // ftop
    unsigned int v14;  // 4101
    unsigned short v41;  // ftop
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v0;  // [bp-0x30]
    unsigned int v1;  // [bp-0x2c]
    unsigned int v2;  // [bp-0x28]
    unsigned int v3;  // [bp-0x24]
    unsigned int v4;  // [bp-0x20]
    unsigned int v5;  // [bp-0x1c]
    unsigned int v6;  // [bp-0x18]
    unsigned int v7;  // [bp-0x14]
    unsigned int v8;  // [bp-0x10]
    unsigned int v9;  // [bp-0xc]

    idx = index->field_478;
    v12 = *((int *)&idx->padding_10[0]);
    if (v12 >= 300)
        return 1;
    idx->field_0 = index->field_430;
    idx->field_4 = index->field_434;
    v13 = index->field_438;
    idx->field_8 = v13;
    if (*((int *)&idx->padding_10[0]) != idx->field_c)
    {
        if ((v12 & 0x80000003) < 0)
        {
            v14 = _ccall(4, 18, ((v12 & 0x80000003) - 1 | 0xfffffffc) + 1, 0, 0);
            if (v14 & 1)
                goto LABEL_46a359;
        }
        else if (!(v12 & 0x80000003))
        {
LABEL_46a359:
            /* unsupported instruction */
            /* unsupported instruction */
            v0 = v13;
            /* unsupported instruction */
            sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            v4 = idx->field_0;
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
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                /* unsupported instruction */
                /* unsupported instruction */
            }
            v6 = idx->field_8;
            v7 = idx->field_0;
            if (/* unsupported instruction */)
            {
                v4 = /* unsupported instruction */;
                /* unsupported instruction */
                v18 = v17 + 1;
            }
            else
            {
                v4 = nan;
                /* unsupported instruction */
                v18 = v17 + 1;
            }
            v5 = idx->field_4;
            v19 = v18 - 1;
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
            v8 = idx->field_4;
            if (/* unsupported instruction */)
            {
                v6 = /* unsupported instruction */;
                /* unsupported instruction */
                v21 = v20 + 1;
            }
            else
            {
                v6 = nan;
                /* unsupported instruction */
                v21 = v20 + 1;
            }
            v9 = idx->field_8;
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
                v24 = v23 + 1;
            }
            else
            {
                v3 = nan;
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
                idx->field_24 = /* unsupported instruction */;
            else
                idx->field_24 = nan;
            /* unsupported instruction */
            /* unsupported instruction */
            v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v28 = v26 + 1;
            if (!((((unsigned short)v28 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x41c00000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
            {
                v29 = v28 - 1;
                if (/* unsupported instruction */)
                {
                    v30 = v29 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v30 = v29 - 1;
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
                    idx->field_2c = /* unsupported instruction */;
                    /* unsupported instruction */
                    v28 = v30 + 1;
                }
                else
                {
                    idx->field_2c = nan;
                    /* unsupported instruction */
                    v28 = v30 + 1;
                }
            }
            sub_40fbe0(&v3, 3, &v4);
            v31 = v28 - 1;
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
            v1 = idx->field_0;
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
            v3 = idx->field_8;
            v2 = idx->field_4;
            if (/* unsupported instruction */)
            {
                v1 = /* unsupported instruction */;
                /* unsupported instruction */
                v33 = (unsigned short)v32 + 1;
            }
            else
            {
                v1 = nan;
                /* unsupported instruction */
                v33 = (unsigned short)v32 + 1;
            }
            v5 = idx->field_4;
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
            v4 = idx->field_0;
            if (/* unsupported instruction */)
            {
                v3 = /* unsupported instruction */;
                /* unsupported instruction */
                v36 = v35 + 1;
            }
            else
            {
                v3 = nan;
                /* unsupported instruction */
                v36 = v35 + 1;
            }
            v6 = idx->field_8;
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
                idx->field_20 = /* unsupported instruction */;
                /* unsupported instruction */
                v39 = v38 + 1;
            }
            else
            {
                idx->field_20 = nan;
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
            /* unsupported instruction */
            /* unsupported instruction */
            v0 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            if (!(((v41 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x41c00000) * 0x100 & 0x4500 & 0x4700) >> 8 & 1))
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
                    idx->field_28 = /* unsupported instruction */;
                    /* unsupported instruction */
                }
                else
                {
                    idx->field_28 = nan;
                    /* unsupported instruction */
                }
            }
            sub_40fbe0(&v0, 3, &v1);
        }
    }
    sub_464a80();
    return 0;
}
