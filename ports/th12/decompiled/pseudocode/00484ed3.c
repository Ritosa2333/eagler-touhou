// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x484ed3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_484ed3(void)
{
    unsigned int *v3;  // ecx
    unsigned int j;  // esi
    unsigned int v13;  // eax
    unsigned int *v14;  // edi
    void* node;  // esi
    unsigned int v15;  // eax
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    void* iter;  // eax
    void* iter1;  // esi
    char v10;  // 4106
    unsigned int v11;  // edx
    unsigned int v0;  // [bp-0x14]
    unsigned int *iter2;  // [bp-0x8], Other Possible Types: unsigned int
    unsigned int v2;  // [bp+0x4]

    iter2 = v3;
    iter2 = 0;
    if (v2)
    {
        node = *(v14);
        if (*(v3) > 1)
        {
            do
            {
                v0 = 10;
                v7 = v6 / v0;
                *((char *)node) = (char)(v6 % v0) + 48;
                node += 1;
                *(v3) = *(v3) - 1;
            } while (v7 > 0 && (v15 = v7, *(v3) > 1));
        }
        iter = *(v14);
        *(v14) = node;
        iter1 = node - 1;
        do
        {
            v10 = *((char *)iter1);
            *((char *)iter1) = *((char *)iter);
            iter1 -= 1;
            *((char *)iter) = v10;
            iter += 1;
        } while (iter < iter1);
        return;
    }
    else if (v11 < *(v3))
    {
        j = v11 - 1;
        if (j != 0xffffffff)
        {
            do
            {
                v0 = 10;
                v13 = v6 / v0;
                iter2 += 1;
                *((char *)(j + *(v14))) = (char)(v6 % v0) + 48;
                j -= 1;
                v6 = v13;
            } while (j != 0xffffffff);
        }
        *(v14) = *(v14) + iter2;
        *(v3) = *(v3) - iter2;
        return;
    }
    else
    {
        *(v3) = 0;
        return;
    }
}
