// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4890bf; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4890bf_0 {
    unsigned short field_0;
    unsigned int field_2;
    char padding_6[4];
    unsigned short field_a;
} st_4890bf_0;

typedef struct st_4890bf_1 {
    unsigned int field_0;
    char padding_4[4];
    int field_8;
    int field_c;
    unsigned int field_10;
    unsigned int field_14;
} st_4890bf_1;


unsigned int sub_4890bf(st_4890bf_0 *iter, unsigned int *a1, st_4890bf_1 *index)
{
    unsigned int v13;  // ecx
    int v14;  // ebx
    unsigned int v23;  // ecx
    int v24;  // eax
    unsigned int v25;  // esi
    unsigned int v26;  // edx
    unsigned int *v27;  // ecx
    int v28;  // eax
    int v29;  // eax
    unsigned int *iter1;  // edi
    unsigned int v31;  // ecx
    st_4890bf_1 *v32;  // ecx
    unsigned int v15;  // edi
    int v33;  // eax
    int v34;  // edx
    int v35;  // eax
    unsigned int v36;  // edx
    unsigned int *v37;  // ebx
    int idx;  // edx
    unsigned int *v39;  // ecx
    unsigned int v40;  // esi
    unsigned int v41;  // ecx
    int v42;  // eax
    unsigned int v16;  // esi
    unsigned int v43;  // edx
    unsigned int v44;  // edx
    unsigned int *v45;  // ebx
    unsigned int v46;  // ecx
    int v47;  // eax
    unsigned int v48;  // esi
    unsigned int v49;  // edx
    unsigned int *v50;  // ecx
    unsigned int v51;  // edi
    unsigned int v52;  // ecx
    int v17;  // eax
    int v53;  // eax
    unsigned int *v54;  // ecx
    unsigned int v55;  // esi
    unsigned int v56;  // edi
    int v57;  // eax
    unsigned int *iter2;  // edi
    unsigned int v59;  // ecx
    unsigned int v60;  // edx
    int v61;  // eax
    unsigned int v62;  // edx
    unsigned int v18;  // esi
    unsigned int *v63;  // ebx
    int idx1;  // edx
    unsigned int *v65;  // ecx
    unsigned int v66;  // eax
    st_4890bf_1 *idx2;  // eax
    int v68;  // eax
    unsigned int v69;  // edx
    unsigned int *v70;  // ebx
    int v71;  // edx
    unsigned int *v72;  // ecx
    unsigned int v19;  // edx
    st_4890bf_1 *v73;  // eax
    int v74;  // ebx
    unsigned int v75;  // edx
    int v76;  // ecx
    unsigned int *v77;  // eax
    st_4890bf_1 *v78;  // edx
    unsigned int v79;  // esi
    unsigned int v20;  // edx
    unsigned int *v21;  // edi
    int v22;  // eax
    int v0;  // [bp-0x40], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x30]
    unsigned int v3;  // [bp-0x2c]
    unsigned int v4;  // [bp-0x28]
    unsigned int v5;  // [bp-0x24]
    unsigned int v6;  // [bp-0x20]
    unsigned int v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0x18]
    int v9;  // [bp-0x14], Other Possible Types: unsigned int
    int v10;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v11;  // [bp-0xc]
    unsigned int node;  // [bp-0x8]

    v13 = *((short *)&iter->padding_6[2]);
    v8 = v13 & 0x8000;
    v5 = *((int *)((char *)&iter->field_2 + 2));
    v14 = (v13 & 0x7fff) - 0x3fff;
    v1 = v15;
    v6 = *((int *)&(&iter->field_0)[1]);
    v7 = iter->field_0 * 0x10000;
    if (v14 == -0x3fff)
    {
        v16 = 0;
        v17 = 0;
        do
        {
            if ((&v5)[v17])
                goto LABEL_489231;
        } while ((v17 = (int)(v17 + 1), v17 < 3));
    }
    else
    {
        iter = 0;
        v2 = v5;
        v3 = v6;
        v4 = v7;
        v18 = index->field_8 - 1;
        v19 = v18 + 1;
        v20 = v19 & 0x8000001f;
        v9 = v14;
        v10 = (int)(v19 + ((int)(v19) >> 31 & 31)) >> 5;
        if ((v19 & 0x8000001f) < 0)
            v20 = (v20 - 1 | 0xffffffe0) + 1;
        v21 = &(&v5)[v10];
        v0 = 31;
        v11 = v0 - v20;
        if (1 << ((char)v11 & 31) & *(v21))
        {
            v22 = v10;
            if (!(~(0xffffffff << ((char)v11 & 31)) & (&v5)[v22]))
            {
                do
                {
                    v22 += 1;
                    if (v22 >= 3)
                        goto LABEL_4891fb;
                } while (!(&v5)[v22]);
            }
            v0 = 31;
            v23 = v0;
            v24 = (int)(v18 + ((int)(v18) >> 31 & v23)) >> 5;
            v25 = v18 & 0x8000001f;
            if ((v18 & 0x8000001f) < 0)
                v25 = (v25 - 1 | 0xffffffe0) + 1;
            node = 0;
            v26 = 1 << ((char)(v23 - v25) & 31);
            v27 = &(&v5)[v24];
            iter = *(v27) + v26;
            if (iter >= *(v27))
            {
                v28 = v24;
                if (iter >= v26)
                    goto LABEL_4891ed;
            }
            while (1)
            {
                node = 1;
                v28 = v24;
                do
                {
LABEL_4891ed:
                    v24 = v28 - 1;
                    *(v27) = iter;
                    if (v28 - 1 < 0 || !node)
                    {
                        iter = node;
                        goto LABEL_4891fb;
                    }
                } while ((node = 0, v27 = &v5 + v24 * 4, iter = *(v27) + 1, iter >= *(v27) && !(v28 = v24, iter < 1)));
            }
        }
LABEL_4891fb:
        v0 = 3;
        *(v21) = *(v21) & 0xffffffff << ((char)v11 & 31);
        v29 = v10 + 1;
        if (v29 < v0)
        {
            iter1 = &(&v5)[v29];
            for (v31 = v0 - v29; v31; iter1 += 1)
            {
                v31 -= 1;
                *(iter1) = 0;
            }
        }
        v16 = 0;
        if (iter)
            v14 += 1;
        v32 = index;
        v33 = (int)v32->padding_4;
        if (v14 < v33 - v32->field_8)
        {
LABEL_489231:
            v5 = 0;
            v6 = 0;
            v7 = 0;
        }
        else if (v14 <= v33)
        {
            v5 = v2;
            v6 = v3;
            v34 = v33 - v9;
            v35 = (int)(v34 + (v34 >> 31 & 31)) >> 5;
            v36 = v34 & 0x8000001f;
            v7 = v4;
            if ((v34 & 0x8000001f) < 0)
                v36 = (v36 - 1 | 0xffffffe0) + 1;
            v10 = 0;
            iter = 0;
            node = 32;
            node -= v36;
            do
            {
                v37 = &(&v5)[iter];
                v9 = *(v37) & ~(0xffffffff << ((char)v36 & 31));
                *(v37) = *(v37) >> ((char)v36 & 31) | v10;
                iter += 1;
                v10 = v9 << ((char)node & 31);
            } while (iter < 3);
            v0 = 2;
            idx = v0;
            v39 = &(&v7)[-1 * v35];
            do
            {
                if (idx >= v35)
                    (&v5)[idx] = *(v39);
                else
                    (&v5)[idx] = 0;
            } while ((idx = (int)(idx - 1), v39 -= 4, idx >= 0));
            v40 = index->field_8 - 1;
            v41 = v40 + 1;
            v42 = (int)(v41 + ((int)(v41) >> 31 & 31)) >> 5;
            v43 = v41;
            v44 = v43 & 0x8000001f;
            v10 = v42;
            if ((v43 & 0x8000001f) < 0)
                v44 = (v44 - 1 | 0xffffffe0) + 1;
            v0 = 31;
            v45 = &(&v5)[v42];
            v9 = v0 - v44;
            if (1 << ((char)v9 & 31) & *(v45))
            {
                if (!(~(0xffffffff << ((char)v9 & 31)) & (&v5)[v42]))
                {
                    do
                    {
                        v42 += 1;
                        if (v42 >= 3)
                            goto LABEL_48939c;
                    } while (!(&v5)[v42]);
                }
                v0 = 31;
                v46 = v0;
                v47 = (int)(v40 + ((int)(v40) >> 31 & v46)) >> 5;
                v48 = v40 & 0x8000001f;
                if ((v40 & 0x8000001f) < 0)
                    v48 = (v48 - 1 | 0xffffffe0) + 1;
                iter = 0;
                v49 = 1 << ((char)(v46 - v48) & 31);
                v50 = &(&v5)[v47];
                v51 = *(v50) + v49;
                if (v51 < *(v50) || v51 < v49)
                    iter = 1;
                *(v50) = v51;
                v52 = iter;
                while (1)
                {
                    v53 = v47 - 1;
                    if (v47 - 1 < 0 || !v52)
                        break;
                    v54 = &(&v5)[v53];
                    v55 = *(v54) + 1;
                    v56 = 0;
                    if (v55 < *(v54) || v55 < 1)
                        v56 = 1;
                    *(v54) = v55;
                    v52 = v56;
                    v47 = v53;
                }
            }
LABEL_48939c:
            *(v45) = *(v45) & 0xffffffff << ((char)v9 & 31);
            v57 = v10 + 1;
            if (v57 < 3)
            {
                v0 = 3;
                iter2 = &(&v5)[v57];
                for (v59 = v0 - v57; v59; iter2 += 1)
                {
                    v59 -= 1;
                    *(iter2) = 0;
                }
            }
            v60 = index->field_c + 1;
            v61 = (int)(v60 + ((int)(v60) >> 31 & 31)) >> 5;
            v62 = v60 & 0x8000001f;
            if ((v60 & 0x8000001f) < 0)
                v62 = (v62 - 1 | 0xffffffe0) + 1;
            v10 = 0;
            iter = 0;
            node = 32;
            node -= v62;
            do
            {
                v63 = &(&v5)[iter];
                v9 = *(v63) & ~(0xffffffff << ((char)v62 & 31));
                *(v63) = *(v63) >> ((char)v62 & 31) | v10;
                iter += 1;
                v10 = v9 << ((char)node & 31);
            } while (iter < 3);
            v0 = 2;
            idx1 = v0;
            v65 = &(&v7)[-1 * v61];
            do
            {
                if (idx1 >= v61)
                    (&v5)[idx1] = *(v65);
                else
                    (&v5)[idx1] = 0;
            } while ((idx1 = (int)(idx1 - 1), v65 -= 4, idx1 >= 0));
            v16 = 0;
        }
        else if (v14 >= v32->field_0)
        {
            v5 = 0;
            v6 = 0;
            v7 = 0;
            v5 |= 0x80000000;
            idx2 = v32;
            v68 = (int)(idx2->field_c + (idx2->field_c >> 31 & 31)) >> 5;
            v69 = idx2->field_c & 0x8000001f;
            if ((idx2->field_c & 0x8000001f) < 0)
                v69 = (v69 - 1 | 0xffffffe0) + 1;
            v10 = 0;
            iter = 0;
            node = 32;
            node -= v69;
            do
            {
                v70 = &(&v5)[iter];
                v9 = *(v70) & ~(0xffffffff << ((char)v69 & 31));
                *(v70) = *(v70) >> ((char)v69 & 31) | v10;
                iter += 1;
                v10 = v9 << ((char)node & 31);
            } while (iter < 3);
            v0 = 2;
            v71 = v0;
            v72 = &(&v7)[-1 * v68];
            do
            {
                if (v71 >= v68)
                    (&v5)[v71] = *(v72);
                else
                    (&v5)[v71] = 0;
            } while ((v71 = (int)(v71 - 1), v72 -= 4, v71 >= 0));
            v16 = index->field_14 + index->field_0;
            v66 = 1;
            goto LABEL_4895a9;
        }
        else
        {
            v5 &= 0x7fffffff;
            v73 = v32;
            v16 = v73->field_14 + v14;
            v74 = (int)(v73->field_c + (v73->field_c >> 31 & 31)) >> 5;
            v75 = v73->field_c & 0x8000001f;
            if ((v73->field_c & 0x8000001f) < 0)
                v75 = (v75 - 1 | 0xffffffe0) + 1;
            v10 = 0;
            iter = 0;
            node = 32;
            node -= v75;
            do
            {
                v9 = (&v5)[iter] & ~(0xffffffff << ((char)v75 & 31));
                (&v5)[iter] = (&v5)[iter] >> ((char)v75 & 31) | v10;
                iter += 1;
                v10 = v9 << ((char)node & 31);
            } while (iter < 3);
            v0 = 2;
            v76 = v0;
            v77 = &(&v7)[-1 * v74];
            do
            {
                if (v76 >= v74)
                    (&v5)[v76] = *(v77);
                else
                    (&v5)[v76] = 0;
            } while ((v76 = (int)(v76 - 1), v77 -= 4, v76 >= 0));
            v66 = 0;
            goto LABEL_4895a9;
        }
        v0 = 2;
        v66 = v0;
    }
LABEL_4895a9:
    v78 = index;
    v0 = 31;
    v79 = v16 << ((char)(v0 - v78->field_c) & 31) | -(0 < v8) & 0x80000000 | v5;
    if (v78->field_10 == 64)
    {
        a1[1] = v79;
        *(a1) = v6;
        return v66;
    }
    if (v78->field_10 != 32)
        return v66;
    *(a1) = v79;
    return v66;
}
