// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c4e8; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void* sub_48c4e8(void* a0, void* iter, unsigned int a2)
{
    void* v4;  // eax
    unsigned int v5;  // ecx
    void* iter1;  // edi
    unsigned int v15;  // ecx
    unsigned int v6;  // edi
    unsigned int v7;  // esi
    unsigned int v8;  // ecx
    unsigned int v9;  // ebx
    void* node;  // edi
    void* iter2;  // esi
    unsigned int v12;  // ecx
    unsigned int v13;  // ecx
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    char *i;  // [bp-0x18]
    char *v3;  // [bp-0x14]

    v4 = a0;
    v5 = ((iter ^ iter >> 31) - (iter >> 31) & 15 ^ iter >> 31) - (iter >> 31);
    v6 = ((v4 ^ v4 >> 31) - (v4 >> 31) & 15 ^ v4 >> 31) - (v4 >> 31);
    if (!v5 && !v6)
    {
        v7 = a2;
        v8 = v7 & 127;
        v1 = v8;
        if (v7 != v8)
        {
            sub_48c461(v4, iter, v7 - v8);
            v4 = a0;
            v8 = v1;
        }
        if (!v8)
            return v4;
        v9 = a2;
        i = iter + v9 - v8;
        for (v3 = v9 + v4 - v8; v1; i += 1)
        {
            v1 -= 1;
            *(v3) = *(i);
            v3 += 1;
        }
        return a0;
    }
    else if (v5 == v6)
    {
        v0 = -(v5) + 16;
        node = a0;
        iter2 = iter;
        for (v12 = v0; v12; iter2 += 1)
        {
            v12 -= 1;
            *((char *)node) = *((char *)iter2);
            node += 1;
        }
        sub_48c4e8(a0 + v0, iter + v0, a2 - v0);
        return a0;
    }
    else
    {
        v13 = a2 >> 2;
        for (iter1 = a0; v13; iter += 4)
        {
            v13 -= 1;
            *((int *)iter1) = *((int *)iter);
            iter1 += 4;
        }
        for (v15 = a2 & 3; v15; iter += 1)
        {
            v15 -= 1;
            *((char *)iter1) = *((char *)iter);
            iter1 += 1;
        }
        return a0;
    }
}
