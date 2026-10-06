// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464af0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_464af0(void)
{
    int v3;  // eax
    int iter;  // edi
    char *v5;  // ecx
    char *iter1;  // esi
    char *v7;  // ebx
    char v8;  // al
    char *node;  // ecx
    char v10;  // al
    char *v11;  // esi
    char *v12;  // esi
    int v0;  // [bp-0x8]
    int v1;  // [bp-0x4]

    iter1 = v5;
    sub_477420(v7, 0, v3, v0, v1);
    v8 = *(iter1);
    if (*(iter1) != 10)
    {
        node = v7;
        while (v8 != 13 && iter)
        {
            *(node) = v8;
            v10 = *(iter1);
            node += 1;
            if (*(iter1) >= 129 && v10 <= 159 || (v11 = iter1, v10 >= 224 && !(v11 = iter1, v10 > 252)))
            {
                v12 = iter1 + 1;
                iter -= 1;
                *(node) = iter1[1];
                node += 1;
                v11 = v12;
            }
            v8 = v11[1];
            iter1 = v11 + 1;
            iter -= 1;
            if (v11[1] == 10)
            {
                iter1 += 1;
                break;
            }
        }
    }
    for (; *(iter1) == 10 || *(iter1) == 13; iter1 += 1);
    return;
}
