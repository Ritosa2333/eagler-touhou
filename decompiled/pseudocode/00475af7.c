// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475af7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_475af7_0 {
    unsigned int field_0;
    unsigned short field_4;
} st_475af7_0;


void sub_475af7(void* idx, void* a1)
{
    void* v10;  // ebx
    unsigned int v11;  // edx
    unsigned int v20;  // esi
    unsigned int node;  // esi
    unsigned int v22;  // 4108
    unsigned int v23;  // 4201
    unsigned int v24;  // 4105
    unsigned int v12;  // ecx
    unsigned short v13;  // cx
    unsigned short v14;  // dx
    unsigned int v15;  // edi
    unsigned int iter;  // esi
    st_475af7_0 *iter1;  // edi
    void* v18;  // ebx
    unsigned int v19;  // ecx
    unsigned int v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x34]
    unsigned int v2;  // [bp-0x30]
    int iter2;  // [bp-0x2c]
    void* v4;  // [bp-0x28]
    unsigned int v5;  // [bp-0x24]
    unsigned int v6;  // [bp-0x20]
    int v7;  // [bp-0x1c]
    unsigned int v8;  // [bp-0x18]
    int v9;  // [bp-0x14]

    v10 = a1;
    v11 = (short)v10[10];
    v2 = 0;
    *((unsigned int *)&v9) = 0;
    *((unsigned int *)&(&v9)[4]) = 0;
    *((unsigned int *)&(&v9)[8]) = 0;
    v12 = (short)idx[10];
    v5 = (v11 ^ v12) & 0x8000;
    v13 = (unsigned short)v12 & 0x7fff;
    v14 = (unsigned short)v11 & 0x7fff;
    v0 = v15;
    iter = (unsigned short)(v14 + v13);
    v1 = iter;
    if (v13 < 0x7fff && v14 < 0x7fff && (unsigned short)iter <= 0xbffd)
    {
        if ((unsigned short)iter <= 16319)
        {
LABEL_475b76:
            *((unsigned int *)&idx[8]) = 0;
            *((unsigned int *)idx) = 0;
            *((unsigned int *)&idx[4]) = 0;
            return;
        }
        if (!(!v13 && !(iter += 1, v1 = iter, (int)idx[8] & 0x7fffffff)))
        {
        }
        else if (!(int)idx[4] && !*((int *)idx))
        {
            *((unsigned short *)&idx[10]) = 0;
            return;
        }
        if (!v14 && !(iter += 1, v1 = iter, (int)v10[8] & 0x7fffffff || (int)v10[4] || *((int *)v10)))
            goto LABEL_475b76;
        v6 = 0;
        iter1 = &v9 + 4;
        v7 = 5;
        do
        {
            iter2 = v7;
            if (v7 > 0)
            {
                v4 = v10 + 8;
                v18 = idx + v6 * 2;
                do
                {
                    v8 = 0;
                    v19 = *((short *)v4) * *((short *)v18);
                    v20 = *((int *)((char *)iter1 - 4)) + v19;
                    if (v20 < *((int *)((char *)iter1 - 4)) || v20 < v19)
                        v8 = 1;
                    *((unsigned int *)((char *)iter1 - 4)) = v20;
                    if (v8)
                        *((unsigned short *)&iter1->field_0) = (short)iter1->field_0 + 1;
                    v4 -= 2;
                    v18 += 2;
                    iter2 -= 1;
                } while (iter2 > 0);
                v10 = a1;
                iter = v1;
            }
        } while ((iter1 += 2, v6 += 1, v7 = (int)(v7 - 1), v7 > 0));
        node = iter + 49154;
        v22 = _ccall(14, 14, (unsigned short)node, 0, 0);
        if (!(v22 & 1))
        {
            do
            {
                if (*((unsigned int *)(&v9 + 8)) & 0x80000000)
                    break;
            } while ((*((unsigned int *)&v9) = *((unsigned int *)(void*)&v9) * 2, node += 0xffff, *((unsigned int *)&(&v9)[4]) = *((unsigned int *)((void*)&v9 + 4)) * 2 | *((unsigned int *)(void*)&v9) >> 31, *((unsigned int *)&(&v9)[8]) = *((unsigned int *)((void*)&v9 + 8)) * 2 | *((unsigned int *)((void*)&v9 + 4)) >> 31, v23 = (unsigned int)_ccall(14, 14, (unsigned int)(unsigned short)node, 0, 0), !(v23 & 1)));
            v24 = _ccall(14, 14, (unsigned short)node, 0, 0);
            if (!(v24 & 1))
                goto LABEL_475cd0;
        }
        node += 0xffff;
        if ((unsigned short)node < 0)
        {
            v8 = (unsigned short)-(node);
            node += v8;
            do
            {
                if (*((char *)&v9) & 1)
                    v2 += 1;
            } while ((*((unsigned int *)&(&v9)[8]) = *((unsigned int *)((void*)&v9 + 8)) >> 1, v8 -= 1, *((unsigned int *)&(&v9)[4]) = *((unsigned int *)((void*)&v9 + 4)) >> 1 | *((unsigned int *)((void*)&v9 + 8)) * 0x80000000, *((unsigned int *)&v9) = *((unsigned int *)(void*)&v9) >> 1 | *((unsigned int *)((void*)&v9 + 4)) * 0x80000000, v8 != 1));
            if (v2)
                v9 |= 1;
        }
LABEL_475cd0:
        if (*((unsigned short *)&v9) > 0x8000 || (*((unsigned int *)&v9) & 0x1ffff) == 0x18000)
        {
            if (*((unsigned int *)(&v9 + 2)) == 0xffffffff)
            {
                *((unsigned int *)&(&v9)[2]) = 0;
                if (*((unsigned int *)(&v9 + 6)) == 0xffffffff)
                {
                    *((unsigned int *)&(&v9)[6]) = 0;
                    if (*((unsigned short *)(&v9 + 10)) == 0xffff)
                    {
                        *((unsigned short *)&(&v9)[10]) = 0x8000;
                        node += 1;
                    }
                    else
                    {
                        *((unsigned short *)&(&v9)[10]) = *((unsigned short *)(&v9 + 10)) + 1;
                    }
                }
                else
                {
                    *((unsigned int *)&(&v9)[6]) = *((unsigned int *)(&v9 + 6)) + 1;
                }
            }
            else
            {
                *((unsigned int *)&(&v9)[2]) = *((unsigned int *)(&v9 + 2)) + 1;
            }
        }
        if ((unsigned short)node < 0x7fff)
        {
            *((unsigned short *)idx) = *((unsigned short *)(&v9 + 2));
            *((unsigned int *)&idx[2]) = *((unsigned int *)(&v9 + 4));
            *((unsigned int *)&idx[6]) = *((unsigned int *)(&v9 + 8));
            *((unsigned short *)&idx[10]) = (unsigned short)node | (unsigned short)v5;
            return;
        }
    }
    *((unsigned int *)&idx[8]) = ((unsigned int)((!(unsigned short)v5) + 1) & 0x80000000) + 0x7fff8000;
    *((unsigned int *)idx) = 0;
    *((unsigned int *)&idx[4]) = 0;
    return;
}
