// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x466f00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_466f00_1 {
    char padding_0[8];
    unsigned short field_8;
    char padding_a[1];
    char field_b;
    char padding_c[4];
    unsigned int field_10;
} st_466f00_1;

typedef struct st_466f00_0 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[4096];
    int field_1008;
} st_466f00_0;

typedef struct st_466f00_4 {
    unsigned int field_0;
    struct st_466f00_1 *field_4;
    char padding_8[4108];
    struct st_466f00_2 *field_1014;
} st_466f00_4;

typedef struct st_466f00_2 {
    char padding_0[4];
    struct st_466f00_0 *field_4;
} st_466f00_2;


int sub_466f00(void)
{
    unsigned int v10;  // esi
    st_466f00_0 *idx;  // eax
    st_466f00_1 *v19;  // eax
    unsigned int v21;  // eax
    unsigned int v22;  // ftop
    unsigned int v23;  // ftop
    unsigned int v24;  // ftop
    unsigned int v25;  // ftop
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int node;  // ftop
    int v12;  // eax
    unsigned int v29;  // ecx
    unsigned int v30;  // ftop
    unsigned int v31;  // cc_dep1
    unsigned int v32;  // ftop
    int v33;  // ecx
    unsigned int v34;  // ecx
    unsigned int v35;  // 4105
    int v36;  // eax
    unsigned int v37;  // edx
    int v38;  // eax
    st_466f00_4 *index;  // edi
    st_466f00_1 *v39;  // ecx
    unsigned int *v40;  // ebx
    st_466f00_2 *idx1;  // esi
    unsigned int v42;  // eax
    unsigned int *v43;  // esi
    unsigned int v14;  // ebx
    unsigned int v15;  // ecx
    unsigned int v16;  // ebp
    unsigned int v17;  // edx
    unsigned int v18;  // ftop
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    int v6;  // [bp-0x8]
    int v7;  // [bp-0x4]
    unsigned int *iter;  // [bp+0x0]
    unsigned int v9;  // [bp+0x4]

    v2 = v10;
    v12 = idx->field_1008;
    v14 = index->field_4->field_10 + v9 * 4 + 4;
    v6 = 0;
    v7 = v12;
    v15 = v12 + 12;
    if (!v12)
    {
        *((unsigned int *)&idx->padding_8[0]) = 0;
        idx->field_1008 = idx->field_1008 + 4;
        v15 = 16;
    }
    v16 = v9 + 1;
    if (v16 < index->field_4->field_b)
    {
        v17 = v14 + 4;
        v9 = &idx->padding_8[v15];
        v5 = v17;
        while (1)
        {
            v19 = index->field_4;
            if (*(&v19->padding_0[v14 + 16]) != 0x66 && *(&v19->padding_0[v14 + 16]) != 103)
            {
                v3 = (&v19->field_10)[v17 >> 2];
                v21 = (!(v19->field_8 & 1 << ((char)v16 & 31)) ? v3 : sub_468f70(v3));
                v3 = v21;
                if (*(&index->field_4->padding_0[v14 + 0x11]) != 0x66)
                    goto LABEL_467017;
                v22 = v18 - 1;
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
                *(iter) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                v18 = v23 + 1;
            }
            else
            {
                v24 = v18 - 1;
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
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v26 = v25 + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v26 = v25 + 1;
                }
                v27 = v26 - 1;
                if (/* unsupported instruction */)
                {
                    node = v27 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                else
                {
                    node = v27 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                }
                v29 = v16;
                if (v19->field_8 & 1 << ((char)v29 & 31))
                {
                    v1 = v29;
                    if (/* unsupported instruction */)
                    {
                        v1 = /* unsupported instruction */;
                        /* unsupported instruction */
                        node += 1;
                    }
                    else
                    {
                        v1 = nan;
                        /* unsupported instruction */
                        node += 1;
                    }
                    sub_468fe0();
                }
                if (/* unsupported instruction */)
                {
                    v3 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v30 = node + 1;
                }
                else
                {
                    v3 = nan;
                    /* unsupported instruction */
                    v30 = node + 1;
                }
                v31 = *(&index->field_4->padding_0[v14 + 0x11]);
                v32 = v30 - 1;
                if (/* unsupported instruction */)
                {
                    v18 = v32 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if (v31 != 0x66)
                        goto LABEL_467012;
                    goto LABEL_46700a;
                }
                else
                {
                    v18 = v32 - 1;
                    /* unsupported instruction */
                    /* unsupported instruction */
                    if (v31 == 0x66)
                    {
LABEL_46700a:
                        *(iter) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v18 += 1;
                    }
                    else
                    {
LABEL_467012:
                        v21 = sub_4931e0();
LABEL_467017:
                        *(iter) = v21;
                    }
                }
            }
            v4 += 8;
            iter += 1;
            v16 += 1;
            v14 += 8;
            if (v16 >= index->field_4->field_b)
                break;
            v17 = v4;
        }
        v12 = v6;
    }
    if (v12)
    {
        v33 = idx->field_1008;
        v34 = v33 - 4;
        v35 = _ccall(8, 3, v33, 0xfffffffc, 0);
        if (!(v35 & 1))
        {
            idx->field_1008 = v34;
            v5 = *((int *)&idx->padding_8[v34]);
        }
        idx->field_1008 = v12;
        *((unsigned int *)(v12 + (char *)idx + 4)) = v5;
    }
    else
    {
        idx->field_1008 = 4;
    }
    if (idx->field_1008 + 4 < 0x1000)
    {
        *((int *)&idx->padding_8[idx->field_1008]) = v33;
        idx->field_1008 = idx->field_1008 + 4;
    }
    v36 = idx->field_1008;
    v37 = v36 + 4;
    if (v12)
    {
        if (v37 < 0x1000)
        {
            *((unsigned int *)&idx->padding_8[v36]) = index->field_0;
            idx->field_1008 = idx->field_1008 + 4;
        }
        v38 = idx->field_1008;
        if (v38 + 4 < 0x1000)
        {
            v39 = index->field_4;
LABEL_4670ef:
            *((st_466f00_1 **)&idx->padding_8[v38]) = v39;
            idx->field_1008 = idx->field_1008 + 4;
        }
    }
    else
    {
        v39 = NULL;
        if (v37 < 0x1000)
        {
            *((unsigned int *)&idx->padding_8[v36]) = 0;
            idx->field_1008 = idx->field_1008 + 4;
        }
        v38 = idx->field_1008;
        if (v38 + 4 < 0x1000)
            goto LABEL_4670ef;
    }
    v40 = &index->field_1014->field_4->field_0;
    index->field_1014->field_4 = idx;
    idx1 = index->field_1014;
    v42 = sub_4694f0(index->field_4 + 1);
    /* unsupported instruction */
    /* unsupported instruction */
    idx1->field_4->field_4 = v42;
    v43 = &idx1->field_4->field_0;
    *(v43) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
    /* unsupported instruction */
    if (!v43[1])
    {
        v0 = index->field_4 + 1;
        sub_466ea0();
        index->field_4 = NULL;
    }
    else
    {
        index->field_1014->field_4 = v40;
    }
    return;
}
