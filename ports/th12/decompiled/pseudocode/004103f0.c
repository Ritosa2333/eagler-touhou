// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4103f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4103f0_0 {
    char padding_0[131];
    char field_83;
    char padding_84[3];
    char field_87;
    char padding_88[56];
    unsigned int field_c0;
    int field_c4;
    unsigned int field_c8;
    unsigned int field_cc;
    struct st_4103f0_1 *field_d0;
} st_4103f0_0;

typedef struct st_4103f0_1 {
    unsigned int field_0;
} st_4103f0_1;

typedef struct st_4103f0_2 {
    char padding_0[1144];
    struct st_4103f0_0 *field_478;
} st_4103f0_2;

unsigned int sub_4103f0(st_4103f0_2 *a0)
{
    st_4103f0_0 *idx;  // edi
    unsigned int v9;  // ebx
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int *v24;  // edx
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int iter;  // ecx
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v11;  // edx
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v40;  // ftop
    unsigned int v41;  // edx
    unsigned int *v42;  // ecx
    unsigned short v44;  // ftop
    unsigned short v45;  // ftop
    unsigned int v12;  // edx
    unsigned int v13;  // esi
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v17;  // ftop
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    st_4103f0_2 *v6;  // [bp-0x4], Other Possible Types: unsigned int

    v6 = a0;
    idx = a0->field_478;
    v9 = idx->field_c8;
    if (v9 >= 16)
        return 1;
    if (v9 != idx->field_c4)
    {
        if (v9 > 0)
        {
            iter = &idx->field_83;
            v11 = v9;
            do
            {
                v12 = v11;
                if (*((char *)iter) >= 16)
                    *((char *)iter) = *((char *)iter) - 16;
                else
                    *((char *)iter) = 0;
            } while ((iter += 4, v11 = v12 - 1, v12 != 1));
        }
        v4 = v13;
        v6 = sub_464440();
        v15 = v14 - 1;
        if (/* unsupported instruction */)
        {
            v16 = v15 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        else
        {
            v16 = v15 - 1;
            /* unsupported instruction */
            /* unsupported instruction */
        }
        if (v6 < 0)
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
            v6 = /* unsupported instruction */;
            /* unsupported instruction */
            v17 = v16 + 1;
        }
        else
        {
            v6 = nan;
            /* unsupported instruction */
            v17 = v16 + 1;
        }
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
        v20 = v19 - 1;
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
        /* unsupported instruction */
        /* unsupported instruction */
        v6 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v2 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_4108e0();
        v22 = v21 + 1;
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
        /* unsupported instruction */
        /* unsupported instruction */
        *(v24) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v24[1] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v5 = sub_464440();
        v25 = v23 - 0;
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
        if (v5 < 0)
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
            v5 = /* unsupported instruction */;
            /* unsupported instruction */
            v27 = v26 + 1;
        }
        else
        {
            v5 = nan;
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
            v5 = /* unsupported instruction */;
            /* unsupported instruction */
            v30 = v29 + 1;
        }
        else
        {
            v5 = nan;
            /* unsupported instruction */
            v30 = v29 + 1;
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
            v5 = /* unsupported instruction */;
            /* unsupported instruction */
            v33 = v32 + 1;
        }
        else
        {
            v5 = nan;
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
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
            v36 = v35 + 1;
        }
        else
        {
            v1 = nan;
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
            v0 = /* unsupported instruction */;
            /* unsupported instruction */
            v39 = v38 + 1;
        }
        else
        {
            v0 = nan;
            /* unsupported instruction */
            v39 = v38 + 1;
        }
        sub_464640();
        if (/* unsupported instruction */)
        {
            idx->field_c0 = /* unsupported instruction */;
            /* unsupported instruction */
            v40 = v39 + 1;
        }
        else
        {
            idx->field_c0 = nan;
            /* unsupported instruction */
            v40 = v39 + 1;
        }
    }
    v41 = idx->field_c8;
    v42 = &idx->field_d0->field_0;
    idx->field_c4 = v41;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    if (!((char)((((unsigned short)v40 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
    {
        v44 = v40 - 1;
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
        /* unsupported instruction */
        if (!((char)(((v45 + 1 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v42)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
            idx->field_c8 = v41 + 1;
            if (!/* unsupported instruction */)
            {
                idx->field_cc = nan;
                /* unsupported instruction */
                return 0;
            }
            idx->field_cc = /* unsupported instruction */;
            /* unsupported instruction */
            return 0;
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    idx->field_cc = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    idx->field_c8 = sub_4931e0();
    return 0;
}
