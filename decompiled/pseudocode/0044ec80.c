// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ec80; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int * sub_44ec80(int *idx, unsigned int a1, int *a2, unsigned int a3)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    unsigned int v13;  // 4101
    unsigned int v5;  // edx
    int *v6;  // eax
    int *v7;  // ecx
    int *v8;  // ecx
    unsigned int v9;  // eax
    unsigned int v10;  // ebx
    int *v11;  // ebx
    int *v12;  // eax
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    v2 = v3;
    v1 = v4;
    if (a2)
    {
        v5 = idx[6];
        v6 = idx + 1;
        if (v5 >= 16)
            v7 = *(v6);
        else
            v7 = v6;
        if (a2 >= v7)
        {
            if (v5 >= 16)
                v8 = *(v6);
            else
                v8 = v6;
            if (idx[5] + (char *)v8 > a2)
            {
                if (v5 >= 16)
                    v6 = *(v6);
                return sub_44eaf0(idx, a2 - v6, a3);
            }
        }
    }
    if (a3 > 0xfffffffe)
        sub_491687();
    v9 = idx[6];
    if (v9 < a3)
    {
        sub_44ef10(a3, idx[5]);
        if (a3 <= NULL)
            return idx;
    }
    else if (!a3)
    {
        idx[5] = 0;
        if (v9 >= 16)
            *((char *)idx[1]) = 0;
        else
            *((char *)&idx[1]) = 0;
        return idx;
    }
    else if (a3 <= NULL)
    {
        return idx;
    }
    v0 = v10;
    v11 = idx + 1;
    if (idx[6] >= 16)
        v12 = *(v11);
    else
        v12 = v11;
    sub_46e210(v12, idx[6], a2, a3);
    v13 = idx[6];
    idx[5] = a3;
    if (v13 >= 16)
        v11 = *(v11);
    *((char *)v11 + a3) = 0;
    return idx;
}
