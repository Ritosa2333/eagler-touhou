// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x489025; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_489025(unsigned int a0, int idx)
{
    int v6;  // eax
    unsigned int v7;  // edx
    unsigned int v8;  // edi
    unsigned int v9;  // ebx
    int index;  // ecx
    unsigned int *v11;  // edx
    int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x10]
    unsigned int v4;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]

    v6 = (int)(idx + (idx >> 31 & 31)) >> 5;
    v7 = idx & 0x8000001f;
    v2 = v8;
    if ((idx & 0x8000001f) < 0)
        v7 = (v7 - 1 | 0xffffffe0) + 1;
    v4 = 0;
    idx = 0;
    v5 = 32;
    v5 -= v7;
    v1 = v9;
    do
    {
        v3 = *((int *)(a0 + idx * 4)) & ~(0xffffffff << ((char)v7 & 31));
        *((unsigned int *)(a0 + idx * 4)) = (unsigned int)(*((int *)(a0 + idx * 4))) >> ((char)v7 & 31) | v4;
        idx += 1;
        v4 = v3 << ((char)v5 & 31);
    } while (idx < 3);
    v0 = 2;
    index = v0;
    v11 = a0 + (index - v6) * 4;
    do
    {
        if (index >= v6)
            *((unsigned int *)(a0 + index * 4)) = *(v11);
        else
            *((unsigned int *)(a0 + index * 4)) = 0;
    } while ((index = (int)(index - 1), v11 -= 4, index >= 0));
    return;
}
