// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x488ee6; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_488ee6(void)
{
    unsigned int v6;  // ecx
    unsigned int v7;  // edi
    unsigned int v16;  // edi
    unsigned int v17;  // edx
    unsigned int v18;  // edi
    unsigned int v19;  // ecx
    int index;  // eax
    unsigned int v21;  // edx
    unsigned int v22;  // edi
    int v23;  // ebx
    unsigned int *iter;  // esi
    unsigned int v25;  // ecx
    int v8;  // edi
    int v9;  // eax
    int idx;  // ebx
    unsigned int v11;  // eax
    unsigned int v12;  // esi
    int v13;  // eax
    unsigned int v14;  // ecx
    int idx1;  // eax
    int v0;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x18]
    unsigned int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x8]
    unsigned int v4;  // [bp+0x4]
    int v5;  // [bp+0x8], Other Possible Types: unsigned int

    v3 = v6;
    v2 = v6;
    v3 = 0;
    v1 = v7;
    v8 = v5 - 1;
    v9 = v8 + 1;
    idx = (int)(v9 + (v9 >> 31 & 31)) >> 5;
    v11 = v9 & 0x8000001f;
    if ((v9 & 0x8000001f) < 0)
        v11 = (v11 - 1 | 0xffffffe0) + 1;
    v12 = v4;
    v0 = 31;
    v2 = v0 - v11;
    if (1 << ((char)v2 & 31) & *((int *)(v12 + idx * 4)))
    {
        v13 = idx;
        if (!(~(0xffffffff << ((char)v2 & 31)) & *((int *)(v12 + v13 * 4))))
        {
            do
            {
                v13 += 1;
                if (v13 >= 3)
                    goto LABEL_488fb0;
            } while (!*((int *)(v12 + v13 * 4)));
        }
        v0 = 31;
        v14 = v0;
        idx1 = (int)(v8 + (v8 >> 31 & v14)) >> 5;
        v16 = v8 & 0x8000001f;
        if ((v8 & 0x8000001f) < 0)
            v16 = (v16 - 1 | 0xffffffe0) + 1;
        v5 = 0;
        v17 = 1 << ((char)(v14 - v16) & 31);
        v18 = *((int *)(v12 + idx1 * 4)) + v17;
        if (v18 < *((int *)(v12 + idx1 * 4)) || v18 < v17)
            v5 = 1;
        v19 = v5;
        *((unsigned int *)(v12 + idx1 * 4)) = v18;
        while (1)
        {
            index = idx1 - 1;
            if (idx1 - 1 < 0 || !v19)
                break;
            v21 = *((int *)(v12 + index * 4)) + 1;
            v22 = 0;
            if (v21 < *((int *)(v12 + index * 4)) || v21 < 1)
                v22 = 1;
            *((unsigned int *)(v12 + index * 4)) = v21;
            v19 = v22;
            idx1 = index;
        }
        v3 = v19;
    }
LABEL_488fb0:
    v0 = 3;
    *((unsigned int *)(v12 + idx * 4)) = *((int *)(v12 + idx * 4)) & 0xffffffff << ((char)v2 & 31);
    v23 = idx + 1;
    if (v23 >= v0)
        return;
    iter = v12 + v23 * 4;
    for (v25 = v0 - v23; v25; iter += 1)
    {
        v25 -= 1;
        *(iter) = 0;
    }
    return;
}
