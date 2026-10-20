// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4639d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_4639d0(void)
{
    unsigned int v6;  // ebp
    char v7;  // al
    int k;  // eax
    void* iter;  // edi
    char v17;  // bl
    unsigned int i;  // edi
    char v8;  // bl
    unsigned int v9;  // edi
    char *iter1;  // esi
    void* v11;  // ecx
    int j;  // eax
    void* node;  // edi
    char v14;  // bl
    void* v0;  // [bp-0x8]
    char *v1;  // [bp-0x4]
    void* v2;  // [bp+0x4]
    char v3;  // [bp+0xc]
    unsigned int v4;  // [bp+0x10]
    unsigned int v5;  // [bp+0x14]

    v6 = v4;
    v8 = v7;
    v9 = i;
    v4 = v9;
    i = (unsigned int)((v9 % v6 >= (int)(v6 + ((int)(v6) >> 31 & 3)) >> 2) + 1) & v9 % v6;
    if (v5 <= v9)
        v4 = v5;
    v0 = v2;
    v1 = sub_46d04a(v4);
    iter1 = v1;
    if (!v1)
        return;
    i = v9 - ((v9 & 1) + i);
    sub_47a330(v1, v2, v4);
    if (i > NULL)
    {
        for (; v5 > NULL; i = i)
        {
            if (i < v6)
                v6 = i;
            v11 = v0 + v6;
            j = (v6 + 1) / 2;
            node = v11 - 1;
            v14 = v8;
            if (j > NULL)
            {
                do
                {
                    v14 = v8 + v3;
                    *((char *)node) = *(iter1) ^ v8;
                    j -= 1;
                    node -= 2;
                    iter1 += 1;
                    v8 = v14;
                } while (j > NULL);
            }
            k = v6 / 2;
            iter = v11 - 2;
            v17 = v14;
            if (k > NULL)
            {
                do
                {
                    v17 = v14 + v3;
                    *((char *)iter) = *(iter1) ^ v14;
                    k -= 1;
                    iter -= 2;
                    iter1 += 1;
                    v14 = v17;
                } while (k > NULL);
            }
            v8 = v17;
            v5 -= v6;
            i -= v6;
            v0 = v11;
            if (i <= NULL)
                break;
        }
    }
    sub_46c941(v1);
    return;
}
