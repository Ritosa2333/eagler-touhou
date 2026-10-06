// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x488e79; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_488e79(unsigned int a0, int a1)
{
    unsigned int v2;  // edi
    int idx;  // eax
    unsigned int v4;  // edx
    unsigned int v5;  // edx
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    int index;  // eax
    unsigned int v9;  // edx
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]

    v1 = v2;
    v0 = 31;
    idx = (int)(a1 + (a1 >> 31 & v0)) >> 5;
    v4 = a1 & 0x8000001f;
    if ((a1 & 0x8000001f) < 0)
        v4 = (v4 - 1 | 0xffffffe0) + 1;
    v5 = 1 << ((char)(v0 - v4) & 31);
    v6 = 0;
    v7 = *((int *)(a0 + idx * 4)) + v5;
    if (v7 < *((int *)(a0 + idx * 4)) || v7 < v5)
        v6 = 1;
    *((unsigned int *)(a0 + idx * 4)) = v7;
    while (1)
    {
        index = idx - 1;
        if (idx - 1 < 0)
        {
            return v6;
        }
        else if (v6)
        {
            v9 = *((int *)(a0 + index * 4)) + 1;
            v6 = 0;
            if (v9 < *((int *)(a0 + index * 4)) || v9 < 1)
                v6 = 1;
            *((unsigned int *)(a0 + index * 4)) = v9;
            idx = index;
        }
        else
        {
            return v6;
        }
    }
}
