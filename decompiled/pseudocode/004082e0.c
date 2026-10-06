// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4082e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4082e0_2 {
    char padding_0[20];
    int field_14;
    char padding_18[48];
    int field_48;
    char padding_4c[1204];
    struct st_4082e0_1 *field_500;
} st_4082e0_2;

typedef struct st_4082e0_1 {
    char padding_0[4];
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[16];
    unsigned int field_20;
    unsigned int field_24;
    unsigned int field_28;
    char padding_2c[8];
    unsigned int field_34;
    char padding_38[76];
    unsigned int field_84;
    char padding_88[40];
    struct st_4082e0_0 *field_b0;
    char padding_b4[4];
    char field_b8;
    char padding_b9[47];
    char field_e8;
} st_4082e0_1;

typedef struct st_4082e0_0 {
    char padding_0[24];
    unsigned int field_18;
    unsigned int field_1c;
    unsigned int field_20;
    char padding_24[64];
    int field_64;
    unsigned int field_68;
} st_4082e0_0;

unsigned int sub_4082e0(unsigned int a0)
{
    int v9;  // edi
    int v10;  // esi
    unsigned int *idx;  // esi
    unsigned int v20;  // ftop
    unsigned int v21;  // ftop
    unsigned int v22;  // ftop
    int i;  // ebx
    unsigned int iter;  // edi
    unsigned int v25;  // ecx
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    int v11;  // ebp
    unsigned int *v29;  // eax
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v32;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // ftop
    unsigned int v35;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    int v12;  // ebx
    unsigned int node;  // ebx
    unsigned int *index;  // eax
    unsigned int *idx1;  // esi
    st_4082e0_2 *idx2;  // ecx
    unsigned int v14;  // eax
    st_4082e0_1 *v15;  // ebp
    unsigned int v16;  // esi
    unsigned int v17;  // esi
    st_4082e0_2 *v18;  // esi
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x38]
    unsigned int v3;  // [bp-0x30]
    unsigned int v4;  // [bp-0x28]
    unsigned int v5;  // [bp-0x28]
    st_4082e0_2 *v6;  // [bp-0x20], Other Possible Types: unsigned int
    unsigned int v7;  // [bp-0x10]
    st_4082e0_2 *v8;  // [bp+0x0]

    sub_406f60(40, v9, v10, v11, v12);
    idx2 = v8;
    v14 = *((int *)&idx2->padding_18[0]);
    v15 = idx2->field_500;
    if (v14 >= 200)
    {
        v16 = 8;
        do
        {
            v17 = v16;
            sub_408030();
        } while ((v16 = v17 - 1, v17 != 1));
        v18 = v8;
        sub_461970(v18->field_48);
        if (v18->field_500)
        {
            sub_46c941(v18->field_500);
            v18->field_500 = NULL;
        }
        idx = sub_46c9ea(0x44);
        if (!idx)
            return 0xffffffff;
        idx[16] = idx[16] & 0xfffffffe;
        sub_477420(idx, 0, 0x44);
        *(idx) = *(idx) | 2;
        sub_452a60(idx, 1, 8, 6, 6, 0, 70);
        return 0xffffffff;
    }
    else
    {
        if (v14 != idx2->field_14 && !v14)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            v6 = idx2;
            /* unsupported instruction */
            v21 = v20;
            sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
            if (/* unsupported instruction */)
            {
                v7 = /* unsupported instruction */;
                /* unsupported instruction */
                v22 = v21 + 1;
            }
            else
            {
                v7 = nan;
                /* unsupported instruction */
                v22 = v21 + 1;
            }
            i = 0;
            iter = &v15->field_34;
            do
            {
                sub_407ea0(i);
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)iter) = *((int *)iter) | 1;
                *((int *)(iter - 16)) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v2 = v25;
                /* unsupported instruction */
                v26 = v22;
                sub_4646e0((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                v1 = v25;
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
                sub_4646e0();
                if (/* unsupported instruction */)
                {
                    *((int *)(iter - 20)) = /* unsupported instruction */;
                    /* unsupported instruction */
                    v28 = v27 + 1;
                }
                else
                {
                    *((unsigned int *)(iter - 20)) = nan;
                    /* unsupported instruction */
                    v28 = v27 + 1;
                }
                v29 = *((int *)(iter + 124));
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
                v0 = v25;
                if (/* unsupported instruction */)
                {
                    *((int *)(iter - 12)) = /* unsupported instruction */;
                    /* unsupported instruction */
                    v32 = v31 + 1;
                }
                else
                {
                    *((unsigned int *)(iter - 12)) = nan;
                    /* unsupported instruction */
                    v32 = v31 + 1;
                }
                v29[26] = 300;
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
                    v35 = v34 + 1;
                }
                else
                {
                    v4 = nan;
                    /* unsupported instruction */
                    v35 = v34 + 1;
                }
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
                    v0 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v38 = v37 + 1;
                }
                else
                {
                    v0 = nan;
                    /* unsupported instruction */
                    v38 = v37 + 1;
                }
                sub_4646e0();
                i += 1;
                if (/* unsupported instruction */)
                {
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v22 = v38 + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v22 = v38 + 1;
                }
                iter += 180;
            } while (i < 8);
        }
        node = &v15->field_4;
        v6 = 8;
        do
        {
            v5 = v4;
            if (*((int *)(node + 128)))
            {
                sub_407b80(node - 4);
                index = *((int *)(node + 172));
                if (index[25] >= 300)
                {
                    sub_408030();
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v0 = v25;
                    /* unsupported instruction */
                    sub_453e20((/* unsupported instruction */ ? /* unsupported instruction */ : nan));
                    idx1 = sub_46c9ea(0x44);
                    if (idx1)
                    {
                        idx1[16] = idx1[16] & 0xfffffffe;
                        sub_477420(idx1, 0, 0x44);
                        *(idx1) = *(idx1) | 2;
                        sub_452a60(idx1, 1, 8, 6, 6, 0, 70);
                    }
                }
                else
                {
                    index[6] = *((int *)node);
                    index[7] = *((int *)(node + 4));
                    index[8] = *((int *)(node + 8));
                }
            }
        } while ((node += 180, v4 = v5 - 1, v5 != 1));
        sub_408510();
        return 0;
    }
}
