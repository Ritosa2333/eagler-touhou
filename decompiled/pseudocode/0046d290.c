// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d290; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void* sub_46d290(void* a0, void* a1, unsigned int a2)
{
    unsigned int v2;  // ecx
    unsigned int v3;  // esi
    unsigned int v11;  // eax
    unsigned int v12;  // ecx
    unsigned int v13;  // ecx
    unsigned int v14;  // edx
    unsigned int v15;  // ebx
    char v16;  // 4107
    unsigned int v17;  // ecx
    unsigned int v18;  // ebx
    unsigned int v19;  // ecx
    unsigned int i;  // ecx
    unsigned int v4;  // ebx
    unsigned int v5;  // ebx
    void* v6;  // esi
    void* iter;  // edi
    unsigned int v8;  // ecx
    void* node;  // esi
    unsigned int v10;  // ebx
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v2 = a2;
    if (!v2)
        return a0;
    v1 = v3;
    v0 = v4;
    v5 = v2;
    v6 = a1;
    iter = a0;
    if (!(v6 & 0x3))
    {
        v8 = v2 >> 2;
        node = v6;
        v10 = v5;
        if (v2 <= 3)
            goto LABEL_46d2e3;
    }
    else
    {
        do
        {
            v11 = _INSERT(v11, 0, *((char *)v6));
            node = v6 + 1;
            *((char *)iter) = *((char *)v6);
            iter += 1;
            v12 = v2 - 1;
            if (v2 == 1)
                return a0;
            if (!(char)v11)
            {
                v19 = v12;
                if (iter & 0x3)
                {
                    do
                    {
                        *((char *)iter) = v11;
                        iter += 1;
                        v19 = v12 - 1;
                        if (v12 == 1)
                            return a0;
                    } while ((v12 = v19, iter & 0x3));
                }
                v10 = v19;
                v17 = v19 >> 2;
                if (v19 <= 3)
                    goto LABEL_46d323;
                goto LABEL_46d397;
            }
        } while ((v2 = v12, v6 = node, v6 & 0x3));
        v10 = v12;
        v8 = v12 >> 2;
        v6 = node;
        if (v12 <= 3)
            goto LABEL_46d2de;
    }
    do
    {
        v13 = v8;
        v11 = *((int *)v6) ^ 0xffffffff ^ 0x7efefeff + *((int *)v6);
        v14 = *((int *)v6);
        node = v6 + 4;
        if (v11 & 0x81010100)
        {
            if ((char)v14)
            {
                if (!*((char *)((void*)&v14 + 1)))
                {
                    *((unsigned int *)iter) = v14 & 0xff;
                    goto LABEL_46d38d;
                }
                else if (!(v14 & 0xff0000))
                {
                    *((unsigned int *)iter) = v14 & 0xffff;
                    goto LABEL_46d38d;
                }
                else if (!(v14 & 0xff000000))
                {
                    *((unsigned int *)iter) = v14;
                    goto LABEL_46d38d;
                }
            }
            *((unsigned int *)iter) = 0;
LABEL_46d38d:
            iter += 4;
            v11 = 0;
            v17 = v13 - 1;
            v18 = v10;
            if (v13 != 1)
            {
LABEL_46d397:
                v11 = 0;
                do
                {
                    i = v17;
                    *((unsigned int *)iter) = 0;
                    iter += 4;
                    v17 = i - 1;
                    v18 = v10;
                } while (i != 1);
            }
            v10 = v18 & 3;
            if (!(v18 & 3))
                return a0;
            goto LABEL_46d323;
            return a0;
        }
    } while ((*((unsigned int *)iter) = v14, iter += 4, v8 = v13 - 1, v6 = node, v13 != 1));
LABEL_46d2de:
    v5 = v10 & 3;
    if (!(v10 & 3))
        return a0;
    while (1)
    {
LABEL_46d2e3:
        v15 = v5;
        v16 = *((char *)node);
        v11 = _INSERT(v11, 0, v16);
        node += 1;
        *((char *)iter) = v16;
        iter += 1;
        if (!v16)
            break;
        v5 = v15 - 1;
        if (v15 == 1)
            return a0;
    }
    while (1)
    {
        v10 = v15 - 1;
        if (v15 == 1)
            break;
LABEL_46d323:
        *((char *)iter) = v11;
        iter += 1;
        v15 = v10;
    }
    return a0;
}
