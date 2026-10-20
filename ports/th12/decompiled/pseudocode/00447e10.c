// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x447e10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_447e10_1 {
    char padding_0[24];
    unsigned int field_18;
    char field_1c;
    char field_1d;
} st_447e10_1;

typedef struct st_447e10_2 {
    char padding_0[24];
    unsigned int field_18;
    char padding_1c[1];
    char field_1d;
} st_447e10_2;

typedef struct st_447e10_0 {
    char padding_0[102272];
    unsigned int field_18f80;
    char padding_18f84[16];
    unsigned int field_18f94;
} st_447e10_0;

extern unsigned int g_4aee60[4];
extern st_447e10_0 *g_4b43b8;
extern unsigned int g_4b451c;

unsigned int sub_447e10(unsigned int *iter)
{
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v30;  // eax
    unsigned int v31;  // eax
    unsigned int *index;  // eax
    st_447e10_1 *v33;  // ecx
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    st_447e10_1 *v36;  // ecx
    unsigned int v37;  // ftop
    st_447e10_2 *v38;  // eax
    unsigned int v39;  // ftop
    unsigned int v22;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // ftop
    unsigned int v42;  // ftop
    unsigned int v43;  // ecx
    int v44;  // edx
    int v45;  // edx
    int v46;  // edx
    unsigned int v23;  // edi
    st_447e10_0 *idx;  // esi
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // edi
    unsigned int v0;  // [bp-0x5c]
    unsigned int v1;  // [bp-0x58]
    st_447e10_1 *v2;  // [bp-0x54]
    unsigned int v3;  // [bp-0x50]
    unsigned int v4;  // [bp-0x4c]
    unsigned int v5;  // [bp-0x48]
    unsigned int v6;  // [bp-0x44]
    unsigned int v7;  // [bp-0x40]
    st_447e10_2 *v8;  // [bp-0x3c], Other Possible Types: unsigned int
    unsigned int v9;  // [bp-0x38]
    unsigned int *v10;  // [bp-0x34], Other Possible Types: unsigned int
    st_447e10_1 *v11;  // [bp-0x30], Other Possible Types: unsigned int, double, unsigned long
    unsigned int v12;  // [bp-0x2c]
    unsigned int v13;  // [bp-0x18]
    unsigned int v14;  // [bp-0x14]
    unsigned int v15;  // [bp-0x14]
    unsigned int v16;  // [bp-0x10]
    unsigned int v17;  // [bp-0xc]
    unsigned int v18;  // [bp-0x8]
    unsigned int v19;  // [bp-0x4]

    if (iter[9] != 2)
        return 1;
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
    v23 = iter[64];
    idx = g_4b43b8;
    if (/* unsupported instruction */)
    {
        v17 = /* unsupported instruction */;
        /* unsupported instruction */
        v25 = v22 + 1;
    }
    else
    {
        v17 = nan;
        /* unsupported instruction */
        v25 = v22 + 1;
    }
    v26 = v25 - 1;
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
    g_4b43b8->field_18f94 = 1;
    if (/* unsupported instruction */)
    {
        v18 = /* unsupported instruction */;
        /* unsupported instruction */
        v28 = v27 + 1;
    }
    else
    {
        v18 = nan;
        /* unsupported instruction */
        v28 = v27 + 1;
    }
    /* unsupported instruction */
    /* unsupported instruction */
    v16 = v23;
    v19 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if (!iter[118])
    {
        iter = 1;
        v29 = v23 * 280;
        v30 = 0xff;
        v13 = 0xff;
        v14 = 10;
        while (1)
        {
            idx->field_18f80 = (v30 * 0x100 | v30) * 0x100 | 0xff0000ff;
            v31 = iter[10] * 17908;
            if (*((int *)(v29 + v31 + g_4b451c + 40)) || *((int *)(v29 + v31 + g_4b451c + 44)))
            {
                index = sub_46d8e4(v29 + v31 + g_4b451c + 8 + 32);
                v33 = iter[10] * 17908 + v29 + g_4b451c;
                v34 = v28 - 1;
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
                v11 = v33;
                v36 = v33 + 1;
                if (/* unsupported instruction */)
                {
                    v11 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v37 = v35 + 1;
                }
                else
                {
                    v11 = (double)nan;
                    /* unsupported instruction */
                    v37 = v35 + 1;
                }
                v10 = g_4aee60[v33->field_1c];
                v9 = index[1];
                v8 = index[2];
                v7 = index[3];
                v6 = index[4] + 1;
                v5 = index[5] + 1900;
                v4 = *(&v36->padding_0[0] - 1);
                v3 = *((int *)(&v36->padding_0[0] - 6));
                v2 = v36;
                v1 = iter;
                v0 = "%2d  %s  %9ld%d  %.4d/%.2d/%.2d %.2d:%.2d  %s  %2.1f%%";
                sub_4015c0();
            }
            else
            {
                v38 = iter[10] * 17908 + v29 + g_4b451c;
                v39 = v28 - 1;
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
                    v11 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v37 = v40 + 1;
                }
                else
                {
                    v11 = (double)nan;
                    /* unsupported instruction */
                    v37 = v40 + 1;
                }
                v10 = v38->field_1d;
                v9 = v38->field_18;
                v8 = v38 + 1;
                v7 = iter;
                v6 = "%2d  %s  %9ld%d  ----/--/-- --:--  Stage -  ---%%";
                sub_4015c0();
            }
            v41 = v37 - 1;
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
            v13 -= 16;
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
            idx = g_4b43b8;
            iter += 1;
            v29 += 28;
            if (/* unsupported instruction */)
            {
                v18 = /* unsupported instruction */;
                /* unsupported instruction */
                v28 = v42 + 1;
            }
            else
            {
                v18 = nan;
                /* unsupported instruction */
                v28 = v42 + 1;
            }
            v15 = v14 - 1;
            if (v14 == 1)
                break;
            v30 = v13;
            v14 = v15;
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
        v17 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v17 = nan;
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
    idx->field_18f80 = 0xffffffff;
    if (/* unsupported instruction */)
    {
        v18 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v18 = nan;
        /* unsupported instruction */
    }
    v12 = *((int *)(iter[10] * 17908 + g_4b451c + 1424));
    v11 = "    %5d";
    sub_4015c0();
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
        v18 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v18 = nan;
        /* unsupported instruction */
    }
    v43 = *((int *)(iter[10] * 17908 + g_4b451c + 1428));
    v44 = (int)((unsigned int)(2290649225 * v43 >> 32) + v43) >> 5;
    v10 = (((unsigned int)(v44) >> 31) + v44) % 60;
    v45 = (int)((unsigned int)(2443359173 * v43 >> 32) + v43) >> 11;
    v9 = (((unsigned int)(v45) >> 31) + v45) % 60;
    v46 = (int)((unsigned int)(2606249785 * v43 >> 32) + v43) >> 0x11;
    v8 = ((unsigned int)(v46) >> 31) + v46;
    v7 = "%3d:%.2d:%.2d";
    sub_4015c0();
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
        v18 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v18 = nan;
        /* unsupported instruction */
    }
    v6 = *((int *)(g_4b451c + (iter[10] * 4477 + v16) * 4 + 1432));
    v5 = "    %5d";
    sub_4015c0();
    g_4b43b8->field_18f80 = 0xffffffff;
    g_4b43b8->field_18f94 = 0;
    return 1;
}
