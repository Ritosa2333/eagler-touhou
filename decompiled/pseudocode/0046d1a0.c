// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46d1a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void* sub_46d1a0(void* a0, char a1)
{
    unsigned int v2;  // ebx
    void* v3;  // edx
    unsigned short v12;  // ax
    void* iter;  // edx
    unsigned int v5;  // edi
    unsigned int v6;  // esi
    unsigned int v7;  // ebx
    unsigned int v8;  // ecx
    unsigned int v9;  // esi
    unsigned int v10;  // eax
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x8]

    v2 = a1;
    v3 = a0;
    iter = v3;
    if (iter & 0x3)
    {
        do
        {
            iter = v3 + 1;
            if (*((char *)v3) == (char)v2)
                return iter - 1;
            if (!*((char *)v3))
                return NULL;
            v3 = iter;
        } while (v3 & 0x3);
    }
    v1 = v5;
    v0 = v6;
    v7 = (v2 | v2 * 0x100) * 0x10000 | v2 | v2 * 0x100;
    while (1)
    {
        v8 = *((int *)iter) ^ v7;
        v9 = 0x7efefeff + *((int *)iter);
        v10 = *((int *)iter) ^ 0xffffffff ^ v9;
        iter += 4;
        if ((v8 ^ v8 + 0x7efefeff ^ 0xffffffff) & 0x81010100)
        {
            v11 = *((int *)((char *)iter - 4));
            if ((char)v11 == (char)v7)
                return iter - 4;
            if (!(char)v11)
                break;
            if (*((char *)((void*)&v11 + 1)) == (char)v7)
                return iter - 3;
            if (!*((char *)((void*)&v11 + 1)))
                break;
            v12 = v11 >> 16;
            if ((char)v12 == (char)v7)
                return iter - 2;
            if (!(char)v12)
                break;
            if (*((char *)((void*)&v12 + 1)) == (char)v7)
                return iter - 1;
            if (!*((char *)((void*)&v12 + 1)))
                break;
        }
        else if (v10 & 0x81010100 && (v10 & 0x1010100 || !(v9 & 0x80000000)))
        {
            break;
        }
    }
    return NULL;
}
