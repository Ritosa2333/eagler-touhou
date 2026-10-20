// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475d72; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_475d72_0 {
    unsigned int field_0;
    unsigned short field_4;
} st_475d72_0;

extern unsigned int g_4adf80;
extern unsigned int g_4ae0e0;

int sub_475d72(void* ptr, int a1, unsigned int a2)
{
    unsigned int v16;  // ebx
    unsigned int v17;  // esi
    unsigned short v26;  // dx
    unsigned int iter;  // edi
    st_475d72_0 *iter1;  // esi
    unsigned int v29;  // ecx
    unsigned int v30;  // edi
    unsigned int node;  // edi
    unsigned int v32;  // 4108
    unsigned int v33;  // esi
    unsigned int v34;  // 4198
    unsigned int v35;  // 4105
    unsigned int v18;  // edi
    unsigned int v36;  // esi
    unsigned int v37;  // esi
    unsigned int v38;  // eax
    unsigned int v39;  // eax
    unsigned int v40;  // eax
    unsigned int v19;  // ecx
    void* v20;  // ebx
    void* v21;  // esi
    void* v22;  // esi
    unsigned int v23;  // ecx
    unsigned int v24;  // edx
    unsigned short v25;  // cx
    unsigned int v0;  // [bp-0x54]
    unsigned int v1;  // [bp-0x50]
    unsigned int v2;  // [bp-0x4c]
    unsigned int v3;  // [bp-0x48]
    void* v4;  // [bp-0x44]
    void* v5;  // [bp-0x40]
    unsigned int v6;  // [bp-0x3c]
    unsigned int v7;  // [bp-0x38]
    unsigned int v8;  // [bp-0x34]
    int iter2;  // [bp-0x30]
    unsigned int v10;  // [bp-0x2c]
    unsigned int v11;  // [bp-0x28]
    int v12;  // [bp-0x24]
    int v13;  // [bp-0x20]
    int v14;  // [bp-0x14]
    int v15;  // [bp+0x8]

    v11 = &g_4adf80;
    if (!a1)
        return v40;
    if (a1 < 0)
    {
        a1 = -(a1);
        v11 = &g_4ae0e0;
    }
    if (!a2)
        *((unsigned short *)ptr) = 0;
    if (!a1)
        return v39;
    v2 = v16;
    v1 = v17;
    v0 = v18;
    do
    {
        v11 += 84;
        v15 = a1 >> 3;
        v19 = a1 & 7;
        if (!v19)
            continue;
        v20 = v19 * 12 + v11;
        if (*((short *)v20) >= 0x8000)
        {
            v21 = v20;
            *((int *)&v13) = *((int *)v21);
            v22 = v21 + 4;
            *((int *)&(&v13)[4]) = *((int *)v22);
            *((int *)&(&v13)[8]) = (int)v22[4];
            *((unsigned int *)&(&v13)[2]) = *((unsigned int *)(&v13 + 2)) - 1;
            v20 = &v13;
        }
        v23 = (short)ptr[10];
        v6 = 0;
        *((unsigned int *)&v14) = 0;
        *((unsigned int *)&(&v14)[4]) = 0;
        *((unsigned int *)&(&v14)[8]) = 0;
        v24 = (short)v20[10];
        v8 = (v24 ^ v23) & 0x8000;
        v25 = (unsigned short)v23 & 0x7fff;
        v26 = (unsigned short)v24 & 0x7fff;
        v10 = (unsigned short)(v26 + v25);
        if (v25 < 0x7fff && v26 < 0x7fff && !(iter = v10, (unsigned short)iter > 0xbffd))
        {
            if ((unsigned short)iter <= 16319)
            {
                *((unsigned int *)&ptr[8]) = 0;
                goto LABEL_476058;
            }
            if (!v25 && (iter += 1, v10 = iter, !((int)ptr[8] & 0x7fffffff) && !(int)ptr[4] && !*((int *)ptr)))
            {
                *((unsigned short *)&ptr[10]) = 0;
                continue;
            }
            if (!v26 && (iter += 1, v10 = iter, !((int)v20[8] & 0x7fffffff) && !(int)v20[4] && !*((int *)v20)))
            {
                memset(ptr, 0, 12);
                continue;
            }
            v7 = 0;
            iter1 = &v14 + 4;
            v12 = 5;
            do
            {
                iter2 = v12;
                if (v12 > 0)
                {
                    v4 = v20 + 8;
                    v5 = v7 * 2 + ptr;
                    do
                    {
                        v3 = 0;
                        v29 = *((short *)v4) * *((short *)v5);
                        v30 = *((int *)((char *)iter1 - 4)) + v29;
                        if (v30 < *((int *)((char *)iter1 - 4)) || v30 < v29)
                            v3 = 1;
                        *((unsigned int *)((char *)iter1 - 4)) = v30;
                        if (v3)
                            *((unsigned short *)&iter1->field_0) = (short)iter1->field_0 + 1;
                        v5 += 2;
                        v4 -= 2;
                        iter2 -= 1;
                    } while (iter2 > 0);
                    iter = v10;
                }
            } while ((iter1 += 2, v7 += 1, v12 = (int)(v12 - 1), v12 > 0));
            node = iter + 49154;
            v32 = _ccall(14, 14, (unsigned short)node, 0, 0);
            if (!(v32 & 1))
            {
                do
                {
                    if (*((unsigned int *)(&v14 + 8)) & 0x80000000)
                        break;
                    v33 = *((unsigned int *)(&v14 + 4));
                    *((unsigned int *)&v14) = *((unsigned int *)&v14) * 2;
                    node += 0xffff;
                    *((unsigned int *)&(&v14)[4]) = v33 * 2 | *((unsigned int *)&v14) >> 31;
                    *((unsigned int *)&(&v14)[8]) = *((unsigned int *)(&v14 + 8)) * 2 | v33 >> 31;
                    v34 = _ccall(14, 14, (unsigned short)node, 0, 0);
                } while (!(v34 & 1));
                v35 = _ccall(14, 14, (unsigned short)node, 0, 0);
                if (!(v35 & 1))
                    goto LABEL_475fc5;
            }
            node += 0xffff;
            if ((unsigned short)node < 0)
            {
                v36 = (unsigned short)-(node);
                node += v36;
                do
                {
                    v37 = v36;
                    if (*((char *)&v14) & 1)
                        v6 += 1;
                } while ((*((unsigned int *)&(&v14)[8]) = *((unsigned int *)((void*)&v14 + 8)) >> 1, v36 = v37 - 1, *((unsigned int *)&(&v14)[4]) = *((unsigned int *)((void*)&v14 + 4)) >> 1 | *((unsigned int *)((void*)&v14 + 8)) * 0x80000000, *((unsigned int *)&v14) = *((unsigned int *)(void*)&v14) >> 1 | *((unsigned int *)((void*)&v14 + 4)) * 0x80000000, v37 != 1));
                if (v6 != v36)
                    v14 |= 1;
            }
LABEL_475fc5:
            if (*((unsigned short *)&v14) > 0x8000 || (*((unsigned int *)&v14) & 0x1ffff) == 0x18000)
            {
                if (*((unsigned int *)(&v14 + 2)) == 0xffffffff)
                {
                    *((unsigned int *)&(&v14)[2]) = 0;
                    if (*((unsigned int *)(&v14 + 6)) == 0xffffffff)
                    {
                        *((unsigned int *)&(&v14)[6]) = 0;
                        if (*((unsigned short *)(&v14 + 10)) == 0xffff)
                        {
                            *((unsigned short *)&(&v14)[10]) = 0x8000;
                            node += 1;
                        }
                        else
                        {
                            *((unsigned short *)&(&v14)[10]) = *((unsigned short *)(&v14 + 10)) + 1;
                        }
                    }
                    else
                    {
                        *((unsigned int *)&(&v14)[6]) = *((unsigned int *)(&v14 + 6)) + 1;
                    }
                }
                else
                {
                    *((unsigned int *)&(&v14)[2]) = *((unsigned int *)(&v14 + 2)) + 1;
                }
            }
            if ((unsigned short)node < 0x7fff)
            {
                *((unsigned short *)ptr) = *((unsigned short *)(&v14 + 2));
                *((unsigned int *)&ptr[2]) = *((unsigned int *)(&v14 + 4));
                *((unsigned int *)&ptr[6]) = *((unsigned int *)(&v14 + 8));
                *((unsigned short *)&ptr[10]) = (unsigned short)node | (unsigned short)v8;
                continue;
            }
            goto LABEL_47603d;
        }
        else
        {
LABEL_47603d:
            *((unsigned int *)&ptr[8]) = ((unsigned int)((!(unsigned short)v8) + 1) & 0x80000000) + 0x7fff8000;
LABEL_476058:
            *((unsigned int *)&ptr[4]) = 0;
            *((unsigned int *)ptr) = 0;
        }
    } while ((a1 = v15, a1));
    return v38;
}
