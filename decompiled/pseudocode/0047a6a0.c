// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47a6a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_47a814[4];
extern unsigned int g_47a9b0[4];
extern unsigned int g_4d52dc;

void* sub_47a6a0(void* a0, void* a1, unsigned int a2)
{
    void* v0;  // esi
    void* v1;  // edi
    unsigned int v10;  // ecx
    unsigned int v11;  // edx
    unsigned int v12;  // ecx
    void* iter;  // esi
    void* node;  // edi
    unsigned int v4;  // ecx
    unsigned int v5;  // edx
    unsigned int v6;  // ecx
    unsigned int v7;  // ecx
    void* iter1;  // edi
    void* iter2;  // esi

    v0 = a1;
    v1 = a0;
    if (v1 > v0 && v1 < a2 + v0)
    {
        iter = a2 + v0 - 4;
        node = a2 + v1 - 4;
        if (!(node & 0x3))
        {
            v4 = a2 >> 2;
            v5 = a2 & 3;
        }
        else
        {
            switch (a2)
            {
            case 0:
                break;
            case 1:
                goto LABEL_47a9c8;
            case 2:
                goto LABEL_47a9d8;
            case 3:
                goto LABEL_47a9ec;
            default:
                v6 = a2 - (node & 3);
                switch (node & 3)
                {
                case 1:
                    v5 = 3 & v6;
                    *((char *)&node[3]) = (char)iter[3];
                    iter -= 1;
                    v4 = v6 >> 2;
                    node -= 1;
                    break;
                case 2:
                    v5 = 3 & v6;
                    *((char *)&node[3]) = (char)iter[3];
                    v4 = v6 >> 2;
                    *((char *)&node[2]) = (char)iter[2];
                    iter -= 2;
                    node -= 2;
                    break;
                case 3:
                    v5 = 3 & v6;
                    *((char *)&node[3]) = (char)iter[3];
                    *((char *)&node[2]) = (char)iter[2];
                    v4 = v6 >> 2;
                    *((char *)&node[1]) = (char)iter[1];
                    iter -= 3;
                    node -= 3;
                    break;
                }
            }
        }
        switch (v4)
        {
        case 0:
            *((int *)((char *)node + 4 * v7 + 28)) = *((int *)((char *)iter + 4 * v7 + 28));
        case 1:
            *((int *)((char *)node + 4 * v7 + 24)) = *((int *)((char *)iter + 4 * v7 + 24));
            break;
        case 2:
            *((int *)((char *)node + 4 * v7 + 20)) = *((int *)((char *)iter + 4 * v7 + 20));
            goto LABEL_47a97c;
        case 3:
LABEL_47a97c:
            *((int *)((char *)node + 4 * v7 + 16)) = *((int *)((char *)iter + 4 * v7 + 16));
            goto LABEL_47a984;
        case 4:
LABEL_47a984:
            *((int *)((char *)node + 4 * v7 + 12)) = *((int *)((char *)iter + 4 * v7 + 12));
            goto LABEL_47a98c;
        case 5:
LABEL_47a98c:
            *((int *)((char *)node + 4 * v7 + 8)) = *((int *)((char *)iter + 4 * v7 + 8));
            goto LABEL_47a994;
        case 6:
LABEL_47a994:
            *((int *)((char *)node + 4 * v7 + 4)) = *((int *)((char *)iter + 4 * v7 + 4));
            goto LABEL_47a9a7;
        case 7:
LABEL_47a9a7:
            goto *((void *)(g_47a9b0[v5]));
        default:
            for (; v4; iter += 0xfffffffc)
            {
                v4 -= 1;
                *((int *)node) = *((int *)iter);
                node += 0xfffffffc;
            }
            switch (v5)
            {
            case 0:
                return a0;
            case 1:
LABEL_47a9c8:
                *((char *)&node[3]) = (char)iter[3];
                return a0;
            case 2:
LABEL_47a9d8:
                *((char *)&node[3]) = (char)iter[3];
                *((char *)&node[2]) = (char)iter[2];
                return a0;
            case 3:
LABEL_47a9ec:
                *((char *)&node[3]) = (char)iter[3];
                *((char *)&node[2]) = (char)iter[2];
                *((char *)&node[1]) = (char)iter[1];
                return a0;
            }
        }
    }
    iter1 = v1;
    iter2 = v0;
    if (a2 >= 0x100)
    {
        iter1 = v1;
        iter2 = v0;
        if (g_4d52dc)
        {
            iter1 = v1;
            iter2 = v0;
            if ((v1 & 15) == (v0 & 15))
                return sub_48c4e8();
        }
    }
    if (!(iter1 & 0x3))
    {
        v10 = a2 >> 2;
        v11 = a2 & 3;
    }
    else
    {
        switch (a2)
        {
        case 0:
            return a0;
        case 1:
            *((char *)iter1) = *((char *)iter2);
            return a0;
        case 2:
            *((char *)iter1) = *((char *)iter2);
            *((char *)&iter1[1]) = (char)iter2[1];
            return a0;
        case 3:
            *((char *)iter1) = *((char *)iter2);
            *((char *)&iter1[1]) = (char)iter2[1];
            *((char *)&iter1[2]) = (char)iter2[2];
            return a0;
        default:
            v12 = a2 - 4 + (iter1 & 3);
            switch (iter1 & 3)
            {
            case 0:
                break;
            case 1:
                v11 = 3 & v12;
                *((char *)iter1) = *((char *)iter2);
                *((char *)&iter1[1]) = (char)iter2[1];
                v10 = v12 >> 2;
                *((char *)&iter1[2]) = (char)iter2[2];
                iter2 += 3;
                iter1 += 3;
                break;
            case 2:
                v11 = 3 & v12;
                *((char *)iter1) = *((char *)iter2);
                v10 = v12 >> 2;
                *((char *)&iter1[1]) = (char)iter2[1];
                iter2 += 2;
                iter1 += 2;
                break;
            case 3:
                v11 = 3 & v12;
                *((char *)iter1) = *((char *)iter2);
                iter2 += 1;
                v10 = v12 >> 2;
                iter1 += 1;
                break;
            }
        }
    }
    switch (v10)
    {
    case 7:
        *((int *)((char *)iter1 + 4 * v10 - 28)) = *((int *)((char *)iter2 + 4 * v10 - 28));
    case 6:
        *((int *)((char *)iter1 + 4 * v10 - 24)) = *((int *)((char *)iter2 + 4 * v10 - 24));
        break;
    case 5:
        *((int *)((char *)iter1 + 4 * v10 - 20)) = *((int *)((char *)iter2 + 4 * v10 - 20));
        goto LABEL_47a7e0;
    case 4:
LABEL_47a7e0:
        *((int *)((char *)iter1 + 4 * v10 - 16)) = *((int *)((char *)iter2 + 4 * v10 - 16));
        goto LABEL_47a7e8;
    case 3:
LABEL_47a7e8:
        *((int *)((char *)iter1 + 4 * v10 - 12)) = *((int *)((char *)iter2 + 4 * v10 - 12));
        goto LABEL_47a7f0;
    case 2:
LABEL_47a7f0:
        *((int *)((char *)iter1 + 4 * v10 - 8)) = *((int *)((char *)iter2 + 4 * v10 - 8));
        goto LABEL_47a7f8;
    case 1:
LABEL_47a7f8:
        *((int *)((char *)iter1 + 4 * v10 - 4)) = *((int *)((char *)iter2 + 4 * v10 - 4));
        goto LABEL_47a80b;
    case 0:
LABEL_47a80b:
        goto *((void *)(g_47a814[v11]));
    default:
        for (; v10; iter2 += 4)
        {
            v10 -= 1;
            *((int *)iter1) = *((int *)iter2);
            iter1 += 4;
        }
        goto *((void *)(g_47a814[v11]));
    }
}
