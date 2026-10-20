// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41aa70; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41aa70_0 {
    char padding_0[100];
    char field_64;
    char padding_65[1215];
    char field_524;
    char padding_525[7];
    unsigned int field_52c;
    unsigned int field_530;
    char padding_534[8];
    unsigned int field_53c;
    char padding_540[86];
    unsigned short field_596;
    char padding_598[1220];
    char field_a5c;
    char padding_a5d[1215];
    char field_f1c;
} st_41aa70_0;

extern st_41aa70_0 *g_4b43c8;

int sub_41aa70(void)
{
    unsigned int v11;  // ebx
    unsigned int v12;  // esi
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // fc3210
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v13;  // edi
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v14;  // edi
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int iter;  // esi
    unsigned int v16;  // ebx
    unsigned int v17;  // ftop
    unsigned int v18;  // ebx
    unsigned int v19;  // ftop
    char *v0;  // [bp-0x3c]
    char *v1;  // [bp-0x38]
    unsigned int v2;  // [bp-0x2c]
    unsigned int v3;  // [bp-0x28]
    char *v4;  // [bp-0x24], Other Possible Types: unsigned int
    unsigned int v5;  // [bp-0x20]
    unsigned int v6;  // [bp-0x1c]
    unsigned int v7;  // [bp-0x18]
    unsigned int v8;  // [bp-0x14]
    char v9;  // [bp-0x4]

    v5 = v11;
    v4 = &v9;
    v3 = v12;
    v2 = v13;
    v14 = &g_4b43c8->field_64;
    iter = v14 + 1216;
    v16 = 2000;
    do
    {
        v18 = v16;
        if (*((char *)v14) & 1 && *((short *)(iter + 114)) == 1)
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
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            v19 = v17 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
            sub_4937c0();
            if (/* unsupported instruction */)
            {
                v4 = /* unsupported instruction */;
                /* unsupported instruction */
                v20 = v19 + 1;
            }
            else
            {
                v4 = nan;
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
                v4 = /* unsupported instruction */;
                /* unsupported instruction */
                v23 = v22 + 1;
            }
            else
            {
                v4 = nan;
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
                v26 = CmpF(/* unsupported instruction */, v4) * 0x100 & 0x4500;
                /* unsupported instruction */
                v17 = v25 + 1;
            }
            else
            {
                v26 = CmpF(nan, v4) * 0x100 & 0x4500;
                /* unsupported instruction */
                v17 = v25 + 1;
            }
            if (!((char)((((unsigned short)v17 & 7) * 0x800 | (unsigned short)v26 & 0x4700) >> 8) & 65))
            {
                v27 = v17 - 1;
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
                v1 = &v5;
                v0 = &v5;
                if (/* unsupported instruction */)
                {
                    v7 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v29 = v28 + 1;
                }
                else
                {
                    v7 = nan;
                    /* unsupported instruction */
                    v29 = v28 + 1;
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
                v8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v5 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                D3DXVec2Normalize();
                v32 = v31 - 0;
                if (/* unsupported instruction */)
                {
                    v33 = v32 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v33 = v32 - 1;
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
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v34 = v33 + 1;
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                    v34 = v33 + 1;
                }
                v35 = v34 - 1;
                if (/* unsupported instruction */)
                {
                    v36 = v35 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    v36 = v35 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
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
                v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)(iter + 8)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
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
                v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v39 = v38 - 0;
                /* unsupported instruction */
                /* unsupported instruction */
                *((int *)(iter + 12)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                sub_4937aa();
                if (/* unsupported instruction */)
                {
                    v2 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v40 = v39 + 1;
                }
                else
                {
                    v2 = nan;
                    /* unsupported instruction */
                    v40 = v39 + 1;
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
                if (/* unsupported instruction */)
                {
                    *((int *)(iter + 24)) = /* unsupported instruction */;
                    /* unsupported instruction */
                    v17 = v42 + 1;
                }
                else
                {
                    *((unsigned int *)(iter + 24)) = nan;
                    /* unsupported instruction */
                    v17 = v42 + 1;
                }
            }
        }
    } while ((v14 += 2552, iter += 2552, v16 = v18 - 1, v18 != 1));
    return;
}
