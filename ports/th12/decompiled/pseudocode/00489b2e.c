// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x489b2e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_489b2e_0 {
    unsigned short field_0;
    unsigned int field_2;
    char padding_6[4];
    unsigned short field_a;
} st_489b2e_0;

extern char g_4adfc8;
extern char g_4adfcc;
extern int g_4adfd0;
extern int g_4adfd4;
extern unsigned int g_4adfd8;
extern unsigned int g_4adfdc;

unsigned int sub_489b2e(st_489b2e_0 *iter, unsigned int *a1)
{
    unsigned int v14;  // ecx
    int v15;  // ebx
    unsigned int *v24;  // edi
    int iter1;  // eax
    unsigned int v26;  // ecx
    int v27;  // eax
    unsigned int v28;  // esi
    unsigned int v29;  // edx
    unsigned int *v30;  // ecx
    int v31;  // eax
    int v32;  // eax
    unsigned int *iter2;  // edi
    unsigned int v16;  // edi
    unsigned int v34;  // ecx
    int v35;  // edx
    int v36;  // eax
    unsigned int v37;  // edx
    unsigned int *v38;  // ebx
    int index;  // edx
    unsigned int *v40;  // ecx
    unsigned int v41;  // esi
    int v42;  // ecx
    int v43;  // eax
    unsigned int v17;  // ebx
    int v44;  // edx
    unsigned int v45;  // edx
    unsigned int *v46;  // ebx
    unsigned int v47;  // ecx
    int v48;  // eax
    unsigned int v49;  // esi
    unsigned int v50;  // edx
    unsigned int *v51;  // ecx
    unsigned int v52;  // edi
    unsigned int v53;  // ecx
    int v18;  // eax
    int v54;  // eax
    unsigned int *v55;  // ecx
    unsigned int v56;  // esi
    unsigned int v57;  // edi
    int v58;  // eax
    unsigned int *k;  // edi
    unsigned int v60;  // ecx
    unsigned int v61;  // edx
    int v62;  // eax
    unsigned int v63;  // edx
    unsigned int v19;  // eax
    unsigned int *v64;  // ebx
    int idx;  // edx
    unsigned int *v66;  // ecx
    int v67;  // eax
    unsigned int v68;  // edx
    unsigned int *v69;  // ebx
    int idx1;  // edx
    unsigned int *v71;  // ecx
    int v72;  // eax
    unsigned int v73;  // edx
    unsigned int v20;  // esi
    int idx2;  // edx
    unsigned int *v75;  // ecx
    unsigned int v76;  // ebx
    unsigned int v21;  // esi
    int v22;  // edx
    unsigned int v23;  // edx
    int v0;  // [bp-0x40], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x3c]
    unsigned int v2;  // [bp-0x38]
    unsigned int v3;  // [bp-0x30]
    unsigned int v4;  // [bp-0x2c]
    unsigned int v5;  // [bp-0x28]
    unsigned int v6;  // [bp-0x24]
    unsigned int v7;  // [bp-0x20]
    unsigned int v8;  // [bp-0x1c]
    unsigned int v9;  // [bp-0x18]
    int v10;  // [bp-0x14], Other Possible Types: unsigned int
    int v11;  // [bp-0x10], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0xc]
    unsigned int node;  // [bp-0x8]

    v14 = *((short *)&iter->padding_6[2]);
    v9 = v14 & 0x8000;
    v6 = *((int *)((char *)&iter->field_2 + 2));
    v15 = (v14 & 0x7fff) - 0x3fff;
    v2 = v16;
    v7 = *((int *)&(&iter->field_0)[1]);
    v8 = iter->field_0 * 0x10000;
    if (v15 == -0x3fff)
    {
        v17 = 0;
        v18 = 0;
        do
        {
            if ((&v6)[v18])
            {
                v6 = 0;
                v7 = 0;
                v1 = 2;
                v8 = 0;
                v19 = v1;
                goto LABEL_48a02f;
            }
        } while ((v18 = (int)(v18 + 1), v18 < 3));
        v19 = 0;
    }
    else
    {
        iter = 0;
        v1 = v20;
        v3 = v6;
        v4 = v7;
        v5 = v8;
        v21 = g_4adfd0 - 1;
        v22 = v21 + 1;
        v23 = v22 & 0x8000001f;
        v10 = v15;
        v11 = (int)(v22 + (v22 >> 31 & 31)) >> 5;
        if ((v22 & 0x8000001f) < 0)
            v23 = (v23 - 1 | 0xffffffe0) + 1;
        v24 = &(&v6)[v11];
        v0 = 31;
        v12 = v0 - v23;
        if (1 << ((char)v12 & 31) & *(v24))
        {
            iter1 = v11;
            if (!(~(0xffffffff << ((char)v12 & 31)) & (&v6)[iter1]))
            {
                do
                {
                    iter1 += 1;
                    if (iter1 >= 3)
                        goto LABEL_489c78;
                } while (!(&v6)[iter1]);
            }
            v0 = 31;
            v26 = v0;
            v27 = (int)(v21 + ((int)(v21) >> 31 & v26)) >> 5;
            v28 = v21 & 0x8000001f;
            if ((v21 & 0x8000001f) < 0)
                v28 = (v28 - 1 | 0xffffffe0) + 1;
            node = 0;
            v29 = 1 << ((char)(v26 - v28) & 31);
            v30 = &(&v6)[v27];
            iter = *(v30) + v29;
            if (iter >= *(v30))
            {
                v31 = v27;
                if (iter >= v29)
                    goto LABEL_489c6a;
            }
            while (1)
            {
                node = 1;
                v31 = v27;
                do
                {
LABEL_489c6a:
                    v27 = v31 - 1;
                    *(v30) = iter;
                    if (v31 - 1 < 0 || !node)
                    {
                        iter = node;
                        goto LABEL_489c78;
                    }
                } while ((node = 0, v30 = &v6 + v27 * 4, iter = *(v30) + 1, iter >= *(v30) && !(v31 = v27, iter < 1)));
            }
        }
LABEL_489c78:
        *(v24) = *(v24) & 0xffffffff << ((char)v12 & 31);
        v32 = v11 + 1;
        if (v32 < 3)
        {
            v0 = 3;
            iter2 = &(&v6)[v32];
            for (v34 = v0 - v32; v34; iter2 += 1)
            {
                v34 -= 1;
                *(iter2) = 0;
            }
        }
        if (iter)
            v15 += 1;
        if (v15 < *((int *)&g_4adfcc) - g_4adfd0)
        {
            v6 = 0;
            v7 = 0;
            v8 = 0;
            goto LABEL_489eca;
        }
        else if (v15 <= *((int *)&g_4adfcc))
        {
            v6 = v3;
            v7 = v4;
            v35 = *((int *)&g_4adfcc) - v10;
            v36 = (int)(v35 + (v35 >> 31 & 31)) >> 5;
            v37 = v35 & 0x8000001f;
            v8 = v5;
            if ((v35 & 0x8000001f) < 0)
                v37 = (v37 - 1 | 0xffffffe0) + 1;
            v11 = 0;
            iter = 0;
            node = 32;
            node -= v37;
            do
            {
                v38 = &(&v6)[iter];
                v10 = *(v38) & ~(0xffffffff << ((char)v37 & 31));
                *(v38) = *(v38) >> ((char)v37 & 31) | v11;
                iter += 1;
                v11 = v10 << ((char)node & 31);
            } while (iter < 3);
            v0 = 2;
            index = v0;
            v40 = &(&v8)[-1 * v36];
            do
            {
                if (index >= v36)
                    (&v6)[index] = *(v40);
                else
                    (&v6)[index] = 0;
            } while ((index = (int)(index - 1), v40 -= 4, index >= 0));
            v41 = g_4adfd0 - 1;
            v42 = v41 + 1;
            v43 = (int)(v42 + (v42 >> 31 & 31)) >> 5;
            v44 = v42;
            v45 = v44 & 0x8000001f;
            v11 = v43;
            if ((v44 & 0x8000001f) < 0)
                v45 = (v45 - 1 | 0xffffffe0) + 1;
            v0 = 31;
            v46 = &(&v6)[v43];
            v10 = v0 - v45;
            if (1 << ((char)v10 & 31) & *(v46))
            {
                if (!(~(0xffffffff << ((char)v10 & 31)) & (&v6)[v43]))
                {
                    do
                    {
                        v43 += 1;
                        if (v43 >= 3)
                            goto LABEL_489e1b;
                    } while (!(&v6)[v43]);
                }
                v0 = 31;
                v47 = v0;
                v48 = (int)(v41 + ((int)(v41) >> 31 & v47)) >> 5;
                v49 = v41 & 0x8000001f;
                if ((v41 & 0x8000001f) < 0)
                    v49 = (v49 - 1 | 0xffffffe0) + 1;
                iter = 0;
                v50 = 1 << ((char)(v47 - v49) & 31);
                v51 = &(&v6)[v48];
                v52 = *(v51) + v50;
                if (v52 < *(v51) || v52 < v50)
                    iter = 1;
                *(v51) = v52;
                v53 = iter;
                while (1)
                {
                    v54 = v48 - 1;
                    if (v48 - 1 < 0 || !v53)
                        break;
                    v55 = &(&v6)[v54];
                    v56 = *(v55) + 1;
                    v57 = 0;
                    if (v56 < *(v55) || v56 < 1)
                        v57 = 1;
                    *(v55) = v56;
                    v53 = v57;
                    v48 = v54;
                }
            }
LABEL_489e1b:
            *(v46) = *(v46) & 0xffffffff << ((char)v10 & 31);
            v58 = v11 + 1;
            if (v58 < 3)
            {
                v0 = 3;
                k = &(&v6)[v58];
                for (v60 = v0 - v58; v60; k += 1)
                {
                    v60 -= 1;
                    *(k) = 0;
                }
            }
            v61 = g_4adfd4 + 1;
            v62 = (int)(v61 + ((int)(v61) >> 31 & 31)) >> 5;
            v63 = v61 & 0x8000001f;
            if ((v61 & 0x8000001f) < 0)
                v63 = (v63 - 1 | 0xffffffe0) + 1;
            v11 = 0;
            iter = 0;
            node = 32;
            node -= v63;
            do
            {
                v64 = &(&v6)[iter];
                v10 = *(v64) & ~(0xffffffff << ((char)v63 & 31));
                *(v64) = *(v64) >> ((char)v63 & 31) | v11;
                iter += 1;
                v11 = v10 << ((char)node & 31);
            } while (iter < 3);
            v0 = 2;
            idx = v0;
            v66 = &(&v8)[-1 * v62];
            do
            {
                if (idx >= v62)
                    (&v6)[idx] = *(v66);
                else
                    (&v6)[idx] = 0;
            } while ((idx = (int)(idx - 1), v66 -= 4, idx >= 0));
LABEL_489eca:
            v0 = 2;
            v17 = 0;
            v19 = v0;
        }
        else if (v15 >= *((int *)&g_4adfc8))
        {
            v6 = 0;
            v7 = 0;
            v8 = 0;
            v6 |= 0x80000000;
            v67 = (int)(g_4adfd4 + (g_4adfd4 >> 31 & 31)) >> 5;
            v68 = g_4adfd4 & 0x8000001f;
            if ((g_4adfd4 & 0x8000001f) < 0)
                v68 = (v68 - 1 | 0xffffffe0) + 1;
            v11 = 0;
            iter = 0;
            node = 32;
            node -= v68;
            do
            {
                v69 = &(&v6)[iter];
                v10 = *(v69) & ~(0xffffffff << ((char)v68 & 31));
                *(v69) = *(v69) >> ((char)v68 & 31) | v11;
                iter += 1;
                v11 = v10 << ((char)node & 31);
            } while (iter < 3);
            v0 = 2;
            idx1 = v0;
            v71 = &(&v8)[-1 * v67];
            do
            {
                if (idx1 >= v67)
                    (&v6)[idx1] = *(v71);
                else
                    (&v6)[idx1] = 0;
            } while ((idx1 = (int)(idx1 - 1), v71 -= 4, idx1 >= 0));
            v17 = g_4adfdc + *((int *)&g_4adfc8);
            v19 = 1;
        }
        else
        {
            v6 &= 0x7fffffff;
            v17 = v15 + g_4adfdc;
            v72 = (int)(g_4adfd4 + (g_4adfd4 >> 31 & 31)) >> 5;
            v73 = g_4adfd4 & 0x8000001f;
            if ((g_4adfd4 & 0x8000001f) < 0)
                v73 = (v73 - 1 | 0xffffffe0) + 1;
            v11 = 0;
            iter = 0;
            node = 32;
            node -= v73;
            do
            {
                v10 = (&v6)[iter] & ~(0xffffffff << ((char)v73 & 31));
                (&v6)[iter] = (&v6)[iter] >> ((char)v73 & 31) | v11;
                iter += 1;
                v11 = v10 << ((char)node & 31);
            } while (iter < 3);
            v0 = 2;
            idx2 = v0;
            v75 = &(&v8)[-1 * v72];
            do
            {
                if (idx2 >= v72)
                    (&v6)[idx2] = *(v75);
                else
                    (&v6)[idx2] = 0;
            } while ((idx2 = (int)(idx2 - 1), v75 -= 4, idx2 >= 0));
            v19 = 0;
        }
    }
LABEL_48a02f:
    v1 = 31;
    v76 = v17 << ((char)(v1 - g_4adfd4) & 31) | -(0 < v9) & 0x80000000 | v6;
    if (g_4adfd8 == 64)
    {
        a1[1] = v76;
        *(a1) = v7;
        return v19;
    }
    if (g_4adfd8 != 32)
        return v19;
    *(a1) = v76;
    return v19;
}
