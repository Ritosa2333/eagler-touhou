// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423ed0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_423ed0_0 {
    void* field_0;
    struct st_423ed0_0 *field_4;
} st_423ed0_0;

typedef struct st_423ed0_2 {
    char padding_0[959];
    char field_3bf;
    char padding_3c0[64];
    char field_400[4];
} st_423ed0_2;

extern char g_4b0cb8;
extern unsigned int g_4ce8cc;

unsigned int sub_423ed0(unsigned int a0)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // esi
    unsigned int v17;  // ftop
    unsigned int v18;  // ftop
    unsigned int v19;  // ftop
    unsigned int v20;  // fpround
    unsigned int v21;  // ecx
    st_423ed0_0 *iter;  // eax
    void* v23;  // edx
    st_423ed0_0 *node;  // eax
    st_423ed0_2 *idx;  // ecx
    unsigned int *index;  // edi
    unsigned int v26;  // ftop
    unsigned int v27;  // ftop
    unsigned int v28;  // ftop
    unsigned int v29;  // ftop
    unsigned int v30;  // ftop
    unsigned int v31;  // ftop
    unsigned int v33;  // ftop
    unsigned int v34;  // 4359
    unsigned int v35;  // ftop
    unsigned int v10;  // ftop
    unsigned int v36;  // ftop
    unsigned int v37;  // ftop
    unsigned int v38;  // ftop
    unsigned int v39;  // ftop
    unsigned int v41;  // ftop
    unsigned int v43;  // ftop
    unsigned int v44;  // ftop
    unsigned int v45;  // ftop
    unsigned int v11;  // ftop
    unsigned int v46;  // ftop
    unsigned int v47;  // ftop
    unsigned int v49;  // 4223
    char v50;  // dl
    unsigned long long v51;  // 4320
    unsigned int v53;  // ftop
    unsigned int v54;  // ftop
    unsigned int v55;  // 4180
    unsigned int v12;  // ftop
    unsigned int v56;  // ftop
    unsigned int v57;  // ftop
    unsigned int v58;  // ftop
    unsigned int v59;  // ftop
    unsigned int v60;  // ftop
    unsigned int v62;  // ftop
    unsigned int v63;  // ftop
    unsigned int v64;  // 4180
    unsigned long long v65;  // 4317
    unsigned int *iter1;  // esi
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    unsigned int v16;  // ftop
    unsigned int v0;  // [bp-0x24]
    unsigned short v1;  // [bp-0x20], Other Possible Types: unsigned int
    unsigned int v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x1c]
    int v4;  // [bp-0x18], Other Possible Types: unsigned int
    unsigned int v5;  // [bp-0x14]
    unsigned int v6;  // [bp-0x10]

    v1 = v7;
    v0 = v8;
    if (index[54] != *((int *)&g_4b0cb8))
        index[5] = 0;
    sub_4237e0(index);
    v11 = v10 - 1;
    if (/* unsupported instruction */)
    {
        v12 = v11 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        v12 = v11 - 1;
        /* unsupported instruction */
        /* unsupported instruction */
    }
    iter1 = index + 95;
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
    v3 = 10;
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
        v14 = v12 + 1;
    }
    else
    {
        v5 = nan;
        /* unsupported instruction */
        v14 = v12 + 1;
    }
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
    do
    {
        v2 = v3;
        v21 = *((int *)((char *)iter1 - 160));
        if (!v21)
        {
LABEL_423f51:
            idx = NULL;
            goto LABEL_423f98;
        }
        else
        {
            iter = *((int *)(g_4ce8cc + 8935096));
            if (*((int *)(g_4ce8cc + 8935096)))
            {
                do
                {
                    v23 = iter->field_0;
                    if (*((int *)v23) == v21)
                        goto LABEL_423f92;
                } while ((iter = (st_423ed0_0 *)iter->field_4, iter));
            }
            node = *((int *)(g_4ce8cc + 8935104));
            if (!*((int *)(g_4ce8cc + 8935104)))
                goto LABEL_423f51;
            while (1)
            {
                v23 = node->field_0;
                if (*((int *)v23) == v21)
                    break;
                node = node->field_4;
                if (!node)
                {
                    idx = NULL;
                    goto LABEL_423f98;
                }
            }
LABEL_423f92:
            idx = v23;
            if (idx)
                goto LABEL_423faa;
LABEL_423f98:
            *((unsigned int *)((char *)iter1 - 160)) = 0;
            if (!idx)
                continue;
LABEL_423faa:
            if (!*((int *)&idx->padding_0[344]))
            {
                v26 = v19 - 1;
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
                    v1 = /* unsupported instruction */;
                    /* unsupported instruction */
                    v28 = v27 + 1;
                }
                else
                {
                    v1 = nan;
                    /* unsupported instruction */
                    v28 = v27 + 1;
                }
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
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v31 = v30 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v33 = v31 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                v34 = _ccall(10, 13, (char)((((unsigned short)v31 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (!(v34 & 1))
                {
                    v35 = v33 - 1;
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
                        v37 = v36 + 1;
                    }
                    else
                    {
                        v4 = nan;
                        /* unsupported instruction */
                        v37 = v36 + 1;
                    }
                    v38 = v37 - 1;
                    if (/* unsupported instruction */)
                    {
                        v39 = v38 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v39 = v38 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v33 = v39 + 1;
                    if (!((char)((((unsigned short)v33 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
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
                        v19 = v33 + 3;
                        idx->field_3bf = idx->field_400[0] >> 2;
                        continue;
                    }
                }
                v41 = v33 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                if (!((char)((((unsigned short)v41 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                {
                    v43 = v41 - 1;
                    if (/* unsupported instruction */)
                    {
                        v44 = v43 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v44 = v43 - 1;
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
                        v45 = v44 + 1;
                    }
                    else
                    {
                        v4 = nan;
                        /* unsupported instruction */
                        v45 = v44 + 1;
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
                    /* unsupported instruction */
                    /* unsupported instruction */
                    /* unsupported instruction */
                    v41 = v47 + 1;
                    if (!((char)((((unsigned short)v41 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v1 = (unsigned int)idx->field_400;
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
                        v49 = _ccall(v20);
                        v1 = v49;
                        v4 = (int)(v1 * 3 + ((int)(v1 * 3) >> 31 & 3)) >> 2;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v4 = v1 | 0xc00;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                        /* unsupported instruction */
                        v19 = v41 + 4;
                        v50 = v4;
                        v51 = _ccall(v1);
                        v20 = v51;
                        goto LABEL_42414f;
                    }
                }
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v53 = v41 + 2;
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v54 = v53 + 1;
                v55 = _ccall(10, 13, (char)((((unsigned short)v53 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
                if (!(v55 & 1))
                {
                    v56 = v54 - 1;
                    if (/* unsupported instruction */)
                    {
                        v57 = v56 - 1;
                        /* unsupported instruction */
                        /* unsupported instruction */
                    }
                    else
                    {
                        v57 = v56 - 1;
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
                        v58 = v57 + 1;
                    }
                    else
                    {
                        v4 = nan;
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
                    if (!((char)((((unsigned short)v60 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
                    {
                        v1 = (unsigned int)idx->field_400;
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
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v64 = _ccall(v20);
                        v1 = v64;
                        v4 = (int)(v1 * 3 + ((int)(v1 * 3) >> 31 & 3)) >> 2;
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v4 = v1 | 0xc00;
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
                        v19 = v63 + 3;
                        idx->field_3bf = v4;
                        v65 = _ccall(v1);
                        v20 = v65;
                        continue;
                    }
                    else
                    {
                        /* unsupported instruction */
                        /* unsupported instruction */
                        /* unsupported instruction */
                        v54 = v60 + 1;
                    }
                }
                v50 = idx->field_400[0];
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                v19 = v54 + 1;
LABEL_42414f:
                idx->field_3bf = v50;
            }
        }
    } while ((iter1 += 4, v3 = v2 - 1, v2 != 1));
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    index[4] = index[4] + 1;
    index[5] = index[5] + 1;
    return 1;
}
