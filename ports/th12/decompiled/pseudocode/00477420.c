// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477420; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4d52dc;

void* sub_477420(void* a0, char a1, unsigned int a2)
{
    unsigned int v1;  // edx
    unsigned int v2;  // eax
    unsigned int k;  // edx
    unsigned int v3;  // edi
    void* iter;  // edi
    void* v5;  // ecx
    unsigned int v6;  // ecx
    unsigned int i;  // ecx
    unsigned int v9;  // ecx
    unsigned int v10;  // ecx
    unsigned int v0;  // [bp-0x4]

    v1 = a2;
    if (!v1)
        return a0;
    v2 = a1;
    if (!a1 && v1 >= 0x100 && g_4d52dc)
        return sub_48b39a();
    v0 = v3;
    iter = a0;
    if (v1 >= 4)
    {
        v5 = -(a0);
        v6 = v5 & 3;
        if (v5 & 0x3)
        {
            v1 -= v6;
            do
            {
                i = v6;
                *((char *)iter) = v2;
                iter += 1;
                v6 = i - 1;
            } while (i != 1);
        }
        v2 *= 0x1010101;
        v9 = v1;
        v1 &= 3;
        if (v9 > 3)
        {
            for (v10 = v9 >> 2; v10; iter += 4)
            {
                v10 -= 1;
                *((unsigned int *)iter) = v2;
            }
            if (!v1)
                return a0;
        }
    }
    do
    {
        k = v1;
        *((char *)iter) = v2;
        iter += 1;
        v1 = k - 1;
    } while (k != 1);
    return a0;
}
